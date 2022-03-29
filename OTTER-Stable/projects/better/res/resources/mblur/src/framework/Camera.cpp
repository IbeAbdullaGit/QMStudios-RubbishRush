/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include "Camera.h"

#include "common.h"

namespace frm {

/*******************************************************************************
	Camera implementation:
*******************************************************************************/

//	PUBLIC:

//	SERVICES:

/*----------------------------------------------------------------------------*/
void Camera::update(TimeT dt) {
#ifdef FRAMEWORK_STORE_PREVIOUS_TRANSFORMS
//	store previous values:
	previous_ = current_;
#endif

	localFrustum_ = Frustum(
		aspect_,
		fov_,
		clipNear_,
		clipFar_,
		isOrthographic_
	);
	worldFrustum_ = localFrustum_;

	if (getParent()) {
		current_.view = sml::inverse(getParent()->getLocalMatrix());
		worldFrustum_.transform(getParent()->getLocalMatrix());
	}

	if (isOrthographic_) {
		float width = tanHalfFov_ * clipFar_ * aspect_ * 2.0f;
		float height = tanHalfFov_ * clipFar_ * 2.0f;
		float fmn = clipFar_ - clipNear_;
		current_.projection = mat4(
			2.0 / width,		0.0,				0.0,				0.0,
			0.0,					2.0 / height,	0.0,				0.0,
			0.0,					0.0,				1.0 / fmn,		-clipNear_ / fmn,
			0.0,					0.0,				0.0,				1.0
		);
	} else {
		float d = 1.0 / tanHalfFov_;
		float fmn = clipFar_ - clipNear_;
		float nf = clipNear_ * clipFar_;
		current_.projection = mat4(
			d / aspect_,		0.0,			0.0,						0.0,
			0.0,					d,				0.0,						0.0,
			0.0,					0.0,			clipFar_ / fmn, 		-nf / fmn,
			0.0,					0.0,			1.0,						0.0
		);
	}

	current_.viewProjection = current_.projection * current_.view;
}

} // namespace frm

