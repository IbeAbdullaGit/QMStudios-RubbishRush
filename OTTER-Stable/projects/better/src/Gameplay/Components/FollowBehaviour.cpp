#include "Gameplay/Components/FollowBehaviour.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Utils/GlmBulletConversions.h"
#include "Gameplay/Components/IComponent.h"
#include <GLFW/glfw3.h>
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/ImGuiHelper.h"

//using namespace Gameplay::Physics;
FollowBehaviour::FollowBehaviour() :
	IComponent()
{ 
	
}
FollowBehaviour::~FollowBehaviour() = default;

void FollowBehaviour::OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) {
	
	//LOG_INFO("Entered trigger: {}", body->GetGameObject()->Name);
}

void FollowBehaviour::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) {
	
	//LOG_INFO("Left trigger: {}", body->GetGameObject()->Name);
	//activated = false;
}
void FollowBehaviour::Update(float deltatime)
{
	Arrival();
	_movementVector *= _desiredVelocity * deltatime;
	
	GetGameObject()->SetPostion(GetGameObject()->GetPosition() + _movementVector);
}

void FollowBehaviour::Arrival()
{
	if (target != nullptr)
	{
		glm::vec3 distance = (target->GetPosition() - GetGameObject()->GetPosition());
		glm::float1 len = sqrt(pow(distance.x, 2) + pow(distance.y, 2) + pow(distance.z, 2));
		distance = distance / len;
		glm::float1 len2 = sqrt(pow(distance.x, 2) + pow(distance.y, 2) + pow(distance.z, 2));
		_desiredVelocity = len2;
		if (len < slowingRadius)
		{
			_desiredVelocity *= _maxVelocity * (len / slowingRadius);
		}
		else
		{
			_desiredVelocity *= _maxVelocity;
		}
		_movementVector = distance;
	}
}

glm::vec3 FollowBehaviour::Catmull(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
{
	return 0.5f * (2.0f * p1 + t * (-p0 + p2)
		+ t * t * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3)
		+ t * t * t * (-p0 + 3.0f * p1 - 3.0f * p2 + p3));
}
void FollowBehaviour::Awake() {

		
}

void FollowBehaviour::RenderImGui() { }

nlohmann::json FollowBehaviour::ToJson() const {

	return {
		
	};
}

FollowBehaviour::Sptr FollowBehaviour::FromJson(const nlohmann::json & blob) {
	FollowBehaviour::Sptr result = std::make_shared<FollowBehaviour>();	
	////result->trash = ResourceManager::Get<GameObject::Sptr>(Guid(blob["trash_collected"]));
	//
	//result->trash = (GameObject::FromJson(Guid(blob["trash_collected"]), result->GetGameObject()->GetScene()));
	return result;
}



void FollowBehaviour::SetTarget(const Gameplay::GameObject::Sptr& object)
{
	target = object;
}

float FollowBehaviour::Evaluate(float x)
{
	float result = pow(2, x) - 1;
	return result;
}
