#include "Gameplay/Components/CollectTrashBehaviour.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Utils/GlmBulletConversions.h"
#include "Gameplay/Components/IComponent.h"
#include <GLFW/glfw3.h>
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/ImGuiHelper.h"
#include "GUI/GuiText.h"
#include "Application/Application.h"
#include "Application/Layers/DefaultSceneLayer.h"
#include "Gameplay/InputEngine.h"


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
		ui->Get<GuiText>()->IsEnabled = true;
		
	}
	LOG_INFO("Entered trigger: {}", body->GetGameObject()->Name);
}

void CollectTrashBehaviour::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) {
	
	LOG_INFO("Left trigger: {}", body->GetGameObject()->Name);
	activated = false;
	ui->Get<GuiText>()->IsEnabled = false;
}
void CollectTrashBehaviour::Update(float deltatime)
{
	Application& app = Application::Get();
	//auto& scene = app.CurrentScene();
	//ui = scene->FindObjectByName("Pickup Feedback");

	if (activated && GetGameObject() !=nullptr)
	{
		if (InputEngine::GetKeyState(GLFW_KEY_E) == ButtonState::Pressed)
		{
			if (_scene->held < inventory)
			{
				
				//differentiate between type of trash
				if (type == "Normal")
				{
					_scene->held_normal += 1;
					_scene->playtrashsound = true;
				}
				else if (_scene->held_recycle < 1) //we are type recycle, now we need to check how many recycle we're holding
				{
					_scene->held_recycle += 1;
					_scene->playrecyclesound = true;
				}
				else //we are recycle but recycle inv is full
				{
					std::cout << "We're already holding a recycle item\n";
					return;
				}
				//delete trash from scene
				Gameplay::GameObject::Sptr trash = _scene->FindObjectByName(GetGameObject()->Name);
				if (!tutorial)
				{
					auto& all_trash = app.GetLayer<DefaultSceneLayer>()->all_trash;
					auto& it = std::find(all_trash.begin(), all_trash.end(), trash);
					
					{
						app.GetLayer<DefaultSceneLayer>()->all_trash.erase(it);
					}
					
				}
				_scene->RemoveGameObject(trash);
				//total held
				_scene->held += 1;
				std::cout << "Current trash collected: " << _scene->held << std::endl;

				activated = false;
				ui->Get<GuiText>()->IsEnabled = false;

			}
			
		}
		/*Gameplay::IComponent::Sptr ptr = Panel.lock();
		if (ptr != nullptr)
		{
			ptr->IsEnabled = !ptr->IsEnabled;
		}*/
	}
}

void CollectTrashBehaviour::Awake() 
{
	_scene = GetGameObject()->GetScene();
	ui = _scene->FindObjectByName("Pickup Feedback");
	inventory = 4; //DEFAULT SIZE
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
