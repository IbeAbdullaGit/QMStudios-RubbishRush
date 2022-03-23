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
	_impulse(0.0f) //Movement Value
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
	//not moving
	is_moving = false;
	
	if (in_spill)
	{
		_impulse = _impulse / 2.0f;
	}
	else
	{
		//_impulse = 0.0f;
	}

	////running
	//if (glfwGetKey(app.GetWindow(), GLFW_KEY_LEFT_SHIFT))
	//{
	//	_impulse *= 2.0f;
	//	is_running = true;
	//}
	//else
	//{
	//	is_running = false;
	//}

	//IF SPACE PRESSED = MOVE
	if (glfwGetKey(app.GetWindow(), GLFW_KEY_W) || glfwGetKey(app.GetWindow(), GLFW_KEY_UP)) {
		if (_body->GetLinearVelocity().y >= -5.0f) {
			_body->ApplyImpulse(glm::vec3(0.0f, -_impulse, 0.0f));
			//GetGameObject()->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			//body->GetGameObject()->SetPostion(_body->GetGameObject()->GetPosition() + glm::vec3(0.0f, -_impulse, 0.0f));
			is_moving = true;
			_impulse += acceleration;
			if (_impulse > max_speed)
			{
				_impulse = max_speed;
			}
		}
	}

	if (glfwGetKey(app.GetWindow(), GLFW_KEY_S) || glfwGetKey(app.GetWindow(), GLFW_KEY_DOWN)) {
		if (_body->GetLinearVelocity().y <= 5.0f) {
			_body->ApplyImpulse(glm::vec3(0.0f, _impulse, 0.0f));
			//GetGameObject()->SetRotation(glm::vec3(90.0f, 0.0f, 180.0f));
			//_body->GetGameObject()->SetPostion(_body->GetGameObject()->GetPosition() + glm::vec3(0.0f, _impulse, 0.0f));
			is_moving = true;
			_impulse += acceleration;
			if (_impulse > max_speed)
			{
				_impulse = max_speed;
			}
		}
	}

	if (glfwGetKey(app.GetWindow(), GLFW_KEY_A)|| glfwGetKey(app.GetWindow(), GLFW_KEY_LEFT)) {
		if (_body->GetLinearVelocity().x <= 5.0f) {
			_body->ApplyImpulse(glm::vec3(_impulse, 0.0f, 0.0f));

			//GetGameObject()->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
			//_body->GetGameObject()->SetPostion(_body->GetGameObject()->GetPosition() + glm::vec3(_impulse, 0.0f, 0.0f));
			is_moving = true;
			_impulse += acceleration;
			if (_impulse > max_speed)
			{
				_impulse = max_speed;
			}
		}
	}
	
	if (glfwGetKey(app.GetWindow(), GLFW_KEY_D)|| glfwGetKey(app.GetWindow(), GLFW_KEY_RIGHT)) {
		if (_body->GetLinearVelocity().x >= -5.0f) {
			_body->ApplyImpulse(glm::vec3(-_impulse, 0.0f, 0.0f));
			//GetGameObject()->SetRotation(glm::vec3(90.0f, 0.0f, -90.0f));
			//_body->GetGameObject()->SetPostion(_body->GetGameObject()->GetPosition() + glm::vec3(-_impulse, 0.0f, 0.0f));
			is_moving = true;
			_impulse += acceleration;
			if (_impulse > max_speed)
			{
				_impulse = max_speed;
			}
		}
	}
	if (!is_moving)
	{
		if (_impulse < 0)
		{
			_impulse += acceleration;
			if (_impulse >= 0)
			{
				_impulse = 0.0f;
			}
		}
		else
		{
			_impulse -= acceleration;
			if (_impulse <= 0)
			{
				_impulse = 0.0f;
			}
		}

	}

	//Rotate when the key is pressed
	if (glfwGetKey(app.GetWindow(), GLFW_KEY_W) && GLFW_PRESS) {
		currentRotation = GetGameObject()->GetRotation();
		targetRotation = glm::quat(glm::radians(glm::vec3(90.0f, 0.0f, 180.0f)));
		direction = glm::vec3(0.0f, 5.0f, 0.0f);
		
	}
	if (glfwGetKey(app.GetWindow(), GLFW_KEY_A) && GLFW_PRESS) {
		currentRotation = GetGameObject()->GetRotation();
		targetRotation = glm::quat(glm::radians(glm::vec3(90.0f, 0.0f, 270.0f)));
		direction = glm::vec3(5.0f, 0.0f, 0.0f);
	}

	if (glfwGetKey(app.GetWindow(), GLFW_KEY_S) && GLFW_PRESS) {
		currentRotation = GetGameObject()->GetRotation();
		targetRotation = glm::quat(glm::radians(glm::vec3(90.0f, 0.0f, 0.0f)));
		direction = glm::vec3(0.0f, -5.0f, 0.0f);
		
	}
	if (glfwGetKey(app.GetWindow(), GLFW_KEY_D) && GLFW_PRESS) {
		currentRotation = GetGameObject()->GetRotation();
		targetRotation = glm::quat(glm::radians(glm::vec3(90.0f, 0.0f, 90.0f)));
		direction = glm::vec3(-5.0f, 0.0f, 0.0f);
	}

	currentRotation = glm::mix(currentRotation, targetRotation, speed * deltaTime);
	GetGameObject()->SetRotation(currentRotation);

	////test creating particles
	//if (glfwGetKey(app.GetWindow(), GLFW_KEY_1) && GLFW_PRESS)
	//{
	//	count += 1;
	//	Gameplay::GameObject::Sptr particles = app.CurrentScene()->CreateGameObject("Particles"+ std::to_string(count));
	//	ParticleSystem::Sptr particleManager = particles->Add<ParticleSystem>();
	//	particleManager->AddEmitter(GetGameObject()->GetPosition(), direction, 5.0f, glm::vec4(1.0f, 0.8f, 0.3f, 1.0f));
	//	//particles_store.push_back(particles);
	//}
	////test creating particles
	//if (glfwGetKey(app.GetWindow(), GLFW_KEY_2) && GLFW_PRESS)
	//{
	//	Gameplay::GameObject::Sptr particles = app.CurrentScene()->FindObjectByName("Particles" + std::to_string(count));
	//	app.CurrentScene()->RemoveGameObject(particles);
	//}

	////update particles at feet
	//if (is_moving)
	//{
	//	if (!created)
	//	{//keep creating particles as we move?
	//	// 
	//	//particleManager->IsEnabled = true;
	//		Gameplay::GameObject::Sptr particles = app.CurrentScene()->CreateGameObject("Particles 2");
	//		ParticleSystem::Sptr particleManager = particles->Add<ParticleSystem>();
	//		particleManager->AddEmitter(GetGameObject()->GetPosition(), direction, 5.0f, glm::vec4(1.0f, 0.8f, 0.3f, 1.0f));
	//		particles_store.push_back(particles);

	//		created = true;
	//	}
	//}
	//else
	//{
	//	//particleManager->IsEnabled = false;
	//	//delete all the particles we created
	//	for (int i = 0; i < particles_store.size(); i++)
	//	{
	//		app.CurrentScene()->RemoveGameObject(particles_store[i]);
	//	}

	//	particles_store.clear();
	//	created = false;
	//	
	//}
	
}

