#include "Gameplay/Components/SubmittingTrashBehaviour.h"
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
#include "Gameplay/InputEngine.h"
#include "AudioEngine.h"


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
		if (GetGameObject()->Has<MorphAnimator>())
		{
			GetGameObject()->Get<MorphAnimator>()->SetFrames(open);
		}
		ui->Get<GuiText>()->IsEnabled = true;

	}
	LOG_INFO("Entered trigger: {}", body->GetGameObject()->Name);
}

void SubmittingTrashBehaviour::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) {

	LOG_INFO("Left trigger: {}", body->GetGameObject()->Name);
	activated = false;
	if (GetGameObject()->Has<MorphAnimator>())
	{
		GetGameObject()->Get<MorphAnimator>()->SetFrames(closed);
	}
	ui->Get<GuiText>()->IsEnabled = false;
}
void SubmittingTrashBehaviour::OnLeavingTrigger(const std::shared_ptr<Gameplay::Physics::TriggerVolume>& trigger)
{
}
void SubmittingTrashBehaviour::Update(float deltatime)
{
	Application& app = Application::Get();
	if (activated)
	{
		press_once = false;
		if (InputEngine::GetKeyState(GLFW_KEY_E) == ButtonState::Pressed && !press_once)
		{
			press_once = true;
			//get our scene, delete this line later
			//_scene = GetGameObject()->GetScene();
			//do we have any trash
			if (_scene->held >=1)
			{
				//if we're a normal trash bin, should have normal trash
				if (type == "Normal" && _scene->held_normal >= 1)
				{
					_scene->held_normal -= 1;
				}
				else if (type == "Recycle" && _scene->held_recycle >= 1)
				{
					_scene->held_recycle -= 1;
				}
				else //no match
				{
					AudioEngine::playEventS("event:/Sounds/SoundEffects/Pickups interactions/TrashPickupStopped");
					
					AudioEngine::EventPosChangeS("event:/Sounds/SoundEffects/Pickups interactions/TrashPickupStopped", GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y, GetGameObject()->GetPosition().z);
					return;
				}
				_scene->held -= 1;
				//update UI

				std::cout << "Submitted trash!\n";
				//increase score
				_scene->score += 1;
				AudioEngine::playEventS("event:/Sounds/SoundEffects/Pickups interactions/DepositTrash");
				AudioEngine::EventPosChangeS("event:/Sounds/SoundEffects/Pickups interactions/DepositTrash", GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y, GetGameObject()->GetPosition().z);
				AudioEngine::EventVolumeChange("event:/Sounds/SoundEffects/Pickups interactions/DepositTrash", -3.f);
				//disable the return ui if we have more inventory space
				if (_scene->held < inventory)
				{
					Gameplay::GameObject::Sptr returnUI = _scene->FindObjectByName("Return Feedback");
					returnUI->Get<GuiText>()->IsEnabled = false;

				}
			}
			else
			{
				std::cout << "No trash to submit!\n";
				AudioEngine::playEventS("event:/Sounds/SoundEffects/Pickups interactions/TrashPickupStopped");
				AudioEngine::EventPosChangeS("event:/Sounds/SoundEffects/Pickups interactions/TrashPickupStopped", GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y, GetGameObject()->GetPosition().z);
			}
			ui->Get<GuiText>()->IsEnabled = false;
		}

	}
	
}

void SubmittingTrashBehaviour::Awake() 
{
	inventory = 4; //DEFAULT SIZE
	_scene = GetGameObject()->GetScene();
	ui = _scene->FindObjectByName("Submit Feedback");

	//ANIMATION STUFF////
	
	//open frames
	Gameplay::MeshResource::Sptr binMesh2 = ResourceManager::CreateAsset<Gameplay::MeshResource>("Open/BigBenOpen_000001.obj");
	Gameplay::MeshResource::Sptr binMesh3 = ResourceManager::CreateAsset<Gameplay::MeshResource>("Open/BigBenOpen_000005.obj");
	Gameplay::MeshResource::Sptr binMesh4 = ResourceManager::CreateAsset<Gameplay::MeshResource>("Open/BigBenOpen_000010.obj");
	Gameplay::MeshResource::Sptr binMesh5 = ResourceManager::CreateAsset<Gameplay::MeshResource>("Open/BigBenOpen_000020.obj");
	open.push_back(binMesh2);
	open.push_back(binMesh3);
	open.push_back(binMesh4);
	open.push_back(binMesh5);
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

void SubmittingTrashBehaviour::getIdle(std::vector<Gameplay::MeshResource::Sptr> frames)
{
	for (int i = 0; i < frames.size(); i++)
	{
		closed.push_back(frames[i]);
	}
}
