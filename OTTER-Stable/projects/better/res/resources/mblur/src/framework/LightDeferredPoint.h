/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_LIGHT_DEFERRED_POINT_H_
#define FRAMEWORK_LIGHT_DEFERRED_POINT_H_

#include "common.h"
#include "IMaterial.h"
#include "Shader.h"

namespace frm {

/* Deferred point light. */
class LightDeferredPoint: public IMaterial {
public:
//	TYPEDEFS/ENUMS:

//	CTORS/DTORS:
	/*	*/
	static LightDeferredPoint* create(
		const vec3 &color = vec3(1.0f),
		const CStringT &name = ""
	);

//	ACCESSORS:
	const vec3& getColor() const;
	void setColor(const vec3&);

//	SERVICES:
	/*	Enable render state, bind textures, pass uniforms. */
	virtual void enable(Shader *pShader, AppBase *pApp);

	/*	Restore render state. */
	//virtual void disable();

protected:
//	MEMBERS:
	vec3 color_;


//	CTORS/DTORS:
	LightDeferredPoint(ShaderUnit *pShader, const CStringT &name);

}; // class LightDeferredPoint

/*******************************************************************************
	LightDeferredPoint inline implementation:
*******************************************************************************/

//	PUBLIC:

//	PROTECTED:

//	CTORS/DTORS:

inline LightDeferredPoint::LightDeferredPoint(ShaderUnit *pShader, const CStringT &name)
:	IMaterial(pShader, name) {
}

inline const vec3& LightDeferredPoint::getColor() const {
	return color_;
}
inline void LightDeferredPoint::setColor(const vec3 &color) {
	color_ = color;
}

} // namespace frm

#endif // FRAMEWORK_LIGHT_DEFERRED_POINT_H_

