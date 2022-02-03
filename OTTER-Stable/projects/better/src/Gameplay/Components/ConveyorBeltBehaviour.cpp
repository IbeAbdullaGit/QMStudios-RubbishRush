#include "Gameplay/Components/ConveyorBeltBehaviour.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Utils/GlmBulletConversions.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Physics/RigidBody.h"

//using namespace Gameplay::Physics;
ConveyorBeltBehaviour::ConveyorBeltBehaviour() :
	IComponent()
{ }
ConveyorBeltBehaviour::~ConveyorBeltBehaviour() = default;


void ConveyorBeltBehaviour::OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>&body)
{
	LOG_INFO("Body has entered our trigger volume: {}", body->GetGameObject()->Name);
	_playerInTrigger = true;
	//may need to change the math for this
	glm::vec3 direction = glm::normalize(glm::mat3_cast(GetGameObject()->GetRotation()) * glm::vec3(0, 1, 0)) * -1.0f;
	direction *= speed;
	//add force to objects that collide
	body->ApplyImpulse(direction);
	//body->GetGameObject()->Get<RigidBody>()->ApplyImpulse(direction);
	
	
	
}

void ConveyorBeltBehaviour::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>&body) {
	LOG_INFO("Body has left our trigger volume: {}", body->GetGameObject()->Name);
	_playerInTrigger = false;
	
}
void ConveyorBeltBehaviour::Update(float deltatime)
{
	currentScroll = currentScroll + deltatime * speed * visualSpeedScalar;
	//texture offset using set function, create a new shader for this?
	//GetGameObject()->Get<RenderComponent>()->GetMaterial()->
	//GetComponent<Renderer>().material.mainTextureOffset = new Vector2(0, currentScroll);

}
void ConveyorBeltBehaviour::RenderImGui() { }

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
