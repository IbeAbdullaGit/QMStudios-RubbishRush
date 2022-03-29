/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_CLOCK_H_
#define FRAMEWORK_CLOCK_H_

namespace frm {

class Clock {
public:
//	TYPEDEFS/ENUMS:
	typedef double RealT;
	typedef long long IntegerT;

//	CTORS/DTORS:
	/*	Default ctor. Initialize internals, 'start' the clock. */
	Clock();

//	ACCESSORS:
	/*	Return ticks/second. */
	IntegerT frequency() const;

	/*	Return number of ticks elapsed since the previous call to update().
		(number of ticks/second can be queried by calling frequency()).*/
	IntegerT ticks() const;

	/*	Return total number of ticks elapsed since the clock was initialized
		or since the previous call to \c reset(). */
	IntegerT totalTicks() const;

	/*	Return number of seconds elapsed since the previous call to update(). */
	RealT seconds() const;

	/*	Return total number of seconds elapsed since the clock was initialized
		or since the previous call to reset(). */
	RealT totalSeconds() const;

//	SERVICES:
	/*	Update the clock. */
	void update();

	/*	Reset the clock. */
	void reset();

private:
//	MEMBERS:
	/* Ticks since previous call to \c udpate(). */
	IntegerT ticks_;

	/* Ticks returned in previous two calls to \c update(). */
	IntegerT cticks_;
	IntegerT pticks_;

	/* Total ticks since clock initialized or previous call to \c reset(). */
	IntegerT totalTicks_;

	/* Ticks/second. */
	IntegerT freq_;

	/* 1/freq_, as a real type.*/
	RealT rfreq_;

	/* Seconds since previous call to \c update(). */
	RealT seconds_;

	/* Total seconds since clock initialized or previous call to \c reset().*/
	RealT totalSeconds_;

}; // class Clock

/*******************************************************************************
	Clock inline implementation:
*******************************************************************************/

//	PUBLIC:

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline Clock::IntegerT Clock::frequency() const {
	return freq_;
}

/*----------------------------------------------------------------------------*/
inline Clock::IntegerT Clock::ticks() const {
	return ticks_;
}

/*----------------------------------------------------------------------------*/
inline Clock::IntegerT Clock::totalTicks() const {
	return totalTicks_;
}

/*----------------------------------------------------------------------------*/
inline Clock::RealT Clock::seconds() const {
	return seconds_;
}

/*----------------------------------------------------------------------------*/
inline Clock::RealT Clock::totalSeconds() const {
	return totalSeconds_;
}

} // namespace frm

#endif // FRAMEWORK_CLOCK_H_

