/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_SHADER_H_
#define FRAMEWORK_SHADER_H_

#include <vector>

#include "common.h"
#include "IResource.h"
#include "ShaderUnit.h"

namespace frm {

/*	Represents a single shader program. Provides static methods for managing
	shaders, recompiling, etc. */
class Shader: public oogl::ShaderProgram, public IResource<Shader> {
public:
//	CTORS/DTORS:
	/*	Create and return a shader, optionally supply a set of ShaderUnit* which
		must all be successfully compiled (NULL ptrs are silently discarded). If
		any of the parameters aren't successfully compiled shader objects, or if
		the shader program fails to link, return 0. */
	static Shader* create(int n, ...);

	static Shader* loadFromFiles(
		const CStringT &vertex,
		const CStringT &fragment
	);
	static Shader* loadFromFiles(
		const CStringT &vertex,
		const CStringT &geometry,
		const CStringT &fragment
	);
	static Shader* loadFromFiles(
		const CStringT &vertex,
		const CStringT &tessControl,
		const CStringT &tessEval,
		const CStringT &geometry,
		const CStringT &fragment
	);

	static void destroy(Shader **ppShader);

//	ACCESSORS:

//	SERVICES:
	/*	Hides oogl::ShaderProgram::attach(). Adds the incoming shader unit to
		the internal dependancy list. */
	void attach(const ShaderUnit *pShader);

	/*	Hides oogl::ShaderProgram::link. Writes additional logging for release
		builds. Returns GL_TRUE, GL_FALSE on failure.*/
	GLint link();

	/*	Hides oogl::ShaderProgram::use(). When debugging is enabled, validates the
		shader, logs error & halts on failure. */
	void use();

	/*	Reload/recompile all shader objects, if successful relink all programs. */
	static bool reloadAll();

private:
//	MEMBERS:
	std::vector<const ShaderUnit*> units_; //	attaches shader units

}; // class Shader

/*******************************************************************************
	Shader inline implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline Shader* Shader::loadFromFiles(const CStringT &vertex, const CStringT &fragment) {
	return loadFromFiles(vertex, 0, 0, 0, fragment);
}

/*----------------------------------------------------------------------------*/
inline Shader* Shader::loadFromFiles(
	const CStringT &vertex,
	const CStringT &geometry,
	const CStringT &fragment
) {
	return loadFromFiles(vertex, 0, 0, geometry, fragment);
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
inline void Shader::attach(const ShaderUnit *pShader) {
	oogl::ShaderProgram::attach(pShader);
	units_.push_back(pShader);
}


} // namespace frm

#endif // FRAMEWORK_SHADER_H_

