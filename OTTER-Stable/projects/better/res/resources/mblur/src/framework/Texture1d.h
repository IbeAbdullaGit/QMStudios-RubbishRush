/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_TEXTURE1D_H_
#define FRAMEWORK_TEXTURE1D_H_

#include "common.h"
#include "ITexture.h"

namespace frm {

/*	*/
class Texture1d: public ITexture, public oogl::Texture1D {
public:
//	CTORS/DTORS:
	/*	Load from fileName. If filename already loaded (and target_ matches),
		don't reload. Return 0 on error. */
	static Texture1d* loadFromFile(
		const CStringT &fileName,
		unsigned optionMask = 0
	);

	/*	Upload from memory. */
	static Texture1d* create(
		GLint format,
		GLsizei width,
		GLenum srcDataFormat = GL_RGBA,
		GLenum srcDataType = GL_UNSIGNED_BYTE,
		const GLvoid *srcData = (const GLvoid*) 0,
		unsigned optionMask = 0
	);

//	SERVICES:
	virtual bool reload();

private:
//	CTORS/DTORS:
	Texture1d(const CStringT &fileName, unsigned optionMask);

}; // class Texture1d

/*******************************************************************************
	Texture1d inline implementation:
*******************************************************************************/

//	PRIVATE:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline Texture1d::Texture1d(const CStringT &fileName, unsigned optionMask)
:	ITexture(fileName, optionMask) {
//	init() called by oogl::Texture1d ctor
}


} // namespace frm

#endif // FRAMEWORK_TEXTURE1D_H_

