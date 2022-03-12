#pragma once
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Components/ParticleSystem.h"
#include "Gameplay/GameObject.h"

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
	bool is_moving = false;

	
protected:
	
	float _impulse;
	bool in_spill = false;
	
	Gameplay::Physics::RigidBody::Sptr _body;
	
	std::vector<Gameplay::GameObject::Sptr> particles_store;
	bool created = false;

	glm::quat currentRotation;
	glm::quat targetRotation;
	float speed = 5.0f;

	glm::vec3 direction;
	int count = 0;

};
