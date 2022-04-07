#include "Gameplay/Components/ConveyorBeltBehaviour.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Utils/GlmBulletConversions.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Physics/RigidBody.h"
#include "Utils/ImGuiHelper.h"

//using namespace Gameplay::Physics;
ConveyorBeltBehaviour::ConveyorBeltBehaviour() :
	IComponent()
{ }
ConveyorBeltBehaviour::~ConveyorBeltBehaviour() = default;


void ConveyorBeltBehaviour::Awake()
{
	//may need to change the math for this
	glm::mat3 globalRot = glm::mat3_cast(GetGameObject()->GetRotation());
	direction = glm::normalize(glm::vec3(globalRot[2][0], globalRot[2][1], globalRot[2][2]));
	direction *= -1.0f;
	direction *= speed;

}

void ConveyorBeltBehaviour::OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>&body)
{
	if (body != nullptr)
	{//LOG_INFO("Body has entered our trigger volume: {}", body->GetGameObject()->Name);
		_playerInTrigger = true;

		//add force to objects that collide
		body->ApplyImpulse(direction);
		body2 = body;
		//body->GetGameObject()->Get<RigidBody>()->ApplyImpulse(direction);
	}
}

void ConveyorBeltBehaviour::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>&body) {
	if (body != nullptr)
	{//LOG_INFO("Body has left our trigger volume: {}", body->GetGameObject()->Name);
		_playerInTrigger = false;
	}
	
}
void ConveyorBeltBehaviour::Update(float deltatime)
{
	currentScroll = currentScroll + deltatime * speed * visualSpeedScalar;
	//texture offset using set function, create a new shader for this?
	GetGameObject()->Get<RenderComponent>()->GetMaterial()->Set("u_Scale", currentScroll);
	//GetComponent<Renderer>().material.mainTextureOffset = new Vector2(0, currentScroll);
	if (_playerInTrigger)
	{
		body2->ApplyImpulse(direction);
	}

}
void ConveyorBeltBehaviour::RenderImGui() {
	LABEL_LEFT(ImGui::DragFloat, "Speed", &speed, 1.0f);
}

nlohmann::json ConveyorBeltBehaviour::ToJson() const {
	return { {"scroll", currentScroll}, {"speed2", speed}, {"visualspeed", visualSpeedScalar} };
}

ConveyorBeltBehaviour::Sptr ConveyorBeltBehaviour::FromJson(const nlohmann::json & blob) {
	ConveyorBeltBehaviour::Sptr result = std::make_shared<ConveyorBeltBehaviour>();
	result->currentScroll = blob["scroll"];
	result->speed = blob["speed2"];
	result->visualSpeedScalar = blob["visualspeed"];
	return result;
}
