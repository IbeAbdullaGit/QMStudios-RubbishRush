/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef NDEBUG
	#define OOGL_DEBUG
#endif
#include <oogl/oogl.h>
#include <oogl/utils.h>

#ifndef NDEBUG
	#define SML_DEBUG
	//#define SML_STRICT_DEBUG
#endif
#define SML_NO_ALIGN
#include <sml/sml.h>
#include <sml/itpl.h>
#include <sml/random.h>

#include "framework/framework.h"

/*	*/
class AppMblur: public frm::App3d {
public:
//	CTORS/DTORS:
	AppMblur(
		const frm::CStringT &appName,
		const frm::ivec2 &windowSize,
		const frm::ivec2 &renderSize,
		bool fullscreen = false
	);

protected:
//	MEMBERS:
	frm::TextureCube *texEnvMap_;
	static frm::CStringT envMapList_[];
	int currentEnvMap_;

	frm::Shader *shaderPostProcess_;
	float exposure_; // exposure override

	//	render textures/framebuffers:
	frm::Framebuffer *fboHdr_;
	frm::Texture2d *texHdr_, *texVelocity_, *texDepth_;

	float maxMotionBlurSamples_, targetFps_;
	bool doMotionBlur_; // turn mblur on/off
	bool doFixedSampling_; // turn fixed sampling on/off (see postprocess.fs.glsl)

	//	auto exposure:
	frm::Framebuffer *fboLuma_, *fboAdaptLuma_[2];
	frm::Texture2d *texLuma_, *texAdaptLuma_[2];
	frm::Shader *shaderLuminance_, *shaderAvgMinMax_, *shaderLuminanceAdapt_;
	int currentAdaptLuma_; // swap between fboAdaptLuma_ per frame
	float adaptionRate_;

//	SERVICES:
	/*	Initialize scene resources/scene graph. */
	virtual bool initScene();

	/*	Initialize GUI components. */
	virtual bool initGui();

	/*	Initialize render textures/framebuffers/shaders. */
	virtual bool initRender();

	/*	Custom event handling for this. */
	virtual bool handleEvent(const sf::Event &event);

	/*	Apply a Gaussian blur to the 0th color attachment of input fbo. The result
		is written to output fbo, which may be the same as in. */
	void gaussBlur(
		frm::Framebuffer *in,
		frm::Framebuffer *aux,
		frm::Framebuffer *out, // can be the same as in
		int radius // blur kernel radius in texels
	);

	/*	Renders to every mip level of the 0th color attachment of input fbo.
		Assumes that shader is already in use. */
	void renderToMipmap(
		frm::Framebuffer *fbo,
		frm::Shader *shader,
		int bindLocation = 0 // used as GL_TEXTURE0 + bindLocation
	);

	/*	Render frame. */
	virtual bool render(float dt);
};

/*******************************************************************************
	AppMblur inline implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
AppMblur::AppMblur(
	const frm::CStringT &appName,
	const frm::ivec2 &windowSize,
	const frm::ivec2 &renderSize,
	bool fullscreen
):	App3d(appName, windowSize, renderSize, fullscreen),
	currentEnvMap_(0),
	exposure_(1.0f),
	maxMotionBlurSamples_(32.0f),
	targetFps_(60.0f),
	doMotionBlur_(true),
	doFixedSampling_(false),
	currentAdaptLuma_(0),
	adaptionRate_(2.5f) {
}

