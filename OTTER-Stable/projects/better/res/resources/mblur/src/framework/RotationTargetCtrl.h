/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_ROTATION_TARGET_CTRL_H_
#define FRAMEWORK_ROTATION_TARGET_CTRL_H_

#include "common.h"
#include "INodeModifier.h"

namespace frm {

/*	 */
template <template <typename> class I = sml::LERP>
class RotationTargetCtrl: public INodeModifier {
public:
//	CTORS/DTORS:
	static RotationTargetCtrl<I>* create(
		const quat &start,
		const quat &end,
		TimeT duration,
		CallbackT onComplete = 0,
		const CStringT &name = "RotationTargetCtrl"
	);

	static RotationTargetCtrl<I>* create(
		const quat &start,
		const quat &end,
		TimeT duration,
		TimeT offset, // in seconds
		CallbackT onComplete = 0,
		const CStringT &name = "RotationTargetCtrl"
	);

//	ACCESSORS:

//	SERVICES:
	/*	Return target position to start. */
	virtual void reset();

	/*	Move end to end + (end - start). */
	virtual void relativeReset();

	/*	Swap start/end, set current time to duration - current time.*/
	virtual void reverse();

	/* Interpolate between start and end at the current time. */
	virtual void update(TimeT dt);

protected:
//	MEMBERS:
	quat start_, end_, rotation_;
	TimeT duration_, currentTime_;

//	CTORS/DTORS:
	RotationTargetCtrl(
		Node *parent,
		const quat &start,
		const quat &end,
		TimeT duration,
		TimeT offset,
		CallbackT onComplete,
		const CStringT &name
	);

}; // class RotationTargetCtrl

/*******************************************************************************
	RotationTargetCtrl inline implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
template <template <typename> class I>
inline RotationTargetCtrl<I>* RotationTargetCtrl<I>::create(
	const quat &start,
	const quat &end,
	TimeT duration,
	CallbackT onComplete,
	const CStringT &name
) {
	RotationTargetCtrl<I> *result = new(std::nothrow) RotationTargetCtrl<I>(
		0,
		start,
		end,
		duration,
		0, // offset,
		onComplete,
		name
	);
	assert(result);
	return result;
}

/*----------------------------------------------------------------------------*/
template <template <typename> class I>
inline RotationTargetCtrl<I>* RotationTargetCtrl<I>::create(
	const quat &start,
	const quat &end,
	TimeT duration,
	TimeT offset,
	CallbackT onComplete,
	const CStringT &name
) {
	RotationTargetCtrl<I> *result = new(std::nothrow) RotationTargetCtrl<I>(
		0,
		start,
		end,
		duration,
		offset,
		onComplete,
		name
	);
	assert(result);
	return result;
}

//	ACCESSORS:

//	SERVICES:

/*----------------------------------------------------------------------------*/
template <template <typename> class I>
inline void RotationTargetCtrl<I>::reset() {
	currentTime_ = 0;
}

/*----------------------------------------------------------------------------*/
template <template <typename> class I>
inline void RotationTargetCtrl<I>::relativeReset() {
//	q1^-1 * q2 represents a rotation from q1 to q2
	end_ = sml::normalize(rotation_ * (sml::conjugate(start_) * end_));
	start_ = sml::normalize(rotation_);
	currentTime_ = 0;
}

/*----------------------------------------------------------------------------*/
template <template <typename> class I>
inline void RotationTargetCtrl<I>::reverse() {
	quat tmp = end_;
	end_ = start_;
	start_ = tmp;
	currentTime_ = duration_ - currentTime_;
}

/*----------------------------------------------------------------------------*/
template <template <typename> class I>
inline void RotationTargetCtrl<I>::update(TimeT dt) {
	currentTime_ = sml::clamp(currentTime_ + dt, 0.0f, duration_);
	TimeT delta = currentTime_ / duration_;
	I<quat> itpl;
	rotation_ = normalize(itpl(start_, end_, delta));
	getParent()->setLocalMatrix(
		getParent()->getLocalMatrix() * mat4::rotation3(rotation_)
	);
	if (delta >= 1) {
		if (onComplete_)
			onComplete_(*this);
	}
}

//	PROTECTED:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
template <template <typename> class I>
inline RotationTargetCtrl<I>::RotationTargetCtrl(
	Node *parent,
	const quat &start,
	const quat &end,
	TimeT duration,
	TimeT offset,
	CallbackT onComplete,
	const CStringT &name
):	INodeModifier(parent, onComplete, name),
	start_(start),
	end_(end),
	rotation_(start),
	duration_(duration),
	currentTime_(offset) {

	if (currentTime_ > duration_) {
		currentTime_ = sml::fract(currentTime_);
	}
}

} // namespace frm

#endif // FRAMEWORK_ROTATION_TARGET_CTRL_H_
