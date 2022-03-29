/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include "App3d.h"
#include "common.h"
#include "Node.h"
#include "FirstPersonCtrl.h"

namespace frm {

/*******************************************************************************
	AppBase implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
App3d::App3d(
	const CStringT &appName,
	const ivec2 &windowSize,
	const ivec2 &renderSize,
	bool fullscreen
):	AppBase(appName, windowSize, renderSize, fullscreen),
	currentFpCtrl_(0) {
}

//	PROTECTED:

//	SERVICES:

/*----------------------------------------------------------------------------*/
Node* App3d::addFirstPersonNode(FirstPersonCtrl *fpCtrl, Node *parent) {
	assert(fpCtrl);
	fpCtrls_.push_back(fpCtrl);
	Node *result = addNode(Node::create(parent, fpCtrl->getName().c_str()));
	result->addModifier(fpCtrl);
	return result;
}

/*----------------------------------------------------------------------------*/
bool App3d::initScene() {
	if (!AppBase::initScene())
		return false;

	addCamera(
		Camera::create(
			(float)getRenderSize().x() / (float)getRenderSize().y(), // aspect
			sml::radians(60.0f), // fov
			0.1f, 500.0f, // near/far
			false // ortho
		),
		addFirstPersonNode(FirstPersonCtrl::create(sml::vec3f(0.0f, 0.0f, -30.0f)))
	);
	pCurrentCamera_ = cameras_[currentCamera_];
	pCurrentClipCamera_ = cameras_[currentClipCamera_];
	return true;
}

/*----------------------------------------------------------------------------*/
bool App3d::handleEvent(const sf::Event &event) {
	switch (event.type) {
		case sf::Event::KeyPressed:
			switch (event.key.code) {
				case sf::Keyboard::C: {
					++currentClipCamera_;
					if (currentClipCamera_ >= cameras_.size())
						currentClipCamera_ = 0;
					if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
						currentCamera_ = currentClipCamera_;

					pCurrentCamera_ = cameras_[currentCamera_];
					pCurrentClipCamera_ = cameras_[currentClipCamera_];

				//	if current camera's parent has an FP controller, set that as the
				//	current FP controller:
					Camera *cam = cameras_[currentCamera_];
					Node *camNode = cam->getParent();
					for (int i = 0; i < camNode->getNModifiers(); ++i) {
						for (int j = 0; j < fpCtrls_.size(); ++j) {
							if (camNode->getModifier(i) == fpCtrls_[j]) {
								currentFpCtrl_ = j;
								return true;
							}
						}
					}



					return true; // override base action
				}
				default:
					break;
			};
			break;

		default:
			break;
	};

	return AppBase::handleEvent(event);
}

/*----------------------------------------------------------------------------*/
bool App3d::update(float dt) {
	if (!AppBase::update(dt))
		return false;

	static const float ROTATION_ACCEL = 50.0f; // rate of acceleration in radians/sec
	static const float TRANSLATION_ACCEL = 100.0f; // rate of acceleration in units/sec
	static const float MOUSE_SENSITIVITY = 1.0f; // rate of acceleration in units/sec
	float rotationDelta = ROTATION_ACCEL * dt;
	float translationDelta = TRANSLATION_ACCEL * dt;

	Camera *camera = cameras_[currentCamera_];
	FirstPersonCtrl *fpCtrl = fpCtrls_[currentFpCtrl_];

//	handle async input:
	sf::Vector2i sfMousePos = sf::Mouse::getPosition(*window_);
	ivec2 mousePos = ivec2(sfMousePos.x, sfMousePos.y);
	static ivec2 prevMousePos = mousePos;

	ivec2 idelta = mousePos - prevMousePos;
	vec2 delta = vec2(
		(float)idelta.x() / (float)getWindowSize().x(),
		(float)idelta.y() / (float)getWindowSize().y()
	);
	delta *= MOUSE_SENSITIVITY;
	prevMousePos = mousePos;

	//	mouse look:
	if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
			float fov = camera->getFov() + delta.y() * rotationDelta;
			fov = sml::clamp(fov, sml::radians(1.0f), sml::radians(179.0f));
			camera->setFov(fov);
		} else {
			fpCtrl->pitch(delta.y() * rotationDelta / dt);
		}
		fpCtrl->yaw(delta.x() * rotationDelta / dt);
	}

	//	WASD motion:
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
		translationDelta *= 2.0f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		fpCtrl->forward(translationDelta);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		fpCtrl->backward(translationDelta);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		fpCtrl->left(translationDelta);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		fpCtrl->right(translationDelta);

	//	joystick motion:
	if (sf::Joystick::isConnected(0)) {
	//	we want xyzv in [0,1], so divide by 100:
		float x = sf::Joystick::getAxisPosition(0, sf::Joystick::X) / 100.0f;
		float y = sf::Joystick::getAxisPosition(0, sf::Joystick::Y) / 100.0f;
		float z = sf::Joystick::getAxisPosition(0, sf::Joystick::Z) / 100.0f;
		float v = sf::Joystick::getAxisPosition(0, sf::Joystick::R) / 100.0f;

		if (sf::Joystick::isButtonPressed(0, 5)) {
			float fov = camera->getFov() + v * rotationDelta * 0.1f;
			fov = sml::clamp(fov, sml::radians(1.0f), sml::radians(179.0f));
			camera->setFov(fov);
		} else {
			fpCtrl->pitch(v * rotationDelta * 0.5f);
		}

		fpCtrl->backward(y * rotationDelta);
		fpCtrl->right(x * rotationDelta);
		fpCtrl->yaw(z * rotationDelta);
	}

	return true;
}

/*----------------------------------------------------------------------------*/
bool App3d::renderHelpers(float dt) {
	if (!AppBase::renderHelpers(dt))
		return false;

	OOGL_CALL(glMatrixMode(GL_PROJECTION));
	OOGL_CALL(glLoadMatrixf(getMatrix(PROJECTION_MATRIX)));
	OOGL_CALL(glMatrixMode(GL_MODELVIEW));
	OOGL_CALL(glLoadMatrixf(getMatrix(MODEL_VIEW_MATRIX)));

	OOGL_CALL(glEnable(GL_LINE_SMOOTH));
	OOGL_CALL(glEnable(GL_BLEND));
	OOGL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)); // for line smooth

	for (int i = 0; i < cameras_.size(); ++i) {
		if (i != currentCamera_) {
			OOGL_CALL(glPushMatrix());
			OOGL_CALL(glMultMatrixf(cameras_[i]->getWorldMatrix()));

			oogl::renderXYZAxes();
			oogl::renderFrustum(
				(const GLfloat*)&cameras_[i]->getWorldFrustum().getVertices()[0],
				(const GLfloat*)&cameras_[i]->getWorldFrustum().getVertices()[4],
				3 // components per frustum vertex
			);

			OOGL_CALL(glPopMatrix());
		}
	}

	OOGL_CALL(glDisable(GL_BLEND));
	OOGL_CALL(glDisable(GL_LINE_SMOOTH));

	return true;
}

} // namespace frm

