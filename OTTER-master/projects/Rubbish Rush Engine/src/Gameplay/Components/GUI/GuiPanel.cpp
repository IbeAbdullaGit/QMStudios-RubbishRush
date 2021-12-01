#include "GuiPanel.h"
#include "Graphics/GuiBatcher.h"
#include "Gameplay/GameObject.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"

GuiPanel::GuiPanel() :
	_borderRadius(-1),
	_color(glm::vec4(1.0f)),
	_texture(nullptr),
	_transform(nullptr)
{ }

GuiPanel::~GuiPanel() = default;

void GuiPanel::SetColor(const glm::vec4& color) {
	_color = color;
}

const glm::vec4& GuiPanel::GetColor() const {
	return _color;
}

int GuiPanel::GetBorderRadius() const {
	return _borderRadius;
}

void GuiPanel::SetBorderRadius(int value) {
	_borderRadius = value;
}

Texture2D::Sptr GuiPanel::GetTexture() const {
	return _texture;
}

void GuiPanel::SetTexture(const Texture2D::Sptr& value) {
	_texture = value;
}

void GuiPanel::Awake() {
	_transform = GetComponent<RectTransform>();
	if (_transform == nullptr) {
		IsEnabled = false;
		LOG_WARN("Failed to find a rect transform for a GUI panel, disabling");
	}
}

void GuiPanel::StartGUI() {
	Texture2D::Sptr tex = _texture != nullptr ? _texture : GuiBatcher::GetDefaultTexture();

	glm::vec2 min = _transform->GetMin();
	glm::vec2 max = _transform->GetMax();

	GuiBatcher::PushRect(min, max, _color, tex, _borderRadius < 0 ? GuiBatcher::GetDefaultBorderRadius() : _borderRadius);

	GuiBatcher::PushScissorRect(min, max);
	GuiBatcher::PushModelTransform(_transform->GetLocalTransform());
}

void GuiPanel::FinishGUI() {
	GuiBatcher::PopModelTransform();
	GuiBatcher::PopScissorRect();
}

void GuiPanel::RenderImGui()
{
	LABEL_LEFT(ImGui::ColorEdit4, "Color ", &_color.x);
	LABEL_LEFT(ImGui::DragInt,    "Radius", &_borderRadius, 1, 0, 128);
}

nlohmann::json GuiPanel::ToJson() const {
	return {
		{ "color",   GlmToJson(_color) },
		{ "border",  _borderRadius },
		{ "texture", _texture  ? _texture->GetGUID().str() : "null" }
	};
}

GuiPanel::Sptr GuiPanel::FromJson(const nlohmann::json& blob) {
	GuiPanel::Sptr result = std::make_shared<GuiPanel>();

	result->_color        = ParseJsonVec4(blob["color"]);
	result->_borderRadius = JsonGet(blob, "border", 0);
	result->_texture      = ResourceManager::Get<Texture2D>(Guid(JsonGet<std::string>(blob, "texture", "null")));

	return result;
}
