/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_NODE_H_
#define FRAMEWORK_NODE_H_

#include <vector>

#include "common.h"
#include "IResource.h"

namespace frm {

/* */
class Node: public IResource<Node> {
public:
//	CTORS/DTORS:
	static Node* create(const CStringT &name = "");
	static Node* create(Node *parent, const CStringT &name = "");

	/*	Destroys ppNode. If destroyChildren, any children of the deleted node are
		recursively destroyed. Otherwise children are reparented to the node's
		parent (if it has one, else their parent becomes 0). All modifiers are
		destoryed. */
	static void destroy(Node **ppNode, bool destroyChildren = true);

//	ACCESSORS:
	/*	Assume a prior call to update() (i.e. after all modifiers have executed). */
	const mat4& getLocalMatrix() const;
	void setLocalMatrix(const mat4&);

	/*	Multiply input onto local matrix. */
	void mulLocalMatrix(const mat4&);

	/*	Assume a prior call to update() (i.e. after all modifiers have executed)
		and world matrix has been built. */
	const mat4& getWorldMatrix() const;
	void setWorldMatrix(const mat4&);

#ifdef FRAMEWORK_STORE_PREVIOUS_TRANSFORMS
	const mat4& getPreviousLocalMatrix() const;
	const mat4& getPreviousWorldMatrix() const;
#endif

	Node* getParent() const;
	void setParent(Node*);

	Node* getChild(int i) const;
	int getNChildren() const;

	INodeModifier* getModifier(int i) const;
	int getNModifiers() const;

//	SERVICES:
	/*	Add to child list. Remove child from its parent node if applicable. */
	void addChild(Node *child);
	/*	Remove from child list. If child is not found, do nothing. This does NOT
		delete the child, or reparent its children to this node. */
	void removeChild(Node *child);

	/*	Add to modifier list. */
	void addModifier(INodeModifier *modifier);
	/*	Remove from modifier list. This does NOT destroy the modifier. */
	void removeModifier(INodeModifier *modifier);

	/* Executes all modifiers in order, then build world matrix. */
	void update(TimeT dt);

#ifndef NDEBUG
	void print(int tabin = 0);
#endif

private:
//	MEMBERS:
	struct NodeMatricesT {
		mat4 local, world;
	};

	NodeMatricesT current_;
#ifdef FRAMEWORK_STORE_PREVIOUS_TRANSFORMS
	NodeMatricesT previous_;
#endif

	Node *parent_;
	std::vector<Node*> children_;

	std::vector<INodeModifier*> modifiers_; // modifiers are executed in-order

//	CTORS/DTORS:
	Node(Node *parent, const CStringT &name);
	~Node();

}; // class Node

/*******************************************************************************
	Node inline implementation:
*******************************************************************************/

//	PUBLIC:

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline const mat4& Node::getLocalMatrix() const {
	return current_.local;
}
/*----------------------------------------------------------------------------*/
inline void Node::setLocalMatrix(const mat4 &m) {
	current_.local = m;
}

/*----------------------------------------------------------------------------*/
inline void Node::mulLocalMatrix(const mat4 &m) {
	current_.local *= m;
}

/*----------------------------------------------------------------------------*/
inline const mat4& Node::getWorldMatrix() const {
	return current_.world;
}
/*----------------------------------------------------------------------------*/
inline void Node::setWorldMatrix(const mat4 &m) {
	current_.world = m;
}

#ifdef FRAMEWORK_STORE_PREVIOUS_TRANSFORMS
/*----------------------------------------------------------------------------*/
inline const mat4& Node::getPreviousLocalMatrix() const {
	return previous_.local;
}
/*----------------------------------------------------------------------------*/
inline const mat4& Node::getPreviousWorldMatrix() const {
	return previous_.world;
}
#endif

/*----------------------------------------------------------------------------*/
inline Node* Node::getParent() const {
	return parent_;
}
/*----------------------------------------------------------------------------*/
inline void Node::setParent(Node *p) {
	parent_ = p;
}

/*----------------------------------------------------------------------------*/
inline Node* Node::getChild(int i) const {
	assert(i < getNChildren());
	return children_[i];
}
/*----------------------------------------------------------------------------*/
inline int Node::getNChildren() const {
	return children_.size();
}

/*----------------------------------------------------------------------------*/
inline INodeModifier* Node::getModifier(int i) const {
	assert(i < getNModifiers());
	return modifiers_[i];
}
/*----------------------------------------------------------------------------*/
inline int Node::getNModifiers() const {
	return modifiers_.size();
}

//	PRIVATE:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline Node::Node(Node *parent, const CStringT &name)
:	IResource(name),
	parent_(0) { // initially have no parent

	if (parent) {
		parent->addChild(this);
		parent_ = parent;
	}

	current_.world = current_.local = mat4::identity();
}

/*----------------------------------------------------------------------------*/
inline Node::~Node() {
//	see destroy() for how children are reparented
}

} // namespace frm

#endif // FRAMEWORK_NODE_H_


