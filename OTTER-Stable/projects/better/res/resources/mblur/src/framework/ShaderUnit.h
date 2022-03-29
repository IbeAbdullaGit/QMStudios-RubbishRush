/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_SHADER_UNIT_H_
#define FRAMEWORK_SHADER_UNIT_H_

#include "common.h"
#include "IResource.h"

namespace frm {

/*	Represents a shader object. */
class ShaderUnit: public oogl::ShaderObject, public IResource<ShaderUnit> {
public:
//	CTORS/DTORS:
	static ShaderUnit* loadFromFile(
		GLenum type,
		const CStringT &fileName,
		bool compile = true
	);

	static ShaderUnit* create(
		GLenum type,
		const CStringT &src,
		bool compile = true
	);

	static void destroy(ShaderUnit **ppShaderUnit);

//	ACCESSORS:
	bool fromFile() const;

//	SERVICES:
	/*	Hides oogl::ShaderObject::compile(). Writes additional logging for
		release builds, returns GL_TRUE, GL_FALSE on failure. */
	GLint compile();

	/*	Reload/recompile shader unit, return true if successful. */
	bool reload();

	/*	Calls reload() on all instances. Returns true, false if any call to
		reload() fails. */
	static bool reloadAll();

private:
//	MEMBERS:
	bool fromFile_;

//	CTORS/DTORS:
	ShaderUnit(GLenum type, GLchar *src, GLboolean compile);

}; // class ShaderUnit

/*******************************************************************************
	ShaderUnit inline implementation:
*******************************************************************************/

//	PUBLIC:

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline bool ShaderUnit::fromFile() const {
	return fromFile_;
}

//	PROTECTED:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline ShaderUnit::ShaderUnit(GLenum type, GLchar *src, GLboolean compile)
:	ShaderObject(type, src, compile),
	fromFile_(false) {

}


} // namespace frm

#endif // FRAMEWORK_SHADER_UNIT_H_
