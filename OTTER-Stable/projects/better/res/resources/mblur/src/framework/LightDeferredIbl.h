/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_LIGHT_DEFERRED_IBL_H_
#define FRAMEWORK_LIGHT_DEFERRED_IBL_H_

#include "common.h"
#include "IMaterial.h"
#include "Shader.h"
#include "TextureCube.h"

namespace frm {

/* */
class LightDeferredIbl: public IMaterial {
public:
//	TYPEDEFS/ENUMS:

//	CTORS/DTORS:
	/*	*/
	static LightDeferredIbl* create(
		TextureCube *envTexture,
		const CStringT &name = ""
	);

//	ACCESSORS:

//	SERVICES:
	/*	Enable render state, bind textures, pass uniforms. */
	virtual void enable(Shader *pShader, AppBase *pApp);

	/*	Restore render state. */
	//virtual void disable();

protected:
//	MEMBERS
	TextureCube *texEnv_;

//	CTORS/DTORS:
	LightDeferredIbl(ShaderUnit *pShader, const CStringT &name);

}; // class LightDeferredIbl

/*******************************************************************************
	LightDeferredIbl inline implementation:
*******************************************************************************/

//	PUBLIC:

//	PROTECTED:

//	CTORS/DTORS:

inline LightDeferredIbl::LightDeferredIbl(ShaderUnit *pShader, const CStringT &name)
:	IMaterial(pShader, name),
	texEnv_(0) {
}

} // namespace frm

#endif // FRAMEWORK_LIGHT_DEFERRED_IBL_H_
