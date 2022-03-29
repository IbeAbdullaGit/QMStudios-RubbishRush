/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_CAMERA_H_
#define FRAMEWORK_CAMERA_H_

#include <vector>

#include "common.h"
#include "INodeModifier.h"
#include "Frustum.h"

namespace frm {

/*	*/
class Camera: public INodeModifier {
public:
//	CTORS/DTORS:
	static Camera *create(
		float aspect			= 1.0f,
		float fov				= sml::radians(60.0f),
		float clipNear			= 0.1f,
		float clipFar			= 1000.0f,
		bool isOrthographic	= false,
		const CStringT &name = "Camera"
	);

//	ACCESSORS:
	/*	Return parent's world matrix.*/
	const mat4& getWorldMatrix() const;

	const mat4& getViewMatrix() const;
	void setViewMatrix(const mat4&);

	const mat4& getProjectionMatrix() const;
	void setProjectionMatrix(const mat4&);

	const mat4& getViewProjectionMatrix() const;

#ifdef FRAMEWORK_STORE_PREVIOUS_TRANSFORMS
	const mat4& getPreviousViewMatrix() const;
	const mat4& getPreviousProjectionMatrix() const;
	const mat4& getPreviousViewProjectionMatrix() const;
#endif

	float getAspect() const;
	void setAspect(float);

	float getFov() const;
	void setFov(float);

	float getTanHalfFov() const;

	float getClipNear() const;
	void setClipNear(float);

	float getClipFar() const;
	void setClipFar(float);

	bool isOrthographic() const;
	void setOrthographic(bool);

	const Frustum& getLocalFrustum() const;
	const Frustum& getWorldFrustum() const;

//	SERVICES:
	/*	Construct the view/projection matrices, set matrix to projection * view.*/
	virtual void update(TimeT dt);

protected:
//	MEMBERS:
	struct CameraMatricesT {
		mat4 view, projection, viewProjection;
	};
	CameraMatricesT current_;
#ifdef FRAMEWORK_STORE_PREVIOUS_TRANSFORMS
	CameraMatricesT previous_;
#endif
	Frustum localFrustum_, worldFrustum_;
	float aspect_, fov_, tanHalfFov_, clipNear_, clipFar_;
	bool isOrthographic_;

//	CTORS/DTORS:
	Camera(
		Node *parent,
		float aspect,
		float fov,
		float clipNear,
		float clipFar,
		bool isOrthographic,
		const CStringT &name
	);

}; // class Camera

/*******************************************************************************
	Camera inline implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline Camera* Camera::create(
	float aspect,
	float fov,
	float clipNear,
	float clipFar,
	bool isOrthographic,
	const CStringT &name
) {
	Camera *result = new(std::nothrow) Camera(
		0,
		aspect,
		fov,
		clipNear,
		clipFar,
		isOrthographic,
		name
	);
	assert(result);
	return result;
}

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline const mat4& Camera::getWorldMatrix() const {
	return getParent()->getWorldMatrix();
}

/*----------------------------------------------------------------------------*/
inline const mat4& Camera::getViewMatrix() const {
	return current_.view;
}
/*----------------------------------------------------------------------------*/
inline void Camera::setViewMatrix(const mat4 &viewMatrix) {
	current_.view = viewMatrix;
	current_.viewProjection = current_.projection * current_.view;
}

/*----------------------------------------------------------------------------*/
inline const mat4& Camera::getProjectionMatrix() const {
	return current_.projection;
}
/*----------------------------------------------------------------------------*/
inline void Camera::setProjectionMatrix(const mat4 &projMatrix) {
	current_.projection = projMatrix;
	current_.viewProjection = current_.projection * current_.view;
}

/*----------------------------------------------------------------------------*/
inline const mat4& Camera::getViewProjectionMatrix() const {
	return current_.viewProjection;
}

#ifdef FRAMEWORK_STORE_PREVIOUS_TRANSFORMS
/*----------------------------------------------------------------------------*/
inline const mat4& Camera::getPreviousViewMatrix() const {
	return previous_.view;
}
/*----------------------------------------------------------------------------*/
inline const mat4& Camera::getPreviousProjectionMatrix() const {
	return previous_.projection;
}
/*----------------------------------------------------------------------------*/
inline const mat4& Camera::getPreviousViewProjectionMatrix() const {
	return previous_.viewProjection;
}
#endif

/*----------------------------------------------------------------------------*/
inline float Camera::getAspect() const {
	return aspect_;
}
/*----------------------------------------------------------------------------*/
inline void Camera::setAspect(float aspect) {
	aspect_ = aspect;
}

/*----------------------------------------------------------------------------*/
inline float Camera::getFov() const {
	return fov_;
}
/*----------------------------------------------------------------------------*/
inline void Camera::setFov(float fov) {
	fov_ = fov;
	tanHalfFov_ = sml::tan(fov_ / 2.0f);
}

/*----------------------------------------------------------------------------*/
inline float Camera::getTanHalfFov() const {
	return tanHalfFov_;
}

/*----------------------------------------------------------------------------*/
inline float Camera::getClipNear() const {
	return clipNear_;
}
/*----------------------------------------------------------------------------*/
inline void Camera::setClipNear(float clipNear) {
	clipNear_ = clipNear;
}

/*----------------------------------------------------------------------------*/
inline float Camera::getClipFar() const {
	return clipFar_;
}
/*----------------------------------------------------------------------------*/
inline void Camera::setClipFar(float clipFar) {
	clipFar_ = clipFar;
}

/*----------------------------------------------------------------------------*/
inline bool Camera::isOrthographic() const {
	return isOrthographic_;
}
/*----------------------------------------------------------------------------*/
inline void Camera::setOrthographic(bool isOrthographic) {
	isOrthographic_ = isOrthographic;
}

/*----------------------------------------------------------------------------*/
inline const Frustum& Camera::getLocalFrustum() const {
	return localFrustum_;
}

/*----------------------------------------------------------------------------*/
inline const Frustum& Camera::getWorldFrustum() const {
	return worldFrustum_;
}

//	PROTECTED:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline Camera::Camera(
	Node *parent,
	float aspect,
	float fov,
	float clipNear, float clipFar,
	bool isOrthographic,
	const CStringT &name
):	INodeModifier(parent, 0, name),
	aspect_(aspect),
	fov_(fov),
	clipNear_(clipNear), clipFar_(clipFar),
	isOrthographic_(isOrthographic) {

	current_.view = current_.projection = current_.viewProjection = mat4::identity();

	tanHalfFov_ = sml::tan(fov_ / 2.0f);
}


} // namespace frm

#endif // FRAMEWORK_CAMERA_H_

