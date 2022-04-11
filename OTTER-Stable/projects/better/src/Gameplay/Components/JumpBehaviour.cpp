#pragma once
#include "Gameplay/Components/JumpBehaviour.h"
#include <GLFW/glfw3.h>
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/ImGuiHelper.h"
#include "Gameplay/InputEngine.h"
#include "Application/Application.h"
#include "Gameplay/Components/GroundBehaviour.h"
#include "AudioEngine.h"

void JumpBehaviour::Awake()
{
	_body = GetComponent<Gameplay::Physics::RigidBody>();
	if (_body == nullptr) {
		IsEnabled = false;
	}
}

void JumpBehaviour::RenderImGui() {
	LABEL_LEFT(ImGui::DragFloat, "Impulse", &_impulse, 1.0f);
}

nlohmann::json JumpBehaviour::ToJson() const {
	return {
		{ "impulse", _impulse }
	};
}
void JumpBehaviour::OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body)
{
	//if the triggering body has the ground tag, aka is the floor
	if (body->GetGameObject()->Has<GroundBehaviour>())
	{
		activated = true;
		in_air = false;
	}
}

void JumpBehaviour::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body)
{
	if (body != nullptr)
	{//activated = false; //when u leave the floor, should disable jump
		if (body->GetGameObject()->Has<GroundBehaviour>())
		{
			activated = false;
			in_air = true;
		}
	}
}

JumpBehaviour::JumpBehaviour() :
	IComponent(),
	_impulse(6.0f)
{ }

JumpBehaviour::~JumpBehaviour() = default;


JumpBehaviour::Sptr JumpBehaviour::FromJson(const nlohmann::json& blob) {
	JumpBehaviour::Sptr result = std::make_shared<JumpBehaviour>();
	result->_impulse = blob["impulse"];
	return result;
}

void JumpBehaviour::Update(float deltaTime) {
	
	if (activated)
	{
		Application& app = Application::Get();

		if (InputEngine::GetKeyState(GLFW_KEY_SPACE) == ButtonState::Pressed) {
			_body->ApplyImpulse(glm::vec3(0.0f, 0.0f, _impulse));
			/*Gameplay::IComponent::Sptr ptr = Panel.lock();
			if (ptr != nullptr) {
				ptr->IsEnabled = !ptr->IsEnabled;
			}*/
			AudioEngine::playEventS("event:/Sounds/SoundEffects/Jump");
			AudioEngine::EventPosChangeS("event:/Sounds/SoundEffects/Jump", _body->GetGameObject()->GetPosition().x, _body->GetGameObject()->GetPosition().y, _body->GetGameObject()->GetPosition().z);
			activated = false; //single jump
			jumpResetTimer = Timing::Current().TimeSinceAppLoad();
		}
	}
	else {
		if (_body->GetGameObject()->GetPosition().z < 0.107f && (Timing::Current().TimeSinceAppLoad() - jumpResetTimer) >= 4.0f) {
			activated = true;
			in_air = false;
		}
	}
}

