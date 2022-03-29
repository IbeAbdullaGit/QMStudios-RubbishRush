/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_POSITION_ROTATION_SCALE_CTRL_H_
#define FRAMEWORK_POSITION_ROTATION_SCALE_CTRL_H_

#include "common.h"
#include "INodeModifier.h"

namespace frm {

/*	 */
class PositionRotationScaleCtrl: public INodeModifier {
public:
//	CTORS/DTORS:
	static PositionRotationScaleCtrl* create(
		const vec3 &position = vec3(0.0f),
		const quat &rotation = quat::identity(),
		const vec3 &scale = vec3(1.0f),
		const CStringT &name = "PositionRotationScaleCtrl"
	);

//	ACCESSORS:
	const vec3& getPosition() const;
	void setPosition(const vec3&);

	const quat& getRotation() const;
	void setRotation(const quat&);

	const vec3& getScale() const;
	void setScale(const vec3&);

//	SERVICES:
	/* Interpolate between start and end at the current time. */
	virtual void update(TimeT dt);

protected:
//	MEMBERS:
	vec3 position_;
	quat rotation_;
	vec3 scale_;
	mat4 prsMatrix_;

//	CTORS/DTORS:
	PositionRotationScaleCtrl(
		Node *parent,
		const vec3 &position,
		const quat &rotation,
		const vec3 &scale,
		const CStringT &name
	);

}; // class PositionRotationScaleCtrl

/*******************************************************************************
	PositionRotationScaleCtrl inline implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline PositionRotationScaleCtrl* PositionRotationScaleCtrl::create(
	const vec3 &position,
	const quat &rotation,
	const vec3 &scale,
	const CStringT &name
) {
	PositionRotationScaleCtrl *result =
		new(std::nothrow) PositionRotationScaleCtrl(
			0,
			position,
			rotation,
			scale,
			name
		);
	assert(result);
	return result;
}

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline const vec3& PositionRotationScaleCtrl::getPosition() const {
	return position_;
}
/*----------------------------------------------------------------------------*/
inline void PositionRotationScaleCtrl::setPosition(const vec3 &position) {
	position_ = position;
	prsMatrix_ = mat4(scale_, rotation_, position_);
}

/*----------------------------------------------------------------------------*/
inline const quat& PositionRotationScaleCtrl::getRotation() const {
	return rotation_;
}
/*----------------------------------------------------------------------------*/
inline void PositionRotationScaleCtrl::setRotation(const quat &rotation) {
	rotation_ = rotation;
	prsMatrix_ = mat4(scale_, rotation_, position_);
}

/*----------------------------------------------------------------------------*/
inline const vec3& PositionRotationScaleCtrl::getScale() const {
	return scale_;
}
/*----------------------------------------------------------------------------*/
inline void PositionRotationScaleCtrl::setScale(const vec3 &scale) {
	scale_ = scale;
	prsMatrix_ = mat4(scale_, rotation_, position_);
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
inline void PositionRotationScaleCtrl::update(TimeT dt) {
	getParent()->setLocalMatrix(getParent()->getLocalMatrix() * prsMatrix_);
}

//	PROTECTED:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline PositionRotationScaleCtrl::PositionRotationScaleCtrl(
	Node *parent,
	const vec3 &position,
	const quat &rotation,
	const vec3 &scale,
	const CStringT &name
):	INodeModifier(parent, 0, name),
	position_(position),
	rotation_(rotation),
	scale_(scale) {

	prsMatrix_ = mat4(scale_, rotation_, position_);
}

} // namespace frm

#endif // FRAMEWORK_LOOK_AT_CTRL_H_

