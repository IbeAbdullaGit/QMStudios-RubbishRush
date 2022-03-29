/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_FRAMEBUFFER_H_
#define FRAMEWORK_FRAMEBUFFER_H_

#include "common.h"
#include "IResource.h"

namespace frm {

/*	*/
class Framebuffer: public oogl::Framebuffer, public IResource<Framebuffer> {
public:
//	CTORS/DTORS:
	static Framebuffer* create(const CStringT &name = "");

	/*	Create a 'defaul't framebuffer (null binding). Width/height must be
		specified to maintain compatibility with user defined framebuffers. */
	static Framebuffer* create(int width, int height, const CStringT &name = "");

	static void destroy(Framebuffer **ppFramebuffer);

private:
//	CTORS/DTORS:
	Framebuffer(const CStringT &name);
	Framebuffer(int width, int height, const CStringT &name);

}; // class Framebuffer

/*******************************************************************************
	Framebuffer inline implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline Framebuffer* Framebuffer::create(const CStringT &name) {
	Framebuffer *result = new(std::nothrow) Framebuffer(name);
	assert(result);
	return result;
}

/*----------------------------------------------------------------------------*/
inline Framebuffer* Framebuffer::create(int width, int height, const CStringT &name) {
	Framebuffer *result = new(std::nothrow) Framebuffer(width, height, name);
	assert(result);
	return result;
}

/*----------------------------------------------------------------------------*/
inline void Framebuffer::destroy(Framebuffer **ppFramebuffer) {
	assert(ppFramebuffer != 0);
	assert(*ppFramebuffer != 0);
	delete *ppFramebuffer;
	*ppFramebuffer = 0;
}

//	PRIVATE:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline Framebuffer::Framebuffer(const CStringT &name)
:	oogl::Framebuffer(),
	IResource(name) {
}

/*----------------------------------------------------------------------------*/
inline Framebuffer::Framebuffer(int width, int height, const CStringT &name)
:	oogl::Framebuffer(width, height),
	IResource(name) {
}

} // namespace frm

#endif // FRAMEWORK_FRAMEBUFFER_H_

