/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_SPIN_CTRL_H_
#define FRAMEWORK_SPIN_CTRL_H_

#include "common.h"
#include "INodeModifier.h"

namespace frm {

/*	 */
class SpinCtrl: public INodeModifier {
public:
//	CTORS/DTORS:
	static SpinCtrl* create(
		const vec3 &axis,
		float rate, // radians/sec
		const CStringT &name = "SpinCtrl"
	);

//	ACCESSORS:

//	SERVICES:
	/*	Set current rotation to identity. */
	virtual void reset();

	/*	Set rate = -rate. */
	virtual void reverse();

	/* Interpolate between start and end at the current time. */
	virtual void update(TimeT dt);

protected:
//	MEMBERS:
	quat current_;
	vec3 axis_;
	float rate_;

//	CTORS/DTORS:
	SpinCtrl(
		Node *parent,
		const vec3 &axis,
		float rate,
		const CStringT &name
	);

}; // class SpinCtrl

/*******************************************************************************
	SpinCtrl inline implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline SpinCtrl* SpinCtrl::create(
	const vec3 &axis,
	float rate,
	const CStringT &name
) {
	SpinCtrl *result = new(std::nothrow) SpinCtrl(
		0,
		axis,
		rate,
		name
	);
	assert(result);
	return result;
}

//	ACCESSORS:

//	SERVICES:

/*----------------------------------------------------------------------------*/
inline void SpinCtrl::reset() {
	current_ = quat::identity();
}

/*----------------------------------------------------------------------------*/
inline void SpinCtrl::reverse() {
	rate_ = -rate_;
}

/*----------------------------------------------------------------------------*/
inline void SpinCtrl::update(TimeT dt) {
	current_ = current_ * quat(axis_, rate_ * dt);
	getParent()->setLocalMatrix(
		getParent()->getLocalMatrix() * mat4::rotation3(sml::normalize(current_))
	);
}

//	PROTECTED:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline SpinCtrl::SpinCtrl(
	Node *parent,
	const vec3 &axis,
	float rate,
	const CStringT &name
):	INodeModifier(parent, 0, name),
	axis_(axis),
	rate_(rate),
	current_(quat::identity()) {
}

} // namespace frm

#endif // FRAMEWORK_SPIN_CTRL_H_

