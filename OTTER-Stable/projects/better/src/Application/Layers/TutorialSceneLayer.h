#pragma once
#include "Application/ApplicationLayer.h"
#include "json.hpp"
#include <GLM/glm.hpp>
#include "Utils/Macros.h"
#include "Gameplay/Scene.h"
#include "Gameplay/MeshResource.h"

/**
 * This example layer handles creating a default test scene, which we will use
 * as an entry point for creating a sample scene
 */
class TutorialSceneLayer final : public ApplicationLayer {
public:
	MAKE_PTRS(TutorialSceneLayer)

		TutorialSceneLayer();
	virtual ~TutorialSceneLayer();

	// Inherited from ApplicationLayer

	void OnSceneUnload() override;
	virtual void OnAppLoad(const nlohmann::json& config) override;
	void OnUpdate() override;

protected:
	void _CreateScene();
	Gameplay::Scene::Sptr _tutcurrentScene;
	Gameplay::MeshResource::Sptr trashMesh;
	Gameplay::Material::Sptr trashMaterial;

	Gameplay::GameObject::Sptr trashyM;

	Gameplay::GameObject::Sptr dumpUI;
	Gameplay::GameObject::Sptr jumpUI;
	Gameplay::GameObject::Sptr walkUI;
	Gameplay::GameObject::Sptr pickupUI;
	
	bool hasMoved = false;
	bool hasCollected = false;

	bool activated = false;
	bool done = false;
	bool nobreak = false;


	bool doUpdate = true;

};