/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include "LightDeferredPoint.h"
#include "common.h"
#include "AppBase.h"
#include "Camera.h"
#include "Shader.h"
#include "ShaderUnit.h"
namespace {
	frm::ShaderUnit *lightDeferredPointFs = 0;
}

namespace frm {

/*******************************************************************************
	LightDeferredPoint implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
LightDeferredPoint* LightDeferredPoint::create(
	const vec3 &color,
	const CStringT &name
) {
	if (!lightDeferredPointFs) {
	//	load fragment shader
		lightDeferredPointFs = ShaderUnit::loadFromFile(GL_FRAGMENT_SHADER, "shaders/materials/LightDeferredPoint.fs.glsl");
		assert(lightDeferredPointFs);
	}

	LightDeferredPoint *result = new(std::nothrow) LightDeferredPoint(lightDeferredPointFs, name);
	assert(result);
	result->setRenderState(
		RenderStateT(
			BLEND_ADD, // additive blending
			GL_NONE, // no depth test,
			GL_FALSE, // no depth writes
			GL_BACK // culling
		)
	);
	result->setColor(color);

	return result;
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
void LightDeferredPoint::enable(Shader *pShader, AppBase *pApp) {
	IMaterial::enable(pShader, pApp); // set render state

	OOGL_CALL(glUniform2f(pShader->getUniformLocation("uNearFar"), pApp->getCurrentCamera()->getClipNear(), pApp->getCurrentCamera()->getClipFar()));
}

} // namespace frm

