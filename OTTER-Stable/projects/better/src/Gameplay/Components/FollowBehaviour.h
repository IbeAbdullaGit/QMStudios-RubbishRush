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

/// <summary>
/// Provides an example behaviour that uses some of the trigger interface to change the material
/// of the game object the component is attached to when entering or leaving a trigger
/// </summary>
//using namespace Gameplay;
class FollowBehaviour : public Gameplay::IComponent {

public:
	typedef std::shared_ptr<FollowBehaviour> Sptr;
	FollowBehaviour();
	virtual ~FollowBehaviour();
	
	// Inherited from IComponent

	virtual void OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void Awake() override;
	virtual void Update(float deltaTime) override;
	virtual void RenderImGui() override;
	virtual nlohmann::json ToJson() const override;
	static FollowBehaviour::Sptr FromJson(const nlohmann::json& blob);
	MAKE_TYPENAME(FollowBehaviour);
	void SetTarget(const Gameplay::GameObject::Sptr& object);
	float Evaluate(float x);

	void Arrival();
	glm::vec3 FollowBehaviour::Catmull(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t);

protected:

	RenderComponent::Sptr _renderer;
	Gameplay::Scene* _scene;
	Gameplay::GameObject::Sptr target;
	glm::vec3 _movementVector = glm::vec3(0.0f, 0.0f, 0.0f);
	float _maxVelocity = 10.0f;
	float slowingRadius = 5.0f;
	float _desiredVelocity = 0.0f;
	
};