/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_MULTIBUF_H_
#define FRAMEWORK_MULTIBUF_H_

#include <iostream>
#include <vector>

#include "common.h"

namespace frm {

/* Multiplexed streambuf, writes to an array of streambufs. */
class multibuf: public std::streambuf {
public:
//	CTORS/DTORS:

//	SERVICES:
	/*	Add buf to the internal streambuf list. */
	void addbuf(std::streambuf *buf);

	/*	Return the ith streambuf. */
	std::streambuf* getbuf(int i);

protected:
//	MEMBERS:
	std::vector<std::streambuf*> buffers_;

//	SERVICES:
	/* Calls sputn on each internal streambuf. */
	//virtual std::streamsize xsputn(const std::streambuf::char_type *s, std::streamsize n);

	virtual int overflow (int c);

}; // class multibuf

/*******************************************************************************
	multibuf inline implementation:
*******************************************************************************/

//	PUBLIC:

//	SERVICES:

/*----------------------------------------------------------------------------*/
inline void multibuf::addbuf(std::streambuf *buf) {
	assert(buf);
	buffers_.push_back(buf);
}

/*----------------------------------------------------------------------------*/
inline std::streambuf* multibuf::getbuf(int i) {
	assert(i < buffers_.size());
	return buffers_[i];
}

//	PROTECTED:

//	SERVICES:

/*----------------------------------------------------------------------------*/
/*inline std::streamsize multibuf::xsputn(const std::streambuf::char_type *s, std::streamsize n) {
	for (int i = 0; i < buffers_.size(); ++i)
		buffers_[i]->sputn(s, n);
	return n;
}*/

/*----------------------------------------------------------------------------*/
inline int multibuf::overflow (int c) {
	for (int i = 0; i < buffers_.size(); ++i)
		buffers_[i]->sputc(c);
	return 1;
}

} // namespace frm

#endif // FRAMEWORK_MULTIBUF_H_


