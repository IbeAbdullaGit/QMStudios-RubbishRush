/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include <vector>

#include "FirstPersonCtrl.h"
#include "common.h"

namespace frm {

/*******************************************************************************
	FirstPersonCtrl implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
FirstPersonCtrl* FirstPersonCtrl::create(
	const vec3 &position,
	const quat &rotation,
	const CStringT &name
){
	FirstPersonCtrl *result = new(std::nothrow) FirstPersonCtrl(
		0,
		position,
		rotation,
		name
	);
	assert(result);
	return result;
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
void FirstPersonCtrl::lookAt(const vec3 &target) {
	vec3 z = target - position_;
	z.normalize();
	vec3 ref = vec3::yAxis();

	//	compute orthonormal basis:
	vec3 x, y;
	if (z == ref || z == -ref) {
		vec3 k = ref + std::numeric_limits<float>::epsilon();
		y = k - z * sml::dot(k, z);
	} else {
		y = ref - z * sml::dot(ref, z);
	}
	y.normalize();
	x = sml::cross(y, z);

//	build rotation from basis matrix:
	rotation_ = quat(mat3(x, y, z));
}

/*----------------------------------------------------------------------------*/
void FirstPersonCtrl::update(TimeT dt) {
//	clamp velocities:
	float velocitySpeed = sml::clamp(velocity_.length(), 0.0f, maxVelocitySpeed_);
	if (velocitySpeed > 0.0f)
		velocity_ = sml::normalize(velocity_) * velocitySpeed;
	float rotationSpeed = sml::clamp(pitchYawRoll_.length(), 0.0f, maxRotationSpeed_);
	if (rotationSpeed > 0.0f)
		pitchYawRoll_ = sml::normalize(pitchYawRoll_) * rotationSpeed;

//	apply velocities:
	position_ += velocity_ * dt;
	quat qPitch(vec3(localMatrix_.col(0)), pitchYawRoll_.x() * dt);
	quat qYaw(vec3::yAxis(), pitchYawRoll_.y() * dt);
	quat qRoll(vec3(localMatrix_.col(2)), pitchYawRoll_.z() * dt);
	rotation_ = qYaw * qPitch * qRoll * rotation_;
	rotation_.normalize();

//	apply banking:
	quat qBank(vec3(localMatrix_.col(2)), -pitchYawRoll_.y() * dt);

//	damp velocities:
	velocity_ *= sml::pow(velocityDamp_, dt);
	pitchYawRoll_ *= pow(rotationDamp_, dt);

//	build matrices:
	localMatrix_ = mat4(vec3(1.0f), rotation_, position_); // don't store bank locally
	getParent()->setLocalMatrix(
		getParent()->getLocalMatrix() * mat4(vec3(1.0f), qBank * rotation_, position_)
	);
}

} // namespace frm

