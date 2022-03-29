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
	_strength(1.0f), _shader2(nullptr), threshold(0.5f)
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

	
}

BloomEffect::~BloomEffect() = default;

void BloomEffect::Apply(const Framebuffer::Sptr& gBuffer)
{
	_shader->Bind();
	_shader->SetUniform("strength", _strength);
	_shader->SetUniform("threshold", threshold);
	//gBuffer->Bind();
	Application& app = Application::Get();
	
	//_shader2->Bind();
	//_shader2->SetUniform("resolution", app.GetWindowSize().x);
	//_shader2->SetUniform("direction", glm::vec2(1.0f, 0.0f));
	//draw??
	//gBuffer->Bind();
	//_shader2->SetUniform("resolution", app.GetWindowSize().y);
	//_shader2->SetUniform("direction", glm::vec2(0.0f, 1.0f));
}

void BloomEffect::RenderImGui()
{
	//LABEL_LEFT(ImGui::LabelText, "LUT", Lut ? Lut->GetDebugName().c_str() : "none");
	LABEL_LEFT(ImGui::SliderFloat, "Strength", &_strength, 0, 1);
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
