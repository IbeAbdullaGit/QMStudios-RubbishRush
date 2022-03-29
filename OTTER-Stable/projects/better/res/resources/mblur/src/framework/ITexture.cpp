/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/

#include "ITexture.h"
#include "common.h"
#include "stb_image.h"

namespace frm {

/*******************************************************************************
	ITexture implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
void ITexture::destroy(ITexture **ppTexture) {
	assert(ppTexture != 0);
	assert(*ppTexture != 0);
	delete *ppTexture;
	*ppTexture = 0;
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
bool ITexture::reloadAll() {
	bool result = true;
	for (size_t i = 0; i < nInstances(); ++i)
		result &= instance(i).reload();
	glFinish();
	return result;
}

//	PRIVATE:

//	SERVICES:

/*----------------------------------------------------------------------------*/
void* ITexture::loadRawData(
	const CStringT &fileName,
	GLsizei *width, GLsizei *height,
	GLsizei *pixelSize,
	GLint *format,
	GLenum *srcDataFormat,
	GLenum *srcDataType
) {
	oogl::logEvent("Loading \"%s\"...", fileName);

	GLsizei width_, height_, pixelSize_;
	GLint format_;
	GLenum srcDataFormat_, srcDataType_;
	void *result = 0;
	int cmp;

	if (stbi_is_hdr(fileName)) {
	//	if hdr, load floating point:
		result = stbi_loadf(fileName, &width_, &height_, &cmp, 0);
		if (!result)
			goto FAILURE;
		switch (cmp) {
			case 1:	format_ = GL_R16F;		srcDataFormat_ = GL_RED;		break;
			case 2:	format_ = GL_RG16F;		srcDataFormat_ = GL_RG;			break;
			case 3:	format_ = GL_RGB16F;		srcDataFormat_ = GL_RGB;		break;
			case 4:
			default:	format_ = GL_RGBA16F;	srcDataFormat_ = GL_RGBA;		break;
		};
		srcDataType_ = GL_FLOAT;
		pixelSize_ = cmp * sizeof(float); // DON'T USE GL_FLOAT
	} else {
	//	else load unsigned char data:
		result = stbi_load(fileName, &width_, &height_, &cmp, 0);
		if (!result)
			goto FAILURE;
		switch (cmp) {
			case 1:	srcDataFormat_ = format_ = GL_RED;	break;
			case 2:	srcDataFormat_ = format_ = GL_RG;	break;
			case 3:	srcDataFormat_ = format_ = GL_RGB;	break;
			case 4:
			default:	srcDataFormat_ = format_ = GL_RGBA;	break;
		};
		srcDataType_ = GL_UNSIGNED_BYTE;
		pixelSize_ = cmp * sizeof(unsigned char); // DON'T USE GL_UNSIGNED_BYTE
	}

//	set outputs:
	if (width)
		*width = width_;
	if (height)
		*height = height_;
	if (pixelSize)
		*pixelSize = pixelSize_;
	if (format)
		*format = format_;
	if (srcDataFormat)
		*srcDataFormat = srcDataFormat_;
	if (srcDataType)
		*srcDataType = srcDataType_;
	oogl::logEvent("...DONE");
	return result;

FAILURE:
	oogl::logEvent("\tError loading \"%s\"", stbi_failure_reason());
	return 0;
}

/*----------------------------------------------------------------------------*/
void ITexture::freeRawData(void *rawData) {
	assert(rawData);
	stbi_image_free(rawData);
}

/*----------------------------------------------------------------------------*/
void ITexture::flipRawDataY(void *rawData, int width, int height, int pixelSize) {
	assert(rawData);

	int imgSize = width * height * pixelSize;
	int rowSize = width * pixelSize;
	char *img = (char*)rawData; // access as a char array
	char *tmp = new(std::nothrow) char[rowSize];
	assert(tmp);

	for (int i = 0; i < height / 2; ++i) { // for each row
		int rowStart = i * rowSize; // start of current row
		assert(rowStart < imgSize);
		int rowSwap = imgSize - rowSize - rowStart; // start of row to swap with
		for (int j = 0; j < rowSize; ++j)
			tmp[j] = img[rowStart + j];
		for (int j = 0; j < rowSize; ++j)
			img[rowStart + j] = img[rowSwap + j];
		for (int j = 0; j < rowSize; ++j)
			img[rowSwap + j] = tmp[j];
	}

	delete[] tmp;
}

} // namespace frm
