#include "Gameplay/Components/SimpleCameraControl.h"
#include <GLFW/glfw3.h>
#define  GLM_SWIZZLE
#include <GLM/gtc/quaternion.hpp>

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"

SimpleCameraControl::SimpleCameraControl() :
	IComponent(),
	_mouseSensitivity({ 0.5f, 0.3f }),
	_moveSpeeds(glm::vec3(1.0f)),
	_shiftMultipler(2.0f),
	_currentRot(glm::vec2(0.0f)),
	_isMousePressed(false)
{ }

SimpleCameraControl::~SimpleCameraControl() = default;

void SimpleCameraControl::Awake() {
	_window = GetGameObject()->GetScene()->Window;
}

void SimpleCameraControl::Update(float deltaTime)
{
	if (glfwGetMouseButton(_window, 0)) {
		if (_isMousePressed == false) {
			glfwGetCursorPos(_window, &_prevMousePos.x, &_prevMousePos.y);
		}
		_isMousePressed = true;
	} else {
		_isMousePressed = false;
	}

	if (_isMousePressed) {
		glm::dvec2 currentMousePos;
		glfwGetCursorPos(_window, &currentMousePos.x, &currentMousePos.y);

		_currentRot.x += static_cast<float>(currentMousePos.x - _prevMousePos.x) * _mouseSensitivity.x;
		_currentRot.y += static_cast<float>(currentMousePos.y - _prevMousePos.y) * _mouseSensitivity.y;
		glm::quat rotX = glm::angleAxis(glm::radians(_currentRot.x), glm::vec3(0, 0, 1));
		glm::quat rotY = glm::angleAxis(glm::radians(_currentRot.y), glm::vec3(1, 0, 0));
		glm::quat currentRot = rotX * rotY;
		GetGameObject()->SetRotation(currentRot);

		_prevMousePos = currentMousePos;

		glm::vec3 input = glm::vec3(0.0f);
		if (glfwGetKey(_window, GLFW_KEY_W)) {
			input.z -= _moveSpeeds.x;
		}
		if (glfwGetKey(_window, GLFW_KEY_S)) {
			input.z += _moveSpeeds.x;
		}
		if (glfwGetKey(_window, GLFW_KEY_A)) {
			input.x -= _moveSpeeds.y;
		}
		if (glfwGetKey(_window, GLFW_KEY_D)) {
			input.x += _moveSpeeds.y;
		}
		if (glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL)) {
			input.y -= _moveSpeeds.z;
		}
		if (glfwGetKey(_window, GLFW_KEY_SPACE)) {
			input.y += _moveSpeeds.z;
		}
		
		if (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT)) {
			input *= _shiftMultipler;
		}

		input *= deltaTime;

		glm::vec3 worldMovement = currentRot * glm::vec4(input, 1.0f);
		GetGameObject()->SetPostion(GetGameObject()->GetPosition() + worldMovement);
	}
}

void SimpleCameraControl::RenderImGui()
{
	LABEL_LEFT(ImGui::DragFloat2, "Mouse Sensitivity", &_mouseSensitivity.x, 0.01f);
	LABEL_LEFT(ImGui::DragFloat3, "Move Speed       ", &_moveSpeeds.x, 0.01f, 0.01f);
	LABEL_LEFT(ImGui::DragFloat , "Shift Multiplier ", &_shiftMultipler, 0.01f, 1.0f);
}

nlohmann::json SimpleCameraControl::ToJson() const {
	return {
		{ "mouse_sensitivity", GlmToJson(_mouseSensitivity) },
		{ "move_speed", GlmToJson(_moveSpeeds) },
		{ "shift_mult", _shiftMultipler }
	};
}

SimpleCameraControl::Sptr SimpleCameraControl::FromJson(const nlohmann::json& blob) {
	SimpleCameraControl::Sptr result = std::make_shared<SimpleCameraControl>();
	result->_mouseSensitivity = ParseJsonVec2(blob["mouse_sensitivity"]);
	result->_moveSpeeds       = ParseJsonVec3(blob["move_speed"]);
	result->_shiftMultipler   = JsonGet(blob, "shift_mult", 2.0f);
	return result;
}
