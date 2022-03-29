/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_BUFFER_H_
#define FRAMEWORK_BUFFER_H_

#include "common.h"
#include "IResource.h"

namespace frm {

/* */
class Buffer: public oogl::Buffer, public IResource<Buffer> {
public:
//	CTORS/DTORS:
	static Buffer *create(
		GLenum target,
		GLenum usage,
		GLsizeiptr srcDataSize,
		const GLvoid *srcData = 0,
		const CStringT &name = ""
	);
	static void destroy(Buffer **ppBuffer);

//	ACCESSORS:

//	SERVICES:

protected:
//	MEMBERS:

//	CTORS/DTORS:
	Buffer(
		GLenum target,
		GLenum usage,
		GLsizeiptr srcDataSize,
		const GLvoid *srcData,
		const CStringT &name
	);
	/*	Dtor needs to be virtual, as Buffer can be derived from. */
	virtual ~Buffer();

}; // class Buffer

/*******************************************************************************
	Buffer inline implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline Buffer* Buffer::create(
	GLenum target,
	GLenum usage,
	GLsizeiptr srcDataSize,
	const GLvoid *srcData,
	const CStringT &name
) {
	Buffer *result = new(std::nothrow) Buffer(target, usage, srcDataSize, srcData, name);
	assert(result);
	return result;
}

/*----------------------------------------------------------------------------*/
inline void Buffer::destroy(Buffer **ppBuffer) {
	assert(ppBuffer != 0);
	assert(*ppBuffer != 0);
	delete *ppBuffer;
	*ppBuffer = 0;
}

//	PROTECTED:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline Buffer::Buffer(
	GLenum target,
	GLenum usage,
	GLsizeiptr srcDataSize,
	const GLvoid *srcData,
	const CStringT &name
):	oogl::Buffer(target, usage, srcDataSize, srcData),
	IResource(name) {
}

/*----------------------------------------------------------------------------*/
inline Buffer::~Buffer() {
}


} // namespace frm

#endif // FRAMEWORK_BUFFER_H_


