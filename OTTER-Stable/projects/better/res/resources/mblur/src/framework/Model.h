/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_MODEL_H_
#define FRAMEWORK_MODEL_H_

#include <vector>

#include "common.h"
#include "IResource.h"
#include "ShaderUnit.h"

namespace frm {

/*	Renderable representation of a Mesh. Manages VBO/IBO. */
class Model: public IResource<Model> {
public:
//	CTORS/DTORS:
	static Model* create(const Mesh *pMesh, const CStringT &name = "");
	static Model* createSAQuad(const CStringT &name = "");
	static void destroy(Model **ppModel);

//	ACCESSORS:
	const oogl::Buffer* getVertexBuffer() const;
	const oogl::Buffer* getIndexBuffer() const;

	/*	Gets/sets the primitive mode to use during a draw call. This defaults to
		GL_TRIANGLES. */
	GLenum getMode() const;
	void setMode(GLenum mode);

	ShaderUnit* getVertexShader() const;
	void setVertexShader(ShaderUnit *pShader);
	ShaderUnit* getTessControlShader() const;
	void setTessControlShader(ShaderUnit *pShader);
	ShaderUnit* getTessEvalShader() const;
	void setTessEvalShader(ShaderUnit *pShader);
	ShaderUnit* getGeometryShader() const;
	void setGeometryShader(ShaderUnit *pShader);

	float getRadius() const;
	const vec3& getMinExtents() const;
	const vec3& getMaxExtents() const;
	const vec3* getExtents() const;

	static unsigned getTotalTriangles();
	static void resetTotalTriangles();

//	SERVICES:
	/*	Enable/disable this model for rendering. */
	virtual void enable(Shader *pShader, AppBase *pApp);
	virtual void disable();

	/*	Perform a draw call. If modeOverride is set, that is used as the
		primitive mod. */
	virtual void render(GLenum modeOverride = GL_NONE);

protected:
//	MEMBERS:
	oogl::VertexArray *vao_;
	oogl::Buffer *vbo_, *ibo_;

	ShaderUnit *vertexShader_, *tessEvalShader_, *tessControlShader_, *geometryShader_;

	GLenum mode_;

	unsigned nVertices_, nIndices_, nTriangles_;

	float radius_; // bounding radius
	vec3 extents_[2]; // min/max extents (bounding box)

	static unsigned totalTriangles_; // summed per render() call, reset to 0 each frame

//	CTORS/DTORS:
	Model(const CStringT &name);
	virtual ~Model();

}; // class Model

/*******************************************************************************
	Model inline implementation:
*******************************************************************************/

//	PUBLIC:

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline const oogl::Buffer* Model::getVertexBuffer() const {
	return vbo_;
}
/*----------------------------------------------------------------------------*/
inline const oogl::Buffer* Model::getIndexBuffer() const {
	return ibo_;
}

/*----------------------------------------------------------------------------*/
inline GLenum Model::getMode() const {
	return mode_;
}
inline void Model::setMode(GLenum mode) {
	assert(
		mode == GL_POINTS ||
		mode == GL_LINE_STRIP ||
		mode == GL_LINE_LOOP ||
		mode == GL_LINES ||
		mode == GL_LINE_STRIP_ADJACENCY ||
		mode == GL_LINES_ADJACENCY ||
		mode == GL_TRIANGLE_STRIP ||
		mode == GL_TRIANGLE_FAN ||
		mode == GL_TRIANGLES ||
		mode == GL_TRIANGLE_STRIP_ADJACENCY ||
		mode == GL_PATCHES ||
		mode == GL_TRIANGLES_ADJACENCY
	);
	mode_ = mode;
}

/*----------------------------------------------------------------------------*/
inline ShaderUnit* Model::getVertexShader() const {
	return vertexShader_;
}
/*----------------------------------------------------------------------------*/
inline void Model::setVertexShader(ShaderUnit *pShader) {
	assert(pShader->getType() == GL_VERTEX_SHADER);
	vertexShader_ = pShader;
}
/*----------------------------------------------------------------------------*/
inline ShaderUnit* Model::getTessControlShader() const {
	return tessControlShader_;
}
/*----------------------------------------------------------------------------*/
inline void Model::setTessControlShader(ShaderUnit *pShader) {
	assert(pShader->getType() == GL_TESS_CONTROL_SHADER);
	tessControlShader_ = pShader;
}
/*----------------------------------------------------------------------------*/
inline ShaderUnit* Model::getTessEvalShader() const {
	return tessEvalShader_;
}
/*----------------------------------------------------------------------------*/
inline void Model::setTessEvalShader(ShaderUnit *pShader) {
	assert(pShader->getType() == GL_TESS_EVALUATION_SHADER);
	tessEvalShader_ = pShader;
}
/*----------------------------------------------------------------------------*/
inline ShaderUnit* Model::getGeometryShader() const {
	return geometryShader_;
}
/*----------------------------------------------------------------------------*/
inline void Model::setGeometryShader(ShaderUnit *pShader) {
	assert(pShader->getType() == GL_GEOMETRY_SHADER);
	geometryShader_ = pShader;
}

/*----------------------------------------------------------------------------*/
inline float Model::getRadius() const {
	return radius_;
}
/*----------------------------------------------------------------------------*/
inline const vec3& Model::getMinExtents() const {
	return extents_[0];
}
/*----------------------------------------------------------------------------*/
inline const vec3& Model::getMaxExtents() const {
	return extents_[1];
}
/*----------------------------------------------------------------------------*/
inline const vec3* Model::getExtents() const {
	return extents_;
}

/*----------------------------------------------------------------------------*/
inline unsigned Model::getTotalTriangles() {
	return totalTriangles_;
}

/*----------------------------------------------------------------------------*/
inline void Model::resetTotalTriangles() {
	totalTriangles_ = 0;
}

//	PROTECTED:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline Model::Model(const CStringT &name)
:	IResource(name),
	vao_(0), vbo_(0), ibo_(0),
	mode_(GL_TRIANGLES),
	vertexShader_(0), tessControlShader_(0), tessEvalShader_(0), geometryShader_(0),
	nVertices_(0), nIndices_(0), nTriangles_(0),
	radius_(1.0f) {
}

/*----------------------------------------------------------------------------*/
inline Model::~Model() {
	if (vao_)
		delete vao_;
	if (vbo_)
		delete vbo_;
	if (ibo_)
		delete ibo_;
}


} // namespace frm

#endif // FRAMEWORK_MODEL_H_



