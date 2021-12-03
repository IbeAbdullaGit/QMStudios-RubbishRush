#include "Gameplay/Components/CollectTrashBehaviour.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Utils/GlmBulletConversions.h"
#include "Gameplay/Components/IComponent.h"
#include <GLFW/glfw3.h>
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/ImGuiHelper.h"


CollectTrashBehaviour::CollectTrashBehaviour() :
	IComponent()
{ }
CollectTrashBehaviour::~CollectTrashBehaviour() = default;

void CollectTrashBehaviour::OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) {
	//press e to collect, and only collide with trash
	if (body->GetGameObject()->Name == "Trashy" /* && glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_E)*/) {
		
		////get our scene, delete this line later
		//_scene = GetGameObject()->GetScene();
		////delete trash from scene
		//_scene->DeleteGameObject(_scene->FindObjectByGUID(GetGameObject()->GUID));
		////increment trash count?
		//_scene->trash += 1;
		//std::cout << "Current trash collected: " << _scene->trash << std::endl;
		activated = true;
		
	}
	LOG_INFO("Entered trigger: {}", body->GetGameObject()->Name);
}

void CollectTrashBehaviour::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) {
	
	LOG_INFO("Left trigger: {}", body->GetGameObject()->Name);
	activated = false;
}
void CollectTrashBehaviour::Update(float deltatime)
{
	if (activated && GetGameObject() !=nullptr)
	{
		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_E))
		{
			//get our scene, delete this line later
			_scene = GetGameObject()->GetScene();
			//delete trash from scene
			GameObject::Sptr trash = _scene->FindObjectByName(GetGameObject()->Name);
			_scene->RemoveGameObject(trash);
			//increment trash count?
			_scene->trash += 1;
			std::cout << "Current trash collected: " << _scene->trash << std::endl;

			activated = false;
		}
		/*Gameplay::IComponent::Sptr ptr = Panel.lock();
		if (ptr != nullptr)
		{
			ptr->IsEnabled = !ptr->IsEnabled;
		}*/
	}
}

void CollectTrashBehaviour::Awake() {
	
}

void CollectTrashBehaviour::RenderImGui() { }

nlohmann::json CollectTrashBehaviour::ToJson() const {
	
	//return {
	//	{ "trash_collected", trash != nullptr ? trash->GUID.str() : "null" }
	//	//{ "exit_material", ExitMaterial != nullptr ? ExitMaterial->GetGUID().str() : "null" }
	//};
	return {};
}

CollectTrashBehaviour::Sptr CollectTrashBehaviour::FromJson(const nlohmann::json & blob) {
	CollectTrashBehaviour::Sptr result = std::make_shared<CollectTrashBehaviour>();	
	////result->trash = ResourceManager::Get<GameObject::Sptr>(Guid(blob["trash_collected"]));
	//
	//result->trash = (GameObject::FromJson(Guid(blob["trash_collected"]), result->GetGameObject()->GetScene()));
	return result;
}
