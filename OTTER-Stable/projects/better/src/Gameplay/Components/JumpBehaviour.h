#pragma once
#include "IComponent.h"
#include "Gameplay/Physics/RigidBody.h"
#include "Application/Timing.h"

/// <summary>
/// A simple behaviour that applies an impulse along the Z axis to the 
/// rigidbody of the parent when the space key is pressed
/// </summary>
class JumpBehaviour : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<JumpBehaviour> Sptr;

	std::weak_ptr<Gameplay::IComponent> Panel;

	JumpBehaviour();
	virtual ~JumpBehaviour();
	virtual void OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void Awake() override;
	virtual void Update(float deltaTime) override;

	virtual void RenderImGui() override;
	MAKE_TYPENAME(JumpBehaviour);
	virtual nlohmann::json ToJson() const override;
	static JumpBehaviour::Sptr FromJson(const nlohmann::json& blob);

	bool in_air = false;

protected:
	float _impulse;
	bool activated = false;
	bool _isPressed = false;
	Gameplay::Physics::RigidBody::Sptr _body;
	glm::vec3 movementVelocity;
	float jumpResetTimer = Timing::Current().TimeSinceAppLoad();
	
};