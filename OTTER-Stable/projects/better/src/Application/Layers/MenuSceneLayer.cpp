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
	Application& app = Application::Get();

	if (!activated) 
	{
		MainMenu = app.CurrentScene()->FindObjectByName("Menu");
		activated = true;

	}
	if (toggle_switch)
	{
		AudioEngine::stopEventS("event:/Sounds/Music/Menu/MenuMusicEvent");
		app.CurrentScene()->should_switch = true;
	}
	else if (toggle_switch2)
	{
		
		app.change_tutorial = true;
	}
	{
		play->Get<GuiPanel>()->SetTexture(playgame);
		tut->Get<GuiPanel>()->SetTexture(playtutorial);
		q->Get<GuiPanel>()->SetTexture(quitgame);
	}
	//track mouse
	_prevMousePos = InputEngine::GetMousePos();
	//if they are hovering over
	if (_prevMousePos.x >= 467 && _prevMousePos.x <= 812) //x boundary, should be same for all
	{
		//play game check
		if (_prevMousePos.y >= 370 && _prevMousePos.y <= 424)
		{
			play->Get<GuiPanel>()->SetTexture(playgame2);

		}
		//play tutorial
		else if (_prevMousePos.y >= 446 && _prevMousePos.y <= 546)
		{
			tut->Get<GuiPanel>()->SetTexture(playtutorial2);
		}
		//quit game
		else if (_prevMousePos.y >= 556 && _prevMousePos.y <= 683)
		{
			q->Get<GuiPanel>()->SetTexture(quitgame2);
		}
	}
	
	
	//actual click check
	if (InputEngine::GetMouseState(GLFW_MOUSE_BUTTON_LEFT) == ButtonState::Pressed)
	{
		_prevMousePos = InputEngine::GetMousePos();
		//std::cout << _prevMousePos.x <<", " << _prevMousePos.y << std::endl;

		
		if (_prevMousePos.x >= 467 && _prevMousePos.x <= 812) //x boundary, should be same for all
		{
			//play game check
			if (_prevMousePos.y >= 370 && _prevMousePos.y <= 424)
			{
				toggle_switch = true;
				//make loading screen
				Gameplay::GameObject::Sptr loading = app.CurrentScene()->CreateGameObject("Load");
				{
					RectTransform::Sptr transform = loading->Add<RectTransform>();
					transform->SetMin({ 0, 0 });
					transform->SetMax({ 1280, 720 });

					GuiPanel::Sptr loadPanel = loading->Add<GuiPanel>();
					loadPanel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/load.png"));
					AudioEngine::stopEventS("event:/Sounds/Music/Menu/MenuMusicEvent");
					AudioEngine::playEventS("event:/Sounds/Music/Loading/LoadingMusicEvent");
					AudioEngine::EventVolumeChange("event:/Sounds/Music/Loading/LoadingMusicEvent", -5.f);
				}
				
				app.CurrentScene()->RemoveGameObject(MainMenu);
				
			}
			//play tutorial
			else if (_prevMousePos.y >= 446 && _prevMousePos.y <= 546)
			{
				toggle_switch2 = true;
				//make loading screen
				Gameplay::GameObject::Sptr loading = app.CurrentScene()->CreateGameObject("Load");
				{
					RectTransform::Sptr transform = loading->Add<RectTransform>();
					transform->SetMin({ 0, 0 });
					transform->SetMax({ 1280, 720 });

					GuiPanel::Sptr loadPanel = loading->Add<GuiPanel>();
					loadPanel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/load.png"));
					AudioEngine::stopEventS("event:/Sounds/Music/Menu/MenuMusicEvent");
					AudioEngine::playEventS("event:/Sounds/Music/Loading/LoadingMusicEvent");
					AudioEngine::EventVolumeChange("event:/Sounds/Music/Loading/LoadingMusicEvent", -5.f);
				}
				
				app.CurrentScene()->RemoveGameObject(MainMenu);
				
			}
			//quit game
			else if (_prevMousePos.y >= 556 && _prevMousePos.y <= 683)
			{
				std::exit(0);
			}
		}
	}
}

void MenuSceneLayer::_CreateScene()
{

	Application& app = Application::Get();
	//all conditions to change between in-game conditions and menus
	bool loadScene = false;

	AudioEngine::playEventS("event:/Sounds/Music/Menu/MenuMusicEvent");
	// For now we can use a toggle to generate our scene vs load from file
	if (loadScene && std::filesystem::exists("scene.json")) {
		app.LoadScene("scene.json");
	}
	else {

		Gameplay::Scene::Sptr scene = std::make_shared<Gameplay::Scene>();	
		// Set up the scene's camera
		Gameplay::GameObject::Sptr camera = scene->MainCamera->GetGameObject()->SelfRef();
		{
			camera->SetPostion(glm::vec3(-1.42f, 4.69f, 5.73f));
			//camera->SetPostion(glm::vec3(-1.42f, 18.67f, 17.420));
			camera->LookAt(glm::vec3(0.0f));
			camera->SetRotation(glm::vec3(59.0f, 0.0f, 177.0f));
			camera->SetScale(glm::vec3(1.0f, 1.0f, 3.1f));
			scene->MainCamera->SetFovDegrees(105.f);
		}


		playgame = ResourceManager::CreateAsset<Texture2D>("textures/playgame.png");
		playtutorial = ResourceManager::CreateAsset<Texture2D>("textures/playtutorial.png");
		quitgame = ResourceManager::CreateAsset<Texture2D>("textures/quitgame.png");
		playgame2 = ResourceManager::CreateAsset<Texture2D>("textures/playgame2.png");
		playtutorial2 = ResourceManager::CreateAsset<Texture2D>("textures/playtutorial2.png");
		quitgame2 = ResourceManager::CreateAsset<Texture2D>("textures/quitgame2.png");

		//LOAD OBJECTS
		//Font: Junk Dog
		Font::Sptr junkDogFont = ResourceManager::CreateAsset<Font>("fonts/JunkDog.otf", 35.f); //Font path, font size
		junkDogFont->Bake();

		/*Gameplay::GameObject::Sptr MenuUI = scene->CreateGameObject("Menu UI Canvas");
		{
			RectTransform::Sptr transform = MenuUI->Add<RectTransform>();*/

			Gameplay::GameObject::Sptr menu = scene->CreateGameObject("Menu");
			{
				RectTransform::Sptr transform = menu->Add<RectTransform>();
				transform->SetMin({ 0, 0 });
				transform->SetMax({ 1280, 720 });

				GuiPanel::Sptr startPanel = menu->Add<GuiPanel>();
				startPanel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/Rubbish_Rush_Menu.png"));
				
			}
			Gameplay::GameObject::Sptr menu2 = scene->CreateGameObject("Play Game");
			{
				RectTransform::Sptr transform = menu2->Add<RectTransform>();
				transform->SetMin({ 0, 0 });
				transform->SetMax({ 1280, 720 });

				GuiPanel::Sptr startPanel = menu2->Add<GuiPanel>();
				startPanel->SetTexture(playgame);

				play = menu2;

			}
			Gameplay::GameObject::Sptr menu3 = scene->CreateGameObject("Play Tutorial");
			{
				RectTransform::Sptr transform = menu3->Add<RectTransform>();
				transform->SetMin({ 0, 0 });
				transform->SetMax({ 1280, 720 });

				GuiPanel::Sptr startPanel = menu3->Add<GuiPanel>();
				startPanel->SetTexture(playtutorial);

				tut = menu3;

			}
			Gameplay::GameObject::Sptr menu4 = scene->CreateGameObject("Quit");
			{
				RectTransform::Sptr transform = menu4->Add<RectTransform>();
				transform->SetMin({ 0, 0 });
				transform->SetMax({ 1280, 720 });

				GuiPanel::Sptr startPanel = menu4->Add<GuiPanel>();
				startPanel->SetTexture(quitgame);

				q = menu4;

			}

		/*	MenuUI->AddChild(menu);
		}*/
		GuiBatcher::SetDefaultTexture(ResourceManager::CreateAsset<Texture2D>("textures/ui/ui-sprite.png"));
		GuiBatcher::SetDefaultBorderRadius(8);

		// Save the asset manifest for all the resources we just loaded
		ResourceManager::SaveManifest("scene-manifest.json");
		// Save the scene to a JSON file
		scene->Save("scene.json");

		// Send the scene to the application
		app.LoadScene(scene);
		scene->IsPlaying = false;
	}
}