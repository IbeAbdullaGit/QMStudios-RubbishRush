/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include <vector>

#include "Node.h"
#include "common.h"
#include "INodeModifier.h"

namespace frm {

/*******************************************************************************
	Node implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
Node* Node::create(const CStringT &name) {
	Node *result = new(std::nothrow) Node(0, name);
	assert(result);
	return result;
}
/*----------------------------------------------------------------------------*/
Node* Node::create(Node *parent, const CStringT &name) {
	Node *result = new(std::nothrow) Node(parent, name);
	assert(result);
	return result;
}

/*----------------------------------------------------------------------------*/
void Node::destroy(Node **ppNode, bool destroyChildren) {
	assert(ppNode != 0);
	assert(*ppNode != 0);

//	destroy/reparent children as appropriate:
	if (destroyChildren) {
		for (int i = 0; i < (*ppNode)->getNChildren(); ++i) {
			Node *pChild= (*ppNode)->getChild(i);
			destroy(&pChild, true);
		}
	} else {
		for (int i = 0; i < (*ppNode)->getNChildren(); ++i)
			(*ppNode)->getChild(i)->setParent((*ppNode)->getParent());
	}

//	destroy modifiers:
	for (int i = 0; i < (*ppNode)->getNModifiers(); ++i) {
		INodeModifier *pModifier = (*ppNode)->getModifier(i);
		pModifier->setParent(0); // null parent to avoid calling removeModifier()
		INodeModifier::destroy(&pModifier);
	}

//	delete the node:
	delete *ppNode;
	*ppNode = 0;
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
void Node::addChild(Node *child) {
	assert(child != 0);
	if (child->getParent())
		child->getParent()->removeChild(child);
	child->setParent(this);
	children_.push_back(child);
}
/*----------------------------------------------------------------------------*/
void Node::removeChild(Node *child) {
	assert(child != 0);
	for (size_t i = 0; i < children_.size(); ++i) {
		if (children_[i] == child) {
			children_.erase(children_.begin() + i);
			break;
		}
	}
	child->setParent(0);
}

/*----------------------------------------------------------------------------*/
void Node::addModifier(INodeModifier *modifier) {
	assert(modifier != 0);
	modifier->setParent(this);
	modifiers_.push_back(modifier);
}
/*----------------------------------------------------------------------------*/
void Node::removeModifier(INodeModifier *modifier) {
	assert(modifier != 0);
	for (size_t i = 0; i < modifiers_.size(); ++i) {
		if (modifiers_[i] == modifier) {
			modifiers_.erase(modifiers_.begin() + i);
			modifier->setParent(0);
			break;
		}
	}
}

/*----------------------------------------------------------------------------*/
void Node::update(TimeT dt) {
#ifdef FRAMEWORK_STORE_PREVIOUS_TRANSFORMS
//	store previous values:
	previous_ = current_;
#endif

//	apply modifiers:
	current_.local = mat4::identity(); // reset local matrix first!
	for (int i = 0; i < getNModifiers(); ++i) {
		getModifier(i)->update(dt);
	}

//	build world matrix:
	if (parent_)
		current_.world = parent_->getWorldMatrix() * current_.local;
	else
		current_.world = current_.local;

//	update children:
	for (int i = 0; i < getNChildren(); ++i)
		getChild(i)->update(dt);
}

#ifndef NDEBUG
/*----------------------------------------------------------------------------*/
void Node::print(int tabin) {
	char *tabstr = new(std::nothrow) char[tabin + 1];
	for (int i = 0; i < tabin; ++i)
		tabstr[i] = '\t';
	tabstr[tabin] = '\0';
	std::cout << tabstr << "" << name_ << " (0x" << this << ") {\n";
	if (getNModifiers() > 0) {
		for (int i = 0;i < getNModifiers(); ++i)
			std::cout << tabstr << "\t+" << getModifier(i)->getName() << "(0x" << getModifier(i) << ")\n";
	}
	for (int i = 0; i < getNChildren(); ++i)
		getChild(i)->print(tabin + 1);
	std::cout << tabstr << "}\n";
	delete[] tabstr;
}
#endif

} // namespace frm
