#include "Gameplay/Components/SteeringBehaviour.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Utils/GlmBulletConversions.h"
#include "Gameplay/Components/IComponent.h"
#include <GLFW/glfw3.h>
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/ImGuiHelper.h"

//using namespace Gameplay::Physics;
SteeringBehaviour::SteeringBehaviour() :
	IComponent()
{ 
	_curve.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	_curve.push_back(glm::vec3(0.15f, 0.1f, 0.0f));
	_curve.push_back(glm::vec3(0.3f, 0.15f, 0.0f));
	_curve.push_back(glm::vec3(0.4f, 0.2f, 0.0f));
	_curve.push_back(glm::vec3(0.75f, 0.5f, 0.0f));
	_curve.push_back(glm::vec3(0.9f, 0.8f, 0.0f));
	_curve.push_back(glm::vec3(0.95f, 0.9f, 0.0f));
	_curve.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
}
SteeringBehaviour::~SteeringBehaviour() = default;

void SteeringBehaviour::OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) {
	
	//LOG_INFO("Entered trigger: {}", body->GetGameObject()->Name);
}

void SteeringBehaviour::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) {
	
	//LOG_INFO("Left trigger: {}", body->GetGameObject()->Name);
	//activated = false;
}
void SteeringBehaviour::Update(float deltatime)
{
	//catmull-rom spline
	_segmentTimer += deltatime*3.0f;
	//std::cout << _segmentTimer << std::endl;
	
	if (_segmentTimer > _segmentTravelTime)
	{
		_segmentTimer = 0.0f;
		_segmentIndex += 1;

		if (_segmentIndex >= points.size())
			_segmentIndex = 0;
	}
	float t = _segmentTimer / _segmentTravelTime;
	//std::cout << t << std::endl;
	if (points.size() < 4)
	{
		return;
	}
	glm::vec3 p0, p1, p2, p3;
	int p0_index, p1_index, p2_index, p3_index;

	p1_index = _segmentIndex;
	p0_index = (p1_index == 0) ? points.size() - 1 : p1_index - 1;
	p2_index = (p1_index + 1) % points.size();
	p3_index = (p2_index + 1) % points.size();

	p0 = points[p0_index];
	p1 = points[p1_index];
	p2 = points[p2_index];
	p3 = points[p3_index];

	//setting the position, might need to change
	GetGameObject()->SetPostion(Catmull(p0, p1, p2, p3, Evaluate(t)));
	//_body->ApplyImpulse(Catmull(p0, p1, p2, p3, Evaluate(t)));
	//_body->SetLinearVelocity(Catmull(p0, p1, p2, p3, Evaluate(t)));
	//GetGameObject()->SetPostion(Catmull(p0, p1, p2, p3, t));
}

glm::vec3 SteeringBehaviour::Catmull(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
{
	return 0.5f * (2.0f * p1 + t * (-p0 + p2)
		+ t * t * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3)
		+ t * t * t * (-p0 + 3.0f * p1 - 3.0f * p2 + p3));
}
void SteeringBehaviour::Awake() {

	_body = GetComponent<Gameplay::Physics::RigidBody>();
	
}

void SteeringBehaviour::RenderImGui() { }

nlohmann::json SteeringBehaviour::ToJson() const {

	return {
		
	};
}

SteeringBehaviour::Sptr SteeringBehaviour::FromJson(const nlohmann::json & blob) {
	SteeringBehaviour::Sptr result = std::make_shared<SteeringBehaviour>();	
	////result->trash = ResourceManager::Get<GameObject::Sptr>(Guid(blob["trash_collected"]));
	//
	//result->trash = (GameObject::FromJson(Guid(blob["trash_collected"]), result->GetGameObject()->GetScene()));
	return result;
}

void SteeringBehaviour::SetPoints(std::vector<glm::vec3> ps)
{
	for (int i = 0; i < ps.size(); i++)
	{
		points.push_back(ps[i]);
	}
}

float SteeringBehaviour::Evaluate(float x)
{
	float result = pow(2, x) - 1;
	return result;
}
