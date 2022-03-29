/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_APP_3D_DEFERRED_H_
#define FRAMEWORK_APP_3D_DEFERRED_H_

#include "common.h"
#include "App3d.h"
#include "Camera.h"

namespace frm {

/* Provides a separate list of lights and mechanism for rendering them. */
class App3dDeferred: public App3d {
public:
//	CTORS/DTORS:
	App3dDeferred(
		const CStringT &appName,
		const ivec2 &windowSize,
		const ivec2 &renderSize,
		bool fullscreen = false
	);

protected:
//	SERVICES:
	Object* addLight(Object*);

	bool renderLights();

private:
//	MEMBERS:
	std::vector<Object*> lights_;

//	SERVICES:

}; // class App3dDeferred

/*******************************************************************************
	App3dDeferred inline implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:
App3dDeferred::App3dDeferred(
	const CStringT &appName,
	const ivec2 &windowSize,
	const ivec2 &renderSize,
	bool fullscreen
): App3d(appName, windowSize, renderSize, fullscreen) {
}

//	ACCESSORS:

//	SERVICES:

/*----------------------------------------------------------------------------*/
inline Object* App3dDeferred::addLight(Object *pLight) {
	assert(pLight);
	lights_.push_back(pLight);
	return pLight;
}

/*----------------------------------------------------------------------------*/
inline bool App3dDeferred::renderLights() {
	for (int i = 0; i < lights_.size(); ++i) {


	//	reload current camera matrices after rendering shadow map:
		loadMatrix(PROJECTION_MATRIX, getCurrentCamera()->getProjectionMatrix());
		loadMatrix(VIEW_MATRIX, getCurrentCamera()->getViewMatrix());

		lights_[i]->render(this);
	}
	return true;
}

} // namespace frm

#endif // FRAMEWORK_APP_3D_DEFERRED_H_
