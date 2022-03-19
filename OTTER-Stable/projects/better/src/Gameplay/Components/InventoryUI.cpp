#include "Gameplay/Components/InventoryUI.h"
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


InventoryUI::InventoryUI() :
	IComponent()
{ }
InventoryUI::~InventoryUI() = default;
void InventoryUI::Update(float deltatime)
{
	Application& app = Application::Get();
	switch (_scene->held)
	{
	case (0):
	{
		ui->SetTexture(tex0);
		break;
	}
	case (1):
	{
		if (_scene->held_recycle >= 1)
		{
			ui->SetTexture(rectex1);
		}
		else
			ui->SetTexture(tex1);
		break;
	}

	case (2):
	{
		if (_scene->held_recycle >= 1)
		{
			ui->SetTexture(rectex2);
		}
		else
			ui->SetTexture(tex2);
		break;
	}
	case (3):
	{
		if (_scene->held_recycle >= 1)
		{
			ui->SetTexture(rectex3);
		}
		else
			ui->SetTexture(tex3);
		break;
	}
	case (4):
	{
		if (_scene->held_recycle >= 1)
		{
			ui->SetTexture(rectex4);
		}
		else
			ui->SetTexture(tex4);
		break;
	}
	}
	
	
}

void InventoryUI::Awake() 
{
	_scene = GetGameObject()->GetScene();
	ui = _scene->FindObjectByName("Inventory UI")->Get<GuiPanel>();

	tex0 = ResourceManager::CreateAsset<Texture2D>("textures/ui/trashui0.png");
	tex1 = ResourceManager::CreateAsset<Texture2D>("textures/ui/trashui1.png");
	tex2 = ResourceManager::CreateAsset<Texture2D>("textures/ui/trashui2.png");
	tex3 = ResourceManager::CreateAsset<Texture2D>("textures/ui/trashui3.png");
	tex4 = ResourceManager::CreateAsset<Texture2D>("textures/ui/trashui4.png");

	rectex1 = ResourceManager::CreateAsset<Texture2D>("textures/ui/rectrashui1.png");
	rectex2= ResourceManager::CreateAsset<Texture2D>("textures/ui/rectrashui2.png");
	rectex3 = ResourceManager::CreateAsset<Texture2D>("textures/ui/rectrashui3.png");
	rectex4= ResourceManager::CreateAsset<Texture2D>("textures/ui/rectrashui4.png");

}

void InventoryUI::RenderImGui() { }

nlohmann::json InventoryUI::ToJson() const {
	
	//return {
	//	{ "trash_collected", trash != nullptr ? trash->GUID.str() : "null" }
	//	//{ "exit_material", ExitMaterial != nullptr ? ExitMaterial->GetGUID().str() : "null" }
	//};
	return {};
}

InventoryUI::Sptr InventoryUI::FromJson(const nlohmann::json & blob) {
	InventoryUI::Sptr result = std::make_shared<InventoryUI>();	
	////result->trash = ResourceManager::Get<GameObject::Sptr>(Guid(blob["trash_collected"]));
	//
	//result->trash = (GameObject::FromJson(Guid(blob["trash_collected"]), result->GetGameObject()->GetScene()));
	return result;
}
