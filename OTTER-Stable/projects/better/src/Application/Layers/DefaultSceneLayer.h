#pragma once
#include "Application/ApplicationLayer.h"
#include "json.hpp"
#include <GLM/glm.hpp>
#include "Utils/Macros.h"
#include "Gameplay/Scene.h"
#include "Gameplay/MeshResource.h"
#include "Gameplay/Material.h"
#include <algorithm>
#include <random>
#include "ToneFire.h"

/**
 * This example layer handles creating a default test scene, which we will use 
 * as an entry point for creating a sample scene
 */
class DefaultSceneLayer final : public ApplicationLayer {
public:
	MAKE_PTRS(DefaultSceneLayer)

	DefaultSceneLayer();
	virtual ~DefaultSceneLayer();

	// Inherited from ApplicationLayer
	virtual void OnAppLoad(const nlohmann::json& config) override;
	void OnUpdate() override;

	std::vector<Gameplay::GameObject::Sptr> all_trash;

protected:
	void _CreateScene();
	void _CreateTrash();
	

	void RandomizePositions();
	
	// The current scene that the application is working on
	Gameplay::Scene::Sptr _currentScene;

	Gameplay::GameObject::Sptr trashyM;
	Gameplay::GameObject::Sptr binM;
	Gameplay::GameObject::Sptr RectangleE;
	Gameplay::GameObject::Sptr TrashyE;
	//Gameplay::MeshResource::Sptr trashMesh;
	//Gameplay::Material::Sptr trashMaterial;

	Gameplay::GameObject::Sptr startMenu;
	Gameplay::GameObject::Sptr pauseMenu;
	Gameplay::GameObject::Sptr failMenu;
	Gameplay::GameObject::Sptr winMenu;
	Gameplay::GameObject::Sptr UIText;
	Gameplay::GameObject::Sptr trashRemainder;
	Gameplay::GameObject::Sptr objective;
	Gameplay::GameObject::Sptr returnUI;
	Gameplay::GameObject::Sptr submitUI;

	ToneFire::FMODStudio studio;
	ToneFire::StudioSound audio;


	//create points we need for lerping
	std::vector<glm::vec3> pointsPos; //rec
	

	std::vector<glm::vec3> pointsPos2; //trashy
	
	std::vector<glm::vec3> pointsS;
	
	std::vector<glm::vec3> pointsR;
	

	std::vector<glm::vec3> pointsS2;
	
	std::vector<glm::vec3> pointsR2;
	
	//all conditions to change between in-game conditions and menus
	bool isPressed = false;
	bool timerDone = false;
	bool timeleveltDone = false;
	bool lose = false;
	bool Victory = false;
	bool lights = true;
	//Variables for changing stuff in the game
	float timeLoop = 7.0f; //The seconds between start time and the menu that plays in the beginning
	float timelevelt = 25000.f; //The time needed to collect all trash
	bool playMenu = true;

	bool start = false;
	bool spressed = false;
	bool isPaused = false;

	bool activated = false;
	int inventory = 4; //DEFAULT INVENTORY COUNT
	int max_trash = 15; //DEFAULT FOR NOW

	bool press_once = false;

	//save trash resources
	Gameplay::MeshResource::Sptr bagtrashMesh;
	Gameplay::Material::Sptr bagtrashMaterial;
	Gameplay::MeshResource::Sptr trashMesh;
	Gameplay::Material::Sptr trashMaterial;
};