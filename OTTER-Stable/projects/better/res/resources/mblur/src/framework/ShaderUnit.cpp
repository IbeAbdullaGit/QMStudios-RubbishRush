/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include "ShaderUnit.h"
#include "common.h"

namespace frm {

/*******************************************************************************
	ShaderUnit implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
ShaderUnit* ShaderUnit::loadFromFile(
	GLenum type,
	const CStringT &fileName,
	bool compile
) {
//	search for existing object (only if from file):
	for (size_t i = 0; i < nInstances(); ++i) {
		if (instance(i).fromFile_) {
			if (instance(i).name_.compare(fileName) == 0) {
				oogl::logEvent("\tReusing previously loaded \"%s\"", fileName);
				return &instance(i);
			}
		}
	}

	ShaderUnit *result = new(std::nothrow) ShaderUnit(type, 0, false);
	assert(result);
	result->name_ = fileName;
	result->fromFile_ = true;

	if (result->reload())
		return result;

//	something went wrong:
	delete result;
	return 0;
}

/*----------------------------------------------------------------------------*/
ShaderUnit* ShaderUnit::create(
	GLenum type,
	const CStringT &src,
	bool compile
) {
	ShaderUnit *result = new(std::nothrow) ShaderUnit(type, (GLchar*)src, compile);
	assert(result);
	if (result->getCompileStatus())
		return result;

//	something went wrong:
	delete result;
	return 0;
}

/*----------------------------------------------------------------------------*/
void ShaderUnit::destroy(ShaderUnit **ppShaderUnit) {
	assert(ppShaderUnit != 0);
	assert(*ppShaderUnit != 0);
	delete *ppShaderUnit;
	*ppShaderUnit = 0;
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
GLint ShaderUnit::compile() {
	GLint compiled = oogl::ShaderObject::compile();
	if (*getInfoLog() != 0) { // if info log not empty
		oogl::logEvent("Compile status: %i, info log:\n\n%s", compiled, getInfoLog());
	}/* else {
		oogl::logEvent("Compile status: %i", compiled);
	}*/
	return compiled;
}

/*----------------------------------------------------------------------------*/
bool ShaderUnit::reload() {
	if (fromFile_){
	#ifdef NDEBUG
		oogl::logEvent("Loading \"%s\"...", name_.c_str());
	#endif
		if (oogl::loadShaderSource(name_.c_str(), this)) {
		#ifdef NDEBUG
			oogl::logEvent("...DONE");
		#endif
			return compile();
		}
	}
//	do nothing if not from file:
	return true;
}

/*----------------------------------------------------------------------------*/
bool ShaderUnit::reloadAll() {
//	reload/recompile:
	for (size_t i = 0; i < nInstances(); ++i) {
		if (!instance(i).reload())
			return false;
	}

	glFinish();
	return true;
}

} // namespace frm

