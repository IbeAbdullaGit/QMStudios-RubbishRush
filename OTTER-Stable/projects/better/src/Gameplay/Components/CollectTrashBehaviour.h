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

/// <summary>
/// Provides an example behaviour that uses some of the trigger interface to change the material
/// of the game object the component is attached to when entering or leaving a trigger
/// </summary>
//using namespace Gameplay;
class CollectTrashBehaviour : public Gameplay::IComponent {

public:
	typedef std::shared_ptr<CollectTrashBehaviour> Sptr;
	CollectTrashBehaviour();
	virtual ~CollectTrashBehaviour();
	
	//Gameplay::IComponent::Wptr Panel;
	// Inherited from IComponent

	//virtual void OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void OnEnteredTrigger(const std::shared_ptr<Gameplay::Physics::TriggerVolume>& trigger) override;
	virtual void OnLeavingTrigger(const std::shared_ptr<Gameplay::Physics::TriggerVolume>& trigger) override;
	//virtual void OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void Awake() override;
	virtual void Update(float deltaTime) override;

	//void CheckTrash();

	virtual void RenderImGui() override;
	virtual nlohmann::json ToJson() const override;
	static CollectTrashBehaviour::Sptr FromJson(const nlohmann::json& blob);
	MAKE_TYPENAME(CollectTrashBehaviour);

	int inventory;
	
	
	std::string type = "Normal";
	bool tutorial = false;
	bool activated = false;
	bool full = false;

protected:

	Guid to_be_deleted;

	RenderComponent::Sptr _renderer;
	Gameplay::Scene* _scene;
	Gameplay::GameObject::Sptr ui;
	
	ToneFire::FMODStudio studio;
	ToneFire::StudioSound test;

};