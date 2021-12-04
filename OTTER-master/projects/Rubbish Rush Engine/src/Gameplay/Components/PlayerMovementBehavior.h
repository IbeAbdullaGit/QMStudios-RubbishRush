#pragma once
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Physics/RigidBody.h"

/// <summary>
/// Component That Controls the First Player
/// </summary>

class PlayerMovementBehavior : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<PlayerMovementBehavior> Sptr;

	PlayerMovementBehavior();
	virtual ~PlayerMovementBehavior();

	virtual void Awake() override;
	virtual void Update(float deltaTime) override;

public:
	virtual void RenderImGui() override;
	MAKE_TYPENAME(PlayerMovementBehavior);
	virtual nlohmann::json ToJson() const override;
	static PlayerMovementBehavior::Sptr FromJson(const nlohmann::json& blob);

	void SetSpill(bool state);

	
protected:
	
	float _impulse;
	bool in_spill = false;
	Gameplay::Physics::RigidBody::Sptr _body;

	glm::quat currentRot = GetGameObject()->GetRotation();
	glm::vec3 rotationTarget = glm::vec3(90.0f, 0.0f, 90.0f);
	
	float timer = 0.0f;
	float lerpDuration = 1.0f;
	float t = 0.0f;
};
