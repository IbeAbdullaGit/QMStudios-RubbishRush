/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_TEXTURE2D_H_
#define FRAMEWORK_TEXTURE2D_H_

#include "common.h"
#include "ITexture.h"

namespace frm {

/*	*/
class Texture2d: public ITexture, public oogl::Texture2D {
public:
//	CTORS/DTORS:
	/*	Load from fileName. If filename already loaded (and target_ matches),
		don't reload. Return 0 on error. */
	static Texture2d* loadFromFile(
		const CStringT &fileName,
		unsigned optionMask = 0
	);

	/*	Create a render texture or upload from memory. */
	static Texture2d* create(
		GLint format,
		GLsizei width,
		GLsizei height,
		GLenum srcDataFormat = GL_RGBA,
		GLenum srcDataType = GL_UNSIGNED_BYTE,
		const GLvoid *srcData = (const GLvoid*) 0,
		unsigned optionMask = 0
	);

//	SERVICES:
	virtual bool reload();

private:
//	CTORS/DTORS:
	Texture2d(const CStringT &fileName, unsigned optionMask);

}; // class Texture2d

/*******************************************************************************
	Texture2d inline implementation:
*******************************************************************************/

//	PRIVATE:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline Texture2d::Texture2d(const CStringT &fileName, unsigned optionMask)
:	ITexture(fileName, optionMask) {
//	init() called by oogl::Texture2D ctor
}


} // namespace frm

#endif // FRAMEWORK_TEXTURE2D_H_
