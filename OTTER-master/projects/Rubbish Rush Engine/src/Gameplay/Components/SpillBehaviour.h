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
class SpillBehaviour : public Gameplay::IComponent {

public:
	typedef std::shared_ptr<SpillBehaviour> Sptr;
	SpillBehaviour();
	virtual ~SpillBehaviour();

	// Inherited from IComponent

	virtual void OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void RenderImGui() override;
	virtual nlohmann::json ToJson() const override;
	static SpillBehaviour::Sptr FromJson(const nlohmann::json& blob);
	MAKE_TYPENAME(SpillBehaviour);

protected:
	bool _playerInTrigger;
	float speed = 0.25f;
};