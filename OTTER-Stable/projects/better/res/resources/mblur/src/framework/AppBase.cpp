/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include "AppBase.h"
#include "common.h"
#include "Camera.h"
#include "Font.h"
#include "GuiProfilePlotter.h"
#include "ITexture.h"
	#include "Texture2d.h"
#include "Model.h"
#include "Profiler.h"
#include "Shader.h"
#include "ShaderUnit.h"

namespace frm {

/*******************************************************************************
	AppBase implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
AppBase::AppBase(
	const CStringT &appName,
	const ivec2 &windowSize,
	const ivec2 &renderSize,
	bool fullscreen
):	initSuccess_(false),
	vsyncEnabled_(true), showOverlay_(true), showHelpers_(false), showRenderTextures_(false),
	fboDefault_(0),
	window_(0),
	guiRoot_(0),
	renderSize_(renderSize),
	sceneGraph_(0),
	currentCamera_(0), currentClipCamera_(0),
	pCurrentCamera_(0), pCurrentClipCamera_(0) {

//	init cout/clog/cerr:
	outbuf_.addbuf(std::cout.rdbuf());
	std::cout.rdbuf(&outbuf_);
	outfile_.open("out.txt");
	outbuf_.addbuf(outfile_.rdbuf());

	logbuf_.addbuf(std::clog.rdbuf());
	std::clog.rdbuf(&logbuf_);
	logfile_.open("log.txt");
	logbuf_.addbuf(logfile_.rdbuf());

	errbuf_.addbuf(std::cerr.rdbuf());
	std::cerr.rdbuf(&errbuf_);
	errfile_.open("err.txt");
	errbuf_.addbuf(errfile_.rdbuf());

//	push matrix identities:
	modelMatrixStack_.push_back(mat4::identity());
	viewMatrixStack_.push_back(mat4::identity());
	projectionMatrixStack_.push_back(mat4::identity());
	modelViewMatrix_ = mat4::identity();
	viewProjectionMatrix_ = mat4::identity();
	modelViewProjectionMatrix_ = mat4::identity();


	initSuccess_ = initMain(appName, windowSize, fullscreen);

//	can't initialize anything else; can't call the virtual methods from the
//	ctor!
}

/*----------------------------------------------------------------------------*/
AppBase::~AppBase() {
oogl::logEvent("Cleaning up...");
	cleanup(); // framework global cleanup

	if (window_)
		delete window_;

oogl::logEvent("Cleanup complete.");

//	restore cout/clog/cerr:
	std::cout.rdbuf(outbuf_.getbuf(0));
	outfile_.close();
	std::clog.rdbuf(logbuf_.getbuf(0));
	logfile_.close();
	std::cerr.rdbuf(errbuf_.getbuf(0));
	errfile_.close();
}

//	ACCESSOR:
/*----------------------------------------------------------------------------*/
Camera* AppBase::getCamera(const CStringT &search) {
	if (!search)
		return 0;

	for (size_t i = 0; i < cameras_.size(); ++i) {
		int j = 0;
		while (cameras_[i]->getName()[j] != 0) {
			if (search[j] == 0)
				break;
			if (search[j] != cameras_[i]->getName()[j])
				break;
			++j;
		}
		if (search[j] == cameras_[i]->getName()[j])
			return cameras_[i];
	}

	return 0; // not found
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
void AppBase::run() {

oogl::logEvent("Initializing...");
	initSuccess_ &= init(); // framework global init
	if (initSuccess_)
		initSuccess_ &= initRender();
	if (initSuccess_)
		initSuccess_ &= initScene();
	if (initSuccess_)
		initSuccess_ &= initGui();

	if (!initSuccess_)
		return;
oogl::logEvent("Initialization complete.");


	bool quitLoop = false;
	while (!quitLoop) {
		Profiler::reset();

		timer_.update();
		float dt = timer_.seconds();

		sf::Event event;
		while (window_->pollEvent(event) && !quitLoop) {
		//	handleEvent() returns false to quit the event loop
			quitLoop = !handleEvent(event);
		}

		if (!update(dt)) {
			oogl::logEvent("update() failed");
			quitLoop = true;
		}

		Model::resetTotalTriangles(); // reset triangle counter
		if (!render(dt)) {
			oogl::logEvent("render() failed");
			quitLoop = true;
		}

		if (showHelpers_) {
			if (!renderHelpers(dt))  {
				oogl::logEvent("renderHelpers()");
				quitLoop = true;
			}
		}

		if (showOverlay_) {
			if (!renderOverlay(dt))  {
				oogl::logEvent("renderOverlay() failed");
				quitLoop = true;
			}
		}

		window_->display();
	}
}

/*----------------------------------------------------------------------------*/
void AppBase::toggleFullscreen() {
	if (windowStyle_ == sf::Style::Fullscreen) {
	//	switch window mode:
		windowStyle_ = sf::Style::Default;
	} else {
	//	switch to fullscreen mode:
		if (!videoMode_.isValid()) {
			oogl::logEvent("Failed to switch to fullscreen mode.");
			return;
		}
		windowStyle_ = sf::Style::Fullscreen;
	}

	window_->create(
		videoMode_,
		"",
		windowStyle_,
		contextSettings_
	);
//	window_ = new(std::nothrow) sf::Window(videoMode, appName, windowStyle, contextSettings_);
}

//	PROTECTED:

//	SERVICES:

/*----------------------------------------------------------------------------*/
bool AppBase::initGui() {
//	create root element:
	guiRoot_ = IGuiComponent::create("root");
	if (!guiRoot_)
		return false;
	guiRoot_->setSizeRef(IGuiComponent::ABSOLUTE, IGuiComponent::ABSOLUTE);
	guiRoot_->setSize(getWindowSize());

//	load default textures:
	Texture2d *tex;

	tex = Texture2d::loadFromFile("media/gui/slider16.png");
	tex->setMinFilter(GL_LINEAR);
	tex->setMagFilter(GL_LINEAR);

	tex = Texture2d::loadFromFile("media/gui/sliderknob16.png");
	tex->setMinFilter(GL_LINEAR);
	tex->setMagFilter(GL_LINEAR);

	tex = Texture2d::loadFromFile("media/gui/checkbox16off.png");
	tex->setMinFilter(GL_LINEAR);
	tex->setMagFilter(GL_LINEAR);

	tex = Texture2d::loadFromFile("media/gui/checkbox16on.png");
	tex->setMinFilter(GL_LINEAR);
	tex->setMagFilter(GL_LINEAR);


	Profiler::setBufferSize(100);
	GuiProfilePlotter *plotter = GuiProfilePlotter::create();
	plotter->setPosition(ivec2(5, 5));
	plotter->setColor(vec4(0.2f, 1.0f, 0.3f, 0.5f));
	guiRoot_->addChild(plotter);

	return true;
}

/*----------------------------------------------------------------------------*/
bool AppBase::initScene() {
	sceneGraph_ = Node::create("root");
	return true;
}

/*----------------------------------------------------------------------------*/
bool AppBase::initRender() {
	fboDefault_ = Framebuffer::create(window_->getSize().x, window_->getSize().y, "default");
	if (!fboDefault_)
		return false;
	return true;
}

/*----------------------------------------------------------------------------*/
bool AppBase::handleEvent(const sf::Event &event) {
	//	only gets updated when a MouseMove event is processed; this will always be
	//	a valid client area mouse position
	ivec2 prevMousePos;

	switch (event.type) {
		case sf::Event::Closed: // window close, quit
			return false; // quit event loop
			break;

		case::sf::Event::Resized:
			setWindowSize(ivec2(event.size.width, event.size.height));
			break;

		case sf::Event::KeyPressed:
			switch (event.key.code) {
				case sf::Keyboard::Escape: // esc pressed, quit
					return false; // quit event loop
					break;

				case sf::Keyboard::Return:
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
						toggleFullscreen();
					break;

				case sf::Keyboard::F1: // toggle overlay
					showOverlay_ = !showOverlay_;
					if (!showOverlay_)
						showHelpers_ = false;
					break;
				case sf::Keyboard::F2: // toggle helpers
					showHelpers_ = !showHelpers_;
					if (showHelpers_)
						showOverlay_ = true;
					break;
				case sf::Keyboard::F3: // toggle render textures
					showRenderTextures_ = !showRenderTextures_ ;
					if (showRenderTextures_)
						showOverlay_ = true;
					break;

				case sf::Keyboard::F4: // toggle vsync
					vsyncEnabled_ = !vsyncEnabled_;
					window_->setVerticalSyncEnabled(vsyncEnabled_);
					break;

				case sf::Keyboard::F8: // reload textures
					oogl::logEvent("\n***Reloading textures...");
					ITexture::reloadAll();
					break;
				case sf::Keyboard::F9: // reload shaders
					oogl::logEvent("\n***Reloading/recompiling shaders...");
					Shader::reloadAll();
					break;

				case sf::Keyboard::C:
					++currentClipCamera_;
					if (currentClipCamera_ >= cameras_.size())
						currentClipCamera_ = 0;
					if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
						currentCamera_ = currentClipCamera_;
					break;


				default:
					break;
			};
			break;

		case sf::Event::MouseButtonPressed:
			if (showOverlay_) {
				ivec2 mpos(event.mouseButton.x, event.mouseButton.y);
				IGuiComponent::MouseButtonT mbut;
				if (event.mouseButton.button == sf::Mouse::Left)
					mbut = IGuiComponent::LBUTTON;
				else if (event.mouseButton.button == sf::Mouse::Middle)
					mbut = IGuiComponent::MBUTTON;
				else if (event.mouseButton.button == sf::Mouse::Right)
					mbut = IGuiComponent::RBUTTON;
				else
					break;// only left/middle/right supported
				guiRoot_->mouseDown(mpos, mbut);
			}
			break;

		case sf::Event::MouseButtonReleased:
			if (showOverlay_) {
				ivec2 mpos(event.mouseButton.x, event.mouseButton.y);
				IGuiComponent::MouseButtonT mbut;
				if (event.mouseButton.button == sf::Mouse::Left)
					mbut = IGuiComponent::LBUTTON;
				else if (event.mouseButton.button == sf::Mouse::Middle)
					mbut = IGuiComponent::MBUTTON;
				else if (event.mouseButton.button == sf::Mouse::Right)
					mbut = IGuiComponent::RBUTTON;
				else
					break; // only left/middle/right supported
				guiRoot_->mouseUp(mpos, mbut);
			}
			break;

		case sf::Event::LostFocus:
		case sf::Event::MouseLeft:
		//	fake mouseUp events for any buttons that are pressed; this
		//	doesn't seem to work 100% of the time
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				guiRoot_->mouseUp(prevMousePos, IGuiComponent::LBUTTON);
			if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
				guiRoot_->mouseUp(prevMousePos, IGuiComponent::MBUTTON);
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
				guiRoot_->mouseUp(prevMousePos, IGuiComponent::RBUTTON);
			break;

		case sf::Event::MouseMoved:
			if (showOverlay_) {
				prevMousePos = ivec2(event.mouseMove.x, event.mouseMove.y);
				guiRoot_->mouseMove(prevMousePos);
			}
			break;

		case sf::Event::MouseWheelMoved:
			break;

		default:
			break;
	};

	return true;
}

/*----------------------------------------------------------------------------*/
bool AppBase::update(float dt) {
	sceneGraph_->update(dt);

//	set common matrices, etc:
	return true;
}

/*----------------------------------------------------------------------------*/
bool AppBase::render(float dt) {
	//return renderObjects(); // force manual call, doing this here is troublesome!
	return true;
}

/*----------------------------------------------------------------------------*/
bool AppBase::renderOverlay(float dt) {
	loadMatrix(PROJECTION_MATRIX, mat4::identity());
	loadMatrix(VIEW_MATRIX, mat4::identity());

//	set gl state:
	Shader::useNone();
	fboDefault_->bindDraw();
	oogl::viewport(fboDefault_);
	OOGL_CALL(glDisable(GL_DEPTH_TEST));
	OOGL_CALL(glDisable(GL_CULL_FACE));

	guiRoot_->render(this);

//	render scene info:
	OOGL_CALL(glEnable(GL_BLEND));
	OOGL_CALL(glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA));

	fontDefault->setColor(1.0f, 1.0f, 1.0f, 1.0f);
	fontDefault->enable(this);
	int fps = (int)(1.0f / dt);
	fontDefault->renderString(5, 5, "%ix%i %if/s VSYNC=%u", renderSize_.x(), renderSize_.y(), fps, (unsigned)vsyncEnabled_);

	fontDefault->setColor(1.0f, 0.4f, 0.4f, 1.0f);
	fontDefault->enable(this);
	fontDefault->renderString(5, 5, "\n%u tri/f", Model::getTotalTriangles());
	fontDefault->setColor(1.0f, 1.0f, 1.0f, 1.0f);

	fontDefault->disable();
	OOGL_CALL(glDisable(GL_BLEND));

//	visualize render textures:
	if (showRenderTextures_) {
		const int height = 128;
		ivec2 cpos(5, window_->getSize().y - height - 5);
		for (int i = 0; i < renderTextures_.size(); ++i) {
			Texture2d &tex = *renderTextures_[i].first;
			Shader &shader = *renderTextures_[i].second;

			int width = height * tex.getWidth() / tex.getHeight();

		//	if it won't fit, go to next line:
			if (cpos.x() > window_->getSize().x - width) {
				cpos.x(5);
				cpos.y(cpos.y() - height - 5);
			}

			OOGL_CALL(glViewport(cpos.x(), cpos.y(), width, height));
			shader.use();
			OOGL_CALL(glActiveTexture(GL_TEXTURE0));
			tex.bind();
			modelSAQuad->enable(&shader, this);
			modelSAQuad->render(GL_TRIANGLE_STRIP);
			modelSAQuad->disable();

			cpos.x(cpos.x() + width + 5);
		}

	//	render text separately:
		oogl::viewport(fboDefault_);
		cpos = ivec2(5, window_->getSize().y - height - 5);
		OOGL_CALL(glEnable(GL_BLEND));
		OOGL_CALL(glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA));
		for (int i = 0; i < renderTextures_.size(); ++i) {
			Texture2d &tex = *renderTextures_[i].first;
			Shader &shader = *renderTextures_[i].second;

			int width = height * tex.getWidth() / tex.getHeight();

		//	if it won't fit, go to next line:
			if (cpos.x() > window_->getSize().x - width) {
				cpos.x(5);
				cpos.y(cpos.y() - height - 5);
			}

			fontDefault->setColor(1.0f, 1.0f, 1.0f, 1.0f);
			fontDefault->enable(this);
			fontDefault->renderString(cpos.x(), fboDefault_->getHeight() - cpos.y() - height, tex.getName().c_str());
			fontDefault->disable();

			cpos.x(cpos.x() + width + 5);
		}

		OOGL_CALL(glDisable(GL_BLEND));
	}

	Shader::useNone();
	Framebuffer::bindNone();

	return true;
}

/*----------------------------------------------------------------------------*/
bool AppBase::renderHelpers(float dt) {
	loadMatrix(PROJECTION_MATRIX, getCurrentCamera()->getProjectionMatrix());
	loadMatrix(VIEW_MATRIX, getCurrentCamera()->getViewMatrix());

	Shader::useNone();
	fboDefault_->bindDraw();
	oogl::viewport(fboDefault_);

	OOGL_CALL(glMatrixMode(GL_PROJECTION));
	OOGL_CALL(glLoadMatrixf(getMatrix(PROJECTION_MATRIX)));
	OOGL_CALL(glMatrixMode(GL_MODELVIEW));
	OOGL_CALL(glLoadMatrixf(getMatrix(MODEL_VIEW_MATRIX)));

	OOGL_CALL(glEnable(GL_BLEND));
	OOGL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)); // for line smooth

	oogl::renderXZGrid(10.0f, 10.0f);

//	sceneGraph_->renderAll();

	for (int i = 0; i < objects_.size(); ++i) {
		if (getCurrentCamera()->getWorldFrustum().isInside(objects_[i]->getPosition(), objects_[i]->getRadius()))
			objects_[i]->renderHelper(this);
	}

	OOGL_CALL(glDisable(GL_BLEND));

	return true;
}

/*----------------------------------------------------------------------------*/
Node* AppBase::addNode(Node *node) {
	assert(sceneGraph_);
	assert(node);
	if (!node->getParent())
		sceneGraph_->addChild(node);
	return node;
}

/*----------------------------------------------------------------------------*/
Object* AppBase::addObject(Object *object) {
	assert(object);
	objects_.push_back(object);
	return object;
}

/*----------------------------------------------------------------------------*/
Camera* AppBase::addCamera(Camera *camera, Node *parent) {
	if (parent == 0)
		parent = addNode(Node::create());
	parent->addModifier(camera);
	cameras_.push_back(camera);
	return camera;
}

/*----------------------------------------------------------------------------*/
IGuiComponent* AppBase::addGuiComponent(IGuiComponent *child, IGuiComponent *parent) {
	assert(child);

//	use gui root by default:
	if (!parent)
		parent = guiRoot_;

	parent->addChild(child);

	return child;
}

/*----------------------------------------------------------------------------*/
void AppBase::addRenderTexture(Texture2d *tex, Shader *shader) {
	assert(tex);
	if (shader == 0) {
		switch (tex->getFormat()) {
			case GL_DEPTH_COMPONENT:
			case GL_DEPTH_STENCIL:
			default:
				shader = shaderDefaultVis_ ;
				break;
		};
	}

	renderTextures_.push_back(std::pair<Texture2d*, Shader*>(tex, shader));
}

/*----------------------------------------------------------------------------*/
bool AppBase::renderObjects() {
	loadMatrix(PROJECTION_MATRIX, getCurrentCamera()->getProjectionMatrix());
	loadMatrix(VIEW_MATRIX, getCurrentCamera()->getViewMatrix());
	for (int i = 0; i < objects_.size(); ++i) {
		//if (objects_[i]->clip(getCurrentCamera()->getWorldFrustum()))
			objects_[i]->render(this);
	}
	return true;
}

/*----------------------------------------------------------------------------*/
bool AppBase::renderObjectShadows(Camera *pCamera) {
	assert(pCamera);

	//	store current camera/clip camera:
	Camera *pOldCamera = pCurrentCamera_;
	Camera *pOldClipCamera = pCurrentClipCamera_;

	// set current camera/clip camera:
	pCurrentCamera_ = pCurrentClipCamera_ = pCamera;

	loadMatrix(PROJECTION_MATRIX, getCurrentCamera()->getProjectionMatrix());
	loadMatrix(VIEW_MATRIX, getCurrentCamera()->getViewMatrix());
	for (int i = 0; i < objects_.size(); ++i) {
		objects_[i]->renderShadow(this);
	}

	//	restore current camera/clip camera:
	pCurrentCamera_ = pOldCamera;
	pCurrentClipCamera_ = pOldClipCamera;

	return true;
}

//	PRIVATE:

//	MEMBERS:

Shader *AppBase::shaderDefaultVis_  = 0;

// SERVICES:

/*----------------------------------------------------------------------------*/
bool AppBase::initMain(
	const CStringT &appName,
	const ivec2 &windowSize,
	bool fullscreen
) {
//	init window/GL context:
	videoMode_ = sf::VideoMode(windowSize.x(), windowSize.y(), 32);
	windowStyle_ = sf::Style::Default;

	if (fullscreen) {
		if (videoMode_.isValid())
			windowStyle_ = sf::Style::Fullscreen;
		else
			oogl::logEvent("Failed to enter fullscreen mode");
	}

	contextSettings_ = sf::ContextSettings(24, 0, 0, 3, 3);
	window_ = new(std::nothrow) sf::Window(videoMode_, appName, windowStyle_, contextSettings_);
	assert(window_);

	window_->setVerticalSyncEnabled(true);
	window_->setActive();
	contextSettings_ = window_->getSettings(); // get acquired settings

//	init external libs:
	if (!oogl::init()) // requires OpenGL context
		return false;
	if (!oogl::checkGLVersion(3, 3)) // min gl version
		return false;
	if (!oogl::validate())
		return false;


//	init default shaders:
	if (shaderDefaultVis_  == 0) {
		const char *vsSrc =
			"#version 420 \n \
			layout(location=0) in vec3 aPosition; \
			noperspective out vec2 vTexcoords; \
			void main() { \
				vTexcoords = aPosition.xy * 0.5 + 0.5; \
				gl_Position = vec4(aPosition.xy, 0.0, 1.0); \
			}\n";
		const char *fsSrc =
			"#version 420 \n \
			layout(binding=0) uniform sampler2D uInputTex; \
			noperspective in vec2 vTexcoords; \
			layout(location=0) out vec4 fResult; \
			void main() { \
				fResult = texture(uInputTex, vTexcoords); \
			}\n";
		shaderDefaultVis_ = Shader::create(
			2,
			ShaderUnit::create(GL_VERTEX_SHADER, vsSrc),
			ShaderUnit::create(GL_FRAGMENT_SHADER, fsSrc)
		);
		assert(shaderDefaultVis_ );
	}

	return true;
}

} // namespace frm
