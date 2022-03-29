/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_POSITION_TARGET_CTRL_H_
#define FRAMEWORK_POSITION_TARGET_CTRL_H_

#include "common.h"
#include "INodeModifier.h"

namespace frm {

/*	 */
template <template <typename> class I = sml::LERP>
class PositionTargetCtrl: public INodeModifier {
public:
//	CTORS/DTORS:
	static PositionTargetCtrl<I>* create(
		const vec3 &start,
		const vec3 &end,
		TimeT duration,
		CallbackT onComplete = 0,
		const CStringT &name = "PositionTargetCtrl"
	);

	static PositionTargetCtrl<I>* create(
		const vec3 &start,
		const vec3 &end,
		TimeT duration,
		TimeT offset, // in seconds
		CallbackT onComplete = 0,
		const CStringT &name = "PositionTargetCtrl"
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
	vec3 start_, end_, position_;
	TimeT duration_, currentTime_;

//	CTORS/DTORS:
	PositionTargetCtrl(
		Node *parent,
		const vec3 &start,
		const vec3 &end,
		TimeT duration,
		TimeT offset,
		CallbackT onComplete,
		const CStringT &name
	);

}; // class PositionTargetCtrl

/*******************************************************************************
	PositionTargetCtrl inline implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
template <template <typename> class I>
inline PositionTargetCtrl<I>* PositionTargetCtrl<I>::create(
	const vec3 &start,
	const vec3 &end,
	TimeT duration,
	CallbackT onComplete,
	const CStringT &name
) {
	PositionTargetCtrl<I> *result = new(std::nothrow) PositionTargetCtrl<I>(
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
inline PositionTargetCtrl<I>* PositionTargetCtrl<I>::create(
	const vec3 &start,
	const vec3 &end,
	TimeT duration,
	TimeT offset,
	CallbackT onComplete,
	const CStringT &name
) {
	PositionTargetCtrl<I> *result = new(std::nothrow) PositionTargetCtrl<I>(
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
inline void PositionTargetCtrl<I>::reset() {
	currentTime_ = 0;
}

/*----------------------------------------------------------------------------*/
template <template <typename> class I>
inline void PositionTargetCtrl<I>::relativeReset() {
	end_ = position_ + (end_ - start_);
	start_ = position_;
	currentTime_ = 0;
}

/*----------------------------------------------------------------------------*/
template <template <typename> class I>
inline void PositionTargetCtrl<I>::reverse() {
	vec3 tmp = end_;
	end_ = start_;
	start_ = tmp;
	currentTime_ = duration_ - currentTime_;
}

/*----------------------------------------------------------------------------*/
template <template <typename> class I>
inline void PositionTargetCtrl<I>::update(TimeT dt) {
	currentTime_ = sml::clamp(currentTime_ + dt, 0.0f, duration_);
	TimeT delta = currentTime_ / duration_;
	I<vec3> itpl;
	position_ = itpl(start_, end_, delta);
	getParent()->setLocalMatrix(
		getParent()->getLocalMatrix() * mat4::translation3(position_)
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
inline PositionTargetCtrl<I>::PositionTargetCtrl(
	Node *parent,
	const vec3 &start,
	const vec3 &end,
	TimeT duration,
	TimeT offset,
	CallbackT onComplete,
	const CStringT &name
):	INodeModifier(parent, onComplete, name),
	start_(start),
	end_(end),
	position_(start),
	duration_(duration),
	currentTime_(offset) {

	if (currentTime_ > duration_) {
		currentTime_ = sml::fract(currentTime_);
	}
}

} // namespace frm

#endif // FRAMEWORK_POSITION_TARGET_CTRL_H_

