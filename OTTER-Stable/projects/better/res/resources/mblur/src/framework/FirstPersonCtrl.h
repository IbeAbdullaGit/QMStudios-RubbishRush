/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_FIRST_PERSON_CTRL_H_
#define FRAMEWORK_FIRST_PERSON_CTRL_H_

#include "common.h"
#include "INodeModifier.h"

namespace frm {

/*	First person controller; constructs a transformation based on position and
	x/y rotation.*/
class FirstPersonCtrl: public INodeModifier {
public:
//	CTORS/DTORS:
	static FirstPersonCtrl* create(
		const vec3 &position = vec3(0.0f),
		const quat &rotation = quat::identity(),
		const CStringT &name = "FirstPersonCtrl"
	);

//	ACCESSORS:
	const vec3& getPosition() const;
	void setPosition(const vec3&);

	const quat& getRotation() const;
	void setRotation(const quat&);

	/*	Apply an impulse to the x/y/z rotation velocity, radians/sec*/
	void pitch(float x);
	void yaw(float y);
	void roll(float z);
	const vec3& getPitchYawRoll() const;
	void setPitchYawRoll(const vec3&);

	/*	Apply an impulse to the */
	void forward(float s);
	void backward(float s);
	void left(float s);
	void right(float s);

//	SERVICES:
	void lookAt(const vec3 &target);

	/* Update position/rotation by velocities, set transformation matrix. */
	virtual void update(TimeT dt);

protected:
//	MEMBERS:
	vec3 position_, velocity_;
	float maxVelocitySpeed_; // speed = velocity magnitude
	float velocityDamp_; // fraction of velocity retained per second

	quat rotation_;
	vec3 pitchYawRoll_; // pitch/yaw/roll in radians/sec
	float maxRotationSpeed_;
	float rotationDamp_;

	mat4 localMatrix_; // represents position/rotation, for getting local axes

//	CTORS/DTORS:
	FirstPersonCtrl(
		Node *parent,
		const vec3 &position,
		const quat &rotation,
		const CStringT &name
	);

//	SERVICES:
	/*	Sets velocity, effects clamping. */
	void setVelocity(const vec3 &v);

}; // class FirstPersonCtrl

/*******************************************************************************
	FirstPersonCtrl inline implementation:
*******************************************************************************/

//	PUBLIC:

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline const vec3& FirstPersonCtrl::getPosition() const {
	return position_;
}
/*----------------------------------------------------------------------------*/
inline void FirstPersonCtrl::setPosition(const vec3 &position) {
	position_ = position;
}

/*----------------------------------------------------------------------------*/
inline const quat& FirstPersonCtrl::getRotation() const {
	return rotation_;
}
/*----------------------------------------------------------------------------*/
inline void FirstPersonCtrl::setRotation(const quat &rotation) {
	rotation_ = rotation;
}

/*----------------------------------------------------------------------------*/
inline void FirstPersonCtrl::pitch(float x) {
	pitchYawRoll_.x(pitchYawRoll_.x() + x);
}
/*----------------------------------------------------------------------------*/
inline void FirstPersonCtrl::yaw(float y) {
	pitchYawRoll_.y(pitchYawRoll_.y() + y);
}
/*----------------------------------------------------------------------------*/
inline void FirstPersonCtrl::roll(float z) {
	pitchYawRoll_.z(pitchYawRoll_.z() + z);
}

/*----------------------------------------------------------------------------*/
inline const vec3& FirstPersonCtrl::getPitchYawRoll() const {
	return pitchYawRoll_;
}
/*----------------------------------------------------------------------------*/
inline void FirstPersonCtrl::setPitchYawRoll(const vec3 &pyr) {
	pitchYawRoll_ = pyr;
}

/*----------------------------------------------------------------------------*/
inline void FirstPersonCtrl::forward(float s) {
	velocity_ += vec3(localMatrix_.col(2)) * s;
}
/*----------------------------------------------------------------------------*/
inline void FirstPersonCtrl::backward(float s) {
	forward(-s);
}
/*----------------------------------------------------------------------------*/
inline void FirstPersonCtrl::left(float s) {
	right(-s);
}
/*----------------------------------------------------------------------------*/
inline void FirstPersonCtrl::right(float s) {
	velocity_ += vec3(localMatrix_.col(0)) * s;
}



//	PROTECTED:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline FirstPersonCtrl::FirstPersonCtrl(
	Node *parent,
	const vec3 &position,
	const quat &rotation,
	const CStringT &name
):	INodeModifier(parent, (INodeModifier::CallbackT)0, name),
	position_(position),
	velocity_(vec3(0.0f)),
	maxVelocitySpeed_(100.0f),
	velocityDamp_(0.001f),
	rotation_(rotation),
	pitchYawRoll_(vec3(0.0f)),
	maxRotationSpeed_(10.0f),
	rotationDamp_(0.000005f),
	localMatrix_(mat4::identity()) {
}

} // namespace frm

#endif // FRAMEWORK_FIRST_PERSON_CTRL_H_

