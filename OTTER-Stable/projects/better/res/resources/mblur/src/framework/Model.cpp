/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include "Model.h"
#include "common.h"
#include "AppBase.h"
#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include "ShaderUnit.h"

namespace frm {

/*******************************************************************************
	Model implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
Model* Model::create(const Mesh *pMesh, const CStringT &name) {
	assert(pMesh);

//	create/bind vao:
	oogl::VertexArray *vao = new(std::nothrow) oogl::VertexArray;
	assert(vao);
	vao->bind();

//	upload vertices to gpu:
	assert(pMesh->nVertices() > 0); // must at least have some vertices
	oogl::Buffer *vbo = new(std::nothrow) oogl::Buffer(
		GL_ARRAY_BUFFER,
		GL_STATIC_DRAW,
		pMesh->nVertices() * sizeof(Mesh::VertexT),
		pMesh->getVertices()
	);
	assert(vbo);

//	set vertex pointers:
	// positions (location=0)
	OOGL_CALL(glEnableVertexAttribArray(0));
	OOGL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::VertexT), (const GLvoid*)0));
	// normals (location=1)
	OOGL_CALL(glEnableVertexAttribArray(1));
	OOGL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::VertexT), (const GLvoid*)sizeof(vec3)));
	// tangents (location=2)
	OOGL_CALL(glEnableVertexAttribArray(2));
	OOGL_CALL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::VertexT), (const GLvoid*)(sizeof(vec3) * 2)));
	// texcoords (location=3)
	OOGL_CALL(glEnableVertexAttribArray(3));
	OOGL_CALL(glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::VertexT), (const GLvoid*)(sizeof(vec3) * 3)));

//	upload indices to gpu (if present):
	oogl::Buffer *ibo = 0;
	if (pMesh->nTriangles() > 0) {
		ibo = new(std::nothrow) oogl::Buffer(
			GL_ELEMENT_ARRAY_BUFFER,
			GL_STATIC_DRAW,
			pMesh->nTriangles() * sizeof(Mesh::TriangleT),
			pMesh->getIndices()

		);
		assert(ibo);
	}

//	create model:
	Model *result = new(std::nothrow) Model(name);
	assert(result);
	result->setVertexShader(ShaderUnit::loadFromFile(GL_VERTEX_SHADER, "shaders/models/GenericModel.vs.glsl"));

	result->nVertices_ = pMesh->nVertices();
	result->nIndices_ = pMesh->nTriangles() * 3;
	result->nTriangles_ = pMesh->nTriangles();
	result->radius_ = pMesh->getRadius();
	result->extents_[0] = pMesh->getMinExtents();
	result->extents_[1] = pMesh->getMaxExtents();
	result->setMode(GL_TRIANGLES);
	result->vbo_ = vbo;
	result->ibo_ = ibo;
	result->vao_ = vao;


//	ensure buffers are not bound:
	oogl::VertexArray::bindNone(); // unbind this first so as not to modify!
	oogl::Buffer::bindNone(GL_ARRAY_BUFFER);
	oogl::Buffer::bindNone(GL_ELEMENT_ARRAY_BUFFER);

	return result;
}

/*----------------------------------------------------------------------------*/
Model* Model::createSAQuad(const CStringT &name) {
//	create/bind vao:
	oogl::VertexArray *vao = new(std::nothrow) oogl::VertexArray;
	assert(vao);
	vao->bind();

//	create/upload vertices:
	GLfloat saqv[8] = {
		-1.0f,	-1.0f,
		1.0f,		-1.0f,
		-1.0f,	1.0f,
		1.0f,		1.0f
	};
	oogl::Buffer *vbo = new(std::nothrow) oogl::Buffer(
		GL_ARRAY_BUFFER,
		GL_STATIC_DRAW,
		8 * sizeof(GLfloat),
		saqv
	);
	assert(vbo);

//	set vertex pointers:
	// positions (location=0)
	OOGL_CALL(glEnableVertexAttribArray(0));
	OOGL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0));

//	create model:
	Model *result = new(std::nothrow) Model(name);
	assert(result);
	result->setVertexShader(ShaderUnit::loadFromFile(GL_VERTEX_SHADER, "shaders/models/SAQuadModel.vs.glsl"));

	result->nVertices_ = 4;
	result->nTriangles_ = 2;
	result->nIndices_ = 0; // 2 triangles but no indices
	result->radius_ = 1.5f;
	result->extents_[0] = vec3(-1.0f, -1.0f, 0.0f);
	result->extents_[1] = vec3(1.0f, 1.0f, 0.0f);
	result->setMode(GL_TRIANGLE_STRIP);
	result->vbo_ = vbo;
	result->vao_ = vao;

//	ensure buffers are not bound:
	oogl::VertexArray::bindNone(); // unbind this first so as not to modify!
	oogl::Buffer::bindNone(GL_ARRAY_BUFFER);

	return result;
}

/*----------------------------------------------------------------------------*/
void Model::destroy(Model **ppModel) {
	assert(ppModel != 0);
	assert(*ppModel != 0);
	delete *ppModel;
	*ppModel = 0;
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
void Model::enable(Shader *pShader, AppBase *pApp) {
	assert(pShader);
	assert(vao_);

	vao_->bind();

//	send common uniforms:
	OOGL_CALL(glUniformMatrix4fv(pShader->getUniformLocation("uModelViewMatrix"), 1, GL_FALSE, pApp->getMatrix(AppBase::MODEL_VIEW_MATRIX)));
	OOGL_CALL(glUniformMatrix4fv(pShader->getUniformLocation("uModelViewProjectionMatrix"), 1, GL_FALSE, pApp->getMatrix(AppBase::MODEL_VIEW_PROJECTION_MATRIX)));
#ifdef FRAMEWORK_STORE_PREVIOUS_TRANSFORMS
	OOGL_CALL(glUniformMatrix4fv(pShader->getUniformLocation("uPrevModelViewProjectionMatrix"), 1, GL_FALSE, pApp->getMatrix(AppBase::PREV_MODEL_VIEW_PROJECTION_MATRIX)));
#endif
	OOGL_CALL(glUniform1f(pShader->getUniformLocation("uTanHalfFov"), pApp->getCurrentCamera()->getTanHalfFov()));
	OOGL_CALL(glUniform1f(pShader->getUniformLocation("uAspectRatio"), pApp->getCurrentCamera()->getAspect()));
}

/*----------------------------------------------------------------------------*/
void Model::disable() {
	oogl::VertexArray::bindNone();
	oogl::Buffer::bindNone(GL_ELEMENT_ARRAY_BUFFER);
}

/*----------------------------------------------------------------------------*/
void Model::render(GLenum modeOverride) {
	GLenum useMode;
	if (modeOverride == GL_NONE)
		useMode = mode_;
	else
		useMode = modeOverride;

	assert(vbo_);
	totalTriangles_+= nTriangles_;
	if (ibo_) {
		OOGL_CALL(glDrawElements(useMode, nIndices_, GL_UNSIGNED_INT, (GLvoid*)0));
	} else {
		OOGL_CALL(glDrawArrays(useMode, 0, nVertices_));
	}
}


//	PROTECTED:

//	MEMBERS:
unsigned Model::totalTriangles_ = 0;

} // namespace frm


