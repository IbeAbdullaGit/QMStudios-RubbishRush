/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_PROFILER_H_
#define FRAMEWORK_PROFILER_H_

#include <stack>
#include <vector>

#include "common.h"
#include "Clock.h"

namespace frm {

/*	Usage:
		Profiler::IdT pfidFRAME = Profiler::start("FRAME");
		//	... code to be profiled
		Profiler::stop(pfidFRAME); // or Profiler::stop("FRAME")


	If a block of profiled code is called multiple times, its call count is
	incremented. The duration of each call is summed and averaged during a call
	to reset; therefore the duration of samples reflects the average duration
	across multiple calls.
*/
class Profiler {
public:
//	TYPEDEFS/ENUMS:
	typedef int IdT;
	static const IdT INVALID_ID = -1;

	struct SampleT {
		int callCount;				// time called per frame
		Clock::RealT start;		// sample start time, in seconds since reset()
		Clock::RealT end;			// sample end time, in seconds since reset()
		Clock::RealT duration;	// end - start
	};

//	CTORS/DTORS:
	/*	Start/stop a profiler by name. If the profiler does not already exist,
		register a new one. Prefer to use ids as they don't require a search of
		the profiler names. */
	static IdT start(const CStringT &name);
	static void stop(const CStringT &name);

	/*	Start/stop a profiler by id. The profiler must already exist. */
	static IdT start(IdT id);
	static void stop(IdT id);

//	ACCESSORS:
	const CStringT& getName() const;
	void setName(const CStringT&);

	IdT getParentId() const;
	void setParentId(IdT);

	IdT getId() const;

	/*	The following apply across the life of the profiler, not just for the
		existing sample set. */
	Clock::RealT getMinDuration() const;
	Clock::RealT getMaxDuration() const;
	Clock::RealT getDurationRange() const;

	/*	Average is calculated only for the current sample set. */
	Clock::RealT getAverageDuration() const;

	/*	Return the ith sample; use the reference immediately and don't keep it
		around! */
	const SampleT& operator[](int i) const;

	/*	Number of samples per profiler; this is uniform across all profiler
		instances. */
	static int getBufferSize();
	static void setBufferSize(int);

	/*	Return the given profiler (by ID or name). */
	static const Profiler& getProfiler(IdT);
	static IdT getProfilerId(const CStringT&); // can return INVALID_ID

	/*	Get number of profilers. Profiler ids range in [0, nProfilers() - 1]. */
	static int getNProfilers();

//	SERVICES:
	/*	Advance index for data buffers, zero call counters. All times are
		measured relative to this call. */
	static void reset();

private:
//	MEMBERS:
	CStringT name_;
	IdT id_, parent_;
	Clock::RealT min_, max_, range_; // for duration
	std::vector<SampleT> samples_;

	static Clock clock_;
	static int bufferSize_; // size of sample data buffers
	static int currentIndex_; // index to current buffer position

	static std::vector<Profiler> profilers_;
	static std::stack<IdT> open_;	// stack of 'open' profilers. The current top
											// becomes the parent of the next

//	CTORS/DTORS:
	/*	Initializes data members*/
	Profiler(const CStringT &name, IdT id, IdT parent);

}; // class Profiler

/*******************************************************************************
	Profiler inline implementation:
*******************************************************************************/

//	PUBLIC:

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline const CStringT& Profiler::getName() const {
	return name_;
}
/*----------------------------------------------------------------------------*/
inline void Profiler::setName(const CStringT &name) {
	assert(name_);
	name_ = name;
}

/*----------------------------------------------------------------------------*/
inline Profiler::IdT Profiler::getParentId() const {
	return parent_;
}
/*----------------------------------------------------------------------------*/
inline void Profiler::setParentId(IdT id) {
	parent_ = id;
}

/*----------------------------------------------------------------------------*/
inline Profiler::IdT Profiler::getId() const {
	return id_;
}

/*----------------------------------------------------------------------------*/
inline Clock::RealT Profiler::getMinDuration() const {
	return min_;
}
/*----------------------------------------------------------------------------*/
inline Clock::RealT Profiler::getMaxDuration() const {
	return max_;
}
/*----------------------------------------------------------------------------*/
inline Clock::RealT Profiler::getDurationRange() const {
	return range_;
}

/*----------------------------------------------------------------------------*/
inline Clock::RealT Profiler::getAverageDuration() const {
	Clock::RealT sum = 0.0f;
	for (int i = 0; i < samples_.size(); ++i)
		sum += samples_[i].duration;
	return sum / (Clock::RealT)bufferSize_;
}

/*----------------------------------------------------------------------------*/
inline const Profiler::SampleT& Profiler::operator[](int i) const {
	assert(i >= 0 && i < bufferSize_);
	return samples_[(currentIndex_ + i) % bufferSize_];
}

/*----------------------------------------------------------------------------*/
inline int Profiler::getBufferSize() {
	return bufferSize_;
}

/*----------------------------------------------------------------------------*/
inline void Profiler::setBufferSize(int size) {
	assert(size > 0);
	bufferSize_ = size;
	for (int i = 0; i < profilers_.size(); ++i)
		profilers_[i].samples_.resize(bufferSize_);
}

/*----------------------------------------------------------------------------*/
inline const Profiler& Profiler::getProfiler(IdT i) {
	assert(i != INVALID_ID);
	assert(i >= 0 && i < profilers_.size());
	return profilers_[i];
}

/*----------------------------------------------------------------------------*/
inline int Profiler::getNProfilers() {
	return profilers_.size();
}

//	PRIVATE:

/*----------------------------------------------------------------------------*/
inline Profiler::Profiler(const CStringT &name, IdT id, IdT parent)
:	name_(name), id_(id), parent_(parent),
	min_(999999999.0f), max_(0.0f), range_(1.0f) {
	assert(name_);
	assert(id_ != INVALID_ID);
}

} // namespace frm

#endif // FRAMEWORK_PROFILER_H_
