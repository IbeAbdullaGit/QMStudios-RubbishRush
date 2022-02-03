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

#include <memory>

/// <summary>
/// Provides an example behaviour that uses some of the trigger interface to change the material
/// of the game object the component is attached to when entering or leaving a trigger
/// </summary>
//using namespace Gameplay;
class MorphMeshRenderer : public Gameplay::IComponent {

public:
	enum class Attrib
	{
		POSITION_0 = 0,
		POSITION_1 = 1,
		NORMAL_0 = 2,
		NORMAL_1 = 3,
		UV = 4
	};
	typedef std::shared_ptr<MorphMeshRenderer> Sptr;
	MorphMeshRenderer();

	void SetMorphMeshRenderer(Gameplay::MeshResource::Sptr baseMesh,
		Gameplay::Material::Sptr mat);
	virtual ~MorphMeshRenderer();

	//MorphMeshRenderer(MorphMeshRenderer&&) = default;
	//MorphMeshRenderer& operator=(MorphMeshRenderer&&) = default;

	void UpdateData(Gameplay::MeshResource::Sptr frame0, Gameplay::MeshResource::Sptr frame1, float t);
	virtual void Draw();
	virtual void Update(float deltaTime) override;
	virtual void OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void Awake() override;
	virtual void RenderImGui() override;
	virtual nlohmann::json ToJson() const override;
	static MorphMeshRenderer::Sptr FromJson(const nlohmann::json& blob);
	MAKE_TYPENAME(MorphMeshRenderer);

protected:

	//Pointers to the frames we're currently in between.
	Gameplay::MeshResource::Sptr m_frame0;
	Gameplay::MeshResource::Sptr m_frame1;
	//The t-value for interpolating between our frames.
	float m_t;

	Gameplay::Material::Sptr m_mat;
	VertexArrayObject::Sptr m_vao;

};