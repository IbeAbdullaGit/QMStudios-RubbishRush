#include "BloomEffect3.h"
#include "Utils/ResourceManager/ResourceManager.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include "Application/Application.h"

BloomEffect3::BloomEffect3() :
	BloomEffect3(true) { }

BloomEffect3::BloomEffect3(bool defaultLut) :
	PostProcessingLayer::Effect(),
	_strength(1.0f), _shader2(nullptr), threshold(0.5f)
{
	Name = "Bloom Effect";
	_format = RenderTargetType::ColorRgb8;

	_shader2 = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
		{ ShaderPartType::Vertex, "shaders/vertex_shaders/fullscreen_quad.glsl" },
		{ ShaderPartType::Fragment, "shaders/fragment_shaders/post_effects/gaussian_blur.glsl" }
	});

	
}

BloomEffect3::~BloomEffect3() = default;

void BloomEffect3::Apply(const Framebuffer::Sptr& gBuffer)
{
	_shader2->Bind();
	_shader2->SetUniform("resolution", Application::Get().GetWindowSize().y);
	_shader2->SetUniform("direction", glm::vec2(0.0f, 1.0f));
}

void BloomEffect3::RenderImGui()
{
	//LABEL_LEFT(ImGui::LabelText, "LUT", Lut ? Lut->GetDebugName().c_str() : "none");
	//LABEL_LEFT(ImGui::SliderFloat, "Strength", &_strength, 0, 1);
	//LABEL_LEFT(ImGui::SliderFloat, "Threshold", &threshold, 0, 1);
}

BloomEffect3::Sptr BloomEffect3::FromJson(const nlohmann::json& data)
{
	BloomEffect3::Sptr result = std::make_shared<BloomEffect3>(false);
	result->Enabled = JsonGet(data, "enabled", true);
	result->_strength = JsonGet(data, "strength", result->_strength);
	result->threshold = JsonGet(data, "threshold", result->threshold);
	//result->Lut = ResourceManager::Get<Texture3D>(Guid(data["lut"].get<std::string>()));
	return result;
}

nlohmann::json BloomEffect3::ToJson() const
{
	return {
		{ "enabled", Enabled },
		//{ "lut", Lut != nullptr ? Lut->GetGUID().str() : "null" }, 
		{ "strength", _strength },
		{"threshold", threshold}
	};
}
