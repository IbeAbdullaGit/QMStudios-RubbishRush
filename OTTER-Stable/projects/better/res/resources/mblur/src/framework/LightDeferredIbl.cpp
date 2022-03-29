/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include "LightDeferredIbl.h"
#include "common.h"
#include "AppBase.h"
#include "Camera.h"
#include "Shader.h"
#include "ShaderUnit.h"
#include "Texture2d.h"
#include "TextureCube.h"

namespace {
	frm::ShaderUnit *lightDeferredIblFs = 0;
}

namespace frm {

/*******************************************************************************
	LightDeferredIbl implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
LightDeferredIbl* LightDeferredIbl::create(
	TextureCube *envTexture,
	const CStringT &name
) {
	assert(envTexture);

	if (!lightDeferredIblFs) {
	//	load fragment shader
		lightDeferredIblFs = ShaderUnit::loadFromFile(GL_FRAGMENT_SHADER, "shaders/materials/LightDeferredIbl.fs.glsl");
		assert(lightDeferredIblFs);
	}

	LightDeferredIbl *result = new(std::nothrow) LightDeferredIbl(lightDeferredIblFs, name);
	assert(result);
	result->setRenderState(RenderStateT(BLEND_ADD, GL_NONE)); // additive blending, no depth test
	result->setRenderState(
		RenderStateT(
			BLEND_ADD, // additive blending
			GL_NONE, // no depth test,
			GL_FALSE, // no depth writes
			GL_BACK // culling
		)
	);
	result->texEnv_ = envTexture;

	return result;
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
void LightDeferredIbl::enable(Shader *pShader, AppBase *pApp) {
	IMaterial::enable(pShader, pApp); // set render state

	OOGL_CALL(glActiveTexture(GL_TEXTURE0));
	texEnv_->bind();

	OOGL_CALL(glUniform2f(pShader->getUniformLocation("uNearFar"), pApp->getCurrentCamera()->getClipNear(), pApp->getCurrentCamera()->getClipFar()));
	OOGL_CALL(glUniform1f(pShader->getUniformLocation("uEnvTexMaxLod"), sml::sqrt((float)texEnv_->getWidth()) * 0.5f));

	sml::mat4f ivm = pApp->getCurrentCamera()->getWorldMatrix();
	OOGL_CALL(glUniformMatrix4fv(pShader->getUniformLocation("uInverseViewMatrix"), 1, GL_FALSE, ivm));
}

} // namespace frm
