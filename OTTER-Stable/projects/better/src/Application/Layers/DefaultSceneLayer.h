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
	
	//Gameplay::MeshResource::Sptr trashMesh;
	//Gameplay::Material::Sptr trashMaterial;

	//conveyor belt
	Gameplay::Material::Sptr conveyor_belt;
	float tracker = 0;

	Gameplay::GameObject::Sptr startMenu;
	Gameplay::GameObject::Sptr HighScorePos1L;
	Gameplay::GameObject::Sptr HighScorePos2L;
	Gameplay::GameObject::Sptr HighScorePos3L;
	Gameplay::GameObject::Sptr HighScorePos4L;
	Gameplay::GameObject::Sptr HighScorePos5L;
	Gameplay::GameObject::Sptr HighScorePos6L;
	Gameplay::GameObject::Sptr HighScorePos7L;
	Gameplay::GameObject::Sptr HighScorePos8L;
	Gameplay::GameObject::Sptr HighScorePos9L;
	Gameplay::GameObject::Sptr HighScorePos10L;
	Gameplay::GameObject::Sptr HighScorePos1V;
	Gameplay::GameObject::Sptr HighScorePos2V;
	Gameplay::GameObject::Sptr HighScorePos3V;
	Gameplay::GameObject::Sptr HighScorePos4V;
	Gameplay::GameObject::Sptr HighScorePos5V;
	Gameplay::GameObject::Sptr HighScorePos6V;
	Gameplay::GameObject::Sptr HighScorePos7V;
	Gameplay::GameObject::Sptr HighScorePos8V;
	Gameplay::GameObject::Sptr HighScorePos9V;
	Gameplay::GameObject::Sptr HighScorePos10V;
	Gameplay::GameObject::Sptr HighscoreLeaderBoard;
	Gameplay::GameObject::Sptr pauseMenu;
	Gameplay::GameObject::Sptr failMenu;
	Gameplay::GameObject::Sptr winMenu;
	Gameplay::GameObject::Sptr UIText;
	Gameplay::GameObject::Sptr trashRemainder;
	Gameplay::GameObject::Sptr objective;
	Gameplay::GameObject::Sptr returnUI;
	Gameplay::GameObject::Sptr submitUI;
	Gameplay::GameObject::Sptr invUI;



	//all conditions to change between in-game conditions and menus
	//bool isPressed = false;
	bool timerDone = false;
	bool timeleveltDone = false;
	bool lose = false;
	bool Victory = false;
	
	bool playrecyclesound = false;
	bool playtrashsound = false;
	bool playmulti = false;
	bool victoryMusicPlayed = false;
	bool full = false;
	//Variables for changing stuff in the game
	float timeLoop = 1.0f; //The seconds between start time and the menu that plays in the beginning
	float roundTime = 190.f; //The time needed to collect all trash
	float timelevelt = roundTime;
	bool playMenu = true;

	bool start = false;
	bool spressed = false;
	bool isPaused = false;
	float currentTime = 0.0f;
	bool activated = false;
	int inventory = 4; //DEFAULT INVENTORY COUNT
	int max_trash = 15; //DEFAULT FOR NOW

	bool highscoreloop = false;

	bool press_once = false;
	bool press_high = true;
	bool press_play = false;
	bool press_fourth = true;

	//save trash resources
	Gameplay::MeshResource::Sptr bagtrashMesh;
	Gameplay::Material::Sptr bagtrashMaterial;
	Gameplay::MeshResource::Sptr trashMesh;
	Gameplay::Material::Sptr trashMaterial;
};