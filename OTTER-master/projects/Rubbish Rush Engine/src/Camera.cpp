#include "Camera.h"

#include <GLM/gtc/matrix_transform.hpp>

Camera::Camera() :
	_nearPlane(0.1f),
	_farPlane(1000.0f),
	_fovRadians(glm::degrees(90.0f)),
	_aspectRatio(1.0f),
	_orthoVerticalScale(1.0f),
	_isOrtho(false),
	_position(glm::vec3(0.0f)),
	_normal(glm::vec3(0.0f, 0.0f, 1.0f)),
	_up(glm::vec3(0.0f, 1.0f, 0.0f)), // Using Y-up coordinate system by default
	_view(glm::mat4(1.0f)),
	_projection(glm::mat4(1.0f)),
	_viewProjection(glm::mat4(1.0f)),
	_isDirty(true)
{
	__CalculateProjection();
}

void Camera::SetPosition(const glm::vec3& position) {
	_position = position;
	__CalculateView();
}

void Camera::SetForward(const glm::vec3& forward) {
	_normal = forward;
	__CalculateView();
}

void Camera::LookAt(const glm::vec3& point) {
	_normal = glm::normalize(point - _position);
	__CalculateView();
}

void Camera::SetUp(const glm::vec3& up) {
	_up = up;
	__CalculateView();
}

void Camera::ResizeWindow(int windowWidth, int windowHeight) {
	_aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
	__CalculateProjection();
}

void Camera::SetOrthoEnabled(bool value)
{
	_isOrtho = true;
	__CalculateProjection();
}

void Camera::SetFovRadians(float value) {
	_fovRadians = value;
	__CalculateProjection();
}

void Camera::SetFovDegrees(float value) {
	SetFovRadians(glm::radians(value));
}

void Camera::SetOrthoVerticalScale(float value) {
	_orthoVerticalScale = value;
	__CalculateProjection();
}

const glm::mat4& Camera::GetViewProjection() const {
	if (_isDirty) {
		_viewProjection = _projection * _view;
		_isDirty = false;
	}
	return _viewProjection;
}

void Camera::__CalculateProjection()
{
	if (_isOrtho) {
		float w = (_orthoVerticalScale * _aspectRatio) / 2.0f;
		float h = (_orthoVerticalScale / 2.0f);
		_projection = glm::ortho(-w, w, -h, h, _nearPlane, _farPlane);
	} else {
		_projection = glm::perspective(_fovRadians, _aspectRatio, _nearPlane, _farPlane);
	}
	_isDirty = true;
}

void Camera::__CalculateView() {
	_view = glm::lookAt(_position, _position + _normal, _up);
	_isDirty = true;
}
