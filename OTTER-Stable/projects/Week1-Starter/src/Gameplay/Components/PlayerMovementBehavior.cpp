#include "Gameplay/Components/PlayerMovementBehavior.h"
#include <GLFW/glfw3.h>
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/ImGuiHelper.h"
#include "Application/Application.h"


void PlayerMovementBehavior::Awake()
{
	_body = GetComponent<Gameplay::Physics::RigidBody>(); //Gets the Physics Body that it's attached to
	if (_body == nullptr) {
		IsEnabled = false;
	}
	/*_body->SetLinearDamping(0.9f);
	_body->SetAngularDamping(1.f);*/
}

void PlayerMovementBehavior::RenderImGui() {
	return;
}

nlohmann::json PlayerMovementBehavior::ToJson() const {
	return {
		{ "impulse", _impulse }, {"spill_state", in_spill}, {"moving", is_moving}
	};
}

PlayerMovementBehavior::PlayerMovementBehavior() :
	IComponent(),
	_impulse(0.1f) //Movement Value
{ }

PlayerMovementBehavior::~PlayerMovementBehavior() = default;

PlayerMovementBehavior::Sptr PlayerMovementBehavior::FromJson(const nlohmann::json & blob) {
	PlayerMovementBehavior::Sptr result = std::make_shared<PlayerMovementBehavior>();
	result->_impulse = blob["impulse"];
	result->in_spill = blob["spill_state"];
	result->is_moving = blob["moving"];
	return result;
}

void PlayerMovementBehavior::SetSpill(bool state)
{
	in_spill = state;
}

void PlayerMovementBehavior::Update(float deltaTime) {

	Application& app = Application::Get();
	is_moving = false;
	if (in_spill)
	{
		_impulse = 0.05f;
	}
	else
	{
		_impulse = 0.1f;
	}

	//IF SPACE PRESSED = MOVE
	if (glfwGetKey(app.GetWindow(), GLFW_KEY_W)) {
		if (_body->GetLinearVelocity().y >= -5.0f) {
			_body->ApplyImpulse(glm::vec3(0.0f, -_impulse, 0.0f));
			//GetGameObject()->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			//body->GetGameObject()->SetPostion(_body->GetGameObject()->GetPosition() + glm::vec3(0.0f, -_impulse, 0.0f));
			is_moving = true;
		}
	}

	if (glfwGetKey(app.GetWindow(), GLFW_KEY_S)) {
		if (_body->GetLinearVelocity().y <= 5.0f) {
			_body->ApplyImpulse(glm::vec3(0.0f, _impulse, 0.0f));
			//GetGameObject()->SetRotation(glm::vec3(90.0f, 0.0f, 180.0f));
			//_body->GetGameObject()->SetPostion(_body->GetGameObject()->GetPosition() + glm::vec3(0.0f, _impulse, 0.0f));
			is_moving = true;
		}
	}

	if (glfwGetKey(app.GetWindow(), GLFW_KEY_A)) {
		if (_body->GetLinearVelocity().x <= 5.0f) {
			_body->ApplyImpulse(glm::vec3(_impulse, 0.0f, 0.0f));

			//GetGameObject()->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
			//_body->GetGameObject()->SetPostion(_body->GetGameObject()->GetPosition() + glm::vec3(_impulse, 0.0f, 0.0f));
			is_moving = true;
		}
	}

	if (glfwGetKey(app.GetWindow(), GLFW_KEY_D)) {
		if (_body->GetLinearVelocity().x >= -5.0f) {
			_body->ApplyImpulse(glm::vec3(-_impulse, 0.0f, 0.0f));
			//GetGameObject()->SetRotation(glm::vec3(90.0f, 0.0f, -90.0f));
			//_body->GetGameObject()->SetPostion(_body->GetGameObject()->GetPosition() + glm::vec3(-_impulse, 0.0f, 0.0f));
			is_moving = true;
		}
	}


	//Rotate when the key is pressed
	if (glfwGetKey(app.GetWindow(), GLFW_KEY_W) && GLFW_PRESS) {
		currentRotation = GetGameObject()->GetRotation();
		targetRotation = glm::quat(glm::radians(glm::vec3(90.0f, 0.0f, 180.0f)));
		
	}
	if (glfwGetKey(app.GetWindow(), GLFW_KEY_A) && GLFW_PRESS) {
		currentRotation = GetGameObject()->GetRotation();
		targetRotation = glm::quat(glm::radians(glm::vec3(90.0f, 0.0f, 270.0f)));
	}

	if (glfwGetKey(app.GetWindow(), GLFW_KEY_S) && GLFW_PRESS) {
		currentRotation = GetGameObject()->GetRotation();
		targetRotation = glm::quat(glm::radians(glm::vec3(90.0f, 0.0f, 0.0f)));
	}
	if (glfwGetKey(app.GetWindow(), GLFW_KEY_D) && GLFW_PRESS) {
		currentRotation = GetGameObject()->GetRotation();
		targetRotation = glm::quat(glm::radians(glm::vec3(90.0f, 0.0f, 90.0f)));
	}

	currentRotation = glm::mix(currentRotation, targetRotation, speed * deltaTime);
	GetGameObject()->SetRotation(currentRotation);
	
}

