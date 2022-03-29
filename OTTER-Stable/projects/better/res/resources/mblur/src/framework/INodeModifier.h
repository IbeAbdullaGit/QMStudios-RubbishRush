/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_INODE_MODIFIER_H_
#define FRAMEWORK_INODE_MODIFIER_H_

#include "common.h"
#include "IResource.h"
#include "Node.h"

namespace frm {

/* Base class for node modifiers. These update a node's local matrix during
	scene graph update. A modifier may only be associated with a single node,
	with which its lifetime is linked. */
class INodeModifier: public IResource<INodeModifier> {
public:
//	TYPEDEFS/ENUMS:
	typedef void(*CallbackT)(INodeModifier&);

//	CTORS/DTORS:
	/*	Destroy modifier, remove from parent. */
	static void destroy(INodeModifier **ppNode);

//	ACCESSORS:
	CallbackT getOnComplete() const;
	void setOnComplete(CallbackT);

	Node* getParent() const;
	void setParent(Node*);

//	SERVICES:
	/*	Updates parent node's local matrix. If onComplete_ is called, update()
		should immediately return, as onComplete_ *may* cause the modifier to
		be destroyed. */
	virtual void update(TimeT dt) = 0;

	/*	Reset the controller. */
	virtual void reset();
	/*	Static for use as a callback. */
	static void reset(INodeModifier&);

	/*	Reset the controller relative to its current state. */
	virtual void relativeReset();
	/*	Static for use as a callback. */
	static void relativeReset(INodeModifier&);

	/*	Reverse the controller. */
	virtual void reverse();
	/*	Static for use as a callback. */
	static void reverse(INodeModifier&);

	/*	Destroy, remove from parent. */
	static void remove(INodeModifier&);

protected:
//	MEMBERS:
	CallbackT onComplete_;

//	CTORS/DTORS:
	INodeModifier(Node *parent, CallbackT onComplete, const CStringT &name);
	/*	Dtor needs to be virtual, as INodeModifier can be derived from. */
	virtual ~INodeModifier();

private:
//	MEMBERS:
	Node *parent_; // each modifier may be associated with exactly 1 node

}; // class INodeModifier

/*******************************************************************************
	INodeModifier inline implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline void INodeModifier::destroy(INodeModifier **ppNode) {
	assert(ppNode != 0);
	assert(*ppNode != 0);

	if ((*ppNode)->parent_)
		(*ppNode)->parent_->removeModifier(*ppNode);

	delete *ppNode;
	*ppNode = 0;
}

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline INodeModifier::CallbackT INodeModifier::getOnComplete() const {
	return onComplete_;
}
/*----------------------------------------------------------------------------*/
inline void INodeModifier::setOnComplete(CallbackT callback) {
	onComplete_ = callback;
}

/*----------------------------------------------------------------------------*/
inline Node* INodeModifier::getParent() const {
	return parent_;
}
/*----------------------------------------------------------------------------*/
inline void INodeModifier::setParent(Node *p) {
	parent_ = p;
}

// SERVICES:
/*----------------------------------------------------------------------------*/
inline void INodeModifier::reset() {
}
/*----------------------------------------------------------------------------*/
inline void INodeModifier::reset(INodeModifier &mdf) {
	mdf.reset();
}

/*----------------------------------------------------------------------------*/
inline void INodeModifier::relativeReset() {
}
/*----------------------------------------------------------------------------*/
inline void INodeModifier::relativeReset(INodeModifier &mdf) {
	mdf.relativeReset();
}

/*----------------------------------------------------------------------------*/
inline void INodeModifier::reverse() {
}
/*----------------------------------------------------------------------------*/
inline void INodeModifier::reverse(INodeModifier &mdf) {
	mdf.reverse();
}

/*----------------------------------------------------------------------------*/
inline void INodeModifier::remove(INodeModifier &mdf) {
	INodeModifier *pMdf = &mdf;
	INodeModifier::destroy(&pMdf);
}

//	PROTECTED:

/*----------------------------------------------------------------------------*/
inline INodeModifier::INodeModifier(Node *parent, CallbackT onComplete, const CStringT &name)
:	IResource(name),
	onComplete_(onComplete),
	parent_(parent) {
}
/*----------------------------------------------------------------------------*/
inline INodeModifier::~INodeModifier() {
}

} // namespace frm

#endif // FRAMEWORK_INODE_MODIFIER_H_
