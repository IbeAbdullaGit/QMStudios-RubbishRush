/*******************************************************************************
	Copyright (C) 2012 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include <algorithm> // find
#include <string>
#include <vector>

#include "IResource.h"
#include "common.h"

// for explicit template instantiation (also see bottom of file):
#include "Buffer.h"
#include "Font.h"
#include "Shader.h"
#include "ShaderUnit.h"
#include "IGuiComponent.h"
#include "ITexture.h"
#include "Mesh.h"
#include "Model.h"
#include "IMaterial.h"
#include "Framebuffer.h"
#include "Object.h"
#include "Node.h"
#include "INodeModifier.h"

namespace frm {

/*******************************************************************************
	IResource implementation:
*******************************************************************************/

//	PUBLIC:

//	SERVICES:

/*----------------------------------------------------------------------------*/
template <class C>
C* IResource<C>::find(const std::string &name) {
	for (size_t i = 0; i < instances_.size(); ++i)
		if (instances_[i]->name_.compare(name) == 0)
			return instances_[i];
	return 0;
}

/*----------------------------------------------------------------------------*/
template <class C>
void IResource<C>::destroyAll() {
//	call destroy on front element; IResource dtor will remove from instances_
//	list hence the list will eventually become empty
	while (!instances_.empty()) {
	// must copy ptr to prevent destroy() nullifying the 0 entry in the intances
	//	list, which would be the wrong entry as ~IResource removes its copy from
	//	the list prior to destroy() nullifying it!
		C *pInstance = instances_[0];
		C::destroy(&pInstance);
	}
}

//	PROTECTED:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
template <class C>
IResource<C>::IResource(const std::string &name)
:	name_(name) {
	instances_.push_back(static_cast<C*>(this));
}

/*----------------------------------------------------------------------------*/
template <class C>
IResource<C>::~IResource() {
	typename std::vector<C*>::iterator it;
	it = std::find(instances_.begin(), instances_.end(), static_cast<C*>(this));
	assert(it != instances_.end());
	instances_.erase(it);
}

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
template <class C>
C& IResource<C>::instance(size_t i) {
	assert(!instances_.empty());
	assert(i < instances_.size());

	return *instances_[i];
}

/*----------------------------------------------------------------------------*/
template <class C>
size_t IResource<C>::nInstances() {
	return instances_.size();
}

//	PRIVATE:

//	MEMBERS:

template <class C>
std::vector<C*> IResource<C>::instances_;


/*******************************************************************************
	Explicit template instantiation:
*******************************************************************************/
template class IResource<Buffer>;
template class IResource<Font>;
template class IResource<Shader>;
template class IResource<ShaderUnit>;
template class IResource<IGuiComponent>;
template class IResource<ITexture>;
template class IResource<Mesh>;
template class IResource<Model>;
template class IResource<IMaterial>;
template class IResource<Framebuffer>;
template class IResource<Object>;
template class IResource<Node>;
template class IResource<INodeModifier>;

} // namespace frm

