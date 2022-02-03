#pragma once
#include "IComponent.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Gameplay/Physics/PhysicsBase.h"
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Scene.h"
#include "Gameplay/Components/IComponent.h"


/// <summary>
/// Provides an example behaviour that uses some of the trigger interface to change the material
/// of the game object the component is attached to when entering or leaving a trigger
/// </summary>
//using namespace Gameplay;
//using namespace Physics;
class DeleteObjectBehaviour : public Gameplay::IComponent {

public:
	typedef std::shared_ptr<DeleteObjectBehaviour> Sptr;
	DeleteObjectBehaviour();
	virtual ~DeleteObjectBehaviour();

	
	void setEnter(Gameplay::Material::Sptr v);
	void setExit(Gameplay::Material::Sptr  v);

	virtual void OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void RenderImGui() override;
	virtual nlohmann::json ToJson() const override;
	static DeleteObjectBehaviour::Sptr FromJson(const nlohmann::json& blob);
	MAKE_TYPENAME(DeleteObjectBehaviour);

protected:
	bool _playerInTrigger;
	Gameplay::Scene* _scene;
	bool canBreak = true;

	// Inherited from IComponent
	Gameplay::Material::Sptr        EnterMaterial;
	Gameplay::Material::Sptr        ExitMaterial;
};