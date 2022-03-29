
/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_LOOK_AT_CTRL_H_
#define FRAMEWORK_LOOK_AT_CTRL_H_

#include "common.h"
#include "INodeModifier.h"

namespace frm {

/*	 */
class LookAtCtrl: public INodeModifier {
public:
//	CTORS/DTORS:
	static LookAtCtrl* create(
		Node *target,
		const CStringT &name = "LookAtCtrl"
	);

//	ACCESSORS:

//	SERVICES:

	/* Interpolate between start and end at the current time. */
	virtual void update(TimeT dt);

protected:
//	MEMBERS:
	Node *target_;
	vec3 upVector_; // ref for basis construction

//	CTORS/DTORS:
	LookAtCtrl(
		Node *parent,
		Node *target,
		const CStringT &name
	);

}; // class LookAtCtrl

/*******************************************************************************
	LookAtCtrl inline implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline LookAtCtrl* LookAtCtrl::create(
	Node *target,
	const CStringT &name
) {
	LookAtCtrl *result = new(std::nothrow) LookAtCtrl(
		0,
		target,
		name
	);
	assert(result);
	return result;
}

//	ACCESSORS:

//	SERVICES:

/*----------------------------------------------------------------------------*/
inline void LookAtCtrl::update(TimeT dt) {
	vec3 targetPosition = vec3(target_->getWorldMatrix().col(3));
	vec3 thisPosition = vec3(getParent()->getWorldMatrix().col(3));

	vec3 z = sml::normalize(targetPosition - thisPosition);

//	compute orthonormal basis:
	vec3 x, y;
	if (z == upVector_ || z == -upVector_) {
		vec3 k = upVector_ + std::numeric_limits<float>::epsilon();
		y = k - z * dot(k, z);
	} else {
		y = upVector_ - z * sml::dot(upVector_, z);
	}
	y.normalize();
	x = cross(y, z);

//	build rotation from vectors:
	getParent()->setLocalMatrix(
		getParent()->getLocalMatrix() * mat4(mat3(x, y, z))
	);
}

//	PROTECTED:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline LookAtCtrl::LookAtCtrl(
	Node *parent,
	Node *target,
	const CStringT &name
):	INodeModifier(parent, 0, name),
	target_(target),
	upVector_(vec3::yAxis()) {
	assert(target_);
}

} // namespace frm

#endif // FRAMEWORK_LOOK_AT_CTRL_H_
