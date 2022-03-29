/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_GENERIC_MATERIAL_IBL_H_
#define FRAMEWORK_GENERIC_MATERIAL_IBL_H_

#include "common.h"
#include "IMaterial.h"
#include "Shader.h"
#include "Texture2d.h"
#include "TextureCube.h"

namespace frm {

/* Opaque, foward-rendered material for image based lighting. */
class GenericMaterialIbl: public IMaterial {
public:
//	TYPEDEFS/ENUMS:

//	CTORS/DTORS:
	/*	Loads textures from files (the texture system won't load the same file
		twice so this can be used indiscriminately). Pass 0 to use a default
		texture. */
	static GenericMaterialIbl* create(
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
	);

	/*	Creates a material with the default textures. */
	static GenericMaterialIbl* create(
		const vec3 &diffuseColor,
		float specularLevel,
		float specularExponent,
		float metallicLevel,
		TextureCube *envTexture,
		const CStringT &name
	);

//	ACCESSORS:
	const vec3& getDiffuseColor() const;
	void setDiffuseColor(const vec3&);

	float getSpecularLevel() const;
	void setSpecularLevel(float);
	float getSpecularExponent() const;
	void setSpecularExponent(float);

	float getMetallicLevel() const;
	void setMetallicLevel(float);

	const vec2& getDetailScale() const;
	void setDetailScale(const vec2&);

//	SERVICES:
	/*	Enable render state, bind textures, pass uniforms. */
	virtual void enable(Shader *pShader, AppBase *pApp);

	/*	Restore render state. */
	//virtual void disable();

protected:
//	MEMBERS:
	vec3 diffuseColor_; // modulate diffuse map
	float specularLevel_, specularExp_, metallicLevel_; // in [0,1]
	vec2 detailScale_; // for texDetail_
	Texture2d *texDiffuse_, *texNormal_, *texDetail_, *texSpecular_;
	TextureCube *texEnv_;

//	CTORS/DTORS:
	GenericMaterialIbl(ShaderUnit *pShader, const CStringT &name);

}; // class GenericMaterialIbl

/*******************************************************************************
	GenericMaterialIbl inline implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline const vec3& GenericMaterialIbl::getDiffuseColor() const {
	return diffuseColor_;
}
/*----------------------------------------------------------------------------*/
inline void GenericMaterialIbl::setDiffuseColor(const vec3 &diffuseColor) {
	diffuseColor_ = diffuseColor;
}

/*----------------------------------------------------------------------------*/
inline float GenericMaterialIbl::getSpecularLevel() const {
	return specularLevel_;
}
/*----------------------------------------------------------------------------*/
inline void GenericMaterialIbl::setSpecularLevel(float specularLevel) {
	specularLevel_ = specularLevel;
}
/*----------------------------------------------------------------------------*/
inline float GenericMaterialIbl::getSpecularExponent() const {
	return specularExp_;
}
/*----------------------------------------------------------------------------*/
inline void GenericMaterialIbl::setSpecularExponent(float specularExp) {
	specularExp_ = specularExp;
}

/*----------------------------------------------------------------------------*/
inline float GenericMaterialIbl::getMetallicLevel() const {
	return metallicLevel_;
}
/*----------------------------------------------------------------------------*/
inline void GenericMaterialIbl::setMetallicLevel(float metallicLevel) {
	metallicLevel_ = metallicLevel;
}

/*----------------------------------------------------------------------------*/
inline const vec2& GenericMaterialIbl::getDetailScale() const {
	return detailScale_;
}
/*----------------------------------------------------------------------------*/
inline void GenericMaterialIbl::setDetailScale(const vec2 &detailScale) {
	detailScale_ = detailScale;
}

//	PROTECTED:

//	CTORS/DTORS:

inline GenericMaterialIbl::GenericMaterialIbl(ShaderUnit *pShader, const CStringT &name)
:	IMaterial(pShader, name),
	diffuseColor_(vec3(1.0f)),
	specularLevel_(1.0f),
	specularExp_(0.5f),
	metallicLevel_(0.0f),
	detailScale_(vec2(0.1f)),
	texDiffuse_(texWhite),
	texNormal_(texNorm),
	texDetail_(texNorm),
	texSpecular_(texWhite),
	texEnv_(0) {
}

} // namespace frm

#endif // FRAMEWORK_GENERIC_MATERIAL_IBL_H_

