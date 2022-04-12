#pragma once
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
#include "Graphics/Textures/Texture2D.h"

class MenuSceneLayer final : public ApplicationLayer
{
public:
	MAKE_PTRS(MenuSceneLayer)

	MenuSceneLayer();
	virtual ~MenuSceneLayer();

	virtual void OnAppLoad(const nlohmann::json& config) override;
	void OnUpdate() override;


protected:
	void _CreateScene();
	Gameplay::GameObject::Sptr MainMenu;
	bool activated = false;
	// The current scene that the application is working on
	Gameplay::Scene::Sptr _currentScene;

	glm::dvec2 _prevMousePos;

	bool toggle_switch = false;
	bool toggle_switch2 = false;

	Texture2D::Sptr playgame;
	Texture2D::Sptr playgame2;
	Texture2D::Sptr playtutorial;
	Texture2D::Sptr playtutorial2;
	Texture2D::Sptr quitgame;
	Texture2D::Sptr quitgame2;

	Gameplay::GameObject::Sptr play;
	Gameplay::GameObject::Sptr tut;
	Gameplay::GameObject::Sptr q;


};

