#include "Gameplay/Components/DeleteObjectBehaviour.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Utils/GlmBulletConversions.h"
#include "Gameplay/Components/IComponent.h"

#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"

//using namespace Gameplay::Physics;
DeleteObjectBehaviour::DeleteObjectBehaviour() :
	IComponent()
{ 
	
}
DeleteObjectBehaviour::~DeleteObjectBehaviour() = default;

void DeleteObjectBehaviour::setEnter(Gameplay::Material::Sptr v)
{
	EnterMaterial = v;
}

void DeleteObjectBehaviour::setExit(Gameplay::Material::Sptr v)
{
	ExitMaterial = v;
}


void DeleteObjectBehaviour::OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody> & body)
{
	LOG_INFO("Body has entered our trigger volume: {}", body->GetGameObject()->Name);
	_playerInTrigger = true;
	//get our scene, delete this line later
	_scene = GetGameObject()->GetScene();
	
	if (GetGameObject()->Get<RenderComponent>()->GetMaterial()->Name == "Block2")
	{
		canBreak = false;
		//swap material and then pass
		GetGameObject()->Get<RenderComponent>()->SetMaterial(ExitMaterial);
		//bounce ball back in opposite direction
		glm::vec3 dir = body->GetGameObject()->GetPosition() - GetGameObject()->GetPosition();
		//normalize
		glm::float1 len = sqrt(pow(dir.x, 2) + pow(dir.y, 2) + pow(dir.z, 2));
		dir = dir / len;
		//this needs to be better?
		if (body->GetLinearVelocity().z > 0) //positive, going up
		{
			const glm::vec3 wForce = glm::vec3(dir.x, 0.0f, -5.0f);
			body->SetLinearVelocity(glm::vec3(0.0, 0.0, 0.0));
			body->GetGameObject()->Get<Gameplay::Physics::RigidBody>()->ApplyImpulse(wForce);
		}
		else //negative, going down
		{
			const glm::vec3 wForce = glm::vec3(dir.x, 0.0f, 5.0f);
			body->SetLinearVelocity(glm::vec3(0.0, 0.0, 0.0));
			body->GetGameObject()->Get<Gameplay::Physics::RigidBody>()->ApplyImpulse(wForce);
		}
		//_scene->score += 1;
	}
	else if (canBreak)
	{
		//delete self
		//bounce ball back in opposite direction
		glm::vec3 dir = body->GetGameObject()->GetPosition() - GetGameObject()->GetPosition();
		//normalize
		glm::float1 len = sqrt(pow(dir.x, 2) + pow(dir.y, 2) + pow(dir.z, 2));
		dir = dir / len;
		//this needs to be better?
		if (body->GetLinearVelocity().z > 0) //positive, going up
		{
			const glm::vec3 wForce = glm::vec3(dir.x, 0.0f, -5.0f);
			body->SetLinearVelocity(glm::vec3(0.0, 0.0, 0.0));
			body->GetGameObject()->Get<Gameplay::Physics::RigidBody>()->ApplyImpulse(wForce);
		}
		else //negative, going down
		{
			const glm::vec3 wForce = glm::vec3(dir.x, 0.0f, 5.0f);
			body->SetLinearVelocity(glm::vec3(0.0, 0.0, 0.0));
			body->GetGameObject()->Get<Gameplay::Physics::RigidBody>()->ApplyImpulse(wForce);
		}
		//convoluted but whatever
		//_scene->getBricks().erase(remove(_scene->getBricks().begin(), _scene->getBricks().end(), GetGameObject()), _scene->getBricks().end());
		//_scene->DeleteGameObject(_scene->FindObjectByGUID(GetGameObject()->GUID));
		//Objects.erase(remove(Objects.begin(), Objects.end(), _deletionQueue[i]), Objects.end());
		//_scene->score += 1;
		//_scene->brick_count -= 1;
	}
	
	
	//increment points here

	//_scene->need_update = true;
	
	
	
}
nlohmann::json DeleteObjectBehaviour::ToJson() const {
	return {
		{ "enter_material", EnterMaterial != nullptr ? EnterMaterial->GetGUID().str() : "null" },
		{ "exit_material", ExitMaterial != nullptr ? ExitMaterial->GetGUID().str() : "null" }
	};
}

DeleteObjectBehaviour::Sptr DeleteObjectBehaviour::FromJson(const nlohmann::json& blob) {
	DeleteObjectBehaviour::Sptr result = std::make_shared<DeleteObjectBehaviour>();
	result->EnterMaterial = ResourceManager::Get<Gameplay::Material>(Guid(blob["enter_material"]));
	result->ExitMaterial = ResourceManager::Get<Gameplay::Material>(Guid(blob["exit_material"]));
	return result;
}

void DeleteObjectBehaviour::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody> & body) {
	LOG_INFO("Body has left our trigger volume: {}", body->GetGameObject()->Name);
	_playerInTrigger = false;
	canBreak = true;
}

void DeleteObjectBehaviour::RenderImGui() { }

