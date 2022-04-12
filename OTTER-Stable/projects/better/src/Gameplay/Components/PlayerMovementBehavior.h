#pragma once
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Components/ParticleSystem.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"

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
	bool is_running = false;

protected:

	float _impulse;
	bool in_spill = false;

	Gameplay::Physics::RigidBody::Sptr _body;

	std::vector<Gameplay::GameObject::Sptr> particles_store;
	bool created = false;

	glm::quat currentRotation;
	glm::quat targetRotation;
	float turnspeed = 7.0f;
	Gameplay::Scene* _scene;
	glm::vec3 direction;
	int count = 0;

	glm::vec3 movementVelocity;

	float max_speed = 0.35f;
	float acceleration = 0.02f;

	/*const float directions[4] = { 0.0f, 90.0f, 180.0f, 270.0f };
	float angle;
	float targetAngle = 0.0f;
	bool input = false;*/

};
