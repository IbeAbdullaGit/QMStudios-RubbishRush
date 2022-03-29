/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include "AppMblur.h"

/*----------------------------------------------------------------------------*/
int main(int argc, char **argv) {
	AppMblur app(
		"",
		frm::ivec2(1280, 720), // window size
		frm::ivec2(1280, 720), // render size
		false
	);
	app.run();
	return 0;
}

/*******************************************************************************
	AppMblur implementation:
*******************************************************************************/

//	PROTECTED:

//	MEMBERS:

frm::CStringT AppMblur::envMapList_[] = {
	"media/environments/uffizi.env.hdr",
	"media/environments/galileo.env.hdr",
	"media/environments/overcast.env.hdr",
	0 // sentinel
};

//	SERVICES:

/*----------------------------------------------------------------------------*/
bool AppMblur::initScene() {
	if (!App3d::initScene())
		return false;

//------------------------------------------------------------------------------
//	TEXTURES
//------------------------------------------------------------------------------
	texEnvMap_ = frm::TextureCube::loadFromFile(envMapList_[currentEnvMap_], frm::ITexture::TEX_MIPMAP);
	if (!texEnvMap_)
		return false;

//------------------------------------------------------------------------------
//	MATERIALS
//------------------------------------------------------------------------------
	frm::EnvmapMaterial *matEnvmap = frm::EnvmapMaterial::create(texEnvMap_);
	if (!matEnvmap)
		return false;

	frm::IMaterial *mat = 0;

//------------------------------------------------------------------------------
//	MODELS:
//------------------------------------------------------------------------------
	frm::Mesh* mesh = 0;
	frm::Model* model = 0;

	mesh = frm::Mesh::sphere(1.0f, 32);
	mesh->transform(sml::mat4f::rotation3(sml::vec3f::xAxis(), sml::radians(-90.0f)));
	if (!mesh)
		return false;
	model = frm::Model::create(mesh, "meshSphere");
	if (!model)
		return false;
	frm::Mesh::destroy(&mesh);

	mesh = frm::Mesh::loadFromFile("media/models/teapot.obj");
	if (!mesh)
		return false;
	//mesh->transformTexcoords(sml::mat3f::scale3(2.0f));
	model = frm::Model::create(mesh, "meshTeapot");
	if (!model)
		return false;
	frm::Mesh::destroy(&mesh);

//------------------------------------------------------------------------------
//	OBECTS:
//------------------------------------------------------------------------------
/*	ENVIRONMENT MAP */
	addObject(
		frm::Object::create(
			addNode(frm::Node::create("ENVMAP")),
			frm::modelSAQuad,
			matEnvmap,
			true // never clip
		)
	);

	frm::Node *node;

	const int NTEAPOTS = 11;
	const int HALF_NTEAPOTS = NTEAPOTS / 2;
	sml::LCG lcg(123456);
	for (int x = 0; x < NTEAPOTS; ++x) {
		for (int z = 0; z < NTEAPOTS; ++z) {
			mat = frm::GenericMaterialIbl::create(
				frm::Texture2d::loadFromFile("media/materials/cheq.diffuse.png", frm::ITexture::TEX_MIPMAP),		// diffusetex
				sml::vec3f((float)x / (float)NTEAPOTS, 0.5f, (float)z / (float)NTEAPOTS) * 1.5f,									// diffuse color

				frm::Texture2d::loadFromFile("media/materials/plastic1.normal.png", frm::ITexture::TEX_MIPMAP),		// normal tex
				frm::Texture2d::loadFromFile("media/materials/plastic1.detail.png", frm::ITexture::TEX_MIPMAP),		// detail tex
				sml::vec2f(2.5f),																												// detail scale

				frm::Texture2d::loadFromFile("media/materials/plastic1.specular.png", frm::ITexture::TEX_MIPMAP),	// specular tex
				1.0f,																																// specular level
				1.0f,																																// specular exp
				1.0f,																																// metallic level
				texEnvMap_,
				""
			);
			if (!mat)
				return false;

			sml::vec3f pos(
				(float)(x - HALF_NTEAPOTS) * 3.0f,
				lcg.get<float>(-10.0f, 10.0f),
				(float)(z - HALF_NTEAPOTS) * 3.0f
			);

			node = addNode(frm::Node::create());
			node->addModifier(frm::PositionTargetCtrl<sml::SMOOTHSTEP>::create(
				pos,
				pos + sml::vec3f(0.0f, lcg.get<float>(0.0f, 10.0f), 0.0f),
				lcg.get<float>(0.2f, 2.0f),
				frm::INodeModifier::reverse
			));
			node->addModifier(frm::SpinCtrl::create(
				sml::normalize(lcg.get<sml::vec3f>()),
				sml::radians(90.0f)
			));
			node->addModifier(
				frm::PositionRotationScaleCtrl::create(
					sml::vec3f(0.0f),
					sml::quatf::identity(),
					sml::vec3f(lcg.get<float>(1.0f, 3.0f))
				)
			);
			addObject(
				frm::Object::create(
					node,
					frm::Model::find("meshTeapot"),
					mat
				)
			);
		}
	}

	return true;
}

/*----------------------------------------------------------------------------*/
bool AppMblur::initGui() {
	if (!App3d::initGui())
		return false;

	frm::GuiListContainer *sliderList = frm::GuiListContainer::create();
	sliderList->setDirection(frm::IGuiComponent::VERTICAL);
	sliderList->setAlignment(frm::IGuiComponent::RIGHT, frm::IGuiComponent::TOP);
	sliderList->setSizeRef(frm::IGuiComponent::ABSOLUTE, frm::IGuiComponent::RELATIVE);
	sliderList->setPosition(sml::vec2i(20, 0));
	sliderList->setSize(sml::vec2i(260, 100));
	addGuiComponent(sliderList);

	addGuiComponent(
		frm::GuiCheckbox::create(
			&doMotionBlur_,
			frm::Texture2d::loadFromFile("media/gui/checkbox16off.png"),
			frm::Texture2d::loadFromFile("media/gui/checkbox16on.png"),
			frm::fontDefault,
			"Enable Motion Blur"
		),
		sliderList
	);
	addGuiComponent(
		frm::GuiCheckbox::create(
			&doFixedSampling_,
			frm::Texture2d::loadFromFile("media/gui/checkbox16off.png"),
			frm::Texture2d::loadFromFile("media/gui/checkbox16on.png"),
			frm::fontDefault,
			"Enable Fixed Sampling"
		),
		sliderList
	);

	addGuiComponent(
		frm::GuiSlider::create(
			&maxMotionBlurSamples_,
			frm::ivec2(4.0f, 128.0f),
			frm::Texture2d::loadFromFile("media/gui/slider16.png"),
			frm::Texture2d::loadFromFile("media/gui/sliderKnob16.png"),
			frm::fontDefault,
			"Max Samples %2.0f"
		),
		sliderList
	);

	addGuiComponent(
		frm::GuiSlider::create(
			&targetFps_,
			frm::ivec2(8.0f, 240.0f),
			frm::Texture2d::loadFromFile("media/gui/slider16.png"),
			frm::Texture2d::loadFromFile("media/gui/sliderKnob16.png"),
			frm::fontDefault,
			"Target FPS %2.0f"
		),
		sliderList
	);


	sliderList->setDirection(frm::IGuiComponent::VERTICAL); // KLUDGE

	return true;
}

/*----------------------------------------------------------------------------*/
bool AppMblur::initRender() {
	if (!App3d::initRender())
		return false;

//------------------------------------------------------------------------------
//	SHADERS
//------------------------------------------------------------------------------
	shaderPostProcess_ = frm::Shader::loadFromFiles("shaders/basic.vs.glsl", "shaders/postprocess.fs.glsl");
	if (!shaderPostProcess_)
		return false;
	shaderLuminance_ = frm::Shader::loadFromFiles("shaders/basic.vs.glsl", "shaders/lum.fs.glsl");
	if (!shaderLuminance_)
		return false;
	shaderAvgMinMax_ = frm::Shader::loadFromFiles("shaders/basic.vs.glsl", "shaders/avgminmax.fs.glsl");
	if (!shaderAvgMinMax_)
		return false;
	shaderLuminanceAdapt_ = frm::Shader::loadFromFiles("shaders/basic.vs.glsl", "shaders/adaptlum.fs.glsl");
	if (!shaderLuminanceAdapt_)
		return false;

//------------------------------------------------------------------------------
//	FRAMEBUFFERS
//------------------------------------------------------------------------------
/*	HDR RENDER BUFFER, VELOCITY & DEPTH BUFFERS */
	texHdr_ = frm::Texture2d::create(GL_RGBA16F, renderSize_.x(), renderSize_.y());
	if (!texHdr_)
		return false;
	texHdr_->setMagFilter(GL_NEAREST);
	texHdr_->setWrap(GL_CLAMP_TO_EDGE);
	texHdr_->setName("HDR");

	texVelocity_ = frm::Texture2d::create(GL_RG8, renderSize_.x(), renderSize_.y());
	//texVelocity_ = frm::Texture2d::create(GL_RG16F, renderSize_.x(), renderSize_.y());
	if (!texVelocity_)
		return false;
	texVelocity_->setMagFilter(GL_NEAREST);
	texVelocity_->setWrap(GL_CLAMP_TO_EDGE);
	texVelocity_->setName("VELOCITY BUFFER");

	texDepth_ = frm::Texture2d::create(GL_DEPTH_COMPONENT, renderSize_.x(), renderSize_.y());
	if (!texDepth_)
		return false;
	texDepth_->setWrap(GL_CLAMP_TO_EDGE);
	texDepth_->setName("DEPTH");

	fboHdr_ = frm::Framebuffer::create();
	if (!fboHdr_)
		return false;
	fboHdr_->attach(texHdr_, GL_COLOR_ATTACHMENT0);
	fboHdr_->attach(texVelocity_, GL_COLOR_ATTACHMENT1);
	fboHdr_->attach(texDepth_, GL_DEPTH_ATTACHMENT);

/*	AUTO EXPOSURE BUFFERS */
	texLuma_ = frm::Texture2d::create(GL_RGB16F, 256, 256);
	if (!texLuma_)
		return false;
	texLuma_->setMinFilter(GL_LINEAR_MIPMAP_NEAREST); // no filtering between mip levels
	texLuma_->setWrap(GL_CLAMP_TO_EDGE);
	texLuma_->generateMipmap(); // allocate mipmap
	fboLuma_ = frm::Framebuffer::create();
	if (!fboLuma_)
		return false;
	fboLuma_->attach(texLuma_, GL_COLOR_ATTACHMENT0);

	for (int i = 0; i < 2; ++i) {
		texAdaptLuma_[i] = frm::Texture2d::create(GL_R16F, 1, 1);
		if (!texAdaptLuma_[i])
			return false;
		texAdaptLuma_[i]->setMinFilter(GL_NEAREST); // don't want filtering between mip levels!
		texAdaptLuma_[i]->setMagFilter(GL_NEAREST); // don't want filtering between mip levels!
		texAdaptLuma_[i]->setWrap(GL_REPEAT);
		fboAdaptLuma_[i] = frm::Framebuffer::create();
		if (!fboAdaptLuma_[i])
			return false;
		fboAdaptLuma_[i]->attach(texAdaptLuma_[i], GL_COLOR_ATTACHMENT0);
	}

//	add render textures to visualize for overlay:
	//addRenderTexture(texHdr_);
	addRenderTexture(texVelocity_);
	//addRenderTexture(texDepth_);

//------------------------------------------------------------------------------
//	OPENGL STATE
//------------------------------------------------------------------------------
	OOGL_CALL(glDisable(GL_DEPTH_TEST));
	OOGL_CALL(glFrontFace(GL_CCW)); // default is GL_CCW
	OOGL_CALL(glCullFace(GL_BACK));
	OOGL_CALL(glEnable(GL_CULL_FACE));
	OOGL_CALL(glClearDepth(1.0f));
	OOGL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
	if (oogl::isSupported("GL_ARB_seamless_cube_map")) {
		OOGL_CALL(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));
	}

	return true;
}

/*----------------------------------------------------------------------------*/
bool AppMblur::handleEvent(const sf::Event &event) {
	switch (event.type) {
		case sf::Event::KeyPressed:
			switch (event.key.code) {
				case sf::Keyboard::B: // cycle environment maps:
					++currentEnvMap_;
					if (envMapList_[currentEnvMap_] == 0)
						currentEnvMap_ = 0;

					texEnvMap_->setFileName(envMapList_[currentEnvMap_]);
					texEnvMap_->reload();
					return true;

				case sf::Keyboard::Return: // override switch to fullscreen
					return true;
				default:
					break;


			};
			break;

		default:
			break;
	};

	return App3d::handleEvent(event);
}


/*----------------------------------------------------------------------------*/
void AppMblur::renderToMipmap(
	frm::Framebuffer *fbo,
	frm::Shader *shader,
	int bindLocation
) {
	assert(fbo);

	oogl::Texture2D *tex = (oogl::Texture2D*)fbo->getAttachment(GL_COLOR_ATTACHMENT0);
	assert(tex);

	sml::vec2i texSize(tex->getWidth(), tex->getHeight());
	int nMipLevels = (int)sml::log((float)texSize.y(), 2.0f);

	assert(shader);
	frm::modelSAQuad->enable(shader, this);
	fbo->bindDraw();
	glActiveTexture(GL_TEXTURE0 + bindLocation);
	tex->bind();

	for (int i = 1; i <= nMipLevels; ++i) {
		fbo->attach(tex, GL_COLOR_ATTACHMENT0, i);
		tex->setBaseLevel(i - 1);
		tex->setMaxLevel(i - 1);
		texSize /= 2;
		OOGL_CALL(glViewport(0, 0, texSize.x(), texSize.y()));
		frm::modelSAQuad->render();
	}

//	restore mip min/max level:
	fbo->attach(tex, GL_COLOR_ATTACHMENT0, 0); // attach the top layer
	tex->setBaseLevel(0);
	tex->setMaxLevel(nMipLevels);

	frm::modelSAQuad->disable();
	frm::Framebuffer::bindNone();
}

/*----------------------------------------------------------------------------*/
bool AppMblur::render(float dt) {
	if (!App3d::render(dt))
		return false;

	frm::Camera &camera = *getCurrentCamera();;
	frm::FirstPersonCtrl &fpCtrl = *fpCtrls_[currentFpCtrl_];
	int previousAdaptLuma = currentAdaptLuma_;
	currentAdaptLuma_ = (currentAdaptLuma_ + 1) % 2;

	frm::Profiler::start("FRAME");

	//---------------------------------------------------------------------------
	//	RENDER SCENE
	//---------------------------------------------------------------------------
		fboHdr_->bindDraw();
		oogl::viewport(fboHdr_);
		OOGL_CALL(glClearDepth(1.0f));
		OOGL_CALL(glClear(GL_DEPTH_BUFFER_BIT));

		renderObjects();

	frm::Profiler::start("POST PROCESS");

	//---------------------------------------------------------------------------
	//	PREP AUTO EXPOSURE
	//---------------------------------------------------------------------------
		//	render initial log luminance:
		fboLuma_->bindDraw();
		oogl::viewport(fboLuma_);
		shaderLuminance_->use();
		glActiveTexture(GL_TEXTURE0);
		texHdr_->bind();
		frm::modelSAQuad->enable(shaderLuminance_, this);
		frm::modelSAQuad->render();
		frm::modelSAQuad->disable();

		// progressive downsample into mip levels:
		shaderAvgMinMax_->use(); // renderToMipmap() assumes this call was made
		renderToMipmap(fboLuma_, shaderAvgMinMax_);

		//	adapt luminance value based on previous frame:
		fboAdaptLuma_[currentAdaptLuma_]->bind();
		oogl::viewport(fboAdaptLuma_[currentAdaptLuma_]);
		shaderLuminanceAdapt_->use();
		OOGL_CALL(glUniform1f(shaderLuminanceAdapt_->getUniformLocation("uAdaptionRate"), dt * adaptionRate_));
		glActiveTexture(GL_TEXTURE0);
		texLuma_->bind();
		glActiveTexture(GL_TEXTURE1);
		texAdaptLuma_[previousAdaptLuma]->bind();
		frm::modelSAQuad->enable(shaderLuminance_, this);
		frm::modelSAQuad->render();
		frm::modelSAQuad->disable();

	//---------------------------------------------------------------------------
	//	FINAL POST PROCESS (TONEMAP, MBLUR, ETC.):
	//---------------------------------------------------------------------------
		//	matrix inputs for camera mblur:
		static sml::mat4f prevViewProjMat = camera.getViewProjectionMatrix();
		sml::mat4f inverseViewProjMat = sml::inverse(camera.getViewProjectionMatrix());


		fboDefault_->bindDraw();
		oogl::viewport(fboDefault_);
		shaderPostProcess_->use();
		OOGL_CALL(glUniform1f(shaderPostProcess_->getUniformLocation("uExposure"), exposure_));
		OOGL_CALL(glUniform1f(shaderPostProcess_->getUniformLocation("uFrameTime"), dt));


		OOGL_CALL(glUniform1f(shaderPostProcess_->getUniformLocation("uMotionScale"), (1.0f / targetFps_) / dt));
		OOGL_CALL(glUniform1i(shaderPostProcess_->getUniformLocation("uMaxMotionBlurSamples"), (int)maxMotionBlurSamples_));
		OOGL_CALL(glUniform1i(shaderPostProcess_->getUniformLocation("uDoMotionBlur"), (int)doMotionBlur_));
		OOGL_CALL(glUniform1i(shaderPostProcess_->getUniformLocation("uDoFixedSampling"), (int)doFixedSampling_));

		OOGL_CALL(glActiveTexture(GL_TEXTURE0));
		texHdr_->bind();
		OOGL_CALL(glActiveTexture(GL_TEXTURE1));
		texDepth_->bind();
		OOGL_CALL(glActiveTexture(GL_TEXTURE2));
		texAdaptLuma_[currentAdaptLuma_]->bind();
		OOGL_CALL(glActiveTexture(GL_TEXTURE3));
		texVelocity_->bind();

		frm::modelSAQuad->enable(shaderPostProcess_, this);
		frm::modelSAQuad->render(GL_TRIANGLE_STRIP);
		frm::modelSAQuad->disable();

	frm::Profiler::stop("POST PROCESS");

	frm::Profiler::stop("FRAME");

	return true;
}

