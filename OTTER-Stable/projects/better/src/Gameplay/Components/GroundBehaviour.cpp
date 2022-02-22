#include "Gameplay/Components/GroundBehaviour.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Utils/GlmBulletConversions.h"
#include "Gameplay/Components/IComponent.h"
#include <GLFW/glfw3.h>
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/ImGuiHelper.h"
#include "MorphAnimator.h"
#include "Application/Application.h"


GroundBehaviour::GroundBehaviour() :
	IComponent()
{ }
GroundBehaviour::~GroundBehaviour() = default;

void GroundBehaviour::OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>&body) {
	
}

void GroundBehaviour::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>&body) {

	
}
void GroundBehaviour::Update(float deltatime)
{
	
}

void GroundBehaviour::Awake()
{
	
}

void GroundBehaviour::RenderImGui() { }

nlohmann::json GroundBehaviour::ToJson() const {

	//return {
	//	{ "trash_collected", trash != nullptr ? trash->GUID.str() : "null" }
	//	//{ "exit_material", ExitMaterial != nullptr ? ExitMaterial->GetGUID().str() : "null" }
	//};
	return {};
}

GroundBehaviour::Sptr GroundBehaviour::FromJson(const nlohmann::json & blob) {
	GroundBehaviour::Sptr result = std::make_shared<GroundBehaviour>();
	////result->trash = ResourceManager::Get<GameObject::Sptr>(Guid(blob["trash_collected"]));
	//
	//result->trash = (GameObject::FromJson(Guid(blob["trash_collected"]), result->GetGameObject()->GetScene()));
	return result;
}

