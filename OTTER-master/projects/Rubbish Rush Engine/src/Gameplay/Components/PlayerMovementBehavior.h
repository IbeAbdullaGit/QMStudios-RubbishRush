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

protected:
	float _impulse;

	Gameplay::Physics::RigidBody::Sptr _body;
};
