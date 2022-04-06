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

	// The current scene that the application is working on
	Gameplay::Scene::Sptr _currentScene;
};

