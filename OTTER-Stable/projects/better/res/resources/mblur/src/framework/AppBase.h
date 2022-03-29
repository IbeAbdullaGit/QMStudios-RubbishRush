/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_APP_BASE_H_
#define FRAMEWORK_APP_BASE_H_

#include <fstream>
#include <iostream>
#include <utility>
#include <stack>
#include <vector>

#define SFML_STATIC
#include <SFML/Window.hpp>

#include "common.h"
#include "Clock.h"
#include "Frustum.h"
#include "Framebuffer.h"
#include "IGuiComponent.h"
#include "multibuf.h"
#include "Object.h"

namespace frm {

/* Common code for all applications. */
class AppBase {
public:
//	TYPEDEFS/ENUMS:
	enum MatrixNameT {
		MODEL_MATRIX,
		VIEW_MATRIX,
		PROJECTION_MATRIX,

		MODEL_VIEW_MATRIX,
		VIEW_PROJECTION_MATRIX,
		MODEL_VIEW_PROJECTION_MATRIX,
		PREV_MODEL_VIEW_PROJECTION_MATRIX
	};

//	CTORS/DTORS:
	AppBase(
		const CStringT &appName,
		const ivec2 &windowSize,
		const ivec2 &renderSize,
		bool fullscreen = false
	);

	/*	Calls all the appropriate cleanup routines. */
	virtual ~AppBase();

//	ACCESSORS:
	/*	Whether the ctor succeeded. */
	bool initSuccess() const;

	/*	Default framebuffer is dependant on window size. */
	ivec2 getWindowSize() const;
	void setWindowSize(const ivec2&);

	/*	Render size may be independant of window size. */
	ivec2 getRenderSize() const;
	void setRenderSize(const ivec2&);

	Camera* getCurrentCamera();
	void setCurrentCamera(Camera*);
	Camera* getCurrentClipCamera();
	void setCurrentClipCamera(Camera*);

	Camera* getCamera(const CStringT &name);

	Node* getRootNode();

//	SERVICES:
	/*	Enters main loop. Only call this if initSuccess(). */
	void run();

	/*	Toggle between fullscreen/windowed mode. */
	void toggleFullscreen();

	/*	Add node to the scene graph, return node. */
	Node* addNode(Node *node);

	/*	Add object to objects list, return object.*/
	Object* addObject(Object *object);

	/*	Add the given camera to the parent node, return camera. If parent == 0,
		create a new Node and attach to the scene graph. */
	Camera* addCamera(Camera *camera, Node *parent = 0);

	/*	Add a gui component to the parent (or to the gui root if parent == 0).
		Returns the child ptr. */
	IGuiComponent* addGuiComponent(IGuiComponent *child, IGuiComponent *parent = 0);

	/*	Add a texture/shader pair to the render texture list. If showOverlay_ &&
		showRenderTextures_, each texture is visualized (as a thumbnail), using
		the given shader. If shader is 0, a default is used. */
	void addRenderTexture(Texture2d *tex, Shader *shader = 0);

	/*	Return the top of the given matrix stack. */
	const mat4& getMatrix(const MatrixNameT &stk) const;
	/*	Clear the given matrix stack, load the given matrix. */
	void loadMatrix(const MatrixNameT &stk, const mat4 &mat);
	/*	Multiply the given matrix onto the given matrix stack. */
	void multMatrix(const MatrixNameT &stk, const mat4 &mat);
	/*	Push the the given matrix stack. */
	void pushMatrix(const MatrixNameT &stk);
	/*	Pop the given matrix stack. */
	void popMatrix(const MatrixNameT &stk);

protected:
//	MEMBERS:
	bool initSuccess_;
	bool vsyncEnabled_, showOverlay_, showHelpers_, showRenderTextures_;
	Framebuffer *fboDefault_;
	ivec2 renderSize_; // render resolution
	sf::Window *window_; // app window instance
	sf::ContextSettings contextSettings_; // acquired GL context settings
	sf::VideoMode videoMode_;
	sf::Uint32 windowStyle_;

	std::vector<Camera*> cameras_;
	int currentCamera_, currentClipCamera_; // indices into camera list
	Camera *pCurrentCamera_, *pCurrentClipCamera_; // actual cameras returned by accessors

//	SERVICES:
	virtual bool initScene();
	virtual bool initGui();
	virtual bool initRender();

	/*	Event handler; return false to quit the event loop. */
	virtual bool handleEvent(const sf::Event &event);

	/*	Update scene (animation, etc.) by dt seconds. */
	virtual bool update(float dt);

	/*	Render scene. */
	virtual bool render(float dt);
	/*	Render overlay (gui, stats, etc.). */
	virtual bool renderOverlay(float dt);
	/*	Render helpers (bounding volumes, etc.). */
	virtual bool renderHelpers(float dt);

	/*	Iterates over and renders the object list, culls against current
		frustum. */
	bool renderObjects();

	/*	Calls renderShadow() on all objects. Uses pCamera, rather than the app's
		current camera. */
	bool renderObjectShadows(Camera *pCamera);

private:
//	MEMBERS:
	Clock timer_;

	multibuf outbuf_, logbuf_, errbuf_; // replaces cout/clog/cerr
	std::ofstream outfile_, logfile_, errfile_;

	Node *sceneGraph_;

	std::vector<Object*> objects_;


//	common matrix stacks (doesn't use stack adapter, we want clear()!):
	std::vector<mat4> modelMatrixStack_;
	std::vector<mat4> viewMatrixStack_;
	std::vector<mat4> projectionMatrixStack_;
	//	the following are derived whenever one of the matrix stacks is changed:
	mat4 modelViewMatrix_, viewProjectionMatrix_, modelViewProjectionMatrix_;
	mat4 prevModelViewProjectionMatrix_; // special, access via getMatrix() and loadMatrix()

	IGuiComponent *guiRoot_;

	std::vector<std::pair<Texture2d*, Shader*> > renderTextures_;
	static Shader *shaderDefaultVis_; // for visualizing render textures

//	SERVICES:
	/*	Init window/gl context + external libs. */
	bool initMain(
		const CStringT &appName,
		const ivec2 &windowSize,
		bool fullscreen
	);

	/*	Computes derived matrices from the tops of the matrix stacks. */
	void computeDerivedMatrices();

}; // class AppBase

/*******************************************************************************
	AppBase inline implementation:
*******************************************************************************/

//	PUBLIC:

//	ACCSSORS:

/*----------------------------------------------------------------------------*/
inline bool AppBase::initSuccess() const {
	return initSuccess_;
}

/*----------------------------------------------------------------------------*/
inline ivec2 AppBase::getWindowSize() const {
	return ivec2(window_->getSize().x, window_->getSize().y);
}

/*----------------------------------------------------------------------------*/
inline void AppBase::setWindowSize(const ivec2 &size) {
	window_->setSize(sf::Vector2u(size.x(), size.y()));
	fboDefault_->setSize(size.x(), size.y());
	guiRoot_->setSize(size);
}

/*----------------------------------------------------------------------------*/
inline ivec2 AppBase::getRenderSize() const {
	return renderSize_;
}

/*----------------------------------------------------------------------------*/
inline void AppBase::setRenderSize(const ivec2 &size) {
	renderSize_ = size;
}

/*----------------------------------------------------------------------------*/
inline Camera* AppBase::getCurrentCamera() {
	return pCurrentCamera_;
}
/*----------------------------------------------------------------------------*/
inline void AppBase::setCurrentCamera(Camera *pCamera) {
	assert(pCamera);
	pCurrentCamera_ = pCamera;
}

/*----------------------------------------------------------------------------*/
inline Camera* AppBase::getCurrentClipCamera() {
	return pCurrentClipCamera_;
}
/*----------------------------------------------------------------------------*/
inline void AppBase::setCurrentClipCamera(Camera *pCamera) {
	assert(pCamera);
	pCurrentClipCamera_ = pCamera;
}


/*----------------------------------------------------------------------------*/
inline Node* AppBase::getRootNode() {
	return sceneGraph_;
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
inline const mat4& AppBase::getMatrix(const MatrixNameT &stk) const {
	switch (stk) {
		case MODEL_MATRIX:
			assert(!modelMatrixStack_.empty());
			return modelMatrixStack_.back();
		case VIEW_MATRIX:
			assert(!viewMatrixStack_.empty());
			return viewMatrixStack_.back();
		case PROJECTION_MATRIX:
			assert(!projectionMatrixStack_.empty());
			return projectionMatrixStack_.back();
		case MODEL_VIEW_MATRIX:
			return modelViewMatrix_;
		case VIEW_PROJECTION_MATRIX:
			return viewProjectionMatrix_;
		case MODEL_VIEW_PROJECTION_MATRIX:
			return modelViewProjectionMatrix_;
		case PREV_MODEL_VIEW_PROJECTION_MATRIX:
			return prevModelViewProjectionMatrix_;
		default:
			assert(false && "Invalid matrix name");
			break;
	};
}
/*----------------------------------------------------------------------------*/
inline void AppBase::loadMatrix(const MatrixNameT &stk, const mat4 &mat) {
	switch (stk) {
		case MODEL_MATRIX:
			modelMatrixStack_.clear();
			modelMatrixStack_.push_back(mat);
			break;
		case VIEW_MATRIX:
			viewMatrixStack_.clear();
			viewMatrixStack_.push_back(mat);
			break;
		case PROJECTION_MATRIX:
			projectionMatrixStack_.clear();
			projectionMatrixStack_.push_back(mat);
			break;
		case PREV_MODEL_VIEW_PROJECTION_MATRIX:
			prevModelViewProjectionMatrix_ = mat;
			break;
		default:
			break;
	};
	computeDerivedMatrices();
}
/*----------------------------------------------------------------------------*/
inline void AppBase::multMatrix(const MatrixNameT &stk, const mat4 &mat) {
	switch (stk) {
		case MODEL_MATRIX:
			modelMatrixStack_.back() = mat * modelMatrixStack_.back();
			break;
		case VIEW_MATRIX:
			viewMatrixStack_.back() = mat * viewMatrixStack_.back();
			break;
		case PROJECTION_MATRIX:
			projectionMatrixStack_.back() = mat * projectionMatrixStack_.back();
			break;
		default:
			break;
	};
	computeDerivedMatrices();
}
/*----------------------------------------------------------------------------*/
inline void AppBase::pushMatrix(const MatrixNameT &stk) {
	switch (stk) {
		case MODEL_MATRIX:
			modelMatrixStack_.push_back(modelMatrixStack_.back());
			break;
		case VIEW_MATRIX:
			viewMatrixStack_.push_back(viewMatrixStack_.back());
			break;
		case PROJECTION_MATRIX:
			projectionMatrixStack_.push_back(projectionMatrixStack_.back());
			break;
		default:
			break;
	};
	computeDerivedMatrices();
}
/*----------------------------------------------------------------------------*/
inline void AppBase::popMatrix(const MatrixNameT &stk) {
	switch (stk) {
		case MODEL_MATRIX:
			modelMatrixStack_.pop_back();
			break;
		case VIEW_MATRIX:
			viewMatrixStack_.pop_back();
			break;
		case PROJECTION_MATRIX:
			projectionMatrixStack_.pop_back();
			break;
		default:
			break;
	};
	computeDerivedMatrices();
}

/*----------------------------------------------------------------------------*/
inline void AppBase::computeDerivedMatrices() {
	modelViewMatrix_ = viewMatrixStack_.back() * modelMatrixStack_.back();
	viewProjectionMatrix_ = projectionMatrixStack_.back() * viewMatrixStack_.back();
	modelViewProjectionMatrix_ = projectionMatrixStack_.back() * modelViewMatrix_;
}

} // namespace frm

#endif // FRAMEWORK_APP_BASE_H_
