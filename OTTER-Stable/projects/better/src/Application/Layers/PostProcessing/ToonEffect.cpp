#include "ToonEffect.h"
#include "Utils/ResourceManager/ResourceManager.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"

ToonEffect::ToonEffect() :
	ToonEffect(true) { }

ToonEffect::ToonEffect(bool defaultLut) :
	PostProcessingLayer::Effect(),
	_shader(nullptr),
	toonterm(nullptr), _strength(16)
{
	Name = "Toon Shading";
	_format = RenderTargetType::ColorRgb8;

	_shader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
		{ ShaderPartType::Vertex, "shaders/vertex_shaders/fullscreen_quad.glsl" },
		{ ShaderPartType::Fragment, "shaders/fragment_shaders/post_effects/toon_filter.glsl" }
	});

	if (defaultLut) {
		toonterm = ResourceManager::CreateAsset<Texture1D>("luts/toon-1D.png");
		toonterm->SetWrap(WrapMode::ClampToEdge);
	}
}

ToonEffect::~ToonEffect() = default;

void ToonEffect::Apply(const Framebuffer::Sptr& gBuffer, VertexArrayObject::Sptr _quadVAO)
{
	_shader->Bind();
	toonterm->Bind(1);
	_shader->SetUniform("Steps", _strength);
}

void ToonEffect::RenderImGui()
{
	LABEL_LEFT(ImGui::LabelText, "Toon Shading", toonterm ? toonterm->GetDebugName().c_str() : "none");
	LABEL_LEFT(ImGui::SliderInt, "Strength", &_strength, 1, 64);
}

ToonEffect::Sptr ToonEffect::FromJson(const nlohmann::json& data)
{
	ToonEffect::Sptr result = std::make_shared<ToonEffect>(false);
	result->Enabled = JsonGet(data, "enabled", true);
	result->_strength = JsonGet(data, "strength", result->_strength);
	result->toonterm = ResourceManager::Get<Texture1D>(Guid(data["lut"].get<std::string>()));
	return result;
}

nlohmann::json ToonEffect::ToJson() const
{
	return {
		{ "enabled", Enabled },
		{ "lut", toonterm != nullptr ? toonterm->GetGUID().str() : "null" },
		{ "strength", _strength }
	};
}
