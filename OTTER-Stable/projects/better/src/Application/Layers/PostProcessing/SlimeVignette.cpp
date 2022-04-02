#include "SlimeVignette.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include "../RenderLayer.h"
#include "Application/Application.h"

SlimeVignette::SlimeVignette() :
	PostProcessingLayer::Effect(),
	_shader(nullptr)
{
	Name = "Slime Vignette";
	_format = RenderTargetType::ColorRgb8;

	_shader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
		{ ShaderPartType::Vertex, "shaders/vertex_shaders/fullscreen_quad.glsl" },
		{ ShaderPartType::Fragment, "shaders/fragment_shaders/post_effects/SlimeVignette.glsl" }
	});

}

SlimeVignette::~SlimeVignette() = default;

void SlimeVignette::Apply(const Framebuffer::Sptr& gBuffer, VertexArrayObject::Sptr _quadVAO)
{

	_shader->Bind();
	_shader->SetUniform("iTime", timer);
}

void SlimeVignette::RenderImGui()
{
	/*const auto& cam = Application::Get().CurrentScene()->MainCamera;

	if (cam != nullptr) {
		ImGui::DragFloat("Focal Depth", &cam->FocalDepth, 0.1f, 0.1f, 100.0f);
		ImGui::DragFloat("Lens Dist. ", &cam->LensDepth,  0.01f, 0.001f, 50.0f);
		ImGui::DragFloat("Aperture   ", &cam->Aperture,   0.1f, 0.1f, 60.0f);
	}*/
	ImGui::SliderFloat("Strength", &timer, 0.0f, 10.0f);
}

SlimeVignette::Sptr SlimeVignette::FromJson(const nlohmann::json& data)
{
	SlimeVignette::Sptr result = std::make_shared<SlimeVignette>();
	result->Enabled = JsonGet(data, "enabled", true);
	return result;
}

nlohmann::json SlimeVignette::ToJson() const
{
	return {
		{ "enabled", Enabled }
	};
}
