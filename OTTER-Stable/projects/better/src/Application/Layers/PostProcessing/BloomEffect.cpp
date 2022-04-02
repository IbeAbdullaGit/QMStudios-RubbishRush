#include "BloomEffect.h"
#include "Utils/ResourceManager/ResourceManager.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include "Application/Application.h"
#include "Application/Layers/RenderLayer.h"

BloomEffect::BloomEffect() :
	BloomEffect(true) { }

BloomEffect::BloomEffect(bool defaultLut) :
	PostProcessingLayer::Effect(),
	_shader(nullptr),
	_strength(1.0f), _shader2(nullptr), threshold(0.5f), current(nullptr), current2(nullptr)
{
	Name = "Bloom Effect";
	_format = RenderTargetType::ColorRgb8;

	_shader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
		{ ShaderPartType::Vertex, "shaders/vertex_shaders/fullscreen_quad.glsl" },
		{ ShaderPartType::Fragment, "shaders/fragment_shaders/post_effects/bloom_threshold_shader.glsl" }
	});
	_shader2 = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
		{ ShaderPartType::Vertex, "shaders/vertex_shaders/fullscreen_quad.glsl" },
		{ ShaderPartType::Fragment, "shaders/fragment_shaders/post_effects/gaussian_blur.glsl" }
	});

	//setup intermediary FBO
	Application& app = Application::Get();
	const glm::uvec4& viewport = app.GetPrimaryViewport();
	FramebufferDescriptor fboDescriptor;
	fboDescriptor.Width = app.GetWindowSize().x;
	fboDescriptor.Height = app.GetWindowSize().y;
	fboDescriptor.RenderTargets[RenderTargetAttachment::Depth] = RenderTargetDescriptor(RenderTargetType::Depth32);
	fboDescriptor.RenderTargets[RenderTargetAttachment::Color0] = RenderTargetDescriptor(RenderTargetType::ColorRgba8);
	//new framebuffer
	current = std::make_shared<Framebuffer>(fboDescriptor);
	current2 = std::make_shared<Framebuffer>(fboDescriptor);
}

BloomEffect::~BloomEffect() = default;

void BloomEffect::Apply(const Framebuffer::Sptr& gBuffer, VertexArrayObject::Sptr _quadVAO)
{
	Application& app = Application::Get();
	//gBuffer->Unbind();
	//gBuffer->Bind();
	// 
	// 
	//disable the output for now
	
	//save initial state
	auto& temp = _output;
	_output->Unbind();

	//change maybe?
	current2->Bind();
	glViewport(0, 0, current2->GetWidth(), current2->GetHeight());
	// Bind color 0 from previous pass to texture slot 0 so our effects can access
	// we already have the texture?
	//_output->BindAttachment(RenderTargetAttachment::Color0, 0);

	//apply1
	{
		_shader->Bind();
		_shader->SetUniform("strength", _strength);
		_shader->SetUniform("threshold", threshold);
		//_quadVAO->Draw();
		
		//carry over
		//_output = current;
		//_output->Unbind();
	}
	
	
	current2->Unbind();
	current->Bind();
	glViewport(0, 0, current->GetWidth(), current->GetHeight());
	// Bind color 0 from previous pass to texture slot 0 so our effects can access
	current2->BindAttachment(RenderTargetAttachment::Color0, 0);

	//apply 2
	{
		_shader2->Bind();
		_shader2->SetUniform("resolution", app.GetWindowSize().x);
		_shader2->SetUniform("direction", glm::vec2(1.0f, 0.0f));
		//_quadVAO->Draw();
		_shader2->SetUniform("resolution", app.GetWindowSize().y);
		_shader2->SetUniform("direction", glm::vec2(0.0f, 1.0f));
	}

	current->Unbind();
	
	//_output = temp;
	_output->Bind();
	glViewport(0, 0, _output->GetWidth(),_output->GetHeight());
	current->BindAttachment(RenderTargetAttachment::Color0, 0);
	//temp->Bind()?
	

	//draw??
	//gBuffer->Bind();
	//_shader2->SetUniform("resolution", app.GetWindowSize().y);
	//_shader2->SetUniform("direction", glm::vec2(0.0f, 1.0f));

	
}

void BloomEffect::RenderImGui()
{
	//LABEL_LEFT(ImGui::LabelText, "LUT", Lut ? Lut->GetDebugName().c_str() : "none");
	LABEL_LEFT(ImGui::SliderFloat, "Strength", &_strength, 0, 10);
	LABEL_LEFT(ImGui::SliderFloat, "Threshold", &threshold, 0, 1);
}

BloomEffect::Sptr BloomEffect::FromJson(const nlohmann::json& data)
{
	BloomEffect::Sptr result = std::make_shared<BloomEffect>(false);
	result->Enabled = JsonGet(data, "enabled", true);
	result->_strength = JsonGet(data, "strength", result->_strength);
	result->threshold = JsonGet(data, "threshold", result->threshold);
	//result->Lut = ResourceManager::Get<Texture3D>(Guid(data["lut"].get<std::string>()));
	return result;
}

nlohmann::json BloomEffect::ToJson() const
{
	return {
		{ "enabled", Enabled },
		//{ "lut", Lut != nullptr ? Lut->GetGUID().str() : "null" }, 
		{ "strength", _strength },
		{"threshold", threshold}
	};
}
