#include "Gameplay/Components/SubmittingTrashBehaviour.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Utils/GlmBulletConversions.h"
#include "Gameplay/Components/IComponent.h"
#include <GLFW/glfw3.h>
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/ImGuiHelper.h"


SubmittingTrashBehaviour::SubmittingTrashBehaviour() :
	IComponent()
{ }
SubmittingTrashBehaviour::~SubmittingTrashBehaviour() = default;

void SubmittingTrashBehaviour::OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>&body) {
	//press e to collect, and only collide with trash
	if (body->GetGameObject()->Name == "Trashy"/*glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_Q)*/) {
		////get our scene, delete this line later
		//_scene = GetGameObject()->GetScene();
		////do we have any trash
		//if (_scene->trash >= 1)
		//{
		//	_scene->trash -= 1;
		//	//update UI

		//	std::cout << "Submitted trash!\n";
		//}
		//else
		//{
		//	std::cout << "No trash to submit!\n";
		//}
		activated = true;

	}
	LOG_INFO("Entered trigger: {}", body->GetGameObject()->Name);
}

void SubmittingTrashBehaviour::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) {

	LOG_INFO("Left trigger: {}", body->GetGameObject()->Name);
	activated = false;
}
void SubmittingTrashBehaviour::Update(float deltatime)
{
	if (activated)
	{
		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_E))
		{
			//get our scene, delete this line later
			_scene = GetGameObject()->GetScene();
			//do we have any trash
			if (_scene->trash >= 1)
			{
				_scene->trash -= 1;
				//update UI

				std::cout << "Submitted trash!\n";
			}
			else
			{
				std::cout << "No trash to submit!\n";
			}

		}
	}
}

void SubmittingTrashBehaviour::Awake() {

}

void SubmittingTrashBehaviour::RenderImGui() { }

nlohmann::json SubmittingTrashBehaviour::ToJson() const {

	//return {
	//	{ "trash_collected", trash != nullptr ? trash->GUID.str() : "null" }
	//	//{ "exit_material", ExitMaterial != nullptr ? ExitMaterial->GetGUID().str() : "null" }
	//};
	return {};
}

SubmittingTrashBehaviour::Sptr SubmittingTrashBehaviour::FromJson(const nlohmann::json & blob) {
	SubmittingTrashBehaviour::Sptr result = std::make_shared<SubmittingTrashBehaviour>();
	////result->trash = ResourceManager::Get<GameObject::Sptr>(Guid(blob["trash_collected"]));
	//
	//result->trash = (GameObject::FromJson(Guid(blob["trash_collected"]), result->GetGameObject()->GetScene()));
	return result;
}
