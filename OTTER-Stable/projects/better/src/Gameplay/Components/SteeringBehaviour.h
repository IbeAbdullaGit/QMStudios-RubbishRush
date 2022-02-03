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
class SteeringBehaviour : public Gameplay::IComponent {

public:
	typedef std::shared_ptr<SteeringBehaviour> Sptr;
	SteeringBehaviour();
	virtual ~SteeringBehaviour();
	
	// Inherited from IComponent

	virtual void OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void Awake() override;
	virtual void Update(float deltaTime) override;
	virtual void RenderImGui() override;
	virtual nlohmann::json ToJson() const override;
	static SteeringBehaviour::Sptr FromJson(const nlohmann::json& blob);
	MAKE_TYPENAME(SteeringBehaviour);

	void SetPoints(std::vector<glm::vec3> ps);
	float Evaluate(float x);
	glm::vec3 SteeringBehaviour::Catmull(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t);

protected:

	RenderComponent::Sptr _renderer;
	Gameplay::Scene* _scene;
	bool activated = false;
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> _curve;
	float _segmentTimer =0;
	float _segmentTravelTime = 5.0f;
	int _segmentIndex = 0;

	Gameplay::Physics::RigidBody::Sptr _body;
};