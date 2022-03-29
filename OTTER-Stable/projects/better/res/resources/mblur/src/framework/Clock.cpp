/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <windows.h> // for perf timer
#else
	#include <ctime> // for clock()
#endif

#include "Clock.h"
#include "common.h"

namespace frm {

/*******************************************************************************
	Clock implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
Clock::Clock()
:	ticks_(0), pticks_(0), totalTicks_(0), seconds_(0), totalSeconds_(0) {
//	get frequency/initialize current ticks:
#ifdef _WIN32
	LARGE_INTEGER tmp;
	QueryPerformanceFrequency(&tmp);
	freq_ = (IntegerT) tmp.QuadPart;
	QueryPerformanceCounter(&tmp);
	cticks_ = (IntegerT) tmp.QuadPart;
#else
	freq_ = (IntegerT) CLOCKS_PER_SEC;
	cticks_ = (IntegerT) clock();
#endif

//	get 1/frequency:
	rfreq_ = RealT(1) / (RealT) freq_;
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
void Clock::update() {
	pticks_ = cticks_;

//	get new value for current ticks:
#ifdef _WIN32
	LARGE_INTEGER tmp;
	QueryPerformanceCounter(&tmp);
	cticks_ = (Clock::IntegerT) tmp.QuadPart;
#else
	cticks_ = (Clock::IntegerT) clock();
#endif

//	calculate deltas:
	ticks_ = cticks_ - pticks_;
	totalTicks_ += ticks_;
	seconds_ = (Clock::RealT) ticks_ * rfreq_;
	totalSeconds_ += seconds_;
}


/*----------------------------------------------------------------------------*/
void Clock::reset() {
	ticks_ = 0;
	totalTicks_ = 0;
	seconds_ = 0;
	totalSeconds_ = 0;

//	get new value for current ticks:
#ifdef _WIN32
	LARGE_INTEGER tmp;
	QueryPerformanceCounter(&tmp);
	cticks_ = (Clock::IntegerT) tmp.QuadPart;
#else
	cticks_ = (Clock::IntegerT) clock();
#endif
}



} // namespace frm

