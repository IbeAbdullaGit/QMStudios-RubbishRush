/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include <cstring> // strcmp

#include "TextureCube.h"
#include "common.h"

namespace frm {

/*******************************************************************************
	TextureCube implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
TextureCube* TextureCube::loadFromFile(
	const CStringT &fileName,
	unsigned optionMask
) {

	assert(fileName);

//	search for existing texture:
	for (int i = 0; i < nInstances(); ++i) {
		if (instance(i).getFileName()) {
			if (strcmp(instance(i).getFileName(), fileName) == 0) {
				oogl::logEvent("\tReusing previously loaded \"%s\"", fileName);
				return static_cast<TextureCube*>(&instance(i)); // KLUDGE!
			}
		}
	}

//	load texture via reload mechanism:
	TextureCube *result = new(std::nothrow) TextureCube(fileName, optionMask);
	assert(result);
	if (result->checkOptionMask(TEX_MIPMAP))
		result->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	else
		result->setMinFilter(GL_LINEAR);
	if (result->reload())
		return result;

	delete result;
	return 0;
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
bool TextureCube::reload() {
	if (fileName_) { // not a render texture
		GLsizei width, height, pixelSize;
		GLint format;
		GLenum srcDataFormat, srcDataType;
		void *srcData = loadRawData(fileName_, &width, &height, &pixelSize, &format, &srcDataFormat, &srcDataType);
		if (!srcData)
			return false;
	//	(re) upload to texture object:
		bind(); // must ensure texture is bound for upload operation!

//	the commented out width/height calcs are for the old horizontal cross method.
		int faceWidth = width;//width / 4;
		int faceHeight = height / 6;//height / 3;
		assert(faceWidth == faceHeight); // must be square

		//	use SRGB if necessary:
		if (checkOptionMask(TEX_SRGB)) {
			if (format == GL_RGB)
				format = GL_SRGB;
			else if (format == GL_SRGB_ALPHA)
				format = GL_SRGB_ALPHA;
		}

		/*if (!proxy(format, width, height, checkOptionMask(TEX_MIPMAP))) {
			oogl::logEvent("Error: could not create texture %ux%u %s", width, height, oogl::getEnumString(format));
			freeRawData(srcData);
			return false;
		}*/

//	this is the old way with the horizontal cross:
	/*	OOGL_CALL(glPixelStorei(GL_UNPACK_ROW_LENGTH, width));

		image(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, faceWidth, faceHeight, srcDataFormat, srcDataType, srcData + width * pixelSize * faceHeight + faceWidth * pixelSize * 2);
		image(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, faceWidth, faceHeight, srcDataFormat, srcDataType, srcData + width * pixelSize * faceHeight);
		image(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, faceWidth, faceHeight, srcDataFormat, srcDataType, srcData + faceWidth * pixelSize);
		image(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, faceWidth, faceHeight, srcDataFormat, srcDataType, srcData + width * pixelSize * faceHeight * 2 + faceWidth * pixelSize);
		image(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, faceWidth, faceHeight, srcDataFormat, srcDataType, srcData + width * pixelSize * faceHeight + faceWidth * pixelSize);
		image(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, faceWidth, faceHeight, srcDataFormat, srcDataType, srcData + width * pixelSize * faceHeight + faceWidth * pixelSize * 3);

	//	reset pixel unpack options:
		OOGL_CALL(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));*/

//	new way with a vertical format:
		int faceSize = faceWidth * faceHeight * pixelSize;
		image(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, faceWidth, faceHeight, srcDataFormat, srcDataType, srcData);
		image(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, faceWidth, faceHeight, srcDataFormat, srcDataType, srcData + faceSize);
		image(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, faceWidth, faceHeight, srcDataFormat, srcDataType, srcData + (faceSize * 2));
		image(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, faceWidth, faceHeight, srcDataFormat, srcDataType, srcData + (faceSize * 3));
		image(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, faceWidth, faceHeight, srcDataFormat, srcDataType, srcData + (faceSize * 4));
		image(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, faceWidth, faceHeight, srcDataFormat, srcDataType, srcData + (faceSize * 5));

		freeRawData(srcData);

		if (checkOptionMask(TEX_MIPMAP))
			generateMipmap();
	}

	return true;
}

} // namespace frm

