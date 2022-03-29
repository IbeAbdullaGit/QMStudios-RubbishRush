/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include <cstring> // strcmp

#include "Texture1d.h"
#include "common.h"

namespace frm {

/*******************************************************************************
	Texture1d implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
Texture1d* Texture1d::loadFromFile(
	const CStringT &fileName,
	unsigned optionMask
) {

	assert(fileName);

//	search for existing texture:
	for (int i = 0; i < nInstances(); ++i) {
		if (instance(i).getFileName()) {
			if (strcmp(instance(i).getFileName(), fileName) == 0) {
				oogl::logEvent("\tReusing previously loaded \"%s\"", fileName);
				return static_cast<Texture1d*>(&instance(i)); // KLUDGE!
			}
		}
	}

//	load texture via reload mechanism:
	Texture1d *result = new(std::nothrow) Texture1d(fileName, optionMask);
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

/*----------------------------------------------------------------------------*/
Texture1d* Texture1d::create(
	GLint format,
	GLsizei width,
	GLenum srcDataFormat,
	GLenum srcDataType,
	const GLvoid *srcData,
	unsigned optionMask
) {
	/*if (!proxy(format, width, height, (GLboolean)(optionMask & TEX_MIPMAP))) {
		oogl::logEvent("Error: could not create texture %ux%u %s", width, height, oogl::getEnumString(format));
		return false;
	}*/

	Texture1d *result = new(std::nothrow) Texture1d("", optionMask);
	assert(result);
	if (result->checkOptionMask(TEX_MIPMAP))
		result->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	else
		result->setMinFilter(GL_LINEAR);
	result->image(0, format, width, srcDataFormat, srcDataType, srcData);
	return result;
}


//	SERVICES:

/*----------------------------------------------------------------------------*/
bool Texture1d::reload() {
	if (fileName_) { // not a render texture
		GLsizei width, height, pixelSize;
		GLint format;
		GLenum srcDataFormat, srcDataType;
		void *srcData = loadRawData(fileName_, &width, &height, &pixelSize, &format, &srcDataFormat, &srcDataType);
		if (!srcData)
			return false;
	//	(re) upload to texture object:
		bind(); // must ensure texture is bound for upload operation!

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

		image(0, format, width, srcDataFormat, srcDataType, srcData);
		freeRawData(srcData);

		if (checkOptionMask(TEX_MIPMAP))
			generateMipmap();
	}

	return true;
}

} // namespace frm

