/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include "EnvmapMaterial.h"
#include "common.h"
#include "AppBase.h"
#include "Camera.h"
#include "Shader.h"
#include "ShaderUnit.h"
#include "TextureCube.h"

namespace {
	frm::ShaderUnit *envmapMaterialFs = 0;
}

namespace frm {

/*******************************************************************************
	EnvmapMaterial implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
EnvmapMaterial* EnvmapMaterial::create(
		TextureCube *envTexture,
		const CStringT &name
) {

	if (!envmapMaterialFs) {
	//	load fragment shader
		envmapMaterialFs = ShaderUnit::loadFromFile(GL_FRAGMENT_SHADER, "shaders/materials/EnvmapMaterial.fs.glsl");
	}

	EnvmapMaterial *result = new(std::nothrow) EnvmapMaterial(envmapMaterialFs, name);
	assert(result);

	result->texEnv_ = envTexture;
	result->setRenderState(
		IMaterial::RenderStateT(
			BLEND_NONE,
			GL_LESS, // do perform depth test
			GL_FALSE, // no depth writes
			GL_BACK // culling
		)
	);

	if (!result->texEnv_) {
		delete result;
		return 0;
	}

	return result;
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
void EnvmapMaterial::enable(Shader *pShader, AppBase *pApp) {
	IMaterial::enable(pShader, pApp); // set shader/render state

	OOGL_CALL(glActiveTexture(GL_TEXTURE0));
	texEnv_->bind();

	//OOGL_CALL(glUniform1f(pShader->getUniformLocation("uTanHalfFov"), app->getCurrentCamera()->getTanHalfFov()));
	//OOGL_CALL(glUniform1f(pShader->getUniformLocation("uAspectRatio"), app->getCurrentCamera()->getAspect()));
	sml::mat4f ivm = pApp->getCurrentCamera()->getWorldMatrix();
	OOGL_CALL(glUniformMatrix4fv(pShader->getUniformLocation("uInverseViewMatrix"), 1, GL_FALSE, ivm));

	mat4 ivpm = sml::inverse(pApp->getCurrentCamera()->getViewProjectionMatrix());
	OOGL_CALL(glUniformMatrix4fv(pShader->getUniformLocation("uInverseViewProjectionMatrix"), 1, GL_FALSE, ivpm));
	mat4 pvpm = pApp->getCurrentCamera()->getPreviousViewProjectionMatrix();
	OOGL_CALL(glUniformMatrix4fv(pShader->getUniformLocation("uPreviousViewProjectionMatrix"), 1, GL_FALSE, pvpm));
}

} // namespace frm

