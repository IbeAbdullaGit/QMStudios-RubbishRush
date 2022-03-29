/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_IMATERIAL_H_
#define FRAMEWORK_IMATERIAL_H_

#include "common.h"
#include "IResource.h"
#include "Shader.h"
#include "ShaderUnit.h"

namespace frm {

/* Base class for materials. Handles rasterizer state + shader. Deriving classes
	should call the base class enable() to set the render state/enable the
	shader. */
class IMaterial: public IResource<IMaterial> {
public:
//	TYPEDEFS/ENUMS:
	enum BlendT {
		BLEND_NONE = 0,
		BLEND_ADD,
		BLEND_SCREEN,
		BLEND_LIGHTEN,
		BLEND_MULTIPLY,
		BLEND_DARKEN,
		BLEND_LINEAR_BURN,
		BLEND_ALPHA
	};

	struct RenderStateT {
		BlendT blend;
		GLenum depthMode; // GL_LESS, GL_EQUAL, etc. or GL_NONE
		GLenum polyMode; // GL_POINT, GL_LINE or GL_FILL
		GLenum cullFace; // GL_FRONT, GL_BACK or GL_NONE
		GLboolean depthMask;

		inline RenderStateT(
			BlendT _blend			= BLEND_NONE,
			GLenum _depthMode		= GL_LESS,
			GLboolean _depthMask	= GL_TRUE,
			GLenum _cullFace		= GL_BACK,
			GLenum _polyMode		= GL_FILL
		): blend(_blend),
			depthMode(_depthMode),
			depthMask(_depthMask),
			cullFace(_cullFace),
			polyMode(_polyMode) {

		}
	};

//	CTORS/DTORS:
	static IMaterial* create(ShaderUnit *pShader, const CStringT &name = "");
	static void destroy(IMaterial **ppMaterial);

//	ACCESSORS:
	ShaderUnit* getFragmentShader() const;
	void setFragmentShader(ShaderUnit *pShader);

	const RenderStateT& getRenderState() const;
	RenderStateT& getRenderState();
	void setRenderState(const RenderStateT&);

//	SERVICES:
	/*	Enable render state, bind textures, pass uniforms. */
	virtual void enable(Shader *pShader, AppBase *pApp);

	/*	Restore render state. */
	virtual void disable();

protected:
//	MEMBERS:
	RenderStateT renderState_;
	ShaderUnit *fragmentShader_;

//	CTORS/DTORS:
	IMaterial(ShaderUnit *pShader, const CStringT &name);
	/*	Dtor needs to be virtual, as you'll be deleting all material objects via
		an IMaterial ptr. */
	virtual ~IMaterial();

}; // class IMaterial

/*******************************************************************************
	IMaterial inline implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline IMaterial* IMaterial::create(ShaderUnit *pShader, const CStringT &name) {
	IMaterial *result = new(std::nothrow) IMaterial(pShader, name);
	assert(result);
	return result;
}
/*----------------------------------------------------------------------------*/
inline void IMaterial::destroy(IMaterial **ppMaterial) {
	assert(ppMaterial != 0);
	assert(*ppMaterial != 0);
	delete *ppMaterial;
	*ppMaterial = 0;
}

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline ShaderUnit* IMaterial::getFragmentShader() const {
	return fragmentShader_;
}
/*----------------------------------------------------------------------------*/
inline void IMaterial::setFragmentShader(ShaderUnit *pShader) {
	assert(pShader);
	fragmentShader_ = pShader;
}

/*----------------------------------------------------------------------------*/
inline const IMaterial::RenderStateT& IMaterial::getRenderState() const {
	return renderState_;
}
/*----------------------------------------------------------------------------*/
inline IMaterial::RenderStateT& IMaterial::getRenderState() {
	return renderState_;
}
/*----------------------------------------------------------------------------*/
inline void IMaterial::setRenderState(const RenderStateT &state) {
	renderState_ = state;
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
inline void IMaterial::enable(Shader *pShader, AppBase *pApp) {
	assert(pShader);
	assert(pApp);

//	set render state:
	if (renderState_.blend != BLEND_NONE) {
		OOGL_CALL(glEnable(GL_BLEND));
		switch (renderState_.blend) {
			case BLEND_ADD:
				OOGL_CALL(glBlendFunc(GL_ONE, GL_ONE));
				OOGL_CALL(glBlendEquation(GL_FUNC_ADD));
				break;
			case BLEND_SCREEN:
				OOGL_CALL(glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR));
				OOGL_CALL(glBlendEquation(GL_FUNC_ADD));
				break;
			case BLEND_LIGHTEN:
				OOGL_CALL(glBlendFunc(GL_ONE, GL_ONE));
				OOGL_CALL(glBlendEquation(GL_MAX));
				break;
			case BLEND_MULTIPLY:
				OOGL_CALL(glBlendFunc(GL_DST_COLOR, GL_ZERO));
				OOGL_CALL(glBlendEquation(GL_FUNC_ADD));
				break;
			case BLEND_DARKEN:
				OOGL_CALL(glBlendFunc(GL_ONE, GL_ONE));
				OOGL_CALL(glBlendEquation(GL_MIN));
				break;
			case BLEND_LINEAR_BURN:
				OOGL_CALL(glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR));
				OOGL_CALL(glBlendEquation(GL_FUNC_SUBTRACT));
				break;
			case BLEND_ALPHA:
				OOGL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
				OOGL_CALL(glBlendEquation(GL_FUNC_ADD));
				break;
			case BLEND_NONE:
			default:
				break;
		};
	}

	if (renderState_.depthMode != GL_NONE) {
		OOGL_CALL(glEnable(GL_DEPTH_TEST));
		OOGL_CALL(glDepthFunc(renderState_.depthMode));
	}

	OOGL_CALL(glDepthMask(renderState_.depthMask));

	OOGL_CALL(glPolygonMode(GL_FRONT_AND_BACK, renderState_.polyMode));

	if (renderState_.cullFace != GL_NONE) {
		OOGL_CALL(glEnable(GL_CULL_FACE));
		OOGL_CALL(glCullFace(renderState_.cullFace));
	}
}

/*----------------------------------------------------------------------------*/
inline void IMaterial::disable() {
	if (renderState_.cullFace != GL_NONE) {
		OOGL_CALL(glDisable(GL_CULL_FACE));
	}
	if (renderState_.depthMode != GL_NONE) {
		OOGL_CALL(glDisable(GL_DEPTH_TEST));
	}
	if (renderState_.blend != BLEND_NONE) {
		OOGL_CALL(glDisable(GL_BLEND));
	}

	Shader::useNone();
}

//	PROTECTED:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline IMaterial::IMaterial(ShaderUnit *pShader, const CStringT &name)
:	IResource(name),
	fragmentShader_(pShader) {
	assert(fragmentShader_);
}

/*----------------------------------------------------------------------------*/
inline IMaterial::~IMaterial() {
}

} // namespace frm

#endif // FRAMEWORK_IMATERIAL_H_
