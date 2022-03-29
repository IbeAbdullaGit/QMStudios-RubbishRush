/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include "GenericMaterialIbl.h"
#include "common.h"
#include "AppBase.h"
#include "Camera.h"
#include "Shader.h"
#include "ShaderUnit.h"
#include "Texture2d.h"
#include "TextureCube.h"

namespace {
	frm::ShaderUnit *genericMaterialIblFs = 0;
}

namespace frm {

/*******************************************************************************
	GenericMaterialIbl implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
GenericMaterialIbl* GenericMaterialIbl::create(
		Texture2d *diffuseTexture,
		const vec3 &diffuseColor,

		Texture2d *normalTexture,
		Texture2d *detailTexture,
		const vec2 &detailScale,

		Texture2d *specularTexture, // r = level, g = exponent, b = metallic
		float specularLevel,
		float specularExponent,
		float metallicLevel,

		TextureCube *envTexture,
		const CStringT &name
) {

	if (!genericMaterialIblFs) {
	//	load fragment shader
		genericMaterialIblFs = ShaderUnit::loadFromFile(GL_FRAGMENT_SHADER, "shaders/materials/GenericMaterialIbl.fs.glsl");
		assert(genericMaterialIblFs);
	}

	GenericMaterialIbl *result = new(std::nothrow) GenericMaterialIbl(genericMaterialIblFs, name);
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
	if (envTexture)
		result->texEnv_ = envTexture;

	if (!(result->texDiffuse_ && result->texNormal_ && result->texDetail_ &&
		result->texSpecular_ && result->texEnv_)) {
		delete result;
		return 0;
	}

	return result;
}

/*----------------------------------------------------------------------------*/
GenericMaterialIbl* GenericMaterialIbl::create(
		const vec3 &diffuseColor,
		float specularLevel,
		float specularExponent,
		float metallicLevel,
		TextureCube *envTexture,
		const CStringT &name
) {

	if (!genericMaterialIblFs) {
	//	load fragment shader
		genericMaterialIblFs = ShaderUnit::loadFromFile(GL_FRAGMENT_SHADER, "shaders/materials/GenericMaterialIbl.fs.glsl");
		assert(genericMaterialIblFs);
	}

	GenericMaterialIbl *result = new(std::nothrow) GenericMaterialIbl(genericMaterialIblFs, name);
	assert(result);
	result->diffuseColor_ = diffuseColor;
	result->specularExp_ = specularExponent;
	result->specularLevel_ = specularLevel;
	result->metallicLevel_ = metallicLevel;
	if (envTexture)
		result->texEnv_ = envTexture;

	if (!(result->texDiffuse_ && result->texNormal_ && result->texDetail_ &&
		result->texSpecular_ && result->texEnv_)) {
		delete result;
		return 0;
	}

	return result;
}


//	SERVICES:

/*----------------------------------------------------------------------------*/
void GenericMaterialIbl::enable(Shader *pShader, AppBase *pApp) {
	IMaterial::enable(pShader, pApp); // set render state

	OOGL_CALL(glActiveTexture(GL_TEXTURE0));
	texEnv_->bind();
	OOGL_CALL(glActiveTexture(GL_TEXTURE1));
	texDiffuse_->bind();
	OOGL_CALL(glActiveTexture(GL_TEXTURE2));
	texNormal_->bind();
	OOGL_CALL(glActiveTexture(GL_TEXTURE3));
	texDetail_->bind();
	OOGL_CALL(glActiveTexture(GL_TEXTURE4));
	texSpecular_->bind();

/*	mat3 nm = sml::transpose(sml::mat3f(sml::inverse(app->getMatrix(AppBase::MODEL_VIEW_MATRIX))));
	OOGL_CALL(glUniformMatrix4fv(pShader->getUniformLocation("uModelViewMatrix"), 1, GL_FALSE, app->getMatrix(AppBase::MODEL_VIEW_MATRIX)));
	OOGL_CALL(glUniformMatrix3fv(pShader->getUniformLocation("uNormalMatrix"), 1, GL_FALSE, nm));
	OOGL_CALL(glUniformMatrix4fv(pShader->getUniformLocation("uModelViewProjectionMatrix"), 1, GL_FALSE, app->getMatrix(AppBase::MODEL_VIEW_PROJECTION_MATRIX)));
*/
	OOGL_CALL(glUniform3fv(pShader->getUniformLocation("uDiffuseColor"), 1, diffuseColor_));
	OOGL_CALL(glUniform1f(pShader->getUniformLocation("uSpecularLevel"), specularLevel_));
	OOGL_CALL(glUniform1f(pShader->getUniformLocation("uSpecularExponent"), specularExp_));
	OOGL_CALL(glUniform1f(pShader->getUniformLocation("uMetallicLevel"), metallicLevel_));
	OOGL_CALL(glUniform2fv(pShader->getUniformLocation("uDetailTexScale"), 1, detailScale_));
	OOGL_CALL(glUniform1f(pShader->getUniformLocation("uEnvTexMaxLod"), sml::sqrt((float)texEnv_->getWidth()) * 0.5f));

	//mat3 ivm = sml::mat3f(sml::inverse(app->getMatrix(AppBase::VIEW_MATRIX)));
	sml::mat4f ivm = pApp->getCurrentCamera()->getWorldMatrix();
	OOGL_CALL(glUniformMatrix4fv(pShader->getUniformLocation("uInverseViewMatrix"), 1, GL_FALSE, ivm));
}

} // namespace frm

