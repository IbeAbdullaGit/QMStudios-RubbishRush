/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_OBJECT_H_
#define FRAMEWORK_OBJECT_H_

#include "common.h"
#include "Frustum.h"
#include "Node.h"
#include "IResource.h"
#include "Model.h"
#include "Shader.h"

namespace frm {

/* Groups a model/material and binds to an Node. */
class Object: public IResource<Object> {
public:
//	CTORS/DTORS:
	/*	Node, model and material cannot be null. */
	static Object* create(
		Node *node,
		Model *model,
		IMaterial *material,
		bool neverClip = false,
		const CStringT &name = ""
	);

	/*	As above, provide custome shadow material. */
	static Object* create(
		Node *node,
		Model *model,
		IMaterial *material,
		IMaterial *shadowMaterial,
		bool neverClip = false,
		const CStringT &name = ""
	);

	static void destroy(Object **ppObject);

//	ACCESSORS:
	Node* getNode();
	void setNode(Node*);

	Model* getModel();
	void setModel(Model*);

	IMaterial* getMaterial();
	void setMaterial(IMaterial*);

	Shader* getShader();
	void setShader(Shader *pShader);

	/*	Get/set the value of neverClip_. If neverClip_ is true, clip() always
		returns true. */
	bool neverClip() const;
	void neverClip(bool);

	/*	Extract & return position from node's matrix*/
	vec3 getPosition() const;
	/*	Return model's radius. */
	float getRadius() const;

//	SERVICES:
	/*	Return whether this object is inside the frustum. */
	bool clip(const Frustum &frustum);

	/*	Enabled model/material, make draw call. */
	void render(AppBase*);

	/*	Enable model/shadow material, draw call. */
	void renderShadow(AppBase*);

	/*	Render bounding box/bounding sphere*/
	void renderHelper(AppBase*);


protected:
//	MEMBERS:
	Node *node_;
	Model *model_;
	IMaterial *material_, *shadowMaterial_;
	Shader *shader_, *shadowShader_; // from model + material

	bool neverClip_;

//	CTORS/DTORS:
	Object(
		Node *node,
		Model *model,
		IMaterial *material,
		Shader *shader,
		IMaterial *shadowMaterial = 0,
		Shader *shadowShader = 0,
		bool neverClip = false,
		const CStringT &name = ""
	);

	virtual ~Object();

}; // class Object

/*******************************************************************************
	Object inline implementation:
*******************************************************************************/

//	PUBLIC:

//	ACCESSORS

/*----------------------------------------------------------------------------*/
inline Node* Object::getNode() {
	return node_;
}
/*----------------------------------------------------------------------------*/
inline void Object::setNode(Node *node) {
	assert(node);
	node_ = node;
}

/*----------------------------------------------------------------------------*/
inline Model* Object::getModel() {
	return model_;
}
/*----------------------------------------------------------------------------*/
inline void Object::setModel(Model *model) {
	assert(model);
	model_ = model;
}

/*----------------------------------------------------------------------------*/
inline IMaterial* Object::getMaterial() {
	return material_;
}
/*----------------------------------------------------------------------------*/
inline void Object::setMaterial(IMaterial *material) {
	assert(material);
	material_ = material;
}

/*----------------------------------------------------------------------------*/
inline Shader* Object::getShader() {
	return shader_;
}

/*----------------------------------------------------------------------------*/
inline void Object::setShader(Shader *pShader) {
	assert(pShader);
	shader_ = pShader;
}

/*----------------------------------------------------------------------------*/
inline bool Object::neverClip() const {
	return neverClip_;
}
/*----------------------------------------------------------------------------*/
inline void Object::neverClip(bool nc) {
	neverClip_ = nc;
}

/*----------------------------------------------------------------------------*/
inline vec3 Object::getPosition() const {
	return vec3(node_->getWorldMatrix().col(3));
}

/*----------------------------------------------------------------------------*/
inline float Object::getRadius() const {
	return model_->getRadius();
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
inline bool Object::clip(const Frustum &frustum) {
	return frustum.isInside(getPosition(), getRadius()) || neverClip_;
}

//	PROTECTED:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline Object::Object(
	Node *node,
	Model *model,
	IMaterial *material,
	Shader *shader,
	IMaterial *shadowMaterial,
	Shader *shadowShader,
	bool neverClip,
	const CStringT &name
):	IResource(name),
	node_(node),
	model_(model),
	material_(material),
	shader_(shader),
	shadowMaterial_(shadowMaterial),
	shadowShader_(shadowShader),
	neverClip_(neverClip) {

	assert(node);
	assert(model);
	assert(material);
	//assert(shadowMaterial); // can be null
	assert(shader);
	//assert(shadowShader); // can be null
}

inline Object::~Object() {
}

} // namespace frm

#endif // FRAMEWORK_OBJECT_H_
