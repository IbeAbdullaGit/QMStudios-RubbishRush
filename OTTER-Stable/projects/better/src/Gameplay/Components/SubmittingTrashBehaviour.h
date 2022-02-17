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
#include "GUI/GuiText.h"

/// <summary>
/// Provides an example behaviour that uses some of the trigger interface to change the material
/// of the game object the component is attached to when entering or leaving a trigger
/// </summary>
//using namespace Gameplay;
class SubmittingTrashBehaviour : public Gameplay::IComponent {

public:
	typedef std::shared_ptr<SubmittingTrashBehaviour> Sptr;
	SubmittingTrashBehaviour();
	virtual ~SubmittingTrashBehaviour();

	// Inherited from IComponent

	virtual void OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void Awake() override;
	virtual void Update(float deltaTime) override;
	virtual void RenderImGui() override;
	virtual nlohmann::json ToJson() const override;
	static SubmittingTrashBehaviour::Sptr FromJson(const nlohmann::json& blob);
	MAKE_TYPENAME(SubmittingTrashBehaviour);

	std::vector<Gameplay::MeshResource::Sptr> open;
	std::vector<Gameplay::MeshResource::Sptr> closed;
	void getIdle(std::vector<Gameplay::MeshResource::Sptr> frames);

	int inventory;

protected:

	RenderComponent::Sptr _renderer;
	Gameplay::Scene* _scene;
	Gameplay::GameObject::Sptr ui;
	
	bool activated = false;
};