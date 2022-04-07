#pragma once
#include "IComponent.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Gameplay/Physics/PhysicsBase.h"
#include "Gameplay/Physics/RigidBody.h"


/// <summary>
/// Provides an example behaviour that uses some of the trigger interface to change the material
/// of the game object the component is attached to when entering or leaving a trigger
/// </summary>
class ConveyorBeltBehaviour : public Gameplay::IComponent {

public:
	typedef std::shared_ptr<ConveyorBeltBehaviour> Sptr;
	ConveyorBeltBehaviour();
	virtual ~ConveyorBeltBehaviour();

	// Inherited from IComponent

	virtual void Awake() override;
	virtual void OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void Update(float deltaTime) override;
	virtual void RenderImGui() override;
	virtual nlohmann::json ToJson() const override;
	static ConveyorBeltBehaviour::Sptr FromJson(const nlohmann::json& blob);
	MAKE_TYPENAME(ConveyorBeltBehaviour);

protected:
	bool _playerInTrigger = false;
	float currentScroll = 1.0f;
	//adjust these how we want
	float speed = 0.5f;
	float visualSpeedScalar = 1.0f; 
	glm::vec3 direction;
	//default is our body
	std::shared_ptr<Gameplay::Physics::RigidBody> body2 = nullptr;
};