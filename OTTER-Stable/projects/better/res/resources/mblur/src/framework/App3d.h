/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_APP_3D_H_
#define FRAMEWORK_APP_3D_H_

#include "common.h"
#include "AppBase.h"
#include "Camera.h"

namespace frm {

/* For 3d apps; provides support for first person camera, etc. */
class App3d: public AppBase {
public:
//	CTORS/DTORS:
	App3d(
		const CStringT &appName,
		const ivec2 &windowSize,
		const ivec2 &renderSize,
		bool fullscreen = false
	);

protected:
//	MEMBERS:
	std::vector<FirstPersonCtrl*> fpCtrls_;
	int currentFpCtrl_; // where to send input

//	SERVICES:
	virtual bool initScene();

	virtual bool handleEvent(const sf::Event &event);

	virtual bool update(float dt);

	/*	Calls AppBase::renderHelpers() to render bounding boxes, etc. then renders
		all cameras (except the current camera). */
	virtual bool renderHelpers(float dt);

	/*	Add the given FirstPersonCtrl to a new scene node (which is returned).
		Optionally parent the new scene node to parent. */
	Node* addFirstPersonNode(FirstPersonCtrl *fpCtrl, Node *parent = 0);

private:
//	MEMBERS:

//	SERVICES:

}; // class App3d

} // namespace frm

#endif // FRAMEWORK_APP_3D_H_

