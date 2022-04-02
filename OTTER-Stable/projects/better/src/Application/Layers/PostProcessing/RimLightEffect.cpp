#include "RimLightEffect.h"
#include "Utils/ResourceManager/ResourceManager.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include "Application/Application.h"

RimLightEffect::RimLightEffect() :
	PostProcessingLayer::Effect(),
	_shader(nullptr),
	_rimColor(glm::vec4(0, 0, 0, 1)),
	strength(1.0f)/*, _scale(1.0f),
	_depthThreshold(0.1f),
	_normalThreshold(0.4f),
	_depthNormalThreshold(0.4f),
	_depthNormalThresholdScale(4)*/
{
	Name = "Rim Lighting Effect";
	_format = RenderTargetType::ColorRgb8;

	_shader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
		{ ShaderPartType::Vertex, "shaders/vertex_shaders/fullscreen_quad.glsl" },
		{ ShaderPartType::Fragment, "shaders/fragment_shaders/post_effects/rim_lighting.glsl" }
	}); 
}

RimLightEffect::~RimLightEffect() = default;

void RimLightEffect::Apply(const Framebuffer::Sptr& gBuffer, VertexArrayObject::Sptr _quadVAO)
{
	
	Application& app = Application::Get();

	_shader->Bind();
	_shader->SetUniform("_color", _rimColor);
	_shader->SetUniform("rimLightPower", strength);
	_shader->SetUniform("iResolution", app.GetWindowSize());
	/*_shader->SetUniform("u_Scale", _scale);
	_shader->SetUniform("u_DepthThreshold", _depthThreshold);
	_shader->SetUniform("u_NormalThreshold", _normalThreshold);
	_shader->SetUniform("u_DepthNormThreshold", _depthNormalThreshold);
	_shader->SetUniform("u_DepthNormThresholdScale", _depthNormalThresholdScale);
	_shader->SetUniform("u_PixelSize", glm::vec2(1.0f) / (glm::vec2)gBuffer->GetSize());*/
	
	gBuffer->BindAttachment(RenderTargetAttachment::Depth, 1); //ddepth?
	gBuffer->BindAttachment(RenderTargetAttachment::Color1, 2); // The normal buffer
}

void RimLightEffect::RenderImGui()
{
	//LABEL_LEFT(ImGui::ColorEdit4,   "Color", &_rimColor.x);
	LABEL_LEFT(ImGui::SliderFloat,  "Strength", &strength, 0.0f, 5.0f);
	//LABEL_LEFT(ImGui::SliderFloat, "Scale", &_scale, 0.1f, 10.0f);
	/*LABEL_LEFT(ImGui::SliderFloat, "Depth Threshold", &_depthThreshold, 0.0f, 1.0f);
	LABEL_LEFT(ImGui::SliderFloat, "Norm. Threshold", &_normalThreshold, 0.0f, 1.0f);
	LABEL_LEFT(ImGui::SliderFloat, "Depth Norm. Threshold", &_depthNormalThreshold, 0.0f, 1.0f);
	LABEL_LEFT(ImGui::SliderFloat, "Depth Norm. Threshold Scale", &_depthNormalThresholdScale, 0.0f, 10.0f);*/
	
}

RimLightEffect::Sptr RimLightEffect::FromJson(const nlohmann::json& data)
{
	RimLightEffect::Sptr result = std::make_shared<RimLightEffect>();
	result->Enabled = JsonGet(data, "enabled", true);
	result->_rimColor = JsonGet(data, "color", result->_rimColor);
	/*result->strength = JsonGet(data, "scale", result->strength); 
	result->_scale = JsonGet(data, "scale", result->_scale);
	result->_depthThreshold = JsonGet(data, "depth_threshold", result->_depthThreshold);
	result->_normalThreshold = JsonGet(data, "normal_threshold", result->_normalThreshold);
	result->_depthNormalThreshold = JsonGet(data, "depth_normal_threshold", result->_depthNormalThreshold);
	result->_depthNormalThresholdScale = JsonGet(data, "depth_normal_threshold_scale", result->_depthNormalThresholdScale);*/
	
	
	return result; 
}

nlohmann::json RimLightEffect::ToJson() const
{
	return {
		{ "enabled", Enabled },
		{ "color", _rimColor }
		/*{ "scale", strength },{ "depth_threshold", _depthThreshold },
		{ "normal_threshold", _normalThreshold },
		{ "depth_normal_threshold", _depthNormalThreshold },
		{ "depth_normal_threshold_scale", _depthNormalThresholdScale }*/
	};
}
