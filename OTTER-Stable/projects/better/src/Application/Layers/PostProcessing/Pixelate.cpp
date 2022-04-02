#include "Pixelate.h"

#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include "../RenderLayer.h"
#include "Application/Application.h"

Pixelate::Pixelate() :
	PostProcessingLayer::Effect(),
	_shader(nullptr)
{
	Name = "Pixelate Effect";
	_format = RenderTargetType::ColorRgb8;

	_shader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
		{ ShaderPartType::Vertex, "shaders/vertex_shaders/fullscreen_quad.glsl" },
		{ ShaderPartType::Fragment, "shaders/fragment_shaders/post_effects/pixelation.glsl" }
	});
}

Pixelate::~Pixelate() = default;

void Pixelate::Apply(const Framebuffer::Sptr& gBuffer, VertexArrayObject::Sptr _quadVAO)
{
	
	_shader->Bind();
	_shader->SetUniform("amount", strength);
}

void Pixelate::RenderImGui()
{
	/*const auto& cam = Application::Get().CurrentScene()->MainCamera;

	if (cam != nullptr) {
		ImGui::DragFloat("Focal Depth", &cam->FocalDepth, 0.1f, 0.1f, 100.0f);
		ImGui::DragFloat("Lens Dist. ", &cam->LensDepth,  0.01f, 0.001f, 50.0f);
		ImGui::DragFloat("Aperture   ", &cam->Aperture,   0.1f, 0.1f, 60.0f);
	}*/
	ImGui::SliderInt("Strength", &strength, 0.0f, 100.0f);
}

Pixelate::Sptr Pixelate::FromJson(const nlohmann::json& data)
{
	Pixelate::Sptr result = std::make_shared<Pixelate>();
	result->Enabled = JsonGet(data, "enabled", true);
	return result;
}

nlohmann::json Pixelate::ToJson() const
{
	return {
		{ "enabled", Enabled }
	};
}
