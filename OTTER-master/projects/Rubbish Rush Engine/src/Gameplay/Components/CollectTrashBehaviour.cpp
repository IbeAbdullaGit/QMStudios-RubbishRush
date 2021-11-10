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
	if (body->GetGameObject()->Name == "Trashy" && glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_E)) {
		
		//get our scene, delete this line later
		_scene = GetGameObject()->GetScene();
		//delete trash from scene
		_scene->DeleteGameObject(_scene->FindObjectByGUID(GetGameObject()->GUID));
		//increment trash count?
		_scene->trash += 1;
		std::cout << "Current trash collected: " << _scene->trash << std::endl;
		
	}
	LOG_INFO("Entered trigger: {}", body->GetGameObject()->Name);
}

void CollectTrashBehaviour::OnLeavingTrigger(const Gameplay::Physics::TriggerVolume::Sptr & trigger) {
	
	LOG_INFO("Left trigger: {}", trigger->GetGameObject()->Name);
}
void CollectTrashBehaviour::Update(float deltatime)
{
	//if (_isPressed)
	//{
	//	timer += deltatime;
	//}
	//if (_isPressed && glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_E) && timer <=cooldownTime)
	//{
	//	_isPressed = false;
	//	//delete trash from scene
	//	GetGameObject()->GetScene()->DeleteGameObject(GetGameObject()->GetScene()->FindObjectByGUID(trash[0]->GUID));
	//	timer = 0;
	//}
	//if (trash.size() !=0)
	//{
	//	//q to let go of trash
	//	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_Q))
	//	{
	//		//put trash where the player is
	//		trash[0]->SetPostion(GetGameObject()->GetPosition());
	//		//put trash back in scene (making new object)
	//		GameObject::Sptr theTrash = GetGameObject()->GetScene()->CreateGameObject("Trash");
	//		theTrash = trash[0];
	//		//set trash to nullptr again since we aren't carrying it anymore
	//		trash.clear();

	//	}
	//}
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
