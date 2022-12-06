#include "glext.h"
#include "err.h"
#include <Windows.h>
#include <algorithm>
#include <iostream>
#include <sstream>

#pragma warning(disable:26451)
#pragma warning(disable:6262)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#pragma warning(default:26451)
#pragma warning(default:6262)
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

using namespace std;
using namespace glExt;
using namespace glExt::err;
using namespace glm;

glExt::shader::shader(GLenum type):compiled(false)
{
	shaderCode = glCreateShader(type);
}

shader::shader(GLenum type, const char* source) :compiled(false) {
	shaderCode = glCreateShader(type);
	compile(source);
}

glExt::shader::shader(GLenum type, const char** const source, unsigned len)
{
	shaderCode = glCreateShader(type);
	compile(source, len);
}

void glExt::shader::compile(const char* source)
{
	if (compiled) throw glShaderCompileFailed("Has Been Compiled");
	glShaderSource(shaderCode, 1, &source, NULL);
	glCompileShader(shaderCode);
	int  success;
	glGetShaderiv(shaderCode, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shaderCode, 512, NULL, infoLog);
		throw glShaderCompileFailed(infoLog);
	}
	compiled = true;
}

void glExt::shader::compile(const char** const source,unsigned len)
{
	if (compiled) throw glShaderCompileFailed("Has Been Compiled");
	glShaderSource(shaderCode, len, source, NULL);
	glCompileShader(shaderCode);
	int  success;
	glGetShaderiv(shaderCode, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shaderCode, 512, NULL, infoLog);
		throw glShaderCompileFailed(infoLog);
	}
	compiled = true;
}

void glExt::program::attach(const shader& _shader)
{
	if (linked) throw glLinkFailed("Try to attach the linked program");
	glAttachShader(programCode, _shader.glCode());
}

void glExt::program::link()
{
	if (linked) throw glLinkFailed("Try to link the linked program");
	glLinkProgram(programCode);
	int success;
	glGetProgramiv(programCode, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[256];
		glGetProgramInfoLog(programCode, sizeof(infoLog), NULL, infoLog);
		throw glLinkFailed(infoLog);
	}
	linked = true;
}

void glExt::buffer::setData(signed long long size, const void* data, unsigned usage)
{
	this->size = size;
	glBindBuffer(bufferType,bufferCode);
	glBufferData(bufferType, size, data, usage);
	checkError();
}
#pragma warning(disable:4312)
void glExt::vertexArray::setVertexAttributes(const attributes& _attr)
{
	bind();
	for (auto i = _attr.cbegin(); i != _attr.cend(); i++) {
		glVertexAttribPointer(i->location, i->size, i->type, i->normalize, i->stride, (void*)i->offest);
		glEnableVertexAttribArray(i->location);
		checkError();
	}
	this->_attrs = _attr;
}
#pragma warning(default:4312)

void glExt::vertexArray::draw(const GLenum mode)
{
	bind();
	glDrawArrays(mode, 0, (int)(size / singleDataLen));
	checkError();
}

void glExt::vertexArray::draw(const GLenum mode,const attribute& _attr)
{
	bind();
	glDrawArrays(mode, _attr.offest, (int)(size / _attr.stride));
	checkError();
}

void glExt::vertexArray::draw(const GLenum mode, const attribute& _attr, const int len)
{
	bind();
	glDrawArrays(mode, _attr.offest, len);
	checkError();
}

void glExt::vertexArray::draw(const GLenum mode, const int offest, const int len)
{
	bind();
	glDrawArrays(mode, offest, len);
	checkError();
}


static bool initialized = false;

LONG WINAPI TopLevelUnHandledException(_In_ struct _EXCEPTION_POINTERS* pExptInfo) {
#ifdef _DEBUG
	return EXCEPTION_CONTINUE_SEARCH;
#else
	wstringstream ss(L"发生未捕获的错误:\n");
	ss << L"错误号:" << pExptInfo->ExceptionRecord->ExceptionCode << L"\n";
	ss << L"错误位置:" << pExptInfo->ExceptionRecord->ExceptionAddress << L"\n";
	ss << L"错误信息" << pExptInfo->ExceptionRecord->ExceptionInformation << L"\n";
	glExt::finalize();
	MessageBox(NULL, ss.str().c_str(), L"错误", MB_OK | MB_ICONERROR);
	return EXCEPTION_EXECUTE_HANDLER;
#endif
};

void glExt::initialize(version version) {
	if (initialized) {
		throw glInitFailed("已经初始化");
	}
	if (glfwInit() == GLFW_FALSE) {
		throw glInitFailed();
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version.major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version.minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef _MAC
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	SetUnhandledExceptionFilter(TopLevelUnHandledException);
	initialized = true;
}
void glExt::finalize()
{
	glfwTerminate();
	initialized = false;
};

std::vector<window::_windowsEvent> window::_windows{};

void glExt::window::_handle_mouseCallback(GLFWwindow* window,int button,int action ,int mode) {
	for (auto i = _windows.begin(); i != _windows.end(); i++) {
		if (i->window->_window == window) {
			if (!i->mouse) return;
			i->mouse(*i->window, button, action, mode);
			return;
		}
	}
	throw;
}

void glExt::window::_handle_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	for (auto i = _windows.begin(); i != _windows.end(); i++) {
		if (i->window->_window == window) {
			if (!i->key) return;
			i->key(*i->window, key, scancode, action, mods);
			return;
		}
	}
	throw;
}

void glExt::window::_handle_cursorCallback(GLFWwindow* window, double xpos,double ypos) {
	for (auto i = _windows.begin(); i != _windows.end(); i++) {
		if (i->window->_window == window) {
			if (!i->cursor) return;
			i->cursor(*i->window, xpos, ypos);
			return;
		}
	}
	throw;
}

void glExt::window::_handle_scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	for (auto i = _windows.begin(); i != _windows.end(); i++) {
		if (i->window->_window == window) {
			if (!i->scroll) return;
			i->scroll(*i->window, xoffset, yoffset);
			return;
		}
	}
	throw;
}

void window::_handle_frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
	for (auto i = _windows.begin(); i != _windows.end(); i++) {
		if (i->window->_window == window) {
			if (!i->frameBufferSize) return;
			i->frameBufferSize(*i->window, width, height);
			return;
		}
	}
	throw;
}

static bool gladInitilized = false;

glExt::window::window(int width, int height, const char* title, bool setCurrent, GLFWmonitor* monitor, GLFWwindow* share)
	:_fullscr_saved()
{
	_window = glfwCreateWindow(width, height, title, monitor, share);
	if (_window == NULL)
	{
		throw glWindowCreateFailed("Window Create Failed");
	}
	if (setCurrent) glfwMakeContextCurrent(_window);
	if (!gladInitilized) {
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			throw glInitFailed("glad initilized failed");
		}
	}
	_windows.push_back({ this ,NULL});
	glfwSetMouseButtonCallback(_window, _handle_mouseCallback);
	glfwSetKeyCallback(_window, _handle_keyCallback);
	glfwSetCursorPosCallback(_window, _handle_cursorCallback);
	glfwSetScrollCallback(_window, _handle_scrollCallback);
	glfwSetFramebufferSizeCallback(_window,_handle_frameBufferSizeCallback);
}

glExt::window::~window() {
	for (auto i = _windows.begin(); i != _windows.end(); i++) {
		if (i->window == this) {
			_windows.erase(i);
			break;
		}
	}
}

void window::fitViewport() {
	int width, height;
	glfwGetWindowSize(_window, &width, &height);
	glViewport(0,0,width, height);
}

constexpr unsigned glExt::getTypeLen(GLenum type) {
	switch (type) {
	case GL_INT:			return sizeof(int);
	case GL_UNSIGNED_INT:	return sizeof(unsigned);
	case GL_FLOAT:			return sizeof(float);
	case GL_DOUBLE:			return sizeof(double);
	case GL_UNSIGNED_BYTE:	return sizeof(unsigned char);
	case GL_BYTE:			return sizeof(unsigned char);
	case GL_SHORT:			return sizeof(short);
	case GL_UNSIGNED_SHORT:	return sizeof(unsigned short);
	default:
		throw;
	}
}

void glExt::textureUnit::active(unsigned index) {
	if (index >= unitLen()) throw glOutofRange();
	glActiveTexture(GL_TEXTURE0 + index);
}

void glExt::fpsCamera::setEulerAngle(float pitch, float yaw,float distance, bool useMoveTarget) {
	this->setLookWay(glm::vec3(cos(pitch) * cos(yaw), sin(pitch), cos(pitch) * sin(yaw)) * distance,useMoveTarget);
}

camera::face glExt::fpsCamera::getMajorFace() const
{
	glm::vec3 _rel = getRelative();
	glm::vec3 _rela = glm::abs(_rel);
	float* start = glm::value_ptr(_rela);
	unsigned index_max = (unsigned)(glExt::max(start, start + 2) - start);
	switch (index_max) {
	case 0:return _rel[0] >= 0 ? face::x_positive : face::x_negative;
	case 1:return _rel[1] >= 0 ? face::y_positive : face::y_negative;
	case 2:return _rel[2] >= 0 ? face::z_positive : face::z_negative;
	default:throw;
	}
}


void glExt::window::setMouseCallback(mouseCallback func) {
	for (auto i = _windows.begin(); i != _windows.end(); i++) {
		if (i->window == this) {
			i->mouse = func;
			return;
		}
	}
	throw glNotRegerister();
}
void glExt::window::setKeyCallback(keyCallback func) {
	for (auto i = _windows.begin(); i != _windows.end(); i++) {
		if (i->window == this) {
			i->key = func;
			return;
		}
	}
	throw glNotRegerister();
}
void glExt::window::setCursorCallback(cursorCallback func) {
	for (auto i = _windows.begin(); i != _windows.end(); i++) {
		if (i->window == this) {
			i->cursor = func;
			return;
		}
	}
	throw glNotRegerister();
}
void glExt::window::setScrollCallback(scrollCallback func) {
	for (auto i = _windows.begin(); i != _windows.end(); i++) {
		if (i->window == this) {
			i->scroll = func;
			return;
		}
	}
	throw glNotRegerister();
}

void window::setFramebufferSizeCallback(frameBufferSizeCallback func) {
	for (auto i = _windows.begin(); i != _windows.end(); i++) {
		if (i->window == this) {
			i->frameBufferSize = func;
			return;
		}
	}
	throw glNotRegerister();
}

camera::face glExt::camera::oppositeFace(const face from)
{
	switch (from) {
	case face::x_positive: return face::x_negative;
	case face::x_negative: return face::x_positive;
	case face::y_positive: return face::y_negative;
	case face::y_negative: return face::y_positive;
	case face::z_positive: return face::z_negative;
	case face::z_negative: return face::z_positive;
	default:throw;
	}
}

camera::face glExt::camera::faceFromVec3(glm::vec3 _vec)
{
	float* start = glm::value_ptr(_vec);
	if (emptyComp(_vec) != 2)
		throw;
	if (_vec.x) return _vec.x > 0 ? face::x_positive : face::x_negative;
	if (_vec.y) return _vec.y > 0 ? face::y_positive : face::y_negative;
	if (_vec.z) return _vec.z > 0 ? face::z_positive : face::z_negative;
	throw;
}

camera::face glExt::camera::faceMajorFromVec3(glm::vec3 _vec)
{
	glm::vec3 _vabs = glm::abs(_vec);
	unsigned _maxind = (unsigned)(max(glm::value_ptr(_vabs), glm::value_ptr(_vabs) + 2) - &_vabs.x);
	glm::vec3 ret(0.0f);
	ret[_maxind] = _vec[_maxind] > 0 ? 1.0f : -1.0f;
	return faceFromVec3(ret);
}

glm::vec3 glExt::camera::vec3FromFace(face _face)
{
	switch (_face) {
	case face::x_positive: return glm::vec3(1.0f, 0.0f, 0.0f);
	case face::x_negative: return glm::vec3(-1.0f, 0.0f, 0.0f);
	case face::y_positive: return glm::vec3(0.0f, 1.0f, 0.0f);
	case face::y_negative: return glm::vec3(0.0f, -1.0f, 0.0f);
	case face::z_positive: return glm::vec3(0.0f, 0.0f, 1.0f);
	case face::z_negative: return glm::vec3(0.0f, 0.0f, -1.0f);
	default:throw;
	}
}

camera::face glExt::camera::getRotatedFace(const face from, const faceRotate way, const face faceUp)
{
	glm::vec3 _from = vec3FromFace(from);
	glm::vec3 _up = vec3FromFace(faceUp);
	glm::vec3 _ret = glm::cross(_from, _up);
	if (_ret == glm::vec3(0.0f)) throw;			// 共线(即相反面)
	switch (way) {
	case faceRotate::up:
		return faceUp;
	case faceRotate::left:
		return faceFromVec3(_ret);
	case faceRotate::down:
		return oppositeFace(faceUp);
	case faceRotate::right:
		return oppositeFace(faceFromVec3(_ret));
	default:throw;
	}
}

camera::face glExt::camera::getRotatedFace(const glm::vec3 from, const faceRotate way, const glm::vec3 faceUp)
{
	glm::vec3 _ret = from * faceUp;
	switch (way) {
	case faceRotate::up:return faceFromVec3(faceUp);
	case faceRotate::left:return faceFromVec3(_ret);
	case faceRotate::right:return oppositeFace(faceFromVec3(faceUp));
	case faceRotate::down:return oppositeFace(faceFromVec3(_ret));
	default:throw;
	}
}

glm::vec3 glExt::camera::getMajorFaceVec(glm::vec3 value)
{
	glm::vec3 _absvec = glm::abs(value);
	unsigned _ind = _absvec.x > _absvec.y ? (_absvec.x > _absvec.z ? 0 : 2) : (_absvec.y > _absvec.z ? 1 : 2);
	glm::vec3 ret{0};
	value_ptr(ret)[_ind] = glm::sign(value[_ind]);
	return ret;
}

float glExt::QuatCamera::_inset_ease(float cur, float start, float change,float duration)
{
	cur /= duration / 2;
	if (cur < 1) return change / 2 * cur * cur + start;
	cur--;
	return -change / 2 * (cur * (cur - 2) - 1) + start;
}

glExt::QuatCamera::QuatCamera(glm::vec3 initialPos, glm::vec3 initialUp, glm::vec3 initialRight) :
	_up(initialUp),
	_right(initialRight),
	_last_quat(glm::angleAxis(0.0f, initialUp)),
	Front(glm::normalize(glm::cross(_up, _right)))
{
	this->_pos = initialPos;
}

glm::quat glExt::QuatCamera::getQuatBetweenVecs(glm::vec3 _from, glm::vec3 _to)
{
	_from = normalize(_from);
	_to = normalize(_to);
	float cosTheta = dot(_from, _to);
	glm::vec3 rotationAxis;
	if (cosTheta < -1 + 0.001f) {
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), _from);
		if (glm::length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
			rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), _from);
		rotationAxis = normalize(rotationAxis);
		return glm::angleAxis(180.0f, rotationAxis);
	}
	rotationAxis = cross(_from, _to);
	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;
	return glm::quat(
		s * 0.5f,
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);
}

void glExt::QuatCamera::setQuatBetweenVecs(glm::vec3 _from, glm::vec3 _to)
{
	_from = normalize(_from);
	_to = normalize(_to);
	glm::vec3 _up(0.0f,1.0f,0.0f);
	float cosTheta = dot(_from, _to);
	float _angle = glm::acos(cosTheta);
	if (cosTheta < -1 + 0.001f) {
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		_up = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), _from);
		if (glm::length2(_up) < 0.01) // bad luck, they were parallel, try again!
			_up = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), _from);
		_up = normalize(_up);
		this->_last_quat =  glm::angleAxis(180.0f, _up);
		return;
	}
	_up = cross(_from, _to);
	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;
	this->_last_quat =  glm::quat(
		s * 0.5f,
		_up.x * invs,
		_up.y * invs,
		_up.z * invs
	);
}

QuatCamera::rotate_raw glExt::QuatCamera::getRotate(glm::vec3 _from, glm::vec3 _to)
{
	rotate_raw ret;
	ret.axis = normalize(cross(_from, _to));
	ret.angle = acos(dot(_from, _to) / (length(_from) * length(_to)));
	return ret;
}

void glExt::QuatCamera::sorround(float xoffset, float yoffset,glm::vec3 dest)
{
	moveVis(xoffset, yoffset);
	this->_pos = -glm::length(_pos) * Front + dest;
}

void QuatCamera::moveVis(float xoffset, float yoffset) {
	yoffset = -yoffset;

	setQuatBetweenVecs(Front, Front + glm::cross(Front,_up) * xoffset * MouseSensitivity + _up * yoffset * MouseSensitivity);
	this->_up = _last_quat * _up;
	_right = _last_quat * _right;
	Front = glm::cross(_up, _right);
}

void QuatCamera::requirePath(path singlePath) {
	if (abs(dot(singlePath.requiredUp, singlePath.requiredRight)) >= errContain) throw;
	_ipath toadd{
		singlePath,
		_up,_right,
		getRotate(_up, singlePath.requiredUp),
		getRotate(_right,singlePath.requiredRight),
		(float)glfwGetTime()
	};
	_path.push(toadd);
}

void glExt::QuatCamera::checkFrame()
{
	if (!_path.empty()) {
		_ipath& tohandle = _path.front();
		float deltatime = (float)glfwGetTime() - tohandle.timeBegin;
		if (deltatime >= tohandle.path.requiredTime) {
			_path.pop();
			if (!_path.empty()) {
				_ipath& next = _path.front();
				next.timeBegin = (float)glfwGetTime();
				next.startUp = _up;
				next.startRight = _right;
				if (next.useAngle) {
					next.path.requiredUp = angleAxis(next.upRaw.angle, next.upRaw.axis) * _up;
					next.path.requiredRight = angleAxis(next.rightRaw.angle, next.rightRaw.axis) * _right;
				}
			}
			return;
		}
		// float timeRatio = deltatime / tohandle.path.requiredTime;
		float upAngle = this->sizer(deltatime,0,tohandle.upRaw.angle,tohandle.path.requiredTime),
			rightAngle = this->sizer(deltatime, 0, tohandle.rightRaw.angle, tohandle.path.requiredTime);
		_up = normalize(angleAxis(upAngle,tohandle.upRaw.axis) * tohandle.startUp);
		_right =normalize(angleAxis(rightAngle, tohandle.rightRaw.axis) * tohandle.startRight);
		Front = normalize(cross(_up, _right));
		if (tohandle.path.useSurround) {
			this->_pos = -glm::length(_pos) * Front;
		}
	}
}

void glExt::QuatCamera::requireRotate(rotate_raw _rotate, float time)
{
	_ipath ipath;
	ipath.path.requiredTime = time;
	ipath.path.useSurround = true;
	glm::quat _quat = angleAxis(_rotate.angle, _rotate.axis);
	ipath.timeBegin = (float)glfwGetTime();
	ipath.startUp = _up;
	ipath.startRight = _right;
	ipath.path.requiredUp = _quat * _up;
	ipath.path.requiredRight = _quat * _right;
	ipath.upRaw = ipath.rightRaw = _rotate;
	ipath.useAngle = true;
	this->_path.push(ipath);
}

picker::picker(program& _pro,window& window):_program(_pro),_window(window) {
	glGenFramebuffers(1, &this->_frameback);
	glBindFramebuffer(GL_FRAMEBUFFER, this->_frameback);
	checkError();

	auto winsize = window.getSize();
	// Create the texture object for the primitive information buffer
	glGenTextures(1, &this->_pickTxt);
	glBindTexture(GL_TEXTURE_2D, this->_pickTxt);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, winsize.x, winsize.y, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->_pickTxt, 0);
	checkError();

	// Create the texture object for the depth buffer
	glGenTextures(1, &this->_depthTxt);
	glBindTexture(GL_TEXTURE_2D, this->_depthTxt);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, winsize.x, winsize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->_depthTxt, 0);
	checkError();

	// Verify that the FBO is correct
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		throw BaseException(-1, "Frame 创建失败");
	}
	checkError();
}
picker::~picker() {
	glDeleteFramebuffers(1, &this->_frameback);

	glDeleteTextures(1, &this->_pickTxt);
	glDeleteTextures(1, &this->_depthTxt);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void picker::enableWrite() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->_frameback);
	checkError();
	this->_program.use();
}

void picker::disableWrite() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	checkError();
}

picker::PixelInfo picker::readIndex(int x, int y) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->_frameback);
	checkError();
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	PixelInfo Pixel;
	glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &Pixel);
	checkError();
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	checkError();
	return Pixel;
}

void window::setFullScreen(bool fullscreen) {
	if (isFullScreen() == fullscreen)
		return;

	if (fullscreen)
	{
		// backup window position and window size
		glfwGetWindowPos(_window, &_fullscr_saved[0].x, &_fullscr_saved[0].y);
		glfwGetWindowSize(_window, &_fullscr_saved[1].x, &_fullscr_saved[1].y);

		// get resolution of monitor
		_monitor = getFitScreen();
		if (_monitor == nullptr) throw;
		const GLFWvidmode* mode = glfwGetVideoMode(_monitor);

		// switch to full screen
		glfwSetWindowMonitor(_window, _monitor, 0, 0, mode->width, mode->height, 0);
		this->pollEvents();
	}
	else
	{
		// restore last window size and position
		glfwSetWindowMonitor(_window, nullptr, _fullscr_saved[0].x, _fullscr_saved[0].y, _fullscr_saved[1].x, _fullscr_saved[1].y, 0);
		_monitor = nullptr;
	}
}

GLFWmonitor* window::getFitScreen() {
	int nmonitors, i;
	int wx, wy, ww, wh;
	int mx, my, mw, mh;
	int overlap, bestoverlap;
	GLFWmonitor* bestmonitor;
	GLFWmonitor** monitors;
	const GLFWvidmode* mode;

	bestoverlap = 0;
	bestmonitor = NULL;

	glfwGetWindowPos(_window, &wx, &wy);
	glfwGetWindowSize(_window, &ww, &wh);
	monitors = glfwGetMonitors(&nmonitors);

	constexpr auto maxi = [](int a, int b) {
		return a > b ? a : b;
	};
	constexpr auto mini = [](int a, int b) {
		return a < b ? a : b;
	};

	for (i = 0; i < nmonitors; i++) {
		mode = glfwGetVideoMode(monitors[i]);
		glfwGetMonitorPos(monitors[i], &mx, &my);
		mw = mode->width;
		mh = mode->height;

		overlap =
			maxi(0, mini(wx + ww, mx + mw) - maxi(wx, mx)) *
			maxi(0, mini(wy + wh, my + mh) - maxi(wy, my));

		if (bestoverlap < overlap) {
			bestoverlap = overlap;
			bestmonitor = monitors[i];
		}
	}

	return bestmonitor;
}


