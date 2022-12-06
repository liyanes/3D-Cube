#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <stb_image.h> 
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <functional>
#include <queue>
#include <type_traits>
#include "err.h"
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#pragma warning(disable:26495)

namespace glExt {
	template <typename T>
	inline T* max(T* start,T* end);
	template <typename T>
	inline const T* max(const T* start, const T* end);

	inline unsigned emptyComp(glm::vec3 value) noexcept;
	inline bool isEmpty(glm::vec3 value) noexcept;

	class window;
	constexpr float errContain = 0.00001f;

	inline void checkError();
	template<typename _retT>
	inline _retT checkError(_retT toret);
	template<typename _retT>
	inline _retT& checkError(_retT& toret);

	template <typename _T>
	struct crood {
		_T x, y;
		inline constexpr crood();
		inline constexpr crood(const crood&);
		inline constexpr crood(const crood&&);
		inline constexpr crood(const _T, const _T);
		inline constexpr bool operator==(const crood<_T>& _right) const noexcept;
		inline constexpr const crood<_T> operator - () const noexcept;
		inline constexpr const crood<_T> operator-(const crood<_T>& _right) const noexcept;
		inline constexpr const crood<_T> operator+(const crood<_T>& _right) const noexcept;
		
		inline constexpr const std::enable_if_t<std::is_arithmetic_v<_T>,_T> length() const;
		inline constexpr const std::enable_if_t<std::is_arithmetic_v<_T>, _T> length2() const;
	};

	using loopFunc = void (*)(window&);
	class window {
	public:
		using mouseCallback = void(*)(window&, int button, int action, int mode);
		using keyCallback = void(*)(window&, int key, int scancode, int action, int mods);
		using cursorCallback = void(*)(window&, double xpos, double ypos);
		using scrollCallback = void(*)(window&, double xoffset, double yoffset);
		using frameBufferSizeCallback = void(*)(window& window, int width, int height);
	private:
		struct _windowsEvent {
			window* window;
			mouseCallback mouse;
			keyCallback key;
			cursorCallback cursor;
			scrollCallback scroll;
			frameBufferSizeCallback frameBufferSize;
		};
		static std::vector<_windowsEvent> _windows;
		GLFWwindow* _window;
		// 全屏时监视器
		GLFWmonitor* _monitor = nullptr;
		// 保存的尺寸
		crood<int> _fullscr_saved[2];

		static void _handle_mouseCallback(GLFWwindow* window, int button, int action, int mode);
		static void _handle_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void _handle_cursorCallback(GLFWwindow* window, double xpos, double ypos);
		static void _handle_scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		static void _handle_frameBufferSizeCallback(GLFWwindow* window, int width, int height);

		GLFWmonitor* getFitScreen();
	public:

		window(int width, int height, const char* title, bool setCurrent = true, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);

		inline void makeContextCurrent() { glfwMakeContextCurrent(_window); };
		inline void setFramebufferSizeCallback(GLFWframebuffersizefun func) { glfwSetFramebufferSizeCallback(_window, func); };

		inline bool noRunning() { return glfwWindowShouldClose(_window); }
		inline bool running() { return !glfwWindowShouldClose(_window); }
		inline void setNoRunning(bool value) { glfwSetWindowShouldClose(_window, value); };
		inline void loop(loopFunc loopfunc) { while (!noRunning())loopfunc(*this); };
		/// <summary>
		/// 获取按键状态
		/// </summary>
		/// <param name="key">指定按键 GLFW_KEY_ 前缀</param>
		/// <returns>0 释放 1 按下 2 重复</returns>
		inline int getKey(int key) { return glfwGetKey(_window, key); };

		inline void swapBuffer() { glfwSwapBuffers(_window); };
		inline static void pollEvents() { glfwPollEvents(); };
		inline void handleSysEvent() { glfwSwapBuffers(_window); pollEvents(); };

		inline crood<int> getSize() const;

		void fitViewport();

		void setMouseCallback(mouseCallback func);
		void setKeyCallback(keyCallback func);
		void setCursorCallback(cursorCallback func);
		void setScrollCallback(scrollCallback func);
		void setFramebufferSizeCallback(frameBufferSizeCallback func);

		inline crood<double> getCursorPos() const;

		inline GLFWwindow* glCode() { return _window; };

		inline bool isFullScreen();
		void setFullScreen(bool fullscreen);

		~window();
	};

	struct version {
		unsigned char major;
		unsigned char minor;
	};
	void initialize(version version = {3,3});
	void finalize();
	inline void enableDepthTest() { glEnable(GL_DEPTH_TEST); checkError(); };
	inline void depthTest() { glClear(GL_DEPTH_BUFFER_BIT); checkError(); };


	class initializer {
	public:
		inline initializer(version version = {3,3});
		inline ~initializer();
	};

	constexpr unsigned getTypeLen(GLenum type);
	inline void clearColor(float red, float green, float blue, float alpha);
	inline void clearColor(float* color);

	namespace textureUnit {
		inline constexpr unsigned unitLen() { return 16; };
		void active(unsigned index);
	};

	class image {
	protected:
		void* _data;
		int width, height, nrChannels;
		bool useFree = true;
	public:
		inline image(const char* filename);
		inline image(const char* filename, bool flip);
		inline image(const image& _image);
		inline image(image&& _image) noexcept;

		inline const image& operator=(const image& _image);
		inline const image& operator=(image&& _image) noexcept;

		inline const void* getData() const noexcept { return this->_data; };
		inline void* getData() noexcept { return this->_data; };

		inline int getWidth() const noexcept { return width; };
		inline int getHeight() const noexcept  { return height; };
		inline int getNrChannel() const noexcept { return nrChannels; };

		inline ~image() { if (useFree) { stbi_image_free(_data); this->useFree = false; } }
	};

	/// <summary>
	/// 着色器
	/// </summary>
	class shader {
		bool compiled;
		GLuint shaderCode;
	public:
		shader(GLenum type);
		shader(GLenum type, const char* source);
		shader(GLenum type, const char** const source, unsigned len);

		void compile(const char* source);
		void compile(const char** const source,unsigned len);

		inline GLenum glCode() const noexcept { return shaderCode; };

		inline ~shader() { glDeleteShader(shaderCode); checkError();};
	};

	template <unsigned _enum>
	concept is_shader = _enum == GL_VERTEX_SHADER || _enum == GL_FRAGMENT_SHADER;

	/// <summary>
	/// 着色器子类模板
	/// </summary>
	/// <typeparam name="_enum">着色器类型</typeparam>
	template<GLenum _enum>
	requires is_shader<_enum>
	class extShader:public shader {
	public:
		extShader() :shader(_enum) {};
		extShader(const char* source) : shader(_enum, source) {};
		extShader(const char** const source, unsigned len) : shader(_enum, source, len) {};
	};

	/// <summary>
	/// 顶点着色器
	/// </summary>
	using vertexShader = extShader<GL_VERTEX_SHADER>;
	/// <summary>
	/// 片段着色器
	/// </summary>
	using fragmentShader = extShader<GL_FRAGMENT_SHADER>;

	/// <summary>
	/// 链接器程序
	/// </summary>
	class program {
		GLuint programCode;
		bool linked;
	public:
		inline program():programCode(glCreateProgram()),linked(false) {};
		inline program(const vertexShader& _vertex, const fragmentShader& _frag) :
			programCode(glCreateProgram()), linked(false) {
			checkError();
			attach(_vertex);
			attach(_frag);
			link();
		}
		inline bool isLinked() const noexcept { return this->linked; }
		
		void attach(const shader& _shader);

		void link();

		inline void use() { glUseProgram(programCode); checkError();};

		inline int getUniformPos(const char* name) {
			return checkError(glGetUniformLocation(programCode, name));
		}

		inline static void uniform(int pos, unsigned val) { glUniform1ui(pos, val); checkError(); }
		inline static void uniform(int pos, unsigned v0, unsigned v1) { glUniform2ui(pos, v0, v1); checkError();}
		inline static void uniform(int pos, unsigned v0, unsigned v1, unsigned v2) { glUniform3ui(pos, v0, v1, v2); checkError();}
		inline static void uniform(int pos, unsigned v0, unsigned v1, unsigned v2, unsigned v3) { glUniform4ui(pos, v0, v1, v2, v3); checkError();}

		inline static void uniform(int pos, int val) { glUniform1i(pos, val); checkError();}
		inline static void uniform(int pos, int v0, int v1) { glUniform2i(pos, v0, v1); checkError();}
		inline static void uniform(int pos, int v0, int v1,int v2) { glUniform3i(pos, v0, v1, v2); checkError();}
		inline static void uniform(int pos, int v0, int v1,int v2,int v3) { glUniform4i(pos, v0, v1, v2, v3); checkError();}

		inline static void uniform(int pos, float val) { glUniform1f(pos, val); checkError();}
		inline static void uniform(int pos, float v0,float v1) { glUniform2f(pos, v0,v1); checkError();}
		inline static void uniform(int pos, float v0,float v1,float v2) { glUniform3f(pos, v0,v1,v2); checkError();}
		inline static void uniform(int pos, float v0,float v1,float v2,float v3) { glUniform4f(pos, v0,v1,v2,v3); checkError();}

		inline static void uniform(int pos, glm::vec3 value) { glUniform3f(pos, value.x, value.y, value.z); checkError();}

		inline static void uniform(int pos, double val) { glUniform1d(pos, val); checkError();}
		inline static void uniform(int pos, double v0,double v1) { glUniform2d(pos, v0,v1); checkError();}
		inline static void uniform(int pos, double v0,double v1,double v2) { glUniform3d(pos, v0,v1,v2); checkError();}
		inline static void uniform(int pos, double v0,double v1,double v2,double v3) { glUniform4d(pos, v0,v1,v2,v3); checkError();}
		
		inline static void uniform(int pos, glm::mat4 _mat, bool transpose = false) { glUniformMatrix4fv(pos, 1, transpose, glm::value_ptr(_mat)); checkError();}

		inline GLuint glCode() const noexcept { return programCode; };

		inline ~program() { glDeleteProgram(programCode); checkError();};
	};

	template <typename _T>
	using arrayData = std::vector<_T>;

	/// <summary>
	/// 缓存区对象
	/// </summary>
	class buffer {
	private:
		GLuint bufferCode;
		GLenum bufferType;
	protected:
		signed long long size;
	public:
		inline buffer(GLenum type = GL_ARRAY_BUFFER):size(0) { glGenBuffers(1, &bufferCode); checkError(); bufferType = type; glBindBuffer(type, bufferCode); checkError();};

		void setData(signed long long size,const void* data,unsigned usage);

		inline void bind() { glBindBuffer(bufferType, bufferCode); checkError();};
		inline void unbind() { glBindBuffer(bufferType, 0); checkError();};
		inline static void unbind(GLenum type) { glBindBuffer(type, 0); checkError();};

		template <typename _T>
		inline void setData(const arrayData<_T> _data, unsigned usage) {
			setData(_data.size() * sizeof(_T), &*_data.cbegin(), usage);
		};
		inline ~buffer() { glDeleteBuffers(1, &bufferCode); checkError();};
	};

	/// <summary>
	/// 数组数据属性
	/// </summary>
	struct attribute {
		GLuint location = 0;
		GLuint size;
		GLenum type = GL_FLOAT;
		GLboolean normalize = GL_FALSE;
		GLuint stride;
		GLuint offest = 0;
	};

	/// <summary>
	/// 数组数据属性集合
	/// </summary>
	using attributes = std::vector<attribute>;

	/// <summary>
	/// 顶点数组
	/// </summary>
	class vertexArray : private buffer {
		GLuint arrayCode;
		attributes _attrs;
		unsigned singleDataLen;
	public:
		inline vertexArray() :buffer(GL_ARRAY_BUFFER),singleDataLen(0) { glGenVertexArrays(1, &arrayCode); checkError();};

		inline void setData(signed long long size, const void* data, unsigned usage,unsigned singleDataLen) {
			bind();
			this->singleDataLen = singleDataLen;
			buffer::setData(size, data, usage);
		};
		template <typename _T>
		inline void setData(const arrayData<_T> _data, unsigned usage) {
			this->setData(_data.size() * sizeof(_T), &*_data.cbegin(), usage,sizeof(_T));
		};

		void setVertexAttributes(const attributes& _attr);

	protected:
		inline void bind(bool bindBuffer = true) { glBindVertexArray(arrayCode); checkError(); if (bindBuffer) buffer::bind();};
		inline void unbind(bool unbindBuffer = true) {if (unbindBuffer) buffer::unbind(); glBindVertexArray(0); checkError();};
		inline static void unbind(GLenum type) { buffer::unbind(type); glBindVertexArray(0); checkError();};

	public:
		void draw(const GLenum mode);
		void draw(const GLenum mode,const attribute& _attr);
		void draw(const GLenum mode,const attribute& _attr, const int len);
		void draw(const GLenum mode, const int offest, const int len);

		inline ~vertexArray() { glDeleteVertexArrays(1, &arrayCode); checkError();};
	};

	

	/// <summary>
	///	纹理(包括2D和3D纹理)
	/// </summary>
	class texture {
		GLuint textureCode;
		GLenum textType;
	public:
		inline texture(GLenum textType):textType(textType) { glGenTextures(1, &textureCode); checkError();};

		enum wrap {
			wrap_s = GL_TEXTURE_WRAP_S,
			wrap_t = GL_TEXTURE_WRAP_T,
			wrap_r = GL_TEXTURE_WRAP_R,
		};

		enum surrounding {
			surr_repeat = GL_REPEAT,
			surr_mirrepeat = GL_MIRRORED_REPEAT,
			surr_edge = GL_CLAMP_TO_EDGE,
			surr_border = GL_CLAMP_TO_BORDER,
		};

		enum operation {
			ope_min = GL_TEXTURE_MIN_FILTER,
			ope_mag = GL_TEXTURE_MAG_FILTER,
		};

		enum filter {
			fil_nearest = GL_NEAREST,
			fil_linear = GL_LINEAR,
			fil_near_near = GL_NEAREST_MIPMAP_NEAREST,
			fil_line_near = GL_LINEAR_MIPMAP_NEAREST,
			fil_near_line = GL_NEAREST_MIPMAP_LINEAR,
			fil_line_line = GL_LINEAR_MIPMAP_LINEAR,
		};

		inline void setSurrounding(wrap _wrap, surrounding _surr) {
			glTexParameteri(textType, _wrap, _surr); checkError();
		}

		inline void bind() { 
			glBindTexture(textType, textureCode); checkError();
		};

		inline void setBorderColor(float* color) {
			glTexParameterfv(textType, GL_TEXTURE_BORDER_COLOR, color); checkError();
		}

		inline void setFilter(operation _ope, filter _fil) {
			glTexParameteri(textType, _ope, _fil); checkError();
		}

		inline GLenum getType() const noexcept { return this->textType; };

		inline int glCode() const noexcept { return this->textureCode; };
	};

	template <GLenum _textType>
	concept is_texture_support_demension = _textType == GL_TEXTURE_1D || _textType == GL_TEXTURE_2D || _textType == GL_TEXTURE_3D || _textType == GL_TEXTURE_CUBE_MAP;
	
	template <auto i,auto j>
	concept is_same = i == j;

	template <GLenum type>
	requires is_texture_support_demension<type>
	class extTexture:public texture {
	public:
		inline extTexture() :texture(type) {};
	};

	template <>
	class extTexture<GL_TEXTURE_1D>:public texture {
	public:
		inline extTexture() :texture(GL_TEXTURE_1D) {};
		inline void generate(void* data, unsigned len, GLenum colorRange, GLenum stroageRange, unsigned level = 0, GLenum stroageType = GL_UNSIGNED_BYTE) {
			this->bind();
			glTexImage1D(GL_TEXTURE_1D, level, colorRange, len, 0, stroageRange, stroageType, data);
			checkError();
			glGenerateMipmap(GL_TEXTURE_1D); checkError();
		}
	};


	template <>
	class extTexture<GL_TEXTURE_2D>:public texture {
	public:
		inline extTexture() :texture(GL_TEXTURE_2D) {};
		inline void generate(const void* data, unsigned width, unsigned height, GLenum colorRange, GLenum stroageRange, unsigned level = 0, GLenum stroageType = GL_UNSIGNED_BYTE) {
			this->bind();
			glTexImage2D(GL_TEXTURE_2D, level, colorRange, width, height, 0, stroageRange, stroageType, data);
			checkError();
			glGenerateMipmap(GL_TEXTURE_2D); checkError();
		};

		inline void loadFromImage(const image& _image, GLenum colorRange = GL_RGB, GLenum stroageRange = GL_RGB, unsigned level = 0, GLenum stroageType = GL_UNSIGNED_BYTE, int unpackalign = 0) {
			glPixelStorei(GL_UNPACK_ALIGNMENT, unpackalign ? unpackalign : _image.getNrChannel());
			checkError();
			this->generate(_image.getData(), _image.getWidth(), _image.getHeight(), colorRange, stroageRange, level, stroageType);
		}
	};

	template<>
	class extTexture<GL_TEXTURE_3D>:public texture {
	public:
		inline extTexture() :texture(GL_TEXTURE_3D) {};
		inline void generate(void* data, unsigned width, unsigned height, unsigned depth, GLenum colorRange, GLenum stroageRange, unsigned level = 0, GLenum stroageType = GL_UNSIGNED_BYTE) {
			this->bind();
			glTexImage3D(GL_TEXTURE_3D, level, colorRange, width, height, depth, 0, stroageRange, stroageType, data);
			checkError();
			glGenerateMipmap(GL_TEXTURE_3D);
			checkError();
		}
	};

	using texture_1D = extTexture<GL_TEXTURE_1D>;
	using texture_2D = extTexture<GL_TEXTURE_2D>;
	using texture_3D = extTexture<GL_TEXTURE_3D>;

	class camera {
	protected:
		glm::vec3 _pos;
	public:
		///<summary>用户操作移动时的输入方式</summary>
		enum moveInputWay {
			surrounding,			// 环绕式观察
			exploring,				// 自由探索式观察
		};
		/// <summary>相机操作移动时方向</summary>
		enum moveWay {
			up,
			down,
			left,
			right,
			front,
			back,
		};
		///<summary>相机移动时的判定依据,即相机移动是否与相机朝向有关</summary>
		enum moveAcrooding {
			cameraSight,
			worldSight,
		};
		///<summary> 指定操作时相机移动方式 </summary>
		struct moveHandler {
			moveWay way;
			moveAcrooding accord;
			GLenum key;
		};
		
		enum class faceRotate {
			up,
			down,
			left,
			right,
		};

		enum class relativeFace {
			front,			// 背对摄像机的面
			back,			// 正对摄像机的面
			left,
			right,
			up,
			down,
		};

		enum class face {
			undefined = -1,
			x_positive,
			x_negative,
			y_positive,
			y_negative,
			z_positive,
			z_negative
		};

		using axis = face;
		/// <summary>
		/// 欧拉角
		/// </summary>
		struct eulars {
			float pitch;
			float yaw;
			float roll;
		};
		static face oppositeFace(const face from);
		/// <summary>
		/// 将向量转换为面(与<reference name="faceFromVec3">faceFromVec3</reference>作用相反)
		/// </summary>
		/// <param name="_face">向量,仅允许6大类,即只有一个参数有数值</param>
		/// <returns>面</returns>
		static face faceFromVec3(glm::vec3 _vec);
		/// <summary>
		/// 判断向量主要在哪个面
		/// </summary>
		/// <param name="_vec">向量</param>
		/// <returns>面</returns>
		static face faceMajorFromVec3(glm::vec3 _vec);
		/// <summary>
		/// 将面转化为向量(向量由原点指向面)
		/// </summary>
		/// <param name="_vec">向量</param>
		/// <returns>对应的面</returns>
		static glm::vec3 vec3FromFace(face _face);
		static face getRotatedFace(const face from,const faceRotate way,const face faceUp = face::y_positive);
		static face getRotatedFace(const glm::vec3 from, const faceRotate way, const glm::vec3 faceUp = glm::vec3(0.0f, 1.0f, 0.0f));
		
		static glm::vec3 getMajorFaceVec(glm::vec3 value);

		using moveHandlerSet = std::vector<moveHandler>;

		virtual inline glm::vec3 getPos() const noexcept;
	};

	class fpsCamera:public camera {
		glm::vec3 _target;
		bool _usePerspective = true;
	public:
		glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

		inline glm::vec3 getDirection() const { return glm::normalize(_pos - _target); };
		inline glm::vec3 getCameraRight() const { return glm::cross(upVector, getDirection()); };
		inline glm::vec3 getCameraUp() const { 
			glm::vec3 _t = getDirection();
			return glm::cross(_t, glm::normalize(glm::cross(upVector, _t)));
		};

		inline glm::mat4 getLookAt() const { return glm::lookAt(_pos, _target, upVector); };
		inline glm::mat4 getView() const { return glm::lookAt(_pos, _target, upVector); };

		inline void usePerspective(bool value = true) { _usePerspective = value; };
		inline bool isUsingPerspective() const { return _usePerspective; };

		/// <summary>获取透视矩阵</summary>
		/// <param name="fovy">视野</param>
		/// <param name="aspect">视图宽高比</param>
		/// <param name="zNear">平截头体近平面</param>
		/// <param name="zFar">平截头体远平面</param>
		/// <returns>透视矩阵</returns>
		inline static glm::mat4 getPerspective(float fovy, float aspect, float zNear, float zFar) {
			return glm::perspective(fovy, aspect, zNear, zFar);
		}

		inline glm::mat4 generate(float aspect = 1) const {
			return _usePerspective ? getView() * getPerspective(glm::radians(45.0f), aspect, 0.1f, 100.0f) : getView();
		}
		inline glm::mat4 generate(float fovy, float aspect, float zNear, float zFar) const {
			return _usePerspective ? getView() * getPerspective(fovy, aspect, zNear, zFar) : getView();
		}

		inline fpsCamera(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 target = glm::vec3(0.0f), bool usePerspective = true):
			_target(target),_usePerspective(usePerspective)
		{
			_pos = pos;
		}

		inline void setPos(glm::vec3 pos, bool moveTarget = false) {
			if (moveTarget)
				_target += pos - _pos;
			_pos = pos;
		}

		inline void setTarget(glm::vec3 target, bool movePos = false) {
			if (movePos)
				_pos += target - _target;
			_target = target;
		}

		inline void setFront(glm::vec3 front) {
			_target = _pos + front;
		}

		/// <summary>设置看向方向向量</summary>
		/// <param name="lookway">看向方向向量</param>
		/// <param name="useMoveTarget">指示是否通过改变Target坐标来实现自由移动,若为false则通过改变Pos实现</param>
		/// <param name="useNormalize">指示是否对向量进行标准化</param>
		inline void setLookWay(glm::vec3 lookway, bool useMoveTarget = true,bool useNormalize = false) {
			if (useMoveTarget) _target = _pos + (useNormalize ? glm::normalize(lookway) : lookway);
			else _pos = _target - (useNormalize ? glm::normalize(lookway) : lookway);
		}

		inline glm::vec3 getPos() const noexcept { return this->_pos; };
		inline glm::vec3 target() const noexcept { return this->_target; };
		inline glm::vec3 getRelative() const { return this->_target - this->_pos; };

		inline void movePos(glm::vec3 way) { _pos += way; }
		inline void moveTarget(glm::vec3 way) { _target += way; }
		inline void move(glm::vec3 way) { _pos += way; _target += way; }

		inline void rotateByTarget(float angle) {
			_pos = glm::vec3(glm::rotate(glm::mat4(1), angle, upVector) * glm::vec4(_pos,1));
		}

		void setEulerAngle(float pitch, float yaw, float distance = 1.0f,bool useMoveTarget = true);

		face getMajorFace() const;


	};

	template <typename T>
	using easeFunc = T(*)(T current, T start, T change,T duration);

	class QuatCamera : public camera {
	public:
		struct path {
			float requiredTime;
			glm::vec3 requiredUp;
			glm::vec3 requiredRight;
			bool useSurround = true;
		};
		struct rotate_raw {
			glm::vec3 axis;
			float angle;
		};
	private:
		glm::vec3 _up;
		glm::vec3 _right;
		glm::quat _last_quat;
		glm::vec3 Front;
		struct _ipath {
			path path;
			glm::vec3 startUp,startRight;
			rotate_raw upRaw, rightRaw;
			float timeBegin;
			bool useAngle = false;		// 指定角度旋转
		};
		std::queue<_ipath> _path;
		static float _inset_ease(float cur,float start,float change,float duration);
	public:
		float MouseSensitivity = 0.002f;
		easeFunc<float> sizer = _inset_ease;

		QuatCamera(glm::vec3 initialPos = glm::vec3(0.0f,0.0f,0.0f), glm::vec3 initialUp = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 initialRight = glm::vec3(1.0f, 0.0f, 0.0f));

		static glm::quat getQuatBetweenVecs(glm::vec3 _from, glm::vec3 _to);
		void setQuatBetweenVecs(glm::vec3 _from, glm::vec3 _to);
		static rotate_raw getRotate(glm::vec3 _from, glm::vec3 _to);

		inline glm::vec3 getUp() const noexcept;
		inline glm::vec3 getRight() const noexcept;

		inline glm::vec3 getPos() const noexcept;
		inline void setPos(glm::vec3 pos) noexcept;
		inline glm::vec3 getFront();
		inline glm::mat4 getLookAt();
		
		void sorround(float x_offset, float y_offset,glm::vec3 dest = glm::vec3(0.0f));
		void moveVis(float x_offest, float y_offest);
		inline void setAxis(glm::vec3 _up, glm::vec3 _right);

		inline face getMajorFace() const;
		inline face getMajorUpFace() const;

		void requirePath(path singlePath);
		void checkFrame();

		void requireRotate(rotate_raw _rotate, float time);
	};

	class picker {
		program& _program;
		GLuint _frameback;
		window& _window;

		GLuint _pickTxt, _depthTxt;
	public:
		struct PixelInfo {
			GLuint ObjectID = 0;
			GLuint DrawID = 0;
			GLuint PrimID = 0;
		};

		picker(program& _pro,glExt::window& window);
		~picker();

		void enableWrite();
		void disableWrite();

		PixelInfo readIndex(int x,int y);


	};

	template <>
	class extTexture<GL_TEXTURE_CUBE_MAP> :public texture {
	public:
		inline extTexture() :texture(GL_TEXTURE_CUBE_MAP) {};
		inline void generate(camera::axis axis, const void* data, unsigned width, unsigned height, GLenum colorRange, GLenum stroageRange, unsigned level = 0, GLenum stroageType = GL_UNSIGNED_BYTE) {
			this->bind();
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + int(axis), level, colorRange, width, height, 0, stroageRange, stroageType, data);
			checkError();
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP); checkError();
		};

		inline void loadFromImage(camera::axis axis, const image& _image, GLenum colorRange = GL_RGB, GLenum stroageRange = GL_RGB, unsigned level = 0, GLenum stroageType = GL_UNSIGNED_BYTE, bool unpackalign = false) {
			glPixelStorei(GL_UNPACK_ALIGNMENT, unpackalign);
			checkError();
			this->generate(axis, _image.getData(), _image.getWidth(), _image.getHeight(), colorRange, stroageRange, level, stroageType);
		}
	};
	using texture_Cube = extTexture<GL_TEXTURE_CUBE_MAP>;
}

#pragma warning(default:26495)
#include "glext.hpp"

