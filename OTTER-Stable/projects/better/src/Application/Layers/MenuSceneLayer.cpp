#include "MenuSceneLayer.h"
// GLM math library
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/gtx/common.hpp> // for fmod (floating modulus)
#include <fstream>
#include <filesystem>
#include "Application/Timing.h"

// Graphics
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/VertexArrayObject.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Textures/Texture2D.h"
#include "Graphics/Textures/TextureCube.h"
#include "Graphics/VertexTypes.h"
#include "Graphics/Font.h"
#include "Graphics/GuiBatcher.h"
#include "Graphics/Framebuffer.h"
#include "Graphics/Framebuffer.h"

// Utilities
#include "Utils/MeshBuilder.h"
#include "Utils/MeshFactory.h"
#include "Utils/ObjLoader.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/ResourceManager/ResourceManager.h"
#include "Utils/FileHelpers.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/StringUtils.h"
#include "Utils/GlmDefines.h"
#include "ToneFire.h"

// Gameplay
#include "Gameplay/Material.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"

// Components
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Components/Camera.h"
#include "Gameplay/Components/RotatingBehaviour.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/MaterialSwapBehaviour.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/SimpleCameraControl.h"
#include "Gameplay/Components/Light.h"

//Rubbish Rush Components
#include "Gameplay/Components/AudioEngine.h"
#include "Gameplay/Components/ShadowCamera.h"

// Physics
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/Colliders/PlaneCollider.h"
#include "Gameplay/Physics/Colliders/SphereCollider.h"
#include "Gameplay/Physics/Colliders/ConvexMeshCollider.h"
#include "Gameplay/Physics/Colliders/CylinderCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Graphics/DebugDraw.h"

// GUI
#include "Gameplay/Components/GUI/RectTransform.h"
#include "Gameplay/Components/GUI/GuiPanel.h"
#include "Gameplay/Components/GUI/GuiText.h"
#include "Gameplay/InputEngine.h"

#include "Application/Application.h"
#include "Gameplay/Components/ParticleSystem.h"
#include "Gameplay/InputEngine.h"
#include "Application/Layers/RenderLayer.h"

MenuSceneLayer::MenuSceneLayer()
{
	Name = "Menu Scene";
	Overrides = AppLayerFunctions::OnAppLoad | AppLayerFunctions::OnUpdate;
}

MenuSceneLayer::~MenuSceneLayer() = default;

void MenuSceneLayer::OnAppLoad(const nlohmann::json& config)
{
	_CreateScene();
}

void MenuSceneLayer::OnUpdate()
{
	if (!activated) 
	{
		MainMenu = _currentScene->FindObjectByName("Menu");
		activated = true;

	}
}

void MenuSceneLayer::_CreateScene()
{

	Application& app = Application::Get();
	//all conditions to change between in-game conditions and menus
	bool loadScene = false;

	// For now we can use a toggle to generate our scene vs load from file
	if (loadScene && std::filesystem::exists("scene.json")) {
		app.LoadScene("scene.json");
	}
	else {

		Gameplay::Scene::Sptr scene = std::make_shared<Gameplay::Scene>();	
		//LOAD OBJECTS
		//Font: Junk Dog
		Font::Sptr junkDogFont = ResourceManager::CreateAsset<Font>("fonts/JunkDog.otf", 35.f); //Font path, font size
		junkDogFont->Bake();

		Gameplay::GameObject::Sptr MenuUI = scene->CreateGameObject("Menu UI Canvas");
		{
			RectTransform::Sptr transform = MenuUI->Add<RectTransform>();

			Gameplay::GameObject::Sptr menu = scene->CreateGameObject("Menu");
			{
				RectTransform::Sptr transform = menu->Add<RectTransform>();
				transform->SetMin({ 0, 0 });
				transform->SetMax({ 1280, 720 });

				GuiPanel::Sptr startPanel = menu->Add<GuiPanel>();
				startPanel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/Temp_Menu.png"));
				
			}

			MenuUI->AddChild(menu);
		}
	}
}