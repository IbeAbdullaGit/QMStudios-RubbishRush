/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include "GenericMaterialDeferred.h"
#include "common.h"
#include "AppBase.h"
#include "Camera.h"
#include "Shader.h"
#include "ShaderUnit.h"
#include "Texture2d.h"
#include "TextureCube.h"

namespace {
	frm::ShaderUnit *genericMaterialDeferredFs = 0;
}

namespace frm {

/*******************************************************************************
	GenericMaterialDeferred implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
GenericMaterialDeferred* GenericMaterialDeferred::create(
		Texture2d *diffuseTexture,
		const vec3 &diffuseColor,

		Texture2d *normalTexture,
		Texture2d *detailTexture,
		const vec2 &detailScale,

		Texture2d *specularTexture, // r = level, g = exponent, b = metallic
		float specularLevel,
		float specularExponent,
		float metallicLevel,

		const CStringT &name
) {

	if (!genericMaterialDeferredFs) {
	//	load fragment shader
		genericMaterialDeferredFs = ShaderUnit::loadFromFile(GL_FRAGMENT_SHADER, "shaders/materials/GenericMaterialDeferred.fs.glsl");
		assert(genericMaterialDeferredFs);
	}

	GenericMaterialDeferred *result = new(std::nothrow) GenericMaterialDeferred(genericMaterialDeferredFs, name);
	assert(result);
	result->diffuseColor_ = diffuseColor;
	result->detailScale_ = detailScale;
	result->specularExp_ = specularExponent;
	result->specularLevel_ = specularLevel;
	result->metallicLevel_ = metallicLevel;

	if (diffuseTexture)
		result->texDiffuse_ = diffuseTexture;
	if (normalTexture)
		result->texNormal_ = normalTexture;
	if (detailTexture)
		result->texDetail_ = detailTexture;
	if (specularTexture)
		result->texSpecular_ = specularTexture;
	if (!(result->texDiffuse_ && result->texNormal_ && result->texDetail_ &&
		result->texSpecular_)) {
		delete result;
		return 0;
	}

	return result;
}

/*----------------------------------------------------------------------------*/
GenericMaterialDeferred* GenericMaterialDeferred::create(
		const vec3 &diffuseColor,
		float specularLevel,
		float specularExponent,
		float metallicLevel,
		const CStringT &name
) {

	if (!genericMaterialDeferredFs) {
	//	load fragment shader
		genericMaterialDeferredFs = ShaderUnit::loadFromFile(GL_FRAGMENT_SHADER, "shaders/materials/GenericMaterialDeferred.fs.glsl");
		assert(genericMaterialDeferredFs);
	}

	GenericMaterialDeferred *result = new(std::nothrow) GenericMaterialDeferred(genericMaterialDeferredFs, name);
	assert(result);
	result->diffuseColor_ = diffuseColor;
	result->specularExp_ = specularExponent;
	result->specularLevel_ = specularLevel;
	result->metallicLevel_ = metallicLevel;

	if (!(result->texDiffuse_ && result->texNormal_ && result->texDetail_ &&
		result->texSpecular_)) {
		delete result;
		return 0;
	}

	return result;
}


//	SERVICES:

/*----------------------------------------------------------------------------*/
void GenericMaterialDeferred::enable(Shader *pShader, AppBase *pApp) {
	IMaterial::enable(pShader, pApp); // set render state

	OOGL_CALL(glActiveTexture(GL_TEXTURE0));
	texDiffuse_->bind();
	OOGL_CALL(glActiveTexture(GL_TEXTURE1));
	texNormal_->bind();
	OOGL_CALL(glActiveTexture(GL_TEXTURE2));
	texDetail_->bind();
	OOGL_CALL(glActiveTexture(GL_TEXTURE3));
	texSpecular_->bind();
	OOGL_CALL(glUniform3fv(pShader->getUniformLocation("uDiffuseColor"), 1, diffuseColor_));
	OOGL_CALL(glUniform1f(pShader->getUniformLocation("uSpecularLevel"), specularLevel_));
	OOGL_CALL(glUniform1f(pShader->getUniformLocation("uSpecularExponent"), specularExp_));
	OOGL_CALL(glUniform1f(pShader->getUniformLocation("uMetallicLevel"), metallicLevel_));
	OOGL_CALL(glUniform2fv(pShader->getUniformLocation("uDetailTexScale"), 1, detailScale_));
}

} // namespace frm


