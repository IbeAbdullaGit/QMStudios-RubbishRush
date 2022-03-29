/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_IRESOURCE_H_
#define FRAMEWORK_IRESOURCE_H_

#include <string>
#include <vector>

#include "common.h"

namespace frm {

/**	Base class for all resource types. Facilitates management of all resource
		instances.
		@todo Make access to static data thread safe.
		@todo Force \c name_ to be unique. */
template <class C>
class IResource {
public:
//	ACCESSORS:
	/**	Get/set name. */
	const std::string& getName() const;
	void setName(const std::string&);

//	SERVICES:
	/**	Search instances for the first matching \c name. Return 0 if \c name
			not found. */
	static C* find(const std::string &name);

	/**	Calls \c destroy() on all instances, destructing each instance and
			releasing all associated resources. */
	static void destroyAll();

protected:
//	MEMBERS:
	/**	Per-instance string. */
	std::string name_;

//	CTORS/DTORS:
	/**	Add \c this to internal instances list. */
	IResource(const std::string &name = "");
	/**	Remove \c this from internal instances list.
			@note Does not need to be virtual since instances of \c C will not
			be deleted via an \c IResource ptr. */
	~IResource();

//	ACCESSORS:
	/**	Return the ith instance from the internal instances list. */
	static C& instance(size_t i);

	/**	Return number of instances. */
	static size_t nInstances();

private:
//	MEMBERS:
	/**	Internal list of instances. */
	static std::vector<C*> instances_;

//	CTORS/DTORS:
	/**	Private, unimplemented copy ctors prevent copying. */
	explicit IResource(const IResource&);
	IResource& operator=(const IResource&);

}; // class IResource


/*******************************************************************************
	IResource inline implementation:
*******************************************************************************/

/*----------------------------------------------------------------------------*/
template <class C>
inline const std::string& IResource<C>::getName() const {
	return name_;
}
/*----------------------------------------------------------------------------*/
template <class C>
inline void IResource<C>::setName(const std::string &name) {
	name_ = name;
}

} // namespace frm

#endif // FRAMEWORK_IRESOURCE_H_

