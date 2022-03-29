/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_ITEXTURE_H_
#define FRAMEWORK_ITEXTURE_H_

#include "common.h"
#include "IResource.h"

namespace frm {

/*	Base class for texture objects. Use as a mixin with oogl::Texture{1D, 2D, etc}
	to get objects which work as textures and as resources.*/
class ITexture: public IResource<ITexture> {
public:
//	TYPEDEFS/ENUMS:
	enum OptionMaskT{
		TEX_SRGB				= 1,
		TEX_MIPMAP			= 2,
		TEX_SRGB_MIPMAP	= 3
	};

//	CTORS/DTORS:
	static void destroy(ITexture **ppTexture);

//	ACCESSORS:
	const CStringT& getFileName() const;
	void setFileName(const CStringT&);

	unsigned getOptionMask() const;
	void setOptionMask(OptionMaskT);
	bool checkOptionMask(OptionMaskT);

//	SERVICES:
	/*	(Re)load this texture object (if associated with a filename). On failure
		return 0, leave resource unchanged. */
	virtual bool reload() = 0;

	/*	Calls reload() on all ITexture instances. */
	static bool reloadAll();

protected:
//	MEMBERS:
	CStringT fileName_; // can be 0, represents a render texture
	unsigned optionMask_; // comination of OptionMastT, see above

//	CTORS/DTORS:
	ITexture(const CStringT &fileName, unsigned optionMask);
	/*	Dtor needs to be virtual, as you'll be deleting all textures via an
		ITexture ptr. */
	virtual ~ITexture();

//	SERVICES:
	/*	Load raw texture data from the given filename. Return 0 on failure. */
	static void* loadRawData(
		const CStringT &fileName,
		GLsizei *width, GLsizei *height, // dimensions
		GLsizei *pixelSize, // size (bytes) of 1 pixel
		GLint *format, // internal format to pass to glTexImage*()
		GLenum *srcDataFormat, // format of the returned data
		GLenum *srcDataType // type of the returned data
	);

	/*	Release a raw data block allocated during a previous call to loadRawData*/
	static void freeRawData(void *rawData);

	/*	Flips raw texture data vertically (use to conform with OpenGL's bottom-
		left origin). This is done in-place. pixelSize is the size of a pixel in
		bytes, so an RGB float format will be 3 * sizeof(float), etc. width and
		height are the dimensions of the image. */
	static void flipRawDataY(void *rawData, int width, int height, int pixelSize);

}; // class ITexture

/*******************************************************************************
	ITexture inline implementation:
*******************************************************************************/

//	PUBLIC:

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline const CStringT& ITexture::getFileName() const {
	return fileName_;
}
/*----------------------------------------------------------------------------*/
inline void ITexture::setFileName(const CStringT &fileName) {
	assert(fileName);
	fileName_ = fileName;
}
/*----------------------------------------------------------------------------*/
inline unsigned ITexture::getOptionMask() const {
	return optionMask_;
}
/*----------------------------------------------------------------------------*/
inline void ITexture::setOptionMask(OptionMaskT mask) {
	optionMask_ = mask;
}
/*----------------------------------------------------------------------------*/
inline bool ITexture::checkOptionMask(OptionMaskT opt) {
	return opt & optionMask_;
}

//	PROTECTED:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline ITexture::ITexture(const CStringT &fileName, unsigned optionMask)
:	IResource(fileName),
	fileName_(fileName),
	optionMask_(optionMask) {
}

/*----------------------------------------------------------------------------*/
inline ITexture::~ITexture() {
}


} // namespace frm

#endif // FRAMEWORK_ITEXTURE_H_

