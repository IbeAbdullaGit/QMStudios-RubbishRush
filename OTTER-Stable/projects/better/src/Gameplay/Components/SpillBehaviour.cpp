#include "Gameplay/Components/SpillBehaviour.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Gameplay//Components/PlayerMovementBehavior.h"
#include "Utils/GlmBulletConversions.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Physics/RigidBody.h"

//using namespace Gameplay::Physics;
SpillBehaviour::SpillBehaviour() :
	IComponent()
{ }
SpillBehaviour::~SpillBehaviour() = default;


void SpillBehaviour::OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body)
{
	LOG_INFO("Body has entered our trigger volume: {}", body->GetGameObject()->Name);
	_playerInTrigger = true;
	if (body->GetGameObject()->Name == "Trashy")
	{
		body->SetLinearVelocity(body->GetLinearVelocity() * speed);
		//adjust movement speed
		body->GetGameObject()->Get<PlayerMovementBehavior>()->SetSpill(true);
	}
}

void SpillBehaviour::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) {
	LOG_INFO("Body has left our trigger volume: {}", body->GetGameObject()->Name);
	_playerInTrigger = false;
	//check its the player first
	if (body->GetGameObject()->Name == "Trashy")
	{//default
		body->GetGameObject()->Get<PlayerMovementBehavior>()->SetSpill(false);
	}
}

void SpillBehaviour::RenderImGui() { }

nlohmann::json SpillBehaviour::ToJson() const {
	return { {"speed", speed} };
}

SpillBehaviour::Sptr SpillBehaviour::FromJson(const nlohmann::json& blob) {
	SpillBehaviour::Sptr result = std::make_shared<SpillBehaviour>();
	result->speed = blob["speed"];
	return result;
}
