/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include "Object.h"

#include "common.h"
#include "AppBase.h"
#include "Camera.h"
#include "IResource.h"
#include "IMaterial.h"
#include "Node.h"
#include "Model.h"
#include "Shader.h"
#include "ShaderUnit.h"

namespace {
	frm::IMaterial *defaultShadowMaterial = 0;
} // namespace

namespace frm {

/*******************************************************************************
	Object implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
Object* Object::create(
	Node *node,
	Model *model,
	IMaterial *material,
	bool neverClip,
	const CStringT &name
) {
	return create(node, model, material, 0, neverClip, name);
}

/*----------------------------------------------------------------------------*/
Object* Object::create(
	Node *node,
	Model *model,
	IMaterial *material,
	IMaterial *shadowMaterial,
	bool neverClip,
	const CStringT &name
) {
//	load default shadow material:
	if (!defaultShadowMaterial) {
		defaultShadowMaterial = IMaterial::create(
			ShaderUnit::loadFromFile(GL_FRAGMENT_SHADER, "shaders/materials/ShadowMaterial.fs.glsl")
		);
		assert(defaultShadowMaterial);
	}

//	attempt to link shader:
	assert(model);
	assert(material);
	Shader *shader = Shader::create(
		5,
		model->getVertexShader(),
		model->getTessControlShader(),
		model->getTessEvalShader(),
		model->getGeometryShader(),
		material->getFragmentShader()
	);
	if (!shader)
		return 0;

//	attempt to link shadow shader:
	if (!shadowMaterial)
		shadowMaterial = defaultShadowMaterial;
	assert(model);
	Shader *shadowShader = Shader::create(
		5,
		model->getVertexShader(),
		model->getTessControlShader(),
		model->getTessEvalShader(),
		model->getGeometryShader(),
		shadowMaterial->getFragmentShader()
	);
	if (!shadowShader)
		return 0;

//	if successful create object:
	Object *result = new(std::nothrow) Object(
		node,
		model,
		material,
		shader,
		shadowMaterial,
		shadowShader,
		neverClip,
		name
	);
	assert(result);
	return result;

}

/*----------------------------------------------------------------------------*/
void Object::destroy(Object **ppObject) {
	assert(ppObject);
	assert(*ppObject);
	delete *ppObject;
	*ppObject = 0;
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
void Object::render(AppBase *app) {
	if (!clip(app->getCurrentClipCamera()->getWorldFrustum()))
		return;

	app->pushMatrix(AppBase::MODEL_MATRIX);
	app->multMatrix(AppBase::MODEL_MATRIX, node_->getWorldMatrix());

#ifdef FRAMEWORK_STORE_PREVIOUS_TRANSFORMS
	app->loadMatrix(
		AppBase::PREV_MODEL_VIEW_PROJECTION_MATRIX,
		app->getCurrentCamera()->getPreviousProjectionMatrix() *
		app->getCurrentCamera()->getPreviousViewMatrix() *
		node_->getPreviousWorldMatrix()
	);
#endif

	shader_->use();
	material_->enable(shader_, app);
	model_->enable(shader_, app);
	model_->render();
	model_->disable();
	material_->disable();

	app->popMatrix(AppBase::MODEL_MATRIX);
}

/*----------------------------------------------------------------------------*/
void Object::renderShadow(AppBase *app) {
	if (!clip(app->getCurrentClipCamera()->getWorldFrustum()))
		return;

	app->pushMatrix(AppBase::MODEL_MATRIX);
	app->multMatrix(AppBase::MODEL_MATRIX, node_->getWorldMatrix());

	shadowShader_->use();
	shadowMaterial_->enable(shadowShader_, app);
	model_->enable(shadowShader_, app);
	model_->render();
	model_->disable();
	shadowMaterial_->disable();

	app->popMatrix(AppBase::MODEL_MATRIX);
}

/*----------------------------------------------------------------------------*/
void Object::renderHelper(AppBase *app) {
	if (!clip(app->getCurrentCamera()->getWorldFrustum()))
		return;

	OOGL_CALL(glPushMatrix());
	OOGL_CALL(glMultMatrixf(node_->getWorldMatrix()));

	//oogl::renderXYZAxes();

	OOGL_CALL(glColor4f(0.1f, 1.0f, 0.1f, 0.25f));
	oogl::renderSphere(model_->getRadius(), 24);

	OOGL_CALL(glColor4f(1.0f, 0.8f, 0.0f, 0.5f));
	OOGL_CALL(oogl::renderBox(model_->getMinExtents(), model_->getMaxExtents()));

	glPopMatrix();
}

} // namespace frm


