#include "Application/Layers/PostProcessingLayer.h"

#include "Application/Application.h"
#include "RenderLayer.h"

#include "PostProcessing/ColorCorrectionEffect.h"
#include "PostProcessing/BoxFilter3x3.h"
#include "PostProcessing/BoxFilter5x5.h"
#include "PostProcessing/OutlineEffect.h"
#include "PostProcessing/ToonEffect.h"
#include "PostProcessing/BloomEffect.h"
#include "PostProcessing/MotionblurEffect.h"
#include "PostProcessing/FilmGrain.h"
#include "PostProcessing/SlimeVignette.h"
#include "PostProcessing/Pixelate.h"
#include "PostProcessing/RimLightEffect.h"
#include "PostProcessing/NightVision.h"
#include "PostProcessing/DepthOfField.h"


PostProcessingLayer::PostProcessingLayer() :
	ApplicationLayer()
{
	Name = "Post Processing";
	Overrides =
		AppLayerFunctions::OnAppLoad |
		AppLayerFunctions::OnSceneLoad | AppLayerFunctions::OnSceneUnload | 
		AppLayerFunctions::OnPostRender |
		AppLayerFunctions::OnWindowResize | AppLayerFunctions::OnUpdate;
}

PostProcessingLayer::~PostProcessingLayer() = default;

void PostProcessingLayer::AddEffect(const Effect::Sptr& effect) {
	_effects.push_back(effect);
}

void PostProcessingLayer::OnAppLoad(const nlohmann::json& config)
{
	// Loads some effects in
	_effects.push_back(std::make_shared<ColorCorrectionEffect>());
	//_effects.push_back(std::make_shared<BoxFilter3x3>());
	//_effects.push_back(std::make_shared<BoxFilter5x5>());
	////_effects.push_back(std::make_shared<OutlineEffect>());
	_effects.push_back(std::make_shared<ToonEffect>());
	//_effects.push_back(std::make_shared<MotionblurEffect>());
	//_effects.push_back(std::make_shared<BloomEffect>());
	//_effects.push_back(std::make_shared<FilmGrain>());
	
	_effects.push_back(std::make_shared<SlimeVignette>());
	//_effects.push_back(std::make_shared<NightVision>());
	//pl
	//_effects.push_back(std::make_shared<DepthOfField>());
	_effects.push_back(std::make_shared<Pixelate>());
	_effects[2]->Enabled = false;

	Application& app = Application::Get();
	const glm::uvec4& viewport = app.GetPrimaryViewport();

	// Initialize all the effect's output FBOs (inefficient) 
	for (const auto& effect : _effects) {
		FramebufferDescriptor fboDesc = FramebufferDescriptor();
		fboDesc.Width  = viewport.z * effect->_outputScale.x;
		fboDesc.Height = viewport.w * effect->_outputScale.y;
		fboDesc.RenderTargets[RenderTargetAttachment::Color0] = RenderTargetDescriptor(effect->_format);

		effect->_output = std::make_shared<Framebuffer>(fboDesc);
	}

	// We need a mesh for drawing fullscreen quads
	glm::vec2 positions[6] = {
		{ -1.0f,  1.0f }, { -1.0f, -1.0f }, { 1.0f, 1.0f },
		{ -1.0f, -1.0f }, {  1.0f, -1.0f }, { 1.0f, 1.0f }
	};

	VertexBuffer::Sptr vbo = std::make_shared<VertexBuffer>();
	vbo->LoadData(positions, 6);

	_quadVAO = VertexArrayObject::Create();
	_quadVAO->AddVertexBuffer(vbo, {
		BufferAttribute(0, 2, AttributeType::Float, sizeof(glm::vec2), 0, AttribUsage::Position)
	});

	//load in luts
	cool_lut = ResourceManager::CreateAsset<Texture3D>("luts/RRCoolLUT.CUBE");
	warm_lut = ResourceManager::CreateAsset<Texture3D>("luts/RRWarmLUT.CUBE");
	other_lut = ResourceManager::CreateAsset<Texture3D>("luts/RRDarkenedLUT.CUBE");
}

void PostProcessingLayer::SetSlime(bool state)
{
	//slime effect
	_effects[2]->Enabled = state;
}

void PostProcessingLayer::OnPostRender()
{
	Application& app = Application::Get();
	const glm::uvec4& viewport = app.GetPrimaryViewport();

	// Grab the render layer from the app, get it's output and the G-Buffer
	const RenderLayer::Sptr& renderer = app.GetLayer<RenderLayer>();
	const Framebuffer::Sptr& output = renderer->GetRenderOutput();
	const Framebuffer::Sptr& gBuffer = renderer->GetGBuffer();

	// Stores the input FBO to the effect, we start with the renderlayer's output 
	Framebuffer::Sptr current = output;

	// Disable depth testing and depth writing, as well as blending
	glDisable(GL_DEPTH_TEST);
	glDepthMask(false);
	glDisable(GL_BLEND);

	// Bind the quad VAO so our effects can use it
	_quadVAO->Bind();

	// Iterate over all the effects in the queue
	for (const auto& effect : _effects) {
		// Only render if it's enabled
		if (effect->Enabled) {
			// Bind the FBO and make sure we're rendering to the whole thing
			effect->_output->Bind();
			glViewport(0, 0, effect->_output->GetWidth(), effect->_output->GetHeight());

			// Bind color 0 from previous pass to texture slot 0 so our effects can access
			current->BindAttachment(RenderTargetAttachment::Color0, 0);

			// Apply the effect and render the fullscreen quad
			effect->Apply(gBuffer);
			_quadVAO->Draw();

			// Unbind output and set it as input for next pass
			effect->_output->Unbind();
			current = effect->_output;
		}
	}
	_quadVAO->Unbind();

	// Restore viewport to game viewport
	glViewport(viewport.x, viewport.y, viewport.z, viewport.w);

	// Bind the output of our post processing as the source for the blit
	current->Bind(FramebufferBinding::Read);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// Blit the color buffer to our game window
	current->Blit(
		{ 0, 0, current->GetWidth(), current->GetHeight() },
		{ viewport.x, viewport.y, viewport.x + viewport.z, viewport.y + viewport.w },
		BufferFlags::Color,
		MagFilter::Linear
	);

	gBuffer->Bind(FramebufferBinding::Read);
	current->Blit(
		{ 0, 0, gBuffer->GetWidth(), gBuffer->GetHeight() },
		{ viewport.x, viewport.y, viewport.x + viewport.z, viewport.y + viewport.w },
		BufferFlags::Depth,
		MagFilter::Nearest
	);

	gBuffer->Unbind();
}

void PostProcessingLayer::OnSceneLoad()
{
	for (const auto& effect : _effects) {
		effect->OnSceneLoad();
	}
}

void PostProcessingLayer::OnSceneUnload()
{
	for (const auto& effect : _effects) {
		effect->OnSceneUnload();
	}
}

void PostProcessingLayer::OnWindowResize(const glm::ivec2& oldSize, const glm::ivec2& newSize)
{
	for (const auto& effect : _effects) {
		effect->OnWindowResize(oldSize, newSize);
		effect->_output->Resize(newSize.x, newSize.y);
	}
}

const std::vector<PostProcessingLayer::Effect::Sptr>& PostProcessingLayer::GetEffects() const
{
	return _effects;
}

void PostProcessingLayer::OnUpdate()
{
	if (InputEngine::GetKeyState(GLFW_KEY_9) == ButtonState::Pressed)//cool
	{
		lut1 = !lut1;
	}
	if (InputEngine::GetKeyState(GLFW_KEY_8) == ButtonState::Pressed)//warm
	{
		lut2 = !lut2;
	}
	if (InputEngine::GetKeyState(GLFW_KEY_0) == ButtonState::Pressed)//custom
	{
		lut3 = !lut3;
	}
	if (InputEngine::GetKeyState(GLFW_KEY_P) == ButtonState::Pressed)//custom
	{
		pixel = !pixel;
	}
	//if (InputEngine::GetKeyState(GLFW_KEY_5) == ButtonState::Pressed) //custom effect
	//{
	//	enable_slime = !enable_slime;
	//}
	//
	//_effects[4]->Enabled = enable_slime;

	_effects[3]->Enabled = pixel;
	
	if (lut1)
	{
		_effects[0]->ChangeLut(cool_lut);
	}
	if (lut2)
	{
		_effects[0]->ChangeLut(warm_lut);
	}
	if (lut3)
	{
		_effects[0]->ChangeLut(other_lut);
	}
	if (!lut1 && !lut2 && !lut3) //none enabled
	{
		_effects[0]->Enabled = false; //turn off color correction
	}
	else //one of them is on
	{
		_effects[0]->Enabled = true;
	}

}

void PostProcessingLayer::Effect::DrawFullscreen()
{
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
