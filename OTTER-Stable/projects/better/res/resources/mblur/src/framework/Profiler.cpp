/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include <cstring> // strcmp

#include "Profiler.h"
#include "common.h"

namespace frm {

/*******************************************************************************
	Profiler implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
Profiler::IdT Profiler::start(const CStringT &name) {
	IdT i = getProfilerId(name); // search for existing profiler
	if (i == INVALID_ID) { // not found, add new
		i = profilers_.size();
		IdT parent = INVALID_ID;
		if (open_.size() > 0)
			parent = open_.top();
		profilers_.push_back(Profiler(name, i, parent));
		profilers_[i].samples_.resize(bufferSize_);
	}
	return start(i);
}

/*----------------------------------------------------------------------------*/
void Profiler::stop(const CStringT &name) {
	stop(getProfilerId(name));
}

/*----------------------------------------------------------------------------*/
Profiler::IdT Profiler::start(IdT i) {
	assert(i != INVALID_ID);
	assert(i >= 0 && i < profilers_.size());

	open_.push(i);
	SampleT& cs = profilers_[i].samples_[currentIndex_];
	++cs.callCount;

//	glFinish() must be called prior to starting the clock to ensure that the
//	command buffer is flushed and all GPU operations are complete. This ensures
//	that the profiler accurately represents only the intended operations between
//	start() and stop() calls
	glFinish();

//	start the timer as late as possible:
	clock_.update();
	cs.start = clock_.totalSeconds();

	return profilers_[i].getId();
}

/*----------------------------------------------------------------------------*/
void Profiler::stop(IdT i) {
//	as above; ensure all GPU operations are complete, stop the clock as soon as
//	possible
	glFinish();
	clock_.update();

	assert(i != INVALID_ID);
	assert(i >= 0 && i < profilers_.size());
	assert(open_.top() == i); // make sure that start/stop calls were matched

	open_.pop();
	Profiler &cp = profilers_[i];
	SampleT& cs = cp.samples_[currentIndex_];
	cs.end = clock_.totalSeconds();
	cs.duration += cs.end - cs.start;

	cp.min_ = sml::min(cp.min_, cs.duration);
	cp.max_ = sml::max(cp.max_, cs.duration);
	cp.range_ = cp.max_ - cp.min_;
}


//	ACCESSORS:

/*----------------------------------------------------------------------------*/
Profiler::IdT Profiler::getProfilerId(const CStringT &name) {
	assert(name);
	for (IdT i = 0; i < profilers_.size(); ++i)
		if (strcmp(profilers_[i].name_, name) == 0)
			return i; // found

	return INVALID_ID; // not found
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
void Profiler::reset() {
	for (IdT i = 0; i < profilers_.size(); ++i)
		profilers_[i].samples_[currentIndex_].duration /= (Clock::RealT)profilers_[i].samples_[currentIndex_].callCount;

	currentIndex_ = (currentIndex_ + 1) % bufferSize_;
	for (IdT i = 0; i < profilers_.size(); ++i) {
		profilers_[i].samples_[currentIndex_].callCount = 0;
		profilers_[i].samples_[currentIndex_].duration = 0;
	}
	clock_.reset();
}


//	PRIVATE:

//	MEMBERS:
Clock Profiler::clock_;
int Profiler::bufferSize_ = 1;
int Profiler::currentIndex_ = 0;

std::vector<Profiler> Profiler::profilers_;
std::stack<Profiler::IdT> Profiler::open_;

} // namespace frm

