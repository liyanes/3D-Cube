#pragma once
#include "glext.h"
#include <array>
#include <queue>
#pragma warning(disable:26495)

/// <summary>
/// 简单版本的Cube生成器
/// </summary>
namespace mycube {
	struct rgbColor {
		float x, y, z;
	};
	inline constexpr rgbColor rgb2float(unsigned color) {
		return {
			(float)((color >> 16) & 0xFF) / 255,
			(float)((color >> 8) & 0xFF) / 255,
			(float)((color) & 0xFF) / 255,
		};
	};
	using scale3 = glm::vec3;
	using scale2 = glm::vec2;
	struct cubeVertex {
		scale3 pos,normalVector;
		rgbColor color;
		scale2 textureMap;
	};

	enum axis {
		ax_x,
		ax_y,
		ax_z
	};
	inline glm::vec3 axis2vec3(axis _value,bool positive = true) {
		switch (_value) {
		case ax_x:return glm::vec3(positive ? 1.0f : -1.0f, 0.0f, 0.0f);
		case ax_y:return glm::vec3( 0.0f, positive ? 1.0f : -1.0f, 0.0f);
		case ax_z:return glm::vec3( 0.0f, 0.0f,positive ? 1.0f : -1.0f);
		default: throw;
		}
	}
	
	using face = glExt::camera::face;
	using VecAxis = glExt::camera::axis;
	using faceRotate = glExt::camera::faceRotate;
	constexpr auto oppositeFace = glExt::camera::oppositeFace;
	constexpr auto faceFromVec3 = glExt::camera::faceFromVec3;
	constexpr auto vec3FromFace = glExt::camera::vec3FromFace;
	inline face getRotatedFace(const face from, const faceRotate way, const face faceUp = face::y_positive) {
		return glExt::camera::getRotatedFace(from, way, faceUp);
	}
	inline face getRotatedFace(const glm::vec3 from, const faceRotate way, const glm::vec3 faceUp = glm::vec3(0.0f, 1.0f, 0.0f)) {
		return glExt::camera::getRotatedFace(from, way, faceUp);
	}

	// 背面
	const cubeVertex facePoint_zn[6] = {
		cubeVertex{{-0.5f, -0.5f, -0.5f},{0.0f,  0.0f, -1.0f},rgb2float(0xFF5800),{0.0f, 0.0f}},
		cubeVertex{{0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f},rgb2float(0xFF5800),{1.0f,  0.0f}},
		cubeVertex{{0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f},rgb2float(0xFF5800),{1.0f,  1.0f}},
		cubeVertex{{0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f},rgb2float(0xFF5800),{1.0f,  1.0f}},
		cubeVertex{{-0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f},rgb2float(0xFF5800),{0.0f,  1.0f}},
		cubeVertex{{-0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f},rgb2float(0xFF5800),{0.0f,  0.0f}}
	};
	const cubeVertex facePoint_zp[6] = {
		cubeVertex{{-0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, rgb2float(0xC41E3A) ,{0.0f,  0.0f}},
		cubeVertex{{0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, rgb2float(0xC41E3A), {1.0f,  0.0f}},
		cubeVertex{{0.5f,  0.5f,  0.5f},  {0.0f,  0.0f,  1.0f}, rgb2float(0xC41E3A),{1.0f,  1.0f}},
		cubeVertex{{0.5f,  0.5f,  0.5f},  {0.0f,  0.0f,  1.0f}, rgb2float(0xC41E3A), {1.0f,  1.0f}},
		cubeVertex{{-0.5f,  0.5f,  0.5f},  {0.0f,  0.0f,  1.0f}, rgb2float(0xC41E3A), {0.0f,  1.0f}},
		cubeVertex{{-0.5f, -0.5f,  0.5f},  {0.0f,  0.0f,  1.0f}, rgb2float(0xC41E3A), {0.0f,  0.0f}},
	};
	const cubeVertex facePoint_xn[6] = {
		cubeVertex{{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f},rgb2float(0x0051BA), { 1.0f,  0.0f }},
		cubeVertex{{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, rgb2float(0x0051BA), {1.0f,  1.0f}},
		cubeVertex{{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, rgb2float(0x0051BA), {0.0f,  1.0f}},
		cubeVertex{{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, rgb2float(0x0051BA), {0.0f,  1.0f}},
		cubeVertex{{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, rgb2float(0x0051BA), {0.0f,  0.0f}},
		cubeVertex{{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, rgb2float(0x0051BA), {1.0f,  0.0f}}, 
	};
	const cubeVertex facePoint_xp[6] = {
		cubeVertex{{0.5f,  0.5f,  0.5f},  {1.0f,  0.0f,  0.0f},rgb2float(0x009E60),{ 1.0f,  0.0f }},
		cubeVertex{{0.5f,  0.5f, -0.5f},  {1.0f,  0.0f,  0.0f}, rgb2float(0x009E60), {1.0f,  1.0f}},
		cubeVertex{{0.5f, -0.5f, -0.5f},  {1.0f,  0.0f,  0.0f},rgb2float(0x009E60),   {0.0f,  1.0f}},
		cubeVertex{{0.5f, -0.5f, -0.5f},  {1.0f,  0.0f,  0.0f},rgb2float(0x009E60),   {0.0f,  1.0f}},
		cubeVertex{{0.5f, -0.5f,  0.5f},  {1.0f,  0.0f,  0.0f},rgb2float(0x009E60),   {0.0f,  0.0f} },
		cubeVertex{ {0.5f,  0.5f,  0.5f},  {1.0f,  0.0f,  0.0f},rgb2float(0x009E60),   {1.0f,  0.0f }}, 
	};
	const cubeVertex facePoint_yn[6] = {
		cubeVertex{{-0.5f, -0.5f, -0.5f},  {0.0f, -1.0f,  0.0f},rgb2float(0xFFFFFF), { 0.0f,  1.0f }},
		cubeVertex{{0.5f, -0.5f, -0.5f},  {0.0f, -1.0f,  0.0f},rgb2float(0xFFFFFF),  {1.0f,  1.0f}},
		cubeVertex{{0.5f, -0.5f,  0.5f},  {0.0f, -1.0f,  0.0f},rgb2float(0xFFFFFF),  {1.0f,  0.0f} },
		cubeVertex{{0.5f, -0.5f,  0.5f},  {0.0f, -1.0f,  0.0f},rgb2float(0xFFFFFF),  {1.0f,  0.0f }},
		cubeVertex{{-0.5f, -0.5f,  0.5f},  {0.0f, -1.0f,  0.0f},rgb2float(0xFFFFFF),  {0.0f,  0.0f }},
		cubeVertex{{-0.5f, -0.5f, -0.5f},  {0.0f, -1.0f,  0.0f},rgb2float(0xFFFFFF),  {0.0f,  1.0f }}, 
	};
	const cubeVertex facePoint_yp[6] = {
		cubeVertex{{-0.5f,  0.5f, -0.5f},  {0.0f,  1.0f,  0.0f},rgb2float(0xFFD500), { 0.0f,  1.0f }},
		cubeVertex{{0.5f,  0.5f, -0.5f},  {0.0f,  1.0f,  0.0f},rgb2float(0xFFD500),  {1.0f,  1.0f}},
		cubeVertex{{0.5f,  0.5f,  0.5f},  {0.0f,  1.0f,  0.0f},rgb2float(0xFFD500),  {1.0f,  0.0f}},
		cubeVertex{{0.5f,  0.5f,  0.5f},  {0.0f,  1.0f,  0.0f},rgb2float(0xFFD500),  {1.0f,  0.0f}},
		cubeVertex{{-0.5f,  0.5f,  0.5f},  {0.0f,  1.0f,  0.0f},rgb2float(0xFFD500),  {0.0f,  0.0f}},
		cubeVertex{{-0.5f,  0.5f, -0.5f},  {0.0f,  1.0f,  0.0f},rgb2float(0xFFD500),  {0.0f,  1.0f}}, 
	};

	struct singleCube;

	struct rotate_vec {
		axis axis;
		bool positive;
		inline rotate_vec(mycube::face face);
		inline operator mycube::face();
	};
	namespace cubeSolver {
		class solver;
	}

	struct singleCubeFace {
		std::array<const cubeVertex*, 6> points;
		singleCube* inSingleCube;
		mycube::face face, nowFace;
		// 是否为展示出来的面
		bool outShow = false;
		// 旋转 ,右手定则
		// positive 表示是否正轴
		void rotate(axis _axis, bool positive, unsigned times = 1);
		void rotate(mycube::face _axis, unsigned times = 1);
		inline constexpr singleCubeFace(const cubeVertex* vertexs, mycube::face face, singleCube* inSingleCube):
			outShow(false),inSingleCube(inSingleCube) {
			for (int i = 0; i < 6; i++) {
				points[i] = &vertexs[i];
			}
			this->nowFace = this->face = face;
		}
	};

	class Cube;

	using cubeIndex = glm::vec<3, unsigned>;

	struct singleCube {
	public:
		Cube* inCube;
		size_t index;
		cubeIndex cubePos;

		std::array<singleCubeFace, 6> faces{
			singleCubeFace(facePoint_xn,face::x_negative,this),
			singleCubeFace(facePoint_xp,face::x_positive,this),
			singleCubeFace(facePoint_yn,face::y_negative,this),
			singleCubeFace(facePoint_yp,face::y_positive,this),
			singleCubeFace(facePoint_zn,face::z_negative,this),
			singleCubeFace(facePoint_zp,face::z_positive,this),
		};
		glm::mat4 initTranslate = glm::mat4(1.0f);
		glm::mat4 rotateMat = glm::mat4(1.0f);
		bool useTimelyRotateMat = false;
		glm::mat4 timelyRotateMat;

		void rotate(axis _axis, bool positive, unsigned times = 1);
		// void rotate(face _axis, unsigned times = 1);
		glExt::arrayData<cubeVertex> generateData() const {
			glExt::arrayData<cubeVertex> _data;
			_data.reserve(36);
			for (auto i = faces.cbegin(); i != faces.cend(); i++) {
				for (auto j = i->points.cbegin(); j != i->points.cend(); j++) {
					_data.push_back(**j);
				}
			}
			return _data;
		}
		void timelyRotate(axis _axis, bool positive, float radians);

		inline singleCubeFace& findSrcFace(face face) {
			for (auto i = this->faces.begin(); i != this->faces.end(); i++) {
				if (i->face == face) return *i;
			}
			throw;
		}

		inline singleCubeFace& findFace(face face) {
			for (auto i = this->faces.begin(); i != this->faces.end(); i++) {
				if (i->nowFace == face) return *i;
			}
			throw;
		}
		glExt::arrayData<cubeVertex> getPointData();
		glExt::arrayData<cubeVertex> getPointData(face face);
	};

	class Cube {
	public:
		struct animateFrame {
			float rate = 1.0f;
			float requiredRadians = glm::radians(90.0f);
			axis _axis;
			bool positive;
			int depth;
		};
	private:
		unsigned level;
		float size;
		std::vector<singleCube*> scubes;
		glExt::arrayData<cubeVertex> _sdata[6];
		glExt::vertexArray _varray[6];
		// 按照后正左右下上排序
		bool useTexture = false;
		std::array<glExt::texture_2D*, 6> textures;
		glExt::program& _program;

		inline unsigned _mapIndex(face face) {
			switch (face) {
			case face::x_negative:return 0;
			case face::x_positive:return 1;
			case face::y_negative:return 2;
			case face::y_positive:return 3;
			case face::z_negative:return 4;
			case face::z_positive:return 5;
			default:throw;
			}
		}
		inline face _mapIndex(unsigned face) {
			switch (face) {
			case 0:return face::x_negative;
			case 1:return face::x_positive;
			case 2:return face::y_negative;
			case 3:return face::y_positive;
			case 4:return face::z_negative;
			case 5:return face::z_positive;
			default:throw;
			}
		}

		struct _animateVec {
			animateFrame data;
			bool firstFrame = true;
			float lastFrame = 0.0f;
		};

		std::queue<_animateVec> _frame;
		static float _inset_ease(float cur, float start, float change, float duration);
	public:
		// frame开始是准备的函数, 为true则执行该帧
		bool (*frameCallback)(animateFrame&) = nullptr;

		glExt::easeFunc<float> sizer = _inset_ease;
		glm::mat4 worldModel = glm::mat4(1.0f);
		
		inline unsigned getLevel() const { return level; };

		inline singleCube& getSrcCube(unsigned x, unsigned y, unsigned z) {
			return *scubes[(size_t)x * level * level + (size_t)y * level + z];
		};

		inline singleCube& getCube(unsigned x, unsigned y, unsigned z) {
			for (auto i = scubes.begin(); i != scubes.end(); i++) {
				if ((*i)->cubePos == cubeIndex{ x, y, z }) {
					return **i;
				}
			}
			throw;
		}
		inline singleCube& getCube(cubeIndex ind) {
			for (auto i = scubes.begin(); i != scubes.end(); i++) {
				if ((*i)->cubePos == ind) {
					return **i;
				}
			}
			throw;
		}

		inline cubeIndex getCubeIndex(unsigned index) {
			return { index / (level * level),index / level % level,index % level };
		};

		inline glExt::program& getProgram() {
			return this->_program;
		};

		// 注意,直接使用该函数将得到外表面的 Face
		std::vector<singleCubeFace*> findSrcFaces(face face);

		// 注意,直接使用该函数将得到外表面的 Face
		std::vector<singleCubeFace*> findFaces(face face);

		Cube(unsigned level,float size,glExt::program& useProgram);

		void paint();

		void paint(std::array<glExt::texture*,6>& _text);

		using paintEachFunc = void(*)(glExt::program& _pro,std::vector<singleCube*>::const_iterator iter,unsigned faceIndex, glExt::vertexArray[6]);
		void paintEach(paintEachFunc func);
		void paintEach(paintEachFunc func, bool useTexture);

		// axis轴 从0->level-1找depth
		std::vector<singleCube*> getCubes(axis _axis,unsigned depth);

		void rotate(axis _axis, bool positive, unsigned depth);
		// 下面的函数旋转时将视depth沿_axis轴递增
		inline void rotate(VecAxis _axis, unsigned depth);

		void timeRotate(axis _axis, bool positive, int depth, float radians);
		void timeRotateEnd(axis _axis,bool positive, int depth);

		void frameRequire(axis _axis, bool positive, int depth, float rate = 1.0f,float requiredRadians = glm::radians(90.0f));
		inline void frameRequire(VecAxis _axis, int depth, float rate = 1.0f,float requiredRadians = glm::radians(90.0f));
		void frameCheck();
		void frameClear();

		friend struct singleCube;

		inline static glExt::camera::face Face(axis _axis, bool positive);

		inline unsigned mapDepth(VecAxis axis, unsigned depth);

		inline ~Cube();

		friend class mycube::cubeSolver::solver;
	};
}

namespace mycube::cubeSolver {
	/// <summary>
	/// 魔方整面
	/// </summary>
	class faceMap : private std::vector<std::vector<mycube::singleCubeFace*>> {
		mycube::Cube& _cube;
		glExt::camera::face _face;
		glExt::camera::face _faceUp;
		unsigned level;
	public:
		/// <summary>
		/// 根据视角左下到右上,第一维横排布
		/// </summary>
		/// <param name="cube">魔方</param>
		/// <param name="_face">处理面</param>
		/// <param name="faceUp">处理面上面</param>
		faceMap(mycube::Cube& cube,glExt::camera::face _face, glExt::camera::face faceUp);

		/// <summary>
		/// 使用 [] 的形式,以视角左下角为起点,第一维为横轴,计算单立方体的面
		/// </summary>
		using std::vector<std::vector<mycube::singleCubeFace*>>::operator[];

		faceMap getNear(face faceUp, faceRotate rotate);
		const faceMap getNear(face faceUp, faceRotate rotate) const;

		/// <summary>
		/// 对当前面进行旋转视图判断(以视角为相对轴,魔方进行旋转操作为准)
		/// </summary>
		/// <param name="anticlock">是否为逆时针</param>
		void ratate(bool anticlock);

		inline unsigned getLevel() const { return level; };

		inline face getFace() const { return _face; }
		inline face getUpFace() const { return _face; }

		// 根据内存地址寻找对应 face , 没有则返回 (unsigned)-1
		glExt::crood<unsigned> findSingleCubeFace(const singleCubeFace& face);
	};

	/// <summary>
	/// 视角轴附着角
	/// </summary>
	const std::array<glm::vec3,26> axisWays{
		// 面心
		glm::vec3{1.0f,0.0f,0.0f},
		glm::vec3{-1.0f,0.0f,0.0f},
		glm::vec3{0.0f,1.0f,0.0f},
		glm::vec3{0.0f,-1.0f,0.0f},
		glm::vec3{0.0f,0.0f,1.0f},
		glm::vec3{0.0f,0.0f,-1.0f},
		// 棱心
		glm::normalize(glm::vec3{1.0f,1.0f,0.0f}),
		glm::normalize(glm::vec3{1.0f,-1.0f,0.0f}),
		glm::normalize(glm::vec3{-1.0f,1.0f,0.0f}),
		glm::normalize(glm::vec3{-1.0f,-1.0f,0.0f}),
		glm::normalize(glm::vec3{0.0f,1.0f,1.0f}),
		glm::normalize(glm::vec3{0.0f,1.0f,-1.0f}),
		glm::normalize(glm::vec3{0.0f,-1.0f,1.0f}),
		glm::normalize(glm::vec3{0.0f,-1.0f,-1.0f}),
		glm::normalize(glm::vec3{1.0f,0.0f,1.0f}),
		glm::normalize(glm::vec3{1.0f,0.0f,-1.0f}),
		glm::normalize(glm::vec3{-1.0f,0.0f,1.0f}),
		glm::normalize(glm::vec3{-1.0f,0.0f,-1.0f}),
		// 顶点
		glm::normalize(glm::vec3{1.0f,1.0f,1.0f}),
		glm::normalize(glm::vec3{1.0f,1.0f,-1.0f}),
		glm::normalize(glm::vec3{1.0f,-1.0f,1.0f}),
		glm::normalize(glm::vec3{1.0f,-1.0f,-1.0f}),
		glm::normalize(glm::vec3{-1.0f,1.0f,1.0f}),
		glm::normalize(glm::vec3{-1.0f,1.0f,-1.0f}),
		glm::normalize(glm::vec3{-1.0f,-1.0f,1.0f}),
		glm::normalize(glm::vec3{-1.0f,-1.0f,-1.0f}),
	};

	/// <summary>
	/// 获取最近的视角附着轴
	/// </summary>
	/// <param name="relativePos">相机的前向量</param>
	/// <returns>最近的附着轴</returns>
	glm::vec3 getNearByAxisWay(glm::vec3 relativePos);

	inline bool isNegative(face _axis);

	enum class placeholder {
		undefined = -1,
		_0 = 0,
		_1,_2,_3,_4,_5,_6
	};

	struct colorPlaceholder {
	private:
		// 以 placeHolder 为索引, face 为值的列表
		std::array<int, 6> holders;
	public:
		inline colorPlaceholder();

		inline face operator[](placeholder holder);
		inline bool operator==(const colorPlaceholder&) const;
		// 设置 placeHolder 对应的颜色值
		inline void set(placeholder holder, face color);
		// 比较 placeHolder 对应的颜色值
		inline bool match(placeholder holder, face color);
		// 测试 placeHolder　对应的颜色值, 如果颜色未设置,则设置并返回 true
		inline bool test(placeholder holder, face color);
	};

	class solutionFaceMap;
	struct facePlaceHolder {
		colorPlaceholder cholder;

		// 匹配面上的颜色, 第一个参数是 faceMap,第二个参数是面上每个值对于的 placeHolder
		// 该操作不会尝试旋转面,故而只匹配一个方向
		// 匹配成功将返回 true ,反则未 false
		bool match(const faceMap&,const std::vector<std::vector<placeholder>*>&);

		// 匹配所有面上的颜色, 第一的参数是 faceMap,第二个参数是面上每个值对于的 placeHolder
		// 
		// 匹配成功将返回faceMap,未成功则 throw
		faceMap matchAll(const faceMap&, const std::vector<std::vector<placeholder>*>&);

		// 匹配面上的颜色, 第一个参数是 faceMap,第二个参数是面上每个值对于的 placeHolder
		// 
		// 匹配成功将返回 true ,且修改 _matchMap 为对应的值,反则未 false
		bool matchAll(const faceMap&, const std::vector<std::vector<placeholder>*>&, _Out_writes_bytes_all_(sizeof(faceMap)) faceMap* _matchMap);
	};

	class solutionFaceLine;
	class solutionFaceMap {
	public:
		struct singleFaceData {
			face srcFace, nowFace;
			inline bool operator==(const singleFaceData& _right);
			inline bool operator!=(const singleFaceData& _right);
		};
	private:
		// 左下为起点,第一维是横轴
		singleFaceData* _data;
	public:
		const unsigned level;
		solutionFaceMap(unsigned level);
		solutionFaceMap(const faceMap&);
		~solutionFaceMap();

		inline singleFaceData* data();

		// 旋转单次 : 顺/逆时针
		void rotateSingle(bool clockwise);
		solutionFaceMap getRotateSingle(bool clockwise);
		// 旋转到反向面
		void rotateBack();
		solutionFaceMap getRotateBack();
		// 旋转 指定时针 次数
		void rotate(unsigned times, bool clockwise);

		inline solutionFaceLine operator[](unsigned);
		inline const solutionFaceLine operator[](unsigned) const;
		inline solutionFaceLine getLine(unsigned,bool);

		friend class solutionFaceLine;
	};

	class solutionFaceLine {
		solutionFaceMap& _inmap;
		unsigned _inwline;
		bool vertical;
	protected:
	struct _iter_end_flag{unsigned flag;};
	public:
		inline solutionFaceLine(solutionFaceMap&, unsigned);
		inline solutionFaceLine(solutionFaceMap&, unsigned,bool);

		class iterator {
			solutionFaceLine& _in;
			unsigned cur;
		public:
			inline iterator(solutionFaceLine& _in);
			inline operator solutionFaceMap::singleFaceData& ();
			inline solutionFaceMap::singleFaceData& operator*();
			inline const iterator& operator++(int);
			inline iterator& operator++();
			inline const iterator& operator--(int);
			inline iterator& operator--();
			inline bool operator==(const iterator& _right) const;
			inline bool operator==(const _iter_end_flag& _right) const;
			inline unsigned getCurrentIndex() const;
		};

		inline iterator begin();
		inline _iter_end_flag end();

		inline solutionFaceMap::singleFaceData& operator[](unsigned index);
		//inline const solutionFaceMap::singleFaceData& operator[](unsigned index) const;
		inline bool operator==(solutionFaceLine& _right);
	};



	class solution {

		// 按照x+(y+),x-(y-),y+(z+),y-(z-),z+(x+),z-(x-)排序
		solutionFaceMap _maps[6];
		inline static unsigned _mapIndex(glExt::camera::face);
		inline static glExt::camera::face _mapIndex(unsigned);
		// 内置的初始化函数,需要初始化后修改变量
		inline solution(unsigned level);
	public:
		const unsigned level;
		solution(Cube&);

		solution getRotated(glExt::camera::faceRotate rotate);
	};

	class solver {
		Cube& _cube;
		glExt::QuatCamera& _quatCam;
		face curWorkFace;
		face curWorkUpFace;
		// 由 releativeAxis 到实际轴的映射
		// 注意一点, front 指的是 向魔方视角方向的那一个面
		std::array<VecAxis, 6> _workmap;

		void updateAxisMap();
		// 默认着色器
		glExt::program* _program = nullptr;
		glExt::picker* _picker = nullptr;
		glExt::program* _pickpro = nullptr;
	public:
		solver(Cube& cube, glExt::QuatCamera& quatCam);
		
		inline faceMap getWorkFace();
		inline void changeWorkFace(const faceMap& face);
		inline void changeWorkFace(const face cur, const face up);
		inline void changeWorkFace();

		inline glm::vec3 getNearAxis();
		
		using relativeAxis = glExt::QuatCamera::relativeFace;

		inline face getRotatedFace(faceRotate way);
		inline void rotateToFace(faceRotate way,float time);

		/// <summary>
		/// 在当前工作面进行魔方旋转操作
		/// </summary>
		/// <param name="_axis">旋转轴</param>
		/// <param name="depth">旋转阶数,从向量方向开始递增</param>
		void rotate(relativeAxis _axis, unsigned depth);

		/// @briefs 随机打乱魔方
		void distribute(unsigned stepnum,float rate = 1.0f);

		inline void frameRequire(relativeAxis _axis,unsigned depth, float rate = 1.0f);

		void initPickMode(glExt::window& window);
		inline void finializePickMode();
		const singleCubeFace* getCurrentPickFace(glExt::window& window,int x,int y, const glm::mat4 stdmat) const;
		// 选择从一面到另一面的方向
		// 成功返回 true, 否则为 false
		bool rotateByFace(singleCubeFace& from, singleCubeFace& to,float rate=(1.0f));

		inline ~solver();
	};
};


inline glExt::camera::face mycube::Cube::Face(axis _axis, bool positive) {
	switch (_axis) {
	case axis::ax_x:return positive ? glExt::camera::face::x_positive : glExt::camera::face::x_negative;
	case axis::ax_y:return positive ? glExt::camera::face::y_positive : glExt::camera::face::y_negative;
	case axis::ax_z:return positive ? glExt::camera::face::z_positive : glExt::camera::face::z_negative;
	default:throw;
	}
}

inline mycube::Cube::~Cube()
{
	for (auto i = this->scubes.begin(); i != this->scubes.end(); i++) {
		delete *i;
		*i = nullptr;
	}
}

inline mycube::cubeSolver::faceMap mycube::cubeSolver::solver::getWorkFace() {
	return faceMap(_cube, curWorkFace, curWorkUpFace);
}

inline void mycube::cubeSolver::solver::changeWorkFace(const faceMap& face) {
	this->curWorkFace = face.getFace();
	this->curWorkUpFace = face.getUpFace();
};

inline void mycube::cubeSolver::solver::changeWorkFace(const face cur, const face up) {
	this->curWorkFace = cur;
	this->curWorkUpFace = up;
}

inline glm::vec3 mycube::cubeSolver::solver::getNearAxis() {
	return getNearByAxisWay(this->_quatCam.getFront());
}

inline mycube::rotate_vec::operator mycube::face() {
	switch (this->axis) {
	case ax_x:return positive ? face::x_positive : face::x_negative;
	case ax_y:return positive ? face::y_positive : face::y_negative;
	case ax_z:return positive ? face::z_positive : face::z_negative;
	default:throw;
	}
}

inline mycube::rotate_vec::rotate_vec(mycube::face face) {
	switch (face) {
	case face::x_positive:
	case face::x_negative:
		this->axis = ax_x;
		break;
	case face::y_positive:
	case face::y_negative:
		this->axis = ax_y;
		break;
	case face::z_positive:
	case face::z_negative:
		this->axis = ax_z;
		break;
	default:throw;
	}
	switch (face) {
	case face::x_positive:
	case face::y_positive:
	case face::z_positive:
		this->positive = true;
		break;
	default:
		this->positive = false;
	}
}

inline void mycube::Cube::rotate(VecAxis _axis, unsigned depth) {
	rotate_vec ret = _axis;
	this->rotate(ret.axis, ret.positive, mycube::cubeSolver::isNegative(_axis) ? level - 1 - depth : depth);
}

inline void mycube::Cube::frameRequire(VecAxis _axis, int depth, float rate, float requiredRadians) {
	rotate_vec ret = _axis;
	this->frameRequire(ret.axis, ret.positive, mycube::cubeSolver::isNegative(_axis) ? level - 1 - depth : depth, rate, requiredRadians);
}

inline bool mycube::cubeSolver::isNegative(face _axis) {
	return (_axis == face::x_negative || _axis == face::y_negative || _axis == face::z_negative);
}

inline unsigned mycube::Cube::mapDepth(VecAxis axis, unsigned depth) {
	if (mycube::cubeSolver::isNegative(axis)) return level - depth - 1;
	else return depth;
}

//inline mycube::rotate_vec::operator mycube::face()
//{
//	switch (this->axis) {
//	case axis::ax_x:return this->positive ? face::x_positive : face::x_negative;
//	case axis::ax_y:return this->positive ? face::y_positive : face::y_negative;
//	case axis::ax_z:return this->positive ? face::z_positive : face::z_negative;
//	default:throw;
//	}
//}

inline void mycube::cubeSolver::solver::frameRequire(relativeAxis _axis, unsigned depth, float rate ) {
	updateAxisMap();
	rotate_vec ret = this->_workmap[int(_axis)];
	return this->_cube.frameRequire(ret.axis, ret.positive, ret.positive ? depth : _cube.getLevel() - 1 - depth, rate);
}

inline void mycube::cubeSolver::solver::changeWorkFace() {
	this->curWorkFace = oppositeFace(this->_quatCam.getMajorFace());
	this->curWorkUpFace = this->_quatCam.getMajorUpFace();
}

inline mycube::cubeSolver::solver::~solver() {
	if (this->_program) delete this->_program;
	if (this->_picker) delete this->_picker;
	if (this->_pickpro) delete this->_pickpro;
}

inline void mycube::cubeSolver::solver::finializePickMode() {
	if (this->_picker) delete this->_picker;
	if (this->_pickpro) delete this->_pickpro;
}

inline mycube::face mycube::cubeSolver::solver::getRotatedFace(faceRotate way) {
	return glExt::camera::getRotatedFace(this->curWorkFace, way, this->curWorkUpFace);
}

inline void mycube::cubeSolver::solver::rotateToFace(faceRotate way,float time) {
	this->_quatCam.requireRotate({ glm::normalize(glm::cross(vec3FromFace(this->curWorkFace),vec3FromFace(glExt::camera::getRotatedFace(this->curWorkFace,way,this->curWorkUpFace)))),glm::radians(90.0f)}, time);
}

inline mycube::cubeSolver::colorPlaceholder::colorPlaceholder() {
	this->holders.fill(-1);
}


inline mycube::face mycube::cubeSolver::colorPlaceholder::operator[](placeholder holder) {
	if (holder == placeholder::undefined) return face(-1);
	return face(this->holders[int(holder)]);
}

inline bool mycube::cubeSolver::colorPlaceholder::match(placeholder holder, face color) {
	if (holder == placeholder::undefined) return true;
	return this->holders[int(holder)] == int(color);
}

inline void mycube::cubeSolver::colorPlaceholder::set(placeholder holder, face color) {
	if (holder == placeholder::undefined) throw;
	this->holders[int(holder)] = int(color);
}

inline bool mycube::cubeSolver::colorPlaceholder::operator==(const colorPlaceholder& _right) const {
	return this->holders == _right.holders;
}

inline bool mycube::cubeSolver::colorPlaceholder::test(placeholder holder, face color) {
	if (holder == placeholder::undefined) return true;
	if (this->holders[int(holder)] == int(color)) return true;
	if (this->holders[int(holder)] == -1) {
		this->holders[int(holder)] = int(color);
		return true;
	}
	return false;
}

namespace mycube::cubeSolver {
	inline solutionFaceLine::solutionFaceLine(solutionFaceMap& _from, unsigned _inwline) :
		_inmap(_from), _inwline(_inwline),vertical(false) {}
	inline solutionFaceLine::solutionFaceLine(solutionFaceMap& _from, unsigned _inwline,bool ver) :
		_inmap(_from), _inwline(_inwline),vertical(ver) {}

	inline solutionFaceLine::iterator::iterator(solutionFaceLine& _from) : 
		_in(_from), cur(0) {};
	inline solutionFaceLine::iterator::operator solutionFaceMap::singleFaceData& (){
		return this->_in.vertical ? this->_in._inmap._data[this->_in._inwline * this->_in._inmap.level + this->cur] : 
			this->_in._inmap._data[this->cur * this->_in._inmap.level + this->_in._inwline];
	};
	inline solutionFaceMap::singleFaceData& solutionFaceLine::iterator::operator*(){
		return (solutionFaceMap::singleFaceData&)(*this);
	}
	inline const solutionFaceLine::iterator& solutionFaceLine::iterator::operator++(int){
		if (this->cur == this->_in._inmap.level) throw;
		this->cur++;
		return (*this);
	}
	inline solutionFaceLine::iterator& solutionFaceLine::iterator::operator++() {
		if (this->cur == this->_in._inmap.level) throw;
		this->cur++;
		return (*this);
	}
	inline const solutionFaceLine::iterator& solutionFaceLine::iterator::operator--(int){
		if (this->cur == 0) throw;
		this->cur--;
		return (*this);
	}
	inline solutionFaceLine::iterator& solutionFaceLine::iterator::operator--() {
		if (this->cur == 0) throw;
		this->cur--;
		return (*this);
	}
	inline bool solutionFaceLine::iterator::operator==(const iterator& _right) const{
		return (&this->_in == &_right._in && this->cur == _right.cur);
	}
	inline bool solutionFaceLine::iterator::operator==(const _iter_end_flag& _right) const{
		return this->cur == _right.flag;
	}
	inline solutionFaceLine::iterator solutionFaceLine::begin(){
		return iterator(*this);
	}
	inline solutionFaceLine::_iter_end_flag solutionFaceLine::end(){
		return {this->_inmap.level};
	}

	inline solutionFaceMap::singleFaceData& solutionFaceLine::operator[](unsigned index){
		return this->vertical ? this->_inmap._data[this->_inwline * this->_inmap.level + index] : 
			this->_inmap._data[index * this->_inmap.level + this->_inwline];
	}
	inline bool solutionFaceLine::operator==(solutionFaceLine& _right){
		for (auto i = this->begin();i!=this->end();i++){
			if (*i != _right[i.getCurrentIndex()]) return false; 
		}
		return true;
	}
	inline bool solutionFaceMap::singleFaceData::operator==(const singleFaceData& _right){
		return this->nowFace == _right.nowFace && this->srcFace == _right.srcFace;
	}
	inline bool solutionFaceMap::singleFaceData::operator!=(const singleFaceData& _right){
		return !(*this == _right);
	}
	inline solutionFaceMap::solutionFaceMap(unsigned level):
		_data(new singleFaceData[level*level]),level(level)
		{}
	inline solutionFaceMap::~solutionFaceMap(){delete[] _data;}
	inline solutionFaceMap::singleFaceData* solutionFaceMap::data(){return this->_data;}
	inline solutionFaceLine solutionFaceMap::operator[](unsigned _index){
		return solutionFaceLine(*this,_index);
	}
	inline const solutionFaceLine solutionFaceMap::operator[](unsigned _index) const{
		return this->operator[](_index);
	}
	inline solutionFaceLine solutionFaceMap::getLine(unsigned _index,bool _vertical){
		return solutionFaceLine(*this,_index,_vertical);
	}
	inline unsigned solution::_mapIndex(glExt::camera::face _face){
		return (unsigned)_face;
	}
	inline glExt::camera::face solution::_mapIndex(unsigned _face){
		return (glExt::camera::face)_face;
	}
	inline solution::solution(unsigned level):
		_maps{solutionFaceMap(level),
		solutionFaceMap(level),
		solutionFaceMap(level),
		solutionFaceMap(level),
		solutionFaceMap(level),
		solutionFaceMap(level)},level(level){}
	
}

#pragma warning(default:26495)
