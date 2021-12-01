#include "Gameplay/Components/GUI/RectTransform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/gtx/matrix_transform_2d.hpp>

#include "Utils/GlmDefines.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"

RectTransform::RectTransform() :
	_position({0.0f, 0.0f}),
	_halfSize({0.5f, 0.5f}),
	_rotation(0.0f),
	_transform(glm::mat3(1.0f)),
	_transformDirty(true)
{ }

RectTransform::~RectTransform() = default;

const glm::vec2& RectTransform::GetPosition() const {
	return _position;
}
void RectTransform::SetPosition(const glm::vec2& pos) {
	_position = pos;
	_transformDirty = true;
}

glm::vec2 RectTransform::GetMin() const {
	return _position - _halfSize;
}
void RectTransform::SetMin(const glm::vec2& value)
{
	// min = position - halfSize
	glm::vec2 newSize = glm::max(value, GetMax()) - glm::min(value, GetMax());
	_halfSize = newSize / 2.0f;
	_position = value + _halfSize;
	_transformDirty = true;
}

glm::vec2 RectTransform::GetMax() const {
	return _position + _halfSize;
}
void RectTransform::SetMax(const glm::vec2& value) {
	glm::vec2 newSize = glm::max(value, GetMin()) - glm::min(value, GetMin());
	_halfSize = newSize / 2.0f;
	_position = value - _halfSize;
	_transformDirty = true;
}

glm::vec2 RectTransform::GetSize() const {
	return _halfSize * 2.0f;
}
void RectTransform::SetSize(const glm::vec2& value) {
	_halfSize = value * 2.0f;
}

void RectTransform::SetRotationDeg(float value) {
	_rotation = glm::radians(value);
}

float RectTransform::GetRotationDeg() const {
	return glm::degrees(_rotation);
}

const glm::mat3& RectTransform::GetLocalTransform() const {
	__RecalcTransforms();
	return _transform;
}

void RectTransform::RenderImGui()
{
	_transformDirty |= LABEL_LEFT(ImGui::DragFloat2, "Position", &_position.x, 0.01f);
	_transformDirty |= LABEL_LEFT(ImGui::DragFloat,  "Rotation", &_rotation, 0.1f);
	glm::vec2 temp = GetSize();
	if (LABEL_LEFT(ImGui::DragFloat2, "Size    ", &temp.x, 0.1f)) {
		SetSize(temp);
	}
	temp = GetMin();
	if (LABEL_LEFT(ImGui::DragFloat2, "Min     ", &temp.x, 0.1f)) {
		SetMin(temp);
	}
	temp = GetMax();
	if (LABEL_LEFT(ImGui::DragFloat2, "Max     ", &temp.x, 0.1f)) {
		SetMax(temp);
	}
}

nlohmann::json RectTransform::ToJson() const {
	return {
		{ "position",   GlmToJson(_position) },
		{ "half_scale", GlmToJson(_halfSize) },
		{ "rotation",   _rotation }
	};
}

RectTransform::Sptr RectTransform::FromJson(const nlohmann::json& blob)
{
	RectTransform::Sptr result = std::make_shared<RectTransform>();
	result->_position = ParseJsonVec<2, float>(blob["position"]);
	result->_halfSize = ParseJsonVec<2, float>(blob["half_scale"]);
	result->_rotation = JsonGet(blob, "rotation", 0.0f);
	return result;
}

void RectTransform::__RecalcTransforms() const {
	if (_transformDirty) {
		_transform = glm::translate(MAT3_IDENTITY, _position) * glm::rotate(MAT3_IDENTITY, _rotation) * glm::translate(MAT3_IDENTITY, -_halfSize);
		_transformDirty = false;
	}
}
