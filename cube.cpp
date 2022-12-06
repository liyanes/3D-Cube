#include "glext.h"
#include "cube.h"
#include <array>
#include <cstdlib>
#include <map>

using namespace mycube;
using namespace glExt;
using namespace std;
using namespace mycube::cubeSolver;

void mycube::singleCubeFace::rotate(axis _axis, bool positive, unsigned times) {
	if (times == 0) return;
	switch (_axis) {
	case ax_x:
		switch (nowFace) {
		case face::x_positive:
		case face::x_negative:return;
		case face::y_positive:nowFace = positive ? face::z_positive : face::z_negative; break;
		case face::y_negative:nowFace = positive ? face::z_negative : face::z_positive; break;
		case face::z_positive:nowFace = positive ? face::y_negative : face::y_positive; break;
		case face::z_negative:nowFace = positive ? face::y_positive : face::y_negative; break;
		default:throw;
		}
		rotate(_axis, positive, times - 1);
		return;
	case ax_y:
		switch (nowFace) {
		case face::y_positive:
		case face::y_negative:return;
		case face::x_positive:nowFace = positive ? face::z_negative : face::z_positive; break;
		case face::x_negative:nowFace = positive ? face::z_positive : face::z_negative; break;
		case face::z_positive:nowFace = positive ? face::x_positive : face::x_negative; break;
		case face::z_negative:nowFace = positive ? face::x_negative : face::x_positive; break;
		default:throw;
		}
		rotate(_axis, positive, times - 1);
		return;
	case ax_z:
		switch (nowFace) {
		case face::z_positive:
		case face::z_negative:return;
		case face::x_positive:nowFace = positive ? face::y_positive : face::y_negative; break;
		case face::x_negative:nowFace = positive ? face::y_negative : face::y_positive; break;
		case face::y_positive:nowFace = positive ? face::x_negative : face::x_positive; break;
		case face::y_negative:nowFace = positive ? face::x_positive : face::x_negative; break;
		default:throw;
		}
		rotate(_axis, positive, times - 1);
		return;
	default:throw;
	}
}

void mycube::singleCubeFace::rotate(mycube::face _axis, unsigned times) {
	if (times == 0) return;
	float angle = times * glm::radians(90.0f);
	this->nowFace = faceFromVec3(angleAxis(angle,vec3FromFace(_axis)) * vec3FromFace(this->nowFace));
}

mycube::Cube::Cube(unsigned level,float size,glExt::program& useProgram):
	level(level),
	size(size),
	_program(useProgram) {
	if (!useProgram.isLinked()) {
		useProgram.attach(vertexShader(
			"#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"layout (location = 1) in vec3 aNormal;\n"
			"layout (location = 2) in vec3 aColor;\n"
			"layout (location = 3) in vec2 aTexCoord;\n"
			"uniform mat4 model;"
			"uniform mat4 proAndView;"
			"uniform mat4 tinvModel;"
			//"uniform mat4 view;"
			//"uniform mat4 projection;"
			"out vec2 TexCoord;"
			"out vec3 color;\n"
			"out vec3 normal;\n"
			"out vec3 FragPos;\n"
			"void main()\n"
			"{\n"
			"   FragPos = vec3(model * vec4(aPos ,1.0));"
			//"   gl_Position = projection * view * vec4(FragPos,1.0);\n"
			"   gl_Position = proAndView * vec4(FragPos,1.0);\n"
			"   TexCoord = aTexCoord;"
			"   color = aColor;"
			//"   normal = mat3(transpose(inverse(model))) * aNormal;\n"
			"   normal = mat3(tinvModel) * aNormal;\n"
			"}"));
		useProgram.attach(fragmentShader(
			"#version 330 core\n"
			"in vec2 TexCoord;\n"
			"in vec3 color;\n"
			"in vec3 normal;\n"
			"in vec3 FragPos;\n"
			"out vec4 FragColor;\n"
			"uniform sampler2D texture0;"
			//"uniform sampler2D texture2;"
			"uniform vec3 lightColor;"
			"uniform vec3 lightPos;"
			"uniform vec3 viewPos;"
			"uniform int usePaint;\n"
			"uniform int useTexture;\n"
			"void main()\n"
			"{\n"
			// 环境光照
			"   float ambientStrength = 0.3;"
			"   vec3 ambient = ambientStrength * lightColor;"

			// 漫反射 
			"   vec3 norm = normalize(normal);"
			"   vec3 lightDir = normalize(lightPos - FragPos);"
			"   float diff = max(dot(norm, lightDir), 0.0);"
			"   vec3 diffuse = diff * lightColor;"

			// 镜面反射
			"   float specularStrength = 0.5;"
			"   vec3 viewDir = normalize(viewPos - FragPos);"
			"   vec3 reflectDir = reflect(-lightDir, norm);"
			"   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);"
			"   vec3 specular = specularStrength * spec * lightColor;"

			"   vec3 result;"
			"   if (useTexture != 0)"
			"      result = (ambient + diffuse + specular) * (usePaint != 0 ? vec3(texture(texture0,TexCoord)) : vec3(0.05f,0.05f,0.05f));"
			"   else result = (ambient + diffuse + specular) * (usePaint != 0 ? color : vec3(0.05f,0.05f,0.05f));"
			"   FragColor = vec4(result, 1.0);"
			"}\n"));
		useProgram.link();
	}
	unsigned level3 = level * level * level;
	const float base = -size / 2;
	const float step = size / level;
	this->scubes.reserve(level3);
	for (unsigned dem0 = 0; dem0 < level; dem0++) {
		for (unsigned dem1 = 0; dem1 < level; dem1++) {
			for (unsigned dem2 = 0; dem2 < level; dem2++) {
				singleCube *_cube = new singleCube();
				if (dem0 == 0) _cube->findFace(face::x_negative).outShow = true;
				else if (dem0 == level - 1) _cube->findFace(face::x_positive).outShow = true;
				if (dem1 == 0) _cube->findFace(face::y_negative).outShow = true;
				else if (dem1 == level - 1) _cube->findFace(face::y_positive).outShow = true;
				if (dem2 == 0) _cube->findFace(face::z_negative).outShow = true;
				else if (dem2 == level - 1) _cube->findFace(face::z_positive).outShow = true;
				_cube->initTranslate = glm::scale(glm::translate(glm::mat4(1.0f),
					glm::vec3(base + step * (dem0 + 0.5f), base + step * (dem1 + 0.5f), base + step * (dem2 + 0.5f))), glm::vec3(step * 0.95f));
				_cube->inCube = this;
				_cube->index = this->scubes.size();
				_cube->cubePos = { dem0,dem1,dem2 };
				this->scubes.push_back(_cube);
			}
		}
	}
	this->_sdata[0] = this->scubes[0]->getPointData(face::x_negative);
	this->_sdata[1] = this->scubes[0]->getPointData(face::x_positive);
	this->_sdata[2] = this->scubes[0]->getPointData(face::y_negative);
	this->_sdata[3] = this->scubes[0]->getPointData(face::y_positive);
	this->_sdata[4] = this->scubes[0]->getPointData(face::z_negative);
	this->_sdata[5] = this->scubes[0]->getPointData(face::z_positive);
	for (unsigned i = 0; i < 6; i++) {
		this->_varray[i].setData(this->_sdata[i], GL_STATIC_DRAW);
		this->_varray[i].setVertexAttributes({
			{0,3,GL_FLOAT,GL_FALSE,11 * sizeof(float),0},
			{1,3,GL_FLOAT,GL_FALSE,11 * sizeof(float),3 * sizeof(float)},
			{2,3,GL_FLOAT,GL_FALSE,11 * sizeof(float),6 * sizeof(float)},
			{3,2,GL_FLOAT,GL_FALSE,11 * sizeof(float),9 * sizeof(float)},
			});
	}
}

glExt::arrayData<mycube::cubeVertex> mycube::singleCube::getPointData() {
	arrayData<cubeVertex> _data;
	_data.reserve(36);
	for (auto i = faces.cbegin(); i != faces.cend(); i++) {
		for (auto j = i->points.cbegin(); j != i->points.cend(); j++) {
			_data.push_back(**j);
		}
	}
	return _data;
}

glExt::arrayData<cubeVertex> singleCube::getPointData(face face) {
	arrayData<cubeVertex> _data;
	_data.reserve(6);
	for (auto i = faces.cbegin(); i != faces.cend(); i++) {
		if (i->face != face) continue;
		for (auto j = i->points.cbegin(); j != i->points.cend(); j++) {
			_data.push_back(**j);
		}
	}
	return _data;
}

std::vector<singleCubeFace*> mycube::Cube::findSrcFaces(face face) {
	std::vector<singleCubeFace*> ret;
	ret.reserve((size_t)this->level * this->level);
	for (auto i = this->scubes.begin(); i != this->scubes.end(); i++) {
		singleCubeFace& _face = (*i)->findSrcFace(face);
		if (_face.outShow) {
			ret.push_back(&_face);
		}
	}
	return ret;
}

std::vector<singleCubeFace*> Cube::findFaces(face face) {
	std::vector<singleCubeFace*> ret;
	ret.reserve((size_t)this->level * this->level);
	for (auto i = this->scubes.begin(); i != this->scubes.end(); i++) {
		singleCubeFace& _face = (*i)->findFace(face);
		if (_face.outShow) {
			ret.push_back(&_face);
		}
	}
	return ret;
}

void Cube::paint() {
	_program.use();
	int posPaint = _program.getUniformPos("usePaint");
	int posModel = _program.getUniformPos("model");
	int posTexture = _program.getUniformPos("useTexture");
	int posTInvModel = _program.getUniformPos("tinvModel");
	_program.uniform(posTexture, 0);
	for (auto i = this->scubes.cbegin(); i != this->scubes.cend(); i++) {
		glm::mat4 model = ((*i)->useTimelyRotateMat ? (*i)->timelyRotateMat : (*i)->rotateMat) * (*i)->initTranslate;
		_program.uniform(posModel, model);
		_program.uniform(posTInvModel, glm::inverse(model),true);
		for (unsigned index = 0; index < 6; index++) {
			_program.uniform(posPaint, (*i)->faces[index].outShow);
			_varray[index].draw(GL_TRIANGLES);
		}
	}
}

void Cube::paintEach(paintEachFunc func) {
	_program.use();
	int posPaint = _program.getUniformPos("usePaint");
	int posModel = _program.getUniformPos("model");
	int posTexture = _program.getUniformPos("useTexture");
	int posTInvModel = _program.getUniformPos("tinvModel");
	_program.uniform(posTexture, 0);
	for (auto i = this->scubes.cbegin(); i != this->scubes.cend(); i++) {
		glm::mat4 model = ((*i)->useTimelyRotateMat ? (*i)->timelyRotateMat : (*i)->rotateMat) * (*i)->initTranslate;
		_program.uniform(posModel, model);
		_program.uniform(posTInvModel, glm::inverse(model), true);
		for (unsigned index = 0; index < 6; index++) {
			_program.uniform(posPaint, (*i)->faces[index].outShow);
			//_varray[index].draw(GL_TRIANGLES);
			func(_program, i, index, _varray);
			_program.use();
		}
	}
}

void Cube::paintEach(paintEachFunc func,bool useTexture) {
	_program.use();
	int posPaint = _program.getUniformPos("usePaint");
	int posModel = _program.getUniformPos("model");
	int posTexture = _program.getUniformPos("useTexture");
	int posTInvModel = _program.getUniformPos("tinvModel");
	_program.uniform(posTexture, useTexture);
	for (auto i = this->scubes.cbegin(); i != this->scubes.cend(); i++) {
		glm::mat4 model = ((*i)->useTimelyRotateMat ? (*i)->timelyRotateMat : (*i)->rotateMat) * (*i)->initTranslate;
		_program.uniform(posModel, model);
		_program.uniform(posTInvModel, glm::inverse(model), true);
		for (unsigned index = 0; index < 6; index++) {
			_program.uniform(posPaint, (*i)->faces[index].outShow);
			//_varray[index].draw(GL_TRIANGLES);
			func(_program, i, index, _varray);
			_program.use();
		}
	}
}

void Cube::paint(std::array<glExt::texture*,6>& _texture) {
	_program.use();
	int posTexture = _program.getUniformPos("useTexture");
	int posPaint = _program.getUniformPos("usePaint");
	int posModel = _program.getUniformPos("model");
	int posTInvModel = _program.getUniformPos("tinvModel");
	_program.uniform(posTexture, 1);
	for (auto i = this->scubes.cbegin(); i != this->scubes.cend(); i++) {
		glm::mat4 model = ((*i)->useTimelyRotateMat ? (*i)->timelyRotateMat : (*i)->rotateMat) * (*i)->initTranslate;
		_program.uniform(posModel, model);
		_program.uniform(posTInvModel, glm::inverse(model), true);
		for (unsigned index = 0; index < 6; index++) {
			_program.uniform(posPaint, (*i)->faces[index].outShow);
			_texture[index]->bind();
			_varray[index].draw(GL_TRIANGLES);
		}
	}
}

std::vector<singleCube*> Cube::getCubes(axis _axis,unsigned depth) {
	if (depth >= level) throw;
	std::vector<singleCube*> ret;
	ret.reserve((size_t)level * level);
	for (unsigned dem0 = 0; dem0 < level; dem0++) {
		for (unsigned dem1 = 0; dem1 < level; dem1++) {
			switch (_axis) {
			case ax_x:ret.push_back(&getCube(depth, dem0, dem1)); break;
			case ax_y:ret.push_back(&getCube(dem0, depth, dem1)); break;
			case ax_z:ret.push_back(&getCube(dem0, dem1, depth)); break;
			default:throw;
			}
		}
	}
	return ret;
}

void Cube::rotate(axis _axis, bool positive, unsigned depth) {
	auto _set = getCubes(_axis, depth);
	for (auto i = _set.begin(); i != _set.end(); i++) {
		(*i)->rotate(_axis, positive);
	}
}

void Cube::timeRotate(axis _axis, bool positive, int depth, float radians) {
	auto _set = getCubes(_axis, depth);
	for (auto i = _set.begin(); i != _set.end(); i++) {
		(*i)->useTimelyRotateMat = true;
		(*i)->timelyRotate(_axis, positive,radians);
	}
}
void Cube::timeRotateEnd(axis _axis,bool positive, int depth) {
	auto _set = getCubes(_axis, depth);
	for (auto i = _set.begin(); i != _set.end(); i++) {
		(*i)->useTimelyRotateMat = false;
	}
	this->rotate(_axis, positive, depth);
}

void Cube::frameRequire(axis _axis, bool positive, int depth, float rate , float requiredRadians ) {
	_animateVec frame;
	frame.lastFrame = _frame.empty() ? (float)glfwGetTime() : 0.0f;
	frame.data._axis = _axis;
	frame.data.depth = depth;
	frame.data.positive = positive;
	frame.data.rate = rate;
	frame.data.requiredRadians = requiredRadians;
	this->_frame.push(
		frame
	);
}

void Cube::frameCheck() {
	if (!_frame.empty()) {
		_animateVec& frame = _frame.front();
		// 旋转中
		if (frame.firstFrame && this->frameCallback && !this->frameCallback(frame.data)) {
			this->_frame.pop();
			return;
		}
		if (frame.firstFrame) {
			frame.firstFrame = false;
		}
		float _rad = this->sizer(
			((float)glfwGetTime() - frame.lastFrame)*frame.data.rate,
			0,frame.data.requiredRadians,
			frame.data.requiredRadians);
		this->timeRotate(frame.data._axis,frame.data.positive,frame.data.depth,_rad);
		if (((float)glfwGetTime() - frame.lastFrame) * frame.data.rate >= frame.data.requiredRadians) {
			timeRotateEnd(frame.data._axis, frame.data.positive,frame.data.depth);
			_frame.pop();
			if (!_frame.empty()) {
				_frame.front().lastFrame = (float)glfwGetTime();
			}
		}
	}
}

void mycube::Cube::frameClear()
{
	if (this->_frame.size() > 1) {
		_animateVec tmpval = this->_frame.front();
		this->_frame = {};
		this->_frame.push(tmpval);
	}
}

void singleCube::rotate(axis _axis, bool positive, unsigned times) {
	for (auto i = faces.begin(); i != faces.end(); i++) i->rotate(_axis, positive, times);
	this->rotateMat = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f * times), axis2vec3(_axis, positive)) * this->rotateMat;
	times %= 4;
	if (!positive) times = 4 - times;
	unsigned temp;
	for (unsigned i = 0; i < times; i++) {
		switch (_axis) {
		case ax_x:
			temp = cubePos.z;
			cubePos.z = cubePos.y;
			cubePos.y = inCube->level - temp - 1;
			break;
		case ax_y:
			temp = cubePos.x;
			cubePos.x = cubePos.z;
			cubePos.z = inCube->level - temp - 1;
			break;
		case ax_z:
			temp = cubePos.y;
			cubePos.y = cubePos.x;
			cubePos.x = inCube->level - temp - 1;
			break;
		default:
			throw;
		}
	}
}

void singleCube::timelyRotate(axis _axis, bool positive, float radians) {
	this->timelyRotateMat = glm::rotate(glm::mat4(1.0f), radians, axis2vec3(_axis, positive)) * this->rotateMat;
}

using namespace glm;

mycube::cubeSolver::faceMap::faceMap(mycube::Cube& cube, glExt::camera::face _face, glExt::camera::face faceUp):
	_cube(cube),_face(_face),_faceUp(faceUp),level(cube.getLevel())
{
	using namespace glExt;
	constexpr auto allLarger = [](glm::vec3 _left, glm::vec3 _right)->bool {
		return _left.x >= _right.x && _left.y >= _right.y && _left.z >= _right.z;
	};
	constexpr auto getNoZero = [](glm::vec3 _ope) -> unsigned {
		if (_ope.x) return 0;
		if (_ope.y) return 1;
		if (_ope.z) return 2;
		throw;
	};

	glm::vec3 up = vec3FromFace(faceUp);
	glm::vec3 cur = vec3FromFace(_face);
	glm::vec3 right = glm::cross(up, cur);
	// 向上对于原坐标递增
	bool upInc = abs(up) == up;
	// 向右对于原坐标递增
	bool rightInc = abs(right) == right;
	glm::vec<3, int> upI = up;
	glm::vec<3, int> rightI = right;
	glm::vec<3, int> base{0};			     //基础方块

	if (allLarger(cur, vec3(0.0f))) base[getNoZero(cur)] = level - 1;
	else base[getNoZero(cur)] = 0;
	if (allLarger(upI, vec3(0.0f))) base[getNoZero(upI)] = 0;
	else base[getNoZero(upI)] = level - 1;
	if (allLarger(rightI, vec3(0.0f))) base[getNoZero(rightI)] = 0;
	else base[getNoZero(rightI)] = level - 1;

	this->reserve(level);
	for (int r = 0; r != level; r ++) {
		std::vector<mycube::singleCubeFace*> lineVector;
		lineVector.reserve(level);
		for (int u = 0; u != level; u++) {
			lineVector.push_back(&cube.getCube(base + r * rightI + u * upI).findFace(_face));
		}
		this->push_back(lineVector);
	}
}

cubeSolver::faceMap mycube::cubeSolver::faceMap::getNear(face faceUp, faceRotate rotate)
{
	switch (rotate) {
	case faceRotate::up:return faceMap(_cube, getRotatedFace(_face, faceRotate::up, faceUp), oppositeFace(faceUp));
	case faceRotate::down:return faceMap(_cube, getRotatedFace(_face, faceRotate::down, faceUp), _face);
	case faceRotate::left:return faceMap(_cube, getRotatedFace(_face, faceRotate::left, faceUp), faceUp);
	case faceRotate::right:return faceMap(_cube, getRotatedFace(_face, faceRotate::right, faceUp), faceUp);
	default:throw;
	}
}

const cubeSolver::faceMap mycube::cubeSolver::faceMap::getNear(face faceUp, faceRotate rotate) const {
	return const_cast<faceMap*>(this)->getNear(faceUp, rotate);
}

void mycube::cubeSolver::faceMap::ratate(bool anticlock)
{
	std::vector<std::vector<singleCubeFace*>> res;
	res.reserve(level);
	for (unsigned i = 0; i < level; i++) res[i].reserve(level);
	for (unsigned dem0 = 0; dem0 < level; dem0++) {
		for (unsigned dem1 = 0; dem1 < level; dem1++) {
			res[dem0][dem1] = anticlock ? (*this)[level - dem1 - 1][dem0] : (*this)[dem1][level - dem0 - 1];
		}
	}
	this->swap(res);
	this->_faceUp = getRotatedFace(_faceUp, anticlock ? faceRotate::left : faceRotate::right, _face);
}

mycube::cubeSolver::solver::solver(Cube& cube, glExt::QuatCamera& quatCam)
	:_cube(cube),_quatCam(quatCam),curWorkFace(quatCam.getMajorFace()),curWorkUpFace(quatCam.getMajorUpFace())
{
	updateAxisMap();
}

glm::vec3 mycube::cubeSolver::getNearByAxisWay(glm::vec3 relativePos)
{
	// 点积最大,即方向上最重合
	glm::vec3 ret = axisWays[0];
	float dis = glm::dot(relativePos,ret);
	for (auto i = axisWays.cbegin(); i != axisWays.cend(); i++) {
		float tmpdis = glm::dot(*i,relativePos);
		if (tmpdis > dis) {
			ret = *i;
			dis = tmpdis;
		}
	}
	return ret;
}

float Cube::_inset_ease(float cur, float start, float change, float duration) {
	cur /= duration / 2;
	if (cur < 1) return change / 2 * cur * cur + start;
	cur--;
	return -change / 2 * (cur * (cur - 2) - 1) + start;
}

void solver::updateAxisMap() {
	this->_workmap[int(solver::relativeAxis::front)] = oppositeFace(curWorkFace);
	this->_workmap[int(solver::relativeAxis::back)] = curWorkFace;
	this->_workmap[int(solver::relativeAxis::left)] = faceFromVec3(glm::cross(vec3FromFace(curWorkFace),vec3FromFace(curWorkUpFace)));
	this->_workmap[int(solver::relativeAxis::right)] = faceFromVec3(glm::cross(vec3FromFace(curWorkUpFace),vec3FromFace(curWorkFace)));
	this->_workmap[int(solver::relativeAxis::up)] = curWorkUpFace;
	this->_workmap[int(solver::relativeAxis::down)] = oppositeFace(curWorkUpFace);
}

void solver::rotate(relativeAxis _axis,unsigned depth) {
	updateAxisMap();
	this->_cube.rotate(this->_workmap[int(_axis)], depth);
}

void solver::distribute(unsigned stepnum,float rate) {
	if (stepnum == 0) stepnum = rand() % 20 + 10;
	for (unsigned i = 0; i < stepnum; i++) {
		this->frameRequire(relativeAxis(rand() % 6), rand() % this->_cube.getLevel(), rate);
	}
}

void solver::initPickMode(window& window) {
	this->_pickpro = new program("#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aNormal;\n"
		"layout (location = 2) in vec3 aColor;\n"
		"layout (location = 3) in vec2 aTexCoord;\n"
		"uniform mat4 stdmat;"
		"void main(){"
		"   gl_Position = stdmat * vec4(aPos,1.0);"
		"}",
		"#version 330 core\n"
		"uniform uint gObjectIndex;"
		"uniform uint gDrawIndex;"
		"out uvec3 FragColor;"
		"void main(){"
		"    FragColor = uvec3(gObjectIndex,gDrawIndex,gl_PrimitiveID);"
		"}");
	this->_picker = new picker(*this->_pickpro,window);
}

const singleCubeFace* solver::getCurrentPickFace(window& window,int x,int y, const glm::mat4 stdmat) const {
	window.makeContextCurrent();
	_picker->enableWrite();
	glExt::clearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	int posStdmat = this->_pickpro->getUniformPos("stdmat");
	int posObjectIndex = this->_pickpro->getUniformPos("gObjectIndex");
	int posDrawIndex = this->_pickpro->getUniformPos("gDrawIndex");

	unsigned appcolor = 1;
	for (auto i = this->_cube.scubes.cbegin(); i != this->_cube.scubes.cend(); i++) {
		_pickpro->uniform(posStdmat,stdmat * ((*i)->useTimelyRotateMat ? (*i)->timelyRotateMat : (*i)->rotateMat) * (*i)->initTranslate);
		for (unsigned index = 0; index < 6; index++) {
			_pickpro->uniform(posDrawIndex, appcolor);
			_pickpro->uniform(posObjectIndex, appcolor++);
			_cube._varray[index].draw(GL_TRIANGLES);
		}
	}

	checkError();

	auto value = _picker->readIndex(x,y);

	checkError();
	_picker->disableWrite();

	if (value.ObjectID == 0) return nullptr;
	return &this->_cube.scubes[(value.ObjectID -1) / 6]->faces[(value.ObjectID -1) % 6];
}

bool facePlaceHolder::match(const faceMap& _map, const std::vector<std::vector<placeholder>*>& _data) {
	// 面大小不匹配
	unsigned level = _map.getLevel();
	if (_data.size() != level || _data[0]->size() != level) throw;
	this->cholder = colorPlaceholder();
	for (unsigned dem0 = 0; dem0 < level; dem0++) {
		for (unsigned dem1 = 0; dem1 < level; dem1++) {
			if (!this->cholder.test((*_data[dem0])[dem1], _map[dem0][dem1]->face)) {
				return false;
			}
		}
	}
	return true;
}

bool solver::rotateByFace(singleCubeFace& from, singleCubeFace& to, float rate) {
	if (from.nowFace == to.nowFace) {
		// 定义任意上轴
		face _tup = from.nowFace == face::x_negative || from.nowFace == face::x_positive ? face::y_positive : face::x_positive;
		faceMap map(this->_cube, from.nowFace, _tup);
		auto pos1 = map.findSingleCubeFace(from), pos2 = map.findSingleCubeFace(to);
		if (pos1.x == (unsigned)-1 || pos2.x == (unsigned)-1) return false;
		if (pos1.y == pos2.y) {
			if (pos2.x > pos1.x) {
				// 上轴是旋转轴
				this->_cube.frameRequire(_tup, pos1.y, rate);
			}
			else {
				// 下轴是旋转轴
				this->_cube.frameRequire(oppositeFace(_tup),this->_cube.getLevel() -  pos1.y - 1, rate);
			}
			return true;
		}
		if (pos1.x == pos2.x) {
			if (pos1.y > pos2.y) {
				//右轴为旋转轴
				this->_cube.frameRequire(camera::faceFromVec3(glm::cross(camera::vec3FromFace(_tup), camera::vec3FromFace(from.nowFace))), pos1.x, rate);
			}
			else {
				// 左轴为旋转轴
				this->_cube.frameRequire(camera::faceFromVec3(glm::cross(camera::vec3FromFace(from.nowFace), camera::vec3FromFace(_tup))), this->_cube.getLevel() - pos1.x - 1, rate);
			}
		}
	}
	return false;
}

glExt::crood<unsigned> faceMap::findSingleCubeFace(const singleCubeFace& face) {
	for (unsigned i = 0; i < this->size(); i++) {
		for (unsigned j = 0; j < this->size(); j++) {
			if ((*this)[i][j] == &face) return {i,j};
		}
	}
	return { (unsigned)-1,(unsigned)-1 };
}

faceMap facePlaceHolder::matchAll(const faceMap& _map, const std::vector<std::vector<placeholder>*>& _data) {
	if (match(_map, _data)) return _map;
	faceMap map1 = _map;
	face curup = map1.getUpFace();
	for (int i = 0; i < 2; i++) {
		map1.ratate(true);
		if (match(_map, _data)) {
			return map1;
		}
	}
	throw;
}

#pragma warning(disable:6101)
bool facePlaceHolder::matchAll(const faceMap& _map, const std::vector<std::vector<placeholder>*>& _data, _Out_writes_bytes_all_(sizeof(faceMap)) faceMap* _matchMap) {
	if (match(_map, _data)) {
		new(_matchMap)faceMap(_map);
		return true;
	}
	faceMap map1(_map);
	face curup = map1.getUpFace();
	for (int i = 0; i < 2; i++) {
		map1.ratate(true);
		if (match(map1, _data)) {
			new(_matchMap)faceMap(map1);
			return true;
		}
	}
	return false;
}

void solutionFaceMap::rotateSingle(bool clockwise){
	singleFaceData* _ndata = new singleFaceData[level*level];
	if (clockwise){
		for (unsigned i = 0; i < level;i++){
			for (unsigned j = 0; j <level;j++){
				_ndata[i*level+j] = _data[(level - j - 1) * level + i];
			}
		}
	}else{
		for (unsigned i = 0; i < level;i++){
			for (unsigned j = 0; j <level;j++){
				_ndata[i*level+j] = _data[j * level + level - i - 1];
			}
		}
	}
	delete[] _data;
	_ndata = _data;
}

void solutionFaceMap::rotateBack(){
	singleFaceData* _ndata = new singleFaceData[level*level];
	for (unsigned i = 0; i < level;i++){
		for (unsigned j = 0; j <level;j++){
			_ndata[i*level+j] = _data[(level - i - 1)*level + level -j - 1];
		}
	}
	delete[] _data;
	_data = _ndata;
}

void solutionFaceMap::rotate(unsigned times,bool clockwise){
	int v = times * (clockwise ? 1 : -1) % 4;
	if (v < 0) v += 4;
	switch (v){
		case 0:break;
		case 1:rotateSingle(true);break;
		case 2:rotateBack();break;
		case 3:rotateSingle(false);break;
	}
}

solutionFaceMap solutionFaceMap::getRotateSingle(bool clockwise){
	solutionFaceMap ret(this->level);
	auto _ndata = ret.data();
	if (clockwise){
		for (unsigned i = 0; i < level;i++){
			for (unsigned j = 0; j <level;j++){
				_ndata[i*level+j] = _data[(level - j - 1) * level + i];
			}
		}
	}else{
		for (unsigned i = 0; i < level;i++){
			for (unsigned j = 0; j <level;j++){
				_ndata[i*level+j] = _data[j * level + level - i - 1];
			}
		}
	}
	return ret;
}

solutionFaceMap::solutionFaceMap(const faceMap& _map):
	level(_map.getLevel()){
	for (unsigned i = 0; i < level;i++){
		for (unsigned j = 0;j < level;j++){
			this->_data[i*level+j] = {_map[i][j]->face,_map[i][j]->nowFace};
		}
	}
}

solution::solution(Cube& _cube):
	level(_cube.getLevel()),
	_maps{solutionFaceMap(faceMap(_cube,face::x_positive,face::y_positive)),
	solutionFaceMap(faceMap(_cube,face::x_negative,face::y_negative)),
	solutionFaceMap(faceMap(_cube,face::y_positive,face::z_positive)),
	solutionFaceMap(faceMap(_cube,face::y_negative,face::z_negative)),
	solutionFaceMap(faceMap(_cube,face::z_positive,face::x_positive)),
	solutionFaceMap(faceMap(_cube,face::z_negative,face::x_negative))}
	{
		
}

solution solution::getRotated(camera::faceRotate rotate){
	throw;
}
#pragma warning(default:6101)
