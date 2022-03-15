#pragma once
#include "Application/ApplicationLayer.h"
#include "json.hpp"
#include <GLM/glm.hpp>
#include "Utils/Macros.h"
#include "Gameplay/Scene.h"
#include "Gameplay/MeshResource.h"
#include "ToneFire.h"

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
	void _CreateHallway();
	Gameplay::Scene::Sptr _tutcurrentScene;
	Gameplay::MeshResource::Sptr trashMesh;
	Gameplay::MeshResource::Sptr benchMesh;
	Gameplay::MeshResource::Sptr spillMesh;
	Gameplay::Material::Sptr trashMaterial;
	Gameplay::Material::Sptr hallwayMat;
	Gameplay::Material::Sptr benchMaterial;
	Gameplay::Material::Sptr spillMaterial;

	Gameplay::GameObject::Sptr trashyM;

	Gameplay::GameObject::Sptr dumpUI;
	Gameplay::GameObject::Sptr jumpUI;
	Gameplay::GameObject::Sptr walkUI;
	Gameplay::GameObject::Sptr pickupUI;
	
	ToneFire::FMODStudio studio;
	ToneFire::StudioSound test;
	bool hasMoved = false;
	bool hasCollected = false;
	bool hasJumped = false;
	bool musicstart = false;
	bool activated = false;
	bool done = false;
	bool nobreak = false;


	bool doUpdate = true;

	int inventory = 1; //DEFAULT INVENTORY COUNT
	int max_trash = 2; //DEFAULT FOR NOW

};