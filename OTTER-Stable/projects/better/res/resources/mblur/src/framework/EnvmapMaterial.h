/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_ENVMAP_MATERIAL_H_
#define FRAMEWORK_ENVMAP_MATERIAL_H_

#include "common.h"
#include "IMaterial.h"
#include "Shader.h"
#include "Texture2d.h"
#include "TextureCube.h"

namespace frm {

/* Environment map material. Use in conjunction with a screen-aligned quad. */
class EnvmapMaterial: public IMaterial {
public:
//	TYPEDEFS/ENUMS:

//	CTORS/DTORS:
	static EnvmapMaterial* create(
		TextureCube *envTexture,
		const CStringT &name = ""
	);

//	ACCESSORS:

//	SERVICES:
	/*	Enable render state, bind textures, pass uniforms. */
	virtual void enable(Shader *pShader, AppBase *pApp);

	/*	Disables shader. Optionally restore render state. */
	//virtual void disable();

protected:
//	MEMBERS:
	TextureCube *texEnv_;

//	CTORS/DTORS:
	EnvmapMaterial(ShaderUnit *pShader, const CStringT &name);

}; // class EnvmapMaterial

/*******************************************************************************
	EnvmapMaterial inline implementation:
*******************************************************************************/

//	PROTECTED:

//	CTORS/DTORS:

inline EnvmapMaterial::EnvmapMaterial(ShaderUnit *pShader, const CStringT &name)
:	IMaterial(pShader, name),
	texEnv_(0) {
}

} // namespace frm

#endif // FRAMEWORK_ENVMAP_MATERIAL_H_


