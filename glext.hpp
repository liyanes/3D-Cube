#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glext.h"

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

template <typename T>
inline T* glExt::max(T* start,T* end) {
	T* _max = start;
	while (++start <= end) {
		if (*_max < *start) _max = start;
	}
	return start;
}

template <typename T>
inline const T* glExt::max(const T* start,const T* end) {
	T* _max = start;
	while (++start <= end) {
		if (*_max < *start) _max = start;
	}
	return start;
}

inline glExt::image::image(const char* filename) :_data(stbi_load(filename, &width, &height, &nrChannels, 0)) {
	if (!_data) {
		throw glExt::err::glImageLoadFailed(filename);
	}
}

inline glExt::image::image(const char* filename, bool flip) : _data((stbi_set_flip_vertically_on_load(flip), stbi_load(filename, &width, &height, &nrChannels, 0))) {
	if (!_data) throw err::glImageLoadFailed(filename);
}

inline void glExt::clearColor(float red, float green, float blue, float alpha) { 
	glClearColor(red, green, blue, alpha); 
	glClear(GL_COLOR_BUFFER_BIT); 
}

inline void glExt::clearColor(float* color) {
	clearColor(color[0], color[1], color[2], color[3]);
};

inline glm::vec3 glExt::QuatCamera::getPos() const
{
	return this->_pos;
}

inline void glExt::QuatCamera::setPos(glm::vec3 pos)
{
	this->_pos = pos;
}

inline glm::vec3 glExt::QuatCamera::getFront()
{
	return glm::cross(_up, _right);
}

inline glm::mat4 glExt::QuatCamera::getLookAt()
{
	return glm::lookAt(_pos,_pos + Front,_up);
}

inline void glExt::QuatCamera::setAxis(glm::vec3 _up, glm::vec3 _right) {
	if (abs(dot(_up, _right)) >= errContain) throw;
	this->_up = _up;
	this->_right = _right;
}

inline glExt::camera::face glExt::QuatCamera::getMajorFace() const {
	return faceFromVec3(getMajorFaceVec(this->Front));
}

inline glExt::camera::face glExt::QuatCamera::getMajorUpFace() const
{
	return faceFromVec3(getMajorFaceVec(this->_up));
}

inline glm::vec3 glExt::QuatCamera::getUp() const {
	return _up;
}

inline glm::vec3 glExt::QuatCamera::getRight() const {
	return _right;
}

inline unsigned glExt::emptyComp(glm::vec3 value)noexcept {
	return (unsigned)(value.x == 0.0f) + (value.y == 0.0f) + (value.z == 0.0f);
}

inline bool glExt::isEmpty(glm::vec3 value) noexcept {
	return (value.x == 0.0f) && (value.y == 0.0f) && (value.z == 0.0f);
}

inline glExt::crood<double> glExt::window::getCursorPos() const {
	crood<double> ret;
	glfwGetCursorPos(this->_window, &ret.x, &ret.y);
	return ret;
}

inline glExt::crood<int> glExt::window::getSize() const {
	crood<int> ret;
	glfwGetWindowSize(this->_window, &ret.x, &ret.y);
	return ret;
}

inline void glExt::checkError() {
	int error = glGetError();
	if (error != GL_NO_ERROR) {
		throw glExt::err::BaseException(error, "Kernal Error");
	}
}

template<typename _retT>
inline _retT glExt::checkError(_retT toret) {
	int error = glGetError();
	if (error != GL_NO_ERROR) {
		throw glExt::err::BaseException(error, "Kernal Error");
	}
	return toret;
}

template<typename _retT>
inline _retT& glExt::checkError(_retT& toret) {
	int error = glGetError();
	if (error != GL_NO_ERROR) {
		throw glExt::err::BaseException(error, "Kernal Error");
	}
	return toret;
}

inline glExt::initializer::initializer() {
	glExt::initialize();
}

inline glExt::initializer::~initializer() {
	glExt::finialize();
}

inline bool glExt::window::isFullScreen() {
	return glfwGetWindowMonitor(this->_window) != nullptr;
}
