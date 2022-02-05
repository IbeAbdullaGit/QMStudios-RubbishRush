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
		GetGameObject()->Get<MorphAnimator>()->SetFrames(open);
		ui->Get<GuiText>()->IsEnabled = true;

	}
	LOG_INFO("Entered trigger: {}", body->GetGameObject()->Name);
}

void SubmittingTrashBehaviour::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) {

	LOG_INFO("Left trigger: {}", body->GetGameObject()->Name);
	activated = false;
	GetGameObject()->Get<MorphAnimator>()->SetFrames(closed);
	ui->Get<GuiText>()->IsEnabled = false;
}
void SubmittingTrashBehaviour::Update(float deltatime)
{
	Application& app = Application::Get();
	if (activated)
	{
		
		if (glfwGetKey(app.GetWindow(), GLFW_KEY_E))
		{
			
			//get our scene, delete this line later
			//_scene = GetGameObject()->GetScene();
			//do we have any trash
			if (_scene->trash >= 1 && _scene->held >=1)
			{
				_scene->trash -= 1;

				_scene->held -= 1;
				//update UI

				std::cout << "Submitted trash!\n";
				//increase score
				_scene->score += 1;

				//disable the return ui if we have more inventory space
				if (_scene->held < 2)
				{
					Gameplay::GameObject::Sptr returnUI = _scene->FindObjectByName("Return Feedback");
					returnUI->Get<GuiText>()->IsEnabled = false;

				}
			}
			else
			{
				std::cout << "No trash to submit!\n";
			}
			ui->Get<GuiText>()->IsEnabled = false;
		}

		
		
	}
	
}

void SubmittingTrashBehaviour::Awake() 
{
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
