/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_GENERIC_MATERIAL_DEFERRED_H_
#define FRAMEWORK_GENERIC_MATERIAL_DEFERRED_H_

#include "common.h"
#include "GenericMaterialIbl.h"

namespace frm {

/* */
class GenericMaterialDeferred: public GenericMaterialIbl {
public:
//	TYPEDEFS/ENUMS:

//	CTORS/DTORS:
	/*	Loads textures from files (the texture system won't load the same file
		twice so this can be used indiscriminately). Pass 0 to use a default
		texture. */
	static GenericMaterialDeferred* create(
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
	);

	/*	Creates a material with the default textures. */
	static GenericMaterialDeferred* create(
		const vec3 &diffuseColor,
		float specularLevel,
		float specularExponent,
		float metallicLevel,
		const CStringT &name
	);

//	ACCESSORS:

//	SERVICES:
	/*	Enable render state, bind textures, pass uniforms. */
	virtual void enable(Shader *pShader, AppBase *pApp);

	/*	Restore render state. */
	//virtual void disable();

protected:

//	CTORS/DTORS:
	GenericMaterialDeferred(ShaderUnit *pShader, const CStringT &name);

}; // class GenericMaterialDeferred

/*******************************************************************************
	GenericMaterialDeferred inline implementation:
*******************************************************************************/

//	PUBLIC:

//	SERVICES:

//	PROTECTED:

//	CTORS/DTORS:

inline GenericMaterialDeferred::GenericMaterialDeferred(ShaderUnit *pShader, const CStringT &name)
:	GenericMaterialIbl(pShader, name) {
}

} // namespace frm

#endif // FRAMEWORK_GENERIC_MATERIAL_DEFERRED_H_


