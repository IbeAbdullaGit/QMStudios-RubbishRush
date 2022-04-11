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
#include "AudioEngine.h"


CollectTrashBehaviour::CollectTrashBehaviour() :
	IComponent()
{ }
CollectTrashBehaviour::~CollectTrashBehaviour() = default;

void CollectTrashBehaviour::OnEnteredTrigger(const std::shared_ptr<Gameplay::Physics::TriggerVolume>& trigger) {
	//press e to collect, and only collide with trash
	if (trigger->GetGameObject()->Name == "Trash" || trigger->GetGameObject()->Name == "Recycling" /* && glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_E)*/) {
		
		////get our scene, delete this line later
		//_scene = GetGameObject()->GetScene();
		////delete trash from scene
		//_scene->DeleteGameObject(_scene->FindObjectByGUID(GetGameObject()->GUID));
		////increment trash count?
		//_scene->trash += 1;
		//std::cout << "Current trash collected: " << _scene->trash << std::endl;
		activated = true;
		ui->Get<GuiText>()->IsEnabled = true;
		if (trigger->GetGameObject()->Name == "Trash")
		{
			type = "Normal";
		}
		else
		{
			type = "Recycling";
		}
		to_be_deleted = trigger->GetGameObject()->GetGUID();
		
		
	}
	LOG_INFO("Entered trigger: {}", trigger->GetGameObject()->Name);
}

void CollectTrashBehaviour::OnLeavingTrigger(const std::shared_ptr<Gameplay::Physics::TriggerVolume>& trigger) {
	
	//if (trigger->GetGameObject()->Name == "Trash" || trigger->GetGameObject()->Name == "Recycling")
	{
		activated = false;
		ui->Get<GuiText>()->IsEnabled = false;
		to_be_deleted.Clear();
	}
	LOG_INFO("Left trigger: {}", trigger->GetGameObject()->Name);
}
void CollectTrashBehaviour::Update(float deltaTime)
{
	Application& app = Application::Get();
	//auto& scene = app.CurrentScene();
	//ui = scene->FindObjectByName("Pickup Feedback");

	if (activated && to_be_deleted.isValid())
	{
		if (InputEngine::GetKeyState(GLFW_KEY_E) == ButtonState::Pressed)
		{
			if (_scene->held < inventory)
			{
				full = false;
				//differentiate between type of trash
				if (type == "Normal")
				{
					_scene->held_normal += 1;
					AudioEngine::playEventS("event:/Sounds/SoundEffects/Pickups interactions/PickUpTrash");
					AudioEngine::EventVolumeChange("event:/Sounds/SoundEffects/Pickups interactions/PickUpTrash", 2.f);
					AudioEngine::EventPosChangeS("event:/Sounds/SoundEffects/Pickups interactions/PickUpTrash", GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y, GetGameObject()->GetPosition().z);
				}
				else if (_scene->held_recycle < 1) //we are type recycle, now we need to check how many recycle we're holding
				{
					_scene->held_recycle += 1;
					AudioEngine::playEventS("event:/Sounds/SoundEffects/Pickups interactions/PickUpCup");
					AudioEngine::EventVolumeChange("event:/Sounds/SoundEffects/Pickups interactions/PickUpCup",2.f);
					AudioEngine::EventPosChangeS("event:/Sounds/SoundEffects/Pickups interactions/PickUpCup", GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y, GetGameObject()->GetPosition().z);
				}
				/*else if (_scene->held_recycle >= 1) {
					AudioEngine::playEventS("event:/Sounds/SoundEffects/Pickups interactions/TrashPickupStopped");
					AudioEngine::EventPosChangeS("event:/Sounds/SoundEffects/Pickups interactions/TrashPickupStopped", GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y, GetGameObject()->GetPosition().z);
				}*/
				else //we are recycle but recycle inv is full
				{
					std::cout << "We're already holding a recycle item\n";
					AudioEngine::playEventS("event:/Sounds/SoundEffects/Pickups interactions/TrashPickupStopped");
					AudioEngine::EventVolumeChange("event:/Sounds/SoundEffects/Pickups interactions/TrashPickupStopped", -1.f);
					AudioEngine::EventPosChangeS("event:/Sounds/SoundEffects/Pickups interactions/TrashPickupStopped", GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y, GetGameObject()->GetPosition().z);
					return;
				}
				//delete trash from scene
				Gameplay::GameObject::Sptr trash = _scene->FindObjectByGUID(to_be_deleted);
				if (!tutorial)
				{
					auto& all_trash = app.GetLayer<DefaultSceneLayer>()->all_trash;
					auto& it = std::find(all_trash.begin(), all_trash.end(), trash);
					
					{
						app.GetLayer<DefaultSceneLayer>()->all_trash.erase(it);
					}
					
				}
				_scene->RemoveGameObject(trash);
				to_be_deleted.Clear();
				//total held
				_scene->held += 1;
				std::cout << "Current trash collected: " << _scene->held << std::endl;

				activated = false;
				ui->Get<GuiText>()->IsEnabled = false;

			}
			if (_scene->held == inventory && full){
				AudioEngine::playEventS("event:/Sounds/SoundEffects/Pickups interactions/TrashPickupStopped");
				AudioEngine::EventVolumeChange("event:/Sounds/SoundEffects/Pickups interactions/TrashPickupStopped", -1.f);
				AudioEngine::EventPosChangeS("event:/Sounds/SoundEffects/Pickups interactions/TrashPickupStopped", GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y, GetGameObject()->GetPosition().z);
			}
		}

		if (_scene->held == inventory && !full) {
			AudioEngine::playEventS("event:/Sounds/SoundEffects/Pickups interactions/TrashyFull");
			AudioEngine::EventPosChangeS("event:/Sounds/SoundEffects/Pickups interactions/TrashyFull", GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y, GetGameObject()->GetPosition().z);
			full = true;
		}

		if (InputEngine::GetKeyState(GLFW_KEY_E) == ButtonState::Pressed && full)
		{
			std::cout << "We're already holding a recycle item\n";
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
