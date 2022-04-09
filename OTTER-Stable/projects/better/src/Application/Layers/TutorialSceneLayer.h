#pragma once
#include "Application/ApplicationLayer.h"
#include "json.hpp"
#include <GLM/glm.hpp>
#include "Utils/Macros.h"
#include "Gameplay/Scene.h"
#include "Gameplay/MeshResource.h"
#include "Gameplay/Material.h"
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
	void _UpdateUIPositions();
	Gameplay::Scene::Sptr _tutcurrentScene;
	Gameplay::MeshResource::Sptr trashMesh;
	Gameplay::MeshResource::Sptr benchMesh;
	Gameplay::MeshResource::Sptr spillMesh;
	Gameplay::MeshResource::Sptr bagtrashMesh;
	Gameplay::Material::Sptr trashMaterial;
	Gameplay::Material::Sptr bagtrashMaterial;
	Gameplay::Material::Sptr hallwayMat;
	Gameplay::Material::Sptr benchMaterial;
	Gameplay::Material::Sptr spillMaterial;

	Gameplay::GameObject::Sptr trashyM;

	Gameplay::GameObject::Sptr dumpUI;
	Gameplay::GameObject::Sptr jumpUI;
	Gameplay::GameObject::Sptr walkUI;
	Gameplay::GameObject::Sptr pickupUI;
	Gameplay::GameObject::Sptr spillUI;

	Gameplay::GameObject::Sptr dialogue1;
	Gameplay::GameObject::Sptr dialogue2;
	Gameplay::GameObject::Sptr dialogue3;
	Gameplay::GameObject::Sptr dialogue4;
	Gameplay::GameObject::Sptr dialogue5;
	Gameplay::GameObject::Sptr dialogue6;
	Gameplay::GameObject::Sptr dialogue7;

	bool play3 = false;
	bool play4 = false;
	bool play5 = false;

	bool diag1 = false;
	bool diag2 = false;

	bool invDiag = false;
	bool recInvDiag = false;

	
	bool hasMoved = false;
	bool hasCollected = false;
	bool hasJumped = false;
	bool hasSpill = false;

	bool musicstart = false;
	bool activated = false;
	bool done = false;
	bool nobreak = false;
	bool hallwayLoaded = false;
	bool spillCrossed = false;

	bool doUpdate = true;

	bool do_once = false;
	bool do_once2 = false;

	

	int inventory = 4; //DEFAULT INVENTORY COUNT
	int max_trash = 5; //DEFAULT FOR NOW

	float currentTime = 0.0f;
	float footstepTimer = 0.0f;
	
	bool isPressed = false;

};