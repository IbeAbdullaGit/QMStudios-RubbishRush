/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_TEXTURE3D_H_
#define FRAMEWORK_TEXTURE3D_H_

#include "common.h"
#include "ITexture.h"

namespace frm {

/*	*/
class Texture3d: public ITexture, public oogl::Texture3D {
public:
//	CTORS/DTORS:
	/*	Load from fileName. Assumes that slices are arranged sequentially (i.e.
		vertically) in the file. If filename already loaded (and target_ matches),
		don't reload. Return 0 on error. */
	static Texture3d* loadFromFile(
		const CStringT &fileName,
		unsigned optionMask = 0
	);

	/*	Upload from memory. */
	static Texture3d* create(
		GLint format,
		GLsizei width,
		GLsizei height,
		GLsizei depth,
		GLenum srcDataFormat = GL_RGBA,
		GLenum srcDataType = GL_UNSIGNED_BYTE,
		const GLvoid *srcData = (const GLvoid*) 0,
		unsigned optionMask = 0
	);

//	SERVICES:
	virtual bool reload();

private:
//	CTORS/DTORS:
	Texture3d(const CStringT &fileName, unsigned optionMask);

}; // class Texture2d

/*******************************************************************************
	Texture2d inline implementation:
*******************************************************************************/

//	PRIVATE:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline Texture3d::Texture3d(const CStringT &fileName, unsigned optionMask)
:	ITexture(fileName, optionMask) {
//	init() called by oogl::Texture3D ctor
}


} // namespace frm

#endif // FRAMEWORK_TEXTURE3D_H_

