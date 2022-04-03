#include "NightVision.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include "../RenderLayer.h"
#include "Application/Application.h"

NightVision::NightVision():
	NightVision(true){}

NightVision::NightVision(bool activate) :
	PostProcessingLayer::Effect(),
	_shader(nullptr),
	_noise(nullptr),
	_mask(nullptr)
{
	Name = "Night Vision";
	_format = RenderTargetType::ColorRgb8;

	_shader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
		{ ShaderPartType::Vertex, "shaders/vertex_shaders/fullscreen_quad.glsl" },
		{ ShaderPartType::Fragment, "shaders/fragment_shaders/post_effects/NightVision.glsl" }
	});

	if(activate){
		_noise = ResourceManager::CreateAsset<Texture2D>("textures/Night/noise.png");
		_mask = ResourceManager::CreateAsset<Texture2D>("textures/Night/mask.png");
		
	}
}

NightVision::~NightVision() = default;

void NightVision::Apply(const Framebuffer::Sptr & gBuffer, VertexArrayObject::Sptr _quadVAO)
{
	timer += 0.01f;
	_shader->Bind();
	_noise->Bind(1);
	_mask->Bind(2);
	_shader->SetUniform("iTime", timer);
	_shader->SetUniform("luminanceThreshold", light);
	_shader->SetUniform("colorAmplification", color);
}

void NightVision::RenderImGui()
{
	/*const auto& cam = Application::Get().CurrentScene()->MainCamera;

	if (cam != nullptr) {
		ImGui::DragFloat("Focal Depth", &cam->FocalDepth, 0.1f, 0.1f, 100.0f);
		ImGui::DragFloat("Lens Dist. ", &cam->LensDepth,  0.01f, 0.001f, 50.0f);
		ImGui::DragFloat("Aperture   ", &cam->Aperture,   0.1f, 0.1f, 60.0f);
	}*/
	ImGui::SliderFloat("Strength", &timer, 0.0f, 10.0f);
	ImGui::SliderFloat("Luminence", &light, 0.0f, 30.0f);
	ImGui::SliderFloat("Color", &color, 0.0f, 40.0f);
}

NightVision::Sptr NightVision::FromJson(const nlohmann::json & data)
{
	NightVision::Sptr result = std::make_shared<NightVision>();
	result->Enabled = JsonGet(data, "enabled", true);
	return result;
}

nlohmann::json NightVision::ToJson() const
{
	return {
		{ "enabled", Enabled }
	};
}
