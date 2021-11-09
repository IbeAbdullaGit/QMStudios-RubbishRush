#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Utils/GlmBulletConversions.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Physics/RigidBody.h"

using namespace Gameplay::Physics;
TriggerVolumeEnterBehaviour::TriggerVolumeEnterBehaviour() :
	IComponent()
{ }
TriggerVolumeEnterBehaviour::~TriggerVolumeEnterBehaviour() = default;


void TriggerVolumeEnterBehaviour::OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body)
{
	LOG_INFO("Body has entered our trigger volume: {}", body->GetGameObject()->Name);
	_playerInTrigger = true;
	glm::vec3 dir = body->GetGameObject()->GetPosition() - GetGameObject()->GetPosition();
	//normalize
	glm::float1 len = sqrt(pow(dir.x, 2) + pow(dir.y, 2) + pow(dir.z, 2));
	dir = dir / len;
	glm::vec3 wForce = glm::vec3(dir.x, 0.0f, 2.0f);// * GetGameObject()->"Player";  //multiply by bodies velocity
	if (body->GetGameObject()->Name == "Wall1")
	{
		wForce = glm::vec3(-0.8f, 0.0f, dir.y);// * GetGameObject()->"Player";  //multiply by bodies velocity
		GetGameObject()->Get<RigidBody>()->ApplyImpulse(wForce);
	}
	else if (body->GetGameObject()->Name == "Wall2")
	{
		wForce = glm::vec3(0.8f, 0.0f, dir.y);// * GetGameObject()->"Player";  //multiply by bodies velocity
		
	}
	
	GetGameObject()->Get<RigidBody>()->ApplyImpulse(wForce);
	
}

void TriggerVolumeEnterBehaviour::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) {
	LOG_INFO("Body has left our trigger volume: {}", body->GetGameObject()->Name);
	_playerInTrigger = false;
	//ballM->SetPostion(glm::vec3(ballM->GetPosition().x, 0.0f, ballM->GetPosition().z));
	//GetGameObject()->SetPostion(glm::vec3(GetGameObject()->GetPosition().x, 0.0f, GetGameObject()->GetPosition().z));
}

void TriggerVolumeEnterBehaviour::RenderImGui() { }

nlohmann::json TriggerVolumeEnterBehaviour::ToJson() const {
	return { };
}

TriggerVolumeEnterBehaviour::Sptr TriggerVolumeEnterBehaviour::FromJson(const nlohmann::json& blob) {
	TriggerVolumeEnterBehaviour::Sptr result = std::make_shared<TriggerVolumeEnterBehaviour>();
	return result;
}
