#pragma once
#include "IComponent.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Utils/GlmBulletConversions.h"
#include "Gameplay/Components/IComponent.h"
#include <GLFW/glfw3.h>
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/ImGuiHelper.h"
#include "ToneFire.h"
#include "GUI/GuiPanel.h"

/// <summary>
/// Provides an example behaviour that uses some of the trigger interface to change the material
/// of the game object the component is attached to when entering or leaving a trigger
/// </summary>
//using namespace Gameplay;
class InventoryUI : public Gameplay::IComponent {

public:
	typedef std::shared_ptr<InventoryUI> Sptr;
	InventoryUI();
	virtual ~InventoryUI();
	
	//Gameplay::IComponent::Wptr Panel;
	// Inherited from IComponent

	virtual void Awake() override;
	virtual void Update(float deltaTime) override;
	virtual void RenderImGui() override;
	virtual nlohmann::json ToJson() const override;
	static InventoryUI::Sptr FromJson(const nlohmann::json& blob);
	MAKE_TYPENAME(InventoryUI);

	int inventory;

	std::string type = "Normal";

protected:

	RenderComponent::Sptr _renderer;
	Gameplay::Scene* _scene;
	GuiPanel::Sptr ui;
	bool activated = false;
	ToneFire::FMODStudio studio;
	ToneFire::StudioSound test;

	Texture2D::Sptr tex0;
	Texture2D::Sptr tex1;
	Texture2D::Sptr tex2;
	Texture2D::Sptr tex3;
	Texture2D::Sptr tex4;

	Texture2D::Sptr rectex1;
	Texture2D::Sptr rectex2;
	Texture2D::Sptr rectex3;
	Texture2D::Sptr rectex4;
};