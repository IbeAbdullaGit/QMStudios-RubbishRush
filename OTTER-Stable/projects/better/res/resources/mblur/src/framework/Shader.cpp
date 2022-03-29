/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include <stdarg.h>

#include <vector>

#include "Shader.h"
#include "ShaderUnit.h"
#include "common.h"

namespace frm {

/*******************************************************************************
	Shader implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
Shader* Shader::create(int n, ...) {
//	get shader unit arguments:
	va_list vl;
	va_start(vl, n);
	std::vector<ShaderUnit*> args;
	for (int i = 0; i < n; ++i) {
		ShaderUnit *unit = va_arg(vl, ShaderUnit*);
		if (unit) // discard null units
			args.push_back(unit);
	}
	va_end(vl);

//	validate shader units were all compiled successfully:
	for (size_t i = 0; i < args.size(); ++i) {
		if (!args[i]->getCompileStatus())
			return 0;
	}

//	search for existing shader:
	for (size_t i = 0; i < nInstances(); ++i) {
		bool match = true;
		for (size_t j = 0; j < instance(i).units_.size(); ++j) {
			if (j >= args.size()) {
				match = false;
				break;
			}
			if (args[j] != instance(i).units_[j]) {
				match = false;
				break;
			}
		}
		if (match) {
			oogl::logEvent("\tReusing previously linked Shader#%u", instance(i).getHandle());
			return &instance(i);
		}
	}

//	create new shader:
	Shader *result = new(std::nothrow) Shader;
	assert(result);

	for (size_t i = 0; i < args.size(); ++i)
		result->attach(args[i]);

	if (result->link())
		return result;

//	something went wrong:
	delete result;
	return 0;
}

/*----------------------------------------------------------------------------*/
Shader* Shader::loadFromFiles(
	const CStringT &vertex,
	const CStringT &tessControl,
	const CStringT &tessEval,
	const CStringT &geometry,
	const CStringT &fragment
) {
	ShaderUnit *unitVertex = 0, *unitTessControl = 0, *unitTessEval = 0,
		*unitGeometry = 0, *unitFragment = 0;
	if (vertex) {
		unitVertex = ShaderUnit::loadFromFile(GL_VERTEX_SHADER, vertex);
		if (!unitVertex)
			return 0;
	}
	if (tessControl) {
		unitTessControl = ShaderUnit::loadFromFile(GL_TESS_CONTROL_SHADER, tessControl);
		if (!unitTessControl)
			return 0;
	}
	if (tessEval) {
		unitTessEval = ShaderUnit::loadFromFile(GL_TESS_EVALUATION_SHADER, tessEval);
		if (!unitTessEval)
			return 0;
	}
	if (geometry) {
		unitGeometry = ShaderUnit::loadFromFile(GL_GEOMETRY_SHADER, geometry);
		if (!unitGeometry)
			return 0;
	}
	if (fragment) {
		unitFragment = ShaderUnit::loadFromFile(GL_FRAGMENT_SHADER, fragment);
		if (!unitFragment)
			return 0;
	}

	return create(
		5,
		unitVertex,
		unitTessControl,
		unitTessEval,
		unitGeometry,
		unitFragment
	);
}

/*----------------------------------------------------------------------------*/
void Shader::destroy(Shader **ppShader) {
	assert(ppShader != 0);
	assert(*ppShader != 0);
	delete *ppShader;
	*ppShader = 0;
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
GLint Shader::link() {
	GLint linked = oogl::ShaderProgram::link();
	if (*getInfoLog() != 0) { // if info log not empty
		oogl::logEvent("Link status: %i, info log:\n%s", linked, getInfoLog());
	}
	return linked;
}

/*----------------------------------------------------------------------------*/
void Shader::use() {
#ifndef NDEBUG
	if (!validate()) {
		oogl::logEvent("Failed to validate Shader#%u.\nInfo log:\n%s", getHandle(), getInfoLog());
		assert(false); // abort!
	}
#endif

	oogl::ShaderProgram::use();
}

/*----------------------------------------------------------------------------*/
bool Shader::reloadAll() {
	Shader::useNone();

	if (!ShaderUnit::reloadAll())
		return false;

//	success, relink:
	for (size_t i = 0; i < nInstances(); ++i) {
		if (!instance(i).link())
			return false;
	}

	glFinish();
	return true;
}

} // namespace frm

