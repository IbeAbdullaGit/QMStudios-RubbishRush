/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_TEXTURECUBE_H_
#define FRAMEWORK_TEXTURECUBE_H_

#include "common.h"
#include "ITexture.h"

namespace frm {

/*	*/
class TextureCube: public ITexture, public oogl::TextureCube {
public:
//	CTORS/DTORS:
	/*	Load from fileName. If filename already loaded (and target_ matches),
		don't reload. Return 0 on error. Assumes the following format for cube
		map faces:

					y+
			x-		z+		x+		z-
					y-

		*/
	static TextureCube* loadFromFile(
		const CStringT &fileName,
		unsigned optionMask = 0
	);

//	SERVICES:
	virtual bool reload();

private:
//	CTORS/DTORS:
	TextureCube(const CStringT &fileName, unsigned optionMask);

}; // class TextureCube

/*******************************************************************************
	TextureCube inline implementation:
*******************************************************************************/

//	PRIVATE:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline TextureCube::TextureCube(const CStringT &fileName, unsigned optionMask)
:	ITexture(fileName, optionMask) {
//	init() called by oogl::TextureCube ctor
}


} // namespace frm

#endif // FRAMEWORK_TEXTURECUBE_H_

