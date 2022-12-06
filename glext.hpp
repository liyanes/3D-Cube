#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glext.h"

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

inline glExt::image::image(const image& _image)
{
	this->width = _image.width;
	this->height = _image.height;
	this->nrChannels = _image.nrChannels;
	this->_data = malloc(_image.width * _image.height * _image.nrChannels * sizeof(char));
	if (!this->_data) throw std::bad_alloc();
	memcpy(this->_data, _image._data, _image.width * _image.height * _image.nrChannels * sizeof(char));
}

inline glExt::image::image(image&& _image) noexcept
{
	this->width = _image.width;
	this->height = _image.height;
	this->nrChannels = _image.nrChannels;
	this->_data = _image._data;
	_image.useFree = false;
}

inline const glExt::image& glExt::image::operator=(const image& _image) {
	this->~image();
	this->width = _image.width;
	this->height = _image.height;
	this->nrChannels = _image.nrChannels;
	this->_data = malloc(_image.width * _image.height * _image.nrChannels * sizeof(char));
	if (!this->_data) throw std::bad_alloc();
	memcpy(this->_data, _image._data, _image.width * _image.height * _image.nrChannels * sizeof(char));
	return *this;
}

inline const glExt::image& glExt::image::operator=(image&& _image) noexcept {
	this->~image();
	new (this)image(_image);
	return *this;
}

inline void glExt::clearColor(float red, float green, float blue, float alpha) { 
	glClearColor(red, green, blue, alpha); 
	glClear(GL_COLOR_BUFFER_BIT); 
}

inline void glExt::clearColor(float* color) {
	clearColor(color[0], color[1], color[2], color[3]);
};

inline glm::vec3 glExt::QuatCamera::getPos() const noexcept
{
	return this->_pos;
}

inline void glExt::QuatCamera::setPos(glm::vec3 pos) noexcept
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

inline glm::vec3 glExt::QuatCamera::getUp() const noexcept {
	return _up;
}

inline glm::vec3 glExt::QuatCamera::getRight() const noexcept {
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

inline glExt::initializer::initializer(version version) {
	glExt::initialize(version);
}

inline glExt::initializer::~initializer() {
	glExt::finalize();
}

inline bool glExt::window::isFullScreen() {
	return glfwGetWindowMonitor(this->_window) != nullptr;
}

template<typename _T>
inline constexpr glExt::crood<_T>::crood()
	:x(),y()
{}

template<typename _T>
inline constexpr glExt::crood<_T>::crood(const crood& _right)
	:x(_right.x),y(_right.y)
{}

template<typename _T>
inline constexpr glExt::crood<_T>::crood(const crood&& _right)
	:x(_right.x),y(_right.y)
{}

template<typename _T>
inline constexpr glExt::crood<_T>::crood(const _T x, const _T y)
	:x(x),y(y)
{}

template <typename _T>
inline bool constexpr glExt::crood<_T>::operator==(const crood<_T>& _right) const noexcept {
	return this->x == _right.x && this->y == _right.y;
}
template <typename _T>
inline constexpr const glExt::crood<_T> glExt::crood<_T>::operator - () const noexcept {
	return glExt::crood<_T>{ -this->x,-this->y };
}
template <typename _T>
inline constexpr const glExt::crood<_T> glExt::crood<_T>::operator-(const crood<_T>& _right) const noexcept {
	return glExt::crood<_T>{ this->x - _right.x,this->y - _right.y };
}

template<typename _T>
inline constexpr const glExt::crood<_T> glExt::crood<_T>::operator+(const crood<_T>& _right) const noexcept{
	return glExt::crood<_T>{this->x + _right.x, this->y + _right.y};
}

template<typename _T>
inline constexpr const std::enable_if_t<std::is_arithmetic_v<_T>, _T> glExt::crood<_T>::length() const{
	return sqrt(x * x + y * y);
}

template<typename _T>
inline constexpr const std::enable_if_t<std::is_arithmetic_v<_T>, _T> glExt::crood<_T>::length2() const {
	return x * x + y * y;
}

inline glm::vec3 glExt::camera::getPos() const noexcept
{
	return this->_pos;
}

