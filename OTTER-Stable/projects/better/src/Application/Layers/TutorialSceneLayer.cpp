#include "TutorialSceneLayer.h"
#include <iostream>

// GLM math library
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtc/random.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/gtx/common.hpp> // for fmod (floating modulus)

#include <filesystem>
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
#include "Application/Timing.h"

// Gameplay
#include "Gameplay/Material.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"

// Components
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Components/Camera.h"
#include "Gameplay/Components/RotatingBehaviour.h"
#include "Gameplay/Components/JumpBehaviour.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/MaterialSwapBehaviour.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/SimpleCameraControl.h"
#include "Gameplay/Components/Light.h"
#include "ToneFire.h"
#include "Gameplay/Components/AudioEngine.h"
#include "Gameplay/Components/ShadowCamera.h"
//ours
#include "Gameplay/Components/DeleteObjectBehaviour.h"
#include "Gameplay/Components/CollectTrashBehaviour.h"
#include "Gameplay/Components/SubmittingTrashBehaviour.h"
#include "Gameplay/Components/PlayerMovementBehavior.h"
#include "Gameplay/Components/ConveyorBeltBehaviour.h"
#include "Gameplay/Components/SpillBehaviour.h"
#include "Gameplay/Components/SteeringBehaviour.h"
#include "Gameplay/Components/FollowBehaviour.h"
#include "Gameplay/Components/MorphAnimator.h"
#include "Gameplay/Components/MorphMeshRenderer.h"
#include "Gameplay/Components/GroundBehaviour.h"
#include "Gameplay/Components/InventoryUI.h"


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

#include "Application/Layers/RenderLayer.h"
#include "Gameplay/InputEngine.h"
#include "Graphics/Textures/Texture1D.h"

TutorialSceneLayer::TutorialSceneLayer() :
	ApplicationLayer()
{
	Name = "Tutorial Scene";
	Overrides = AppLayerFunctions::OnAppLoad | AppLayerFunctions::OnUpdate /* | AppLayerFunctions::OnSceneUnload*/;
}

TutorialSceneLayer::~TutorialSceneLayer() = default;

void TutorialSceneLayer::OnSceneUnload()
{
	//we no longer want to run the update function
	doUpdate = false;
	//delete all objects in the scene
	//might not be necessary?
	_tutcurrentScene->RemoveGameObject(trashyM);
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Plane"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Trash1"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Trash2"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Trash3"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Layout"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Layout2"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Layout Wall Front"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Layout Wall Back"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Layout Wall Right Bottom"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Layout Wall Left Bottom"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Layout Wall Center Left"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Layout Wall Center Right"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Layout Wall Top Right"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Layout Wall Top Left"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Bin"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Feedback UI"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Pickup Feedback"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Submit Feedback"));

	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Load"));

	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Bench1"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Bench2"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Bench3"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Bench5"));

	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Dialogue 1"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Dialogue 2"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Dialogue 3"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Dialogue 4"));
	_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Dialogue 5"));
	
}

void TutorialSceneLayer::OnAppLoad(const nlohmann::json& config) {
	_CreateScene();
	
}

double tutlastFrame = glfwGetTime();
void TutorialSceneLayer::OnUpdate()
{
	
	
	if (doUpdate)
	{
		
		//put this at the top to create a delay affect, creates time for the loading screen to render
		if (done) //LOAD NEXT SCENE
		{
			_tutcurrentScene->should_switch = true; //maybe add some delay here
			//studio.~FMODStudio();
		}

		Application& app = Application::Get();
		_tutcurrentScene = app.CurrentScene();
		isPressed = false;
		//enable/disable lighting, only detect once
		
		// Figure out the current time, and the time since the last frame
		double thisFrame = glfwGetTime();
		float dt = static_cast<float>(thisFrame - tutlastFrame);

		if (!activated)
		{
			walkUI = _tutcurrentScene->FindObjectByName("Walk Tutorial UI");
			jumpUI = _tutcurrentScene->FindObjectByName("Jump Tutorial UI");
			pickupUI = _tutcurrentScene->FindObjectByName("Pickup Trash Tutorial UI");
			dumpUI = _tutcurrentScene->FindObjectByName("Dump Tutorial UI");
			spillUI = _tutcurrentScene->FindObjectByName("Spill Tutorial UI");

			dialogue1 = _tutcurrentScene->FindObjectByName("Dialogue 1");
			dialogue2 = _tutcurrentScene->FindObjectByName("Dialogue 2");
			dialogue3 = _tutcurrentScene->FindObjectByName("Dialogue 3");
			dialogue4 = _tutcurrentScene->FindObjectByName("Dialogue 4");
			dialogue5 = _tutcurrentScene->FindObjectByName("Dialogue 5");
			

			trashyM = _tutcurrentScene->FindObjectByName("Trashy");
			trashyM->Get<Gameplay::Physics::RigidBody>()->SetAngularFactor(glm::vec3(0.0f, 0.0f, 0.0f));
			trashyM->Get<Gameplay::Physics::RigidBody>()->SetLinearDamping(0.9f);
			activated = true;
			//starting variables
			//_tutcurrentScene->trash = 1;
			
		}


		if (_tutcurrentScene != nullptr) {
			//MENU ANIMATED UPDATED
			if (_tutcurrentScene->IsPlaying && !done &&!musicstart) {
				
				AudioEngine::playEventS("event:/Music Regular");
				
				musicstart = true;
			}

			//if (_tutcurrentScene->playrecyclesound) {
			//	AudioEngine::playEventS("event:/Sounds/SoundEffects/Pickups interactions/PickUpCup");
			//	_tutcurrentScene->playrecyclesound = false;
			//}

			//if (_tutcurrentScene->playtrashsound) {
			////	test.PlayEvent("event:/Can Crush");
			//	AudioEngine::playEventS("event:/Sounds/SoundEffects/Pickups interactions/PickUpTrash");
			//	_tutcurrentScene->playtrashsound = false;
			//}

			//if (_tutcurrentScene->playmulti) {
			//	//test.SetEventParameter("event:/Trash multi", "parameter:/Pitch", glm::linearRand(0.f, 1.f));
			//	AudioEngine::playEventS("event:/Sounds/SoundEffects/Pickups interactions/DepositTrash");
			//	_tutcurrentScene->playmulti = false;
			//}

			if (_tutcurrentScene->IsPlaying && !done)
			{
				if (_tutcurrentScene->score == 2 && hallwayLoaded == false) {
					hallwayLoaded = true;
					_tutcurrentScene->RemoveGameObject(_tutcurrentScene->FindObjectByName("Layout Wall Top Right Barrier"));
					dumpUI->Get<GuiPanel>()->IsEnabled = false;
					dialogue3->Get<GuiPanel>()->IsEnabled = true;
					play3 = true;
					currentTime = Timing::Current().TimeSinceAppLoad();
					_CreateHallway(); //Create the second part of the level
				}
				if (hallwayLoaded)
				{
					//dialogue
					if (Timing::Current().TimeSinceAppLoad() - currentTime >= 2.5f && !play4)
					{
						std::cout << "ok";
						play4 = true;
						dialogue3->Get<GuiPanel>()->IsEnabled = false;
						dialogue4->Get<GuiPanel>()->IsEnabled = true;
						currentTime = Timing::Current().TimeSinceAppLoad();
					}
					
					if (Timing::Current().TimeSinceAppLoad() - currentTime >= 4.f && play4)
					{
						dialogue4->Get<GuiPanel>()->IsEnabled = false;
						dialogue5->Get<GuiPanel>()->IsEnabled = true;
						play5 = true;
					}
					if (Timing::Current().TimeSinceAppLoad() - currentTime >= 7.0f && play5) {
						dialogue5->Get<GuiPanel>()->IsEnabled = false;
					}
					

					if (!do_once) //only run this once
					{//Player Movement JUMP Tutorial
						if (trashyM->GetPosition().x <= -9.0f &&(glfwGetKey(app.GetWindow(), GLFW_KEY_SPACE)) && hasJumped == false) {
							hasJumped = true;
						}
						if (trashyM->GetPosition().x <= -9.0f && hasJumped == false) { //how far along player is
							jumpUI->Get<GuiPanel>()->IsEnabled = true;

						}
						else if (hasJumped){
							jumpUI->Get<GuiPanel>()->IsEnabled = false;
							do_once = true;
						}
					}
					if (!do_once2) //slime tut
					{
						if (trashyM->GetPosition().x <= -3.0f && hasSpill == false) { //how far along player is
							spillUI->Get<GuiPanel>()->IsEnabled = true;
							hasSpill = true;

						}
						else if (trashyM->GetPosition().x <= -6.0f){
							spillUI->Get<GuiPanel>()->IsEnabled = false;
							do_once2 = true;
						}
					}
				}

				if (_tutcurrentScene->score == max_trash) 
				{
					done = true; //LOAD NEXT SCENE
					//disable diagloue?
					dialogue5->Get<GuiPanel>()->IsEnabled = false;

					//dumpUI->Get<RenderComponent>()->IsEnabled = false;
					//pickupUI->Get<GuiPanel>()->IsEnabled = false;
					AudioEngine::stopEventS("event:/Music Regular");
					//make loading screen
					Gameplay::GameObject::Sptr loading = _tutcurrentScene->CreateGameObject("Load");
					{
						RectTransform::Sptr transform = loading->Add<RectTransform>();
						transform->SetMin({ 0, 0 });
						transform->SetMax({ 1280, 720 });

						GuiPanel::Sptr loadPanel = loading->Add<GuiPanel>();
						loadPanel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/load.png"));
					}
				}
			}
			else if (_tutcurrentScene->IsPlaying && done) {

				
			}

			
			//Player Movement Tutorial
			if ((glfwGetKey(app.GetWindow(), GLFW_KEY_W) || glfwGetKey(app.GetWindow(), GLFW_KEY_A) || glfwGetKey(app.GetWindow(), GLFW_KEY_S) || glfwGetKey(app.GetWindow(), GLFW_KEY_D)) && hasMoved == false) {
				hasMoved = true;
			}
			
			
			if (Timing::Current().TimeSinceAppLoad() - currentTime >= 0.3f && _tutcurrentScene->walk) {
				AudioEngine::playEventS("event:/Sounds/SoundEffects/Footstep");
				currentTime = Timing::Current().TimeSinceAppLoad();
			}

			if (Timing::Current().TimeSinceAppLoad() - currentTime >= 0.5f && _tutcurrentScene->walk == false || trashyM->Get<JumpBehaviour>()->in_air) {
				AudioEngine::stopEventS("event:/Sounds/SoundEffects/Footstep");
			}



			if (Timing::Current().Timing::TimeSinceSceneLoad() > 7.5f && hasMoved == false) { //If the player has not moved for a set amount of seconds, show the tutorial UI for movement
				walkUI->Get<GuiPanel>()->IsEnabled = true;
				
			}
			else {
				walkUI->Get<GuiPanel>()->IsEnabled = false;
			}


			//dialogue 1
			if (trashyM->GetPosition().y >= -2.5f)
			{
				dialogue1->Get<GuiPanel>()->IsEnabled = true;
			}
			else if (trashyM->GetPosition().y <= -4.0f)
			{
				dialogue1->Get<GuiPanel>()->IsEnabled = false;
			}

			if (trashyM->GetPosition().y < -5.0f   &&  (_tutcurrentScene->held < 1) && hasCollected == false) { //Pick up Trash tutorial stuff
				
				if(!hallwayLoaded){
					pickupUI->Get<GuiPanel>()->IsEnabled = true;
					dialogue2->Get<GuiPanel>()->IsEnabled = true;
				}
				
			}
			else if (_tutcurrentScene->score >= 2)
			{
				dialogue2->Get<GuiPanel>()->IsEnabled = false;
			}
			else {
				pickupUI->Get<GuiPanel>()->IsEnabled = false;
				
				if (_tutcurrentScene->held >= 1) {
					hasCollected = true;
				}
			}
			

			if (hasCollected == true) {//If the player has picked up the trash, then display the UI to teach them how to dump the trash
				if (!hallwayLoaded) {
					dumpUI->Get<GuiPanel>()->IsEnabled = true;
				}
			}

		}

		// Grab shorthands to the camera and shader from the _currentScene
		Gameplay::Camera::Sptr camera = _tutcurrentScene->MainCamera;

		//if (!camera->GetComponent<SimpleCameraControl>()->moving)
		//{
			//Set Camera position
			camera->GetGameObject()->SetPostion(trashyM->GetPosition() + glm::vec3(0.0f, 4.0f, 6.f));
			camera->GetGameObject()->LookAt(trashyM->GetPosition() + glm::vec3(0.0f, -3.9f, -2.0f));
		//}

		// Store timing for next loop
		tutlastFrame = thisFrame;
	}

	

}

void TutorialSceneLayer::_CreateScene()
{
	//using namespace Gameplay;
	//using namespace Gameplay::Physics;

	Application& app = Application::Get();
	//all conditions to change between in-game conditions and menus
	bool loadScene = false;


	//Initialization of Animations
	std::vector <Gameplay::MeshResource::Sptr> walking;
	std::vector <Gameplay::MeshResource::Sptr> idle;
	std::vector <Gameplay::MeshResource::Sptr> jumping;
	std::vector <Gameplay::MeshResource::Sptr> interaction;


	// For now we can use a toggle to generate our scene vs load from file
	if (loadScene && std::filesystem::exists("scene.json")) {
		app.LoadScene("scene.json");
	}
	else {
		
		// This shader handles our basic materials without reflections (cause they expensive)
		ShaderProgram::Sptr rackShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/deferred_forwardRACK.glsl" }
		});
		
		// ANIMATION SHADER??
		ShaderProgram::Sptr animShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/morph.vert" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/deferred_forward.glsl" }
		});
		// Basic gbuffer generation with no vertex manipulation
		ShaderProgram::Sptr deferredForward = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/deferred_forward.glsl" }
		});
		deferredForward->SetDebugName("Deferred - GBuffer Generation");
		//shader for trash outline
		ShaderProgram::Sptr deferredTrash = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/deferred_forwardTRASH.glsl" }
		});
		

#pragma region Basic Texture Creation
		Texture2DDescription singlePixelDescriptor;
		singlePixelDescriptor.Width = singlePixelDescriptor.Height = 1;
		singlePixelDescriptor.Format = InternalFormat::RGB8;

		float normalMapDefaultData[3] = { 0.5f, 0.5f, 1.0f };
		Texture2D::Sptr normalMapDefault = ResourceManager::CreateAsset<Texture2D>(singlePixelDescriptor);
		normalMapDefault->LoadData(1, 1, PixelFormat::RGB, PixelType::Float, normalMapDefaultData);

		float solidBlack[3] = { 0.5f, 0.5f, 0.5f };
		Texture2D::Sptr solidBlackTex = ResourceManager::CreateAsset<Texture2D>(singlePixelDescriptor);
		solidBlackTex->LoadData(1, 1, PixelFormat::RGB, PixelType::Float, solidBlack);

		float solidGrey[3] = { 0.0f, 0.0f, 0.0f };
		Texture2D::Sptr solidGreyTex = ResourceManager::CreateAsset<Texture2D>(singlePixelDescriptor);
		solidGreyTex->LoadData(1, 1, PixelFormat::RGB, PixelType::Float, solidGrey);

		float solidWhite[3] = { 1.0f, 1.0f, 1.0f };
		Texture2D::Sptr solidWhiteTex = ResourceManager::CreateAsset<Texture2D>(singlePixelDescriptor);
		solidWhiteTex->LoadData(1, 1, PixelFormat::RGB, PixelType::Float, solidWhite);

#pragma endregion 
		

		// Here we'll load in the cubemap, as well as a special shader to handle drawing the skybox
		TextureCube::Sptr testCubemap = ResourceManager::CreateAsset<TextureCube>("cubemaps/lot/lot.png");
		ShaderProgram::Sptr      skyboxShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/skybox_vert.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/skybox_frag.glsl" }
		});

		// Create an empty scene
		Gameplay::Scene::Sptr scene = std::make_shared<Gameplay::Scene>();

		// Setting up our enviroment map
		scene->SetSkyboxTexture(testCubemap);
		scene->SetSkyboxShader(skyboxShader);
		// Since the skybox I used was for Y-up, we need to rotate it 90 deg around the X-axis to convert it to z-up
		scene->SetSkyboxRotation(glm::rotate(MAT4_IDENTITY, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)));

		// Create some lights for our scene
		Gameplay::GameObject::Sptr lightParent = scene->CreateGameObject("Lights");
		{
			Gameplay::GameObject::Sptr light = scene->CreateGameObject("Light");
			light->SetPostion(glm::vec3(11.77f, 0.21f, 3.0f));
			lightParent->AddChild(light);

			Light::Sptr lightComponent = light->Add<Light>();
			lightComponent->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
			lightComponent->SetRadius(5.0f);
			lightComponent->SetIntensity(55.0f);
		}
		{
			Gameplay::GameObject::Sptr light = scene->CreateGameObject("Light");
			light->SetPostion(glm::vec3(10.98f, -8.26f, 3.59f));
			lightParent->AddChild(light);

			Light::Sptr lightComponent = light->Add<Light>();
			lightComponent->SetColor(glm::vec3(1.0f, 1.0f, 1.1f));
			lightComponent->SetRadius(6.5f);
			lightComponent->SetIntensity(20.0f);
		}
		{
			Gameplay::GameObject::Sptr light = scene->CreateGameObject("Light");
			light->SetPostion(glm::vec3(9.86f, 7.16f, 0.88f));
			lightParent->AddChild(light);

			Light::Sptr lightComponent = light->Add<Light>();
			lightComponent->SetColor(glm::vec3(1.0f, 1.0f, 1.1f));
			lightComponent->SetRadius(5.5f);
			lightComponent->SetIntensity(15.0f);
		}
	/*	for (int ix = 0; ix < 50; ix++) {
			Gameplay::GameObject::Sptr light = scene->CreateGameObject("Light");
			light->SetPostion(glm::vec3(glm::diskRand(25.0f), 1.0f));
			lightParent->AddChild(light);

			Light::Sptr lightComponent = light->Add<Light>();
			lightComponent->SetColor(glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0f)));
			lightComponent->SetRadius(glm::linearRand(0.1f, 10.0f));
			lightComponent->SetIntensity(glm::linearRand(1.0f, 2.0f));
		}*/
		// We'll create a mesh that is a simple plane that we can resize later
		Gameplay::MeshResource::Sptr planeMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>();
		planeMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(1.0f)));
		planeMesh->GenerateMesh();

		Gameplay::MeshResource::Sptr sphere = ResourceManager::CreateAsset<Gameplay::MeshResource>();
		sphere->AddParam(MeshBuilderParam::CreateIcoSphere(ZERO, ONE, 5));
		sphere->GenerateMesh();

		// Set up the scene's camera
		Gameplay::GameObject::Sptr camera = scene->MainCamera->GetGameObject()->SelfRef();
		{
			camera->SetPostion(glm::vec3(-1.42f, 4.69f, 5.73f));
			//camera->SetPostion(glm::vec3(-1.42f, 18.67f, 17.420));
			camera->LookAt(glm::vec3(0.0f));
			camera->SetRotation(glm::vec3(59.0f, 0.0f, 177.0f));
			camera->SetScale(glm::vec3(1.0f, 1.0f, 3.1f));
			scene->MainCamera->SetFovDegrees(105.f);
			

			//Gameplay::Camera::Sptr cam = camera->Add<Gameplay::Camera>();
			//cam->SetOrthoEnabled(true);
			//cam->SetOrthoVerticalScale(10.0f);
			// Make sure that the camera is set as the scene's main camera!
			//scene->MainCamera = cam;
			//camera->Add<SimpleCameraControl>();
		}
	
		Gameplay::GameObject::Sptr shadowCaster = scene->CreateGameObject("Shadow Light");
		{
			// Set position in the scene
			shadowCaster->SetPostion(glm::vec3(3.0f, 3.0f, 5.0f));
			shadowCaster->LookAt(glm::vec3(0.0f));

			// Create and attach a renderer for the monkey
			ShadowCamera::Sptr shadowCam = shadowCaster->Add<ShadowCamera>();
			//shadowCam->SetProjection(glm::perspective(glm::radians(120.0f), 1.0f, 0.1f, 100.0f));
			shadowCam->SetProjection(glm::ortho(15.0f, 30.0f, 30.0f, 15.0f, 0.225f, 22555.f));
		}

		// Set up all our sample objects
		//setup trashy
		Gameplay::MeshResource::Sptr trashyMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("models/trashy.obj");
		Texture2D::Sptr trashyTex = ResourceManager::CreateAsset<Texture2D>("textures/trashyTEX.png");
		// Create our material
		Gameplay::Material::Sptr trashyMaterial = ResourceManager::CreateAsset<Gameplay::Material>(animShader);
		{
			trashyMaterial->Name = "Trashy";
			trashyMaterial->Set("u_Material.AlbedoMap", trashyTex);
			//testMaterial->Set("u_Material.Specular", boxSpec);
			trashyMaterial->Set("u_Material.Shininess", 0.0f);
			trashyMaterial->Set("u_Material.NormalMap", normalMapDefault);
		}
		
		Gameplay::GameObject::Sptr trashyM = scene->CreateGameObject("Trashy"); //SEARCHBAR TAGS: PLAYERENTITY, PLAYER, TRASHYENTITY, TRASHYOBJECT
		{
			trashyM->SetPostion(glm::vec3(6.318f, -0.788f, 0.106f));
			trashyM->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			trashyM->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
			// Add a render component
			RenderComponent::Sptr renderer = trashyM->Add<RenderComponent>();
			renderer->SetMesh(trashyMesh);
			renderer->SetMaterial(trashyMaterial);
			// Add a dynamic rigid body to this monkey
			Gameplay::Physics::RigidBody::Sptr physics = trashyM->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Dynamic);
			Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();

			box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
			box->SetScale(glm::vec3(0.33f, 0.580f, 0.22f));
			//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
			physics->AddCollider(box);
			//physics->AddCollider(BoxCollider::Create());
			//physics->SetMass(0.0f);
			//add trigger for collisions and behaviours
			Gameplay::Physics::TriggerVolume::Sptr volume = trashyM->Add<Gameplay::Physics::TriggerVolume>();
			Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();

			box2->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
			box2->SetScale(glm::vec3(0.33f, 0.58f, 0.22f));
			//box2->SetExtents(glm::vec3(0.8, 2.68, 0.83));
			volume->AddCollider(box2);
			JumpBehaviour::Sptr behaviour = trashyM->Add<JumpBehaviour>();
			//INVENTORY UI SYSTEM
			InventoryUI::Sptr behaviour2 = trashyM->Add<InventoryUI>();

			PlayerMovementBehavior::Sptr movement = trashyM->Add<PlayerMovementBehavior>();

			//ANIMATION STUFF////
			MorphMeshRenderer::Sptr morph1 = trashyM->Add<MorphMeshRenderer>();
			morph1->SetMorphMeshRenderer(trashyMesh, trashyMaterial);
			MorphAnimator::Sptr morph2 = trashyM->Add<MorphAnimator>();

			//walking frames
			//std::vector <MeshResource::Sptr> frames;
			//MeshResource::Sptr trashyMesh1 = ResourceManager::CreateAsset<MeshResource>("trashyWalk/trashywalk_000001.obj");
			Gameplay::MeshResource::Sptr trashyMesh2 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyWalk/trashywalk_000002.obj");
			Gameplay::MeshResource::Sptr trashyMesh7 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyWalk/trashywalk_000010.obj");
			Gameplay::MeshResource::Sptr trashyMesh3 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyWalk/trashywalk_000016.obj");
			Gameplay::MeshResource::Sptr trashyMesh4 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyWalk/trashywalk_000020.obj");
			Gameplay::MeshResource::Sptr trashyMesh5 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyWalk/trashywalk_000025.obj");
			Gameplay::MeshResource::Sptr trashyMesh6 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyWalk/trashywalk_000030.obj");
			//idle frames
			//std::vector <MeshResource::Sptr> frames2;
			Gameplay::MeshResource::Sptr trashyMesh8 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyIdle/trashyidle_000001.obj");
			Gameplay::MeshResource::Sptr trashyMesh11 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyIdle/trashyidle_000011.obj");
			Gameplay::MeshResource::Sptr trashyMesh10 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyIdle/trashyidle_000020.obj");
			Gameplay::MeshResource::Sptr trashyMesh12 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyIdle/trashyidle_000030.obj");
			Gameplay::MeshResource::Sptr trashyMesh9 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyIdle/trashyidle_000040.obj");
			//jump frames
			Gameplay::MeshResource::Sptr trashyJump1 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyJump/trashy jump_000001.obj");
			Gameplay::MeshResource::Sptr trashyJump13 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyJump/trashy jump_000003.obj");
			Gameplay::MeshResource::Sptr trashyJump2 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyJump/trashy jump_000005.obj");
			Gameplay::MeshResource::Sptr trashyJump3 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyJump/trashy jump_000008.obj");
			Gameplay::MeshResource::Sptr trashyJump12 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyJump/trashy jump_000010.obj");
			//fall, land
			Gameplay::MeshResource::Sptr trashyJump4 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyJump/trashy jump_000013.obj");
			Gameplay::MeshResource::Sptr trashyJump5 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyJump/trashy jump_000014.obj");
			Gameplay::MeshResource::Sptr trashyJump6 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyJump/trashy jump_000015.obj");
			Gameplay::MeshResource::Sptr trashyJump7 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyJump/trashy jump_000016.obj");
			Gameplay::MeshResource::Sptr trashyJump8 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyJump/trashy jump_000017.obj");
			Gameplay::MeshResource::Sptr trashyJump9 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyJump/trashy jump_000018.obj");
			Gameplay::MeshResource::Sptr trashyJump10 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyJump/trashy jump_000019.obj");
			Gameplay::MeshResource::Sptr trashyJump11 = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashyJump/trashy jump_000022.obj");

			idle.push_back(trashyMesh8);
			idle.push_back(trashyMesh11);
			idle.push_back(trashyMesh10);
			idle.push_back(trashyMesh12);
			idle.push_back(trashyMesh9);

			//need to also set frames for other animations
			//simply switch the set frames
			//frames.push_back(trashyMesh1);
			walking.push_back(trashyMesh2);
			walking.push_back(trashyMesh7);
			walking.push_back(trashyMesh3);
			walking.push_back(trashyMesh4);
			walking.push_back(trashyMesh5);
			walking.push_back(trashyMesh6);
			//walking = frames; //may need to more manually copy frames over
			morph2->SetInitial();
			morph2->SetFrameTime(0.2f);
			morph2->SetFrames(idle);

			//for changing animations
			morph2->SetIdle(idle);
			morph2->SetWalking(walking);

			//jump frames
			jumping.push_back(trashyJump1);
			jumping.push_back(trashyJump13);
			jumping.push_back(trashyJump2);
			jumping.push_back(trashyJump3);
			jumping.push_back(trashyJump12);
			jumping.push_back(trashyJump4);
			jumping.push_back(trashyJump5);
			jumping.push_back(trashyJump6);
			jumping.push_back(trashyJump7);
			jumping.push_back(trashyJump8);
			jumping.push_back(trashyJump9);
			jumping.push_back(trashyJump10);
			jumping.push_back(trashyJump11);

			morph2->SetJumping(jumping);
		}

		Texture2D::Sptr planeTex = ResourceManager::CreateAsset<Texture2D>("textures/therealthing.jpg");
		//MeshResource::Sptr layoutMesh = ResourceManager::CreateAsset<MeshResource>("layout2.obj");
		Gameplay::Material::Sptr planeMaterial = ResourceManager::CreateAsset<Gameplay::Material>(deferredForward); {
			planeMaterial->Name = "Plane";
			planeMaterial->Set("u_Material.AlbedoMap", planeTex);
			planeMaterial->Set("u_Material.Shininess", 0.1f);
			planeMaterial->Set("u_Material.NormalMap", normalMapDefault);
		}

		// Set up all our sample objects
		Gameplay::GameObject::Sptr plane = scene->CreateGameObject("Plane");
		{
			plane->SetPostion(glm::vec3(0.0f, 0.0f, -0.08));
			plane->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			// Make a big tiled mesh
			Gameplay::MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(100.0f), glm::vec2(20.0f)));
			tiledMesh->GenerateMesh();


			//// Create and attach a RenderComponent to the object to draw our mesh
			//RenderComponent::Sptr renderer = plane->Add<RenderComponent>();
			//renderer->SetMesh(planeMesh);
			//renderer->SetMaterial(planeMaterial);

			// Attach a plane collider that extends infinitely along the X/Y axis
			Gameplay::Physics::RigidBody::Sptr physics = plane->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
			Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
			//box->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
			box->SetScale(glm::vec3(50.0f, -0.12f, 50.0f));
			physics->AddCollider(box);
			Gameplay::Physics::TriggerVolume::Sptr volume = plane->Add<Gameplay::Physics::TriggerVolume>();
			Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
			//box2->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
			box2->SetScale(glm::vec3(50.0f, -0.12f, 50.0f));
			volume->AddCollider(box2);
			//give to our floor tiles to tag them
			GroundBehaviour::Sptr behaviour = plane->Add<GroundBehaviour>();

		}

		//placeholder trash object  TAGS: PLACEHOLDER, this is how u create a new object for trash
		//setup trash
		Gameplay::MeshResource::Sptr trashMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("models/cup.obj");
		Texture2D::Sptr trashTex = ResourceManager::CreateAsset<Texture2D>("textures/cup.jpg");
		// Create our material
		Gameplay::Material::Sptr trashMaterial = ResourceManager::CreateAsset<Gameplay::Material>(deferredTrash);
		{
			trashMaterial->Name = "Trash";
			trashMaterial->Set("u_Material.AlbedoMap", trashTex);
			trashMaterial->Set("u_Material.Shininess", 0.3f);
			trashMaterial->Set("u_Material.NormalMap", normalMapDefault);
			trashMaterial->Set("u_Material.s_Depth", normalMapDefault);
			trashMaterial->Set("u_OutlineColor",glm::vec4(0.0, 0.0, 1.0, 1.0));
			trashMaterial->Set("u_Scale", 3.0f);
			trashMaterial->Set("u_DepthThreshold", 0.1f);
			trashMaterial->Set("u_NormalThreshold", 0.4f);
			trashMaterial->Set("u_DepthNormThreshold", 0.4f);
			trashMaterial->Set("u_DepthNormThresholdScale", 4.0f);
			trashMaterial->Set("u_PixelSize", glm::vec2(1.0f) / glm::vec2(128.0f, 128.0f));

		}

		bagtrashMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("models/Trashbag.obj");
		Texture2D::Sptr bagtrashTex = ResourceManager::CreateAsset<Texture2D>("textures/TrashBagTex.jpg");
		
		bagtrashMaterial = ResourceManager::CreateAsset<Gameplay::Material>(deferredTrash);
		{
			bagtrashMaterial->Name = "Bag Trash";
			bagtrashMaterial->Set("u_Material.AlbedoMap", bagtrashTex);
			bagtrashMaterial->Set("u_Material.Shininess", 0.3f);
			bagtrashMaterial->Set("u_Material.s_Depth", normalMapDefault);
			bagtrashMaterial->Set("u_OutlineColor", glm::vec4(0.0, 0.0,0.0, 1.0));
			bagtrashMaterial->Set("u_Scale", 3.0f);
			bagtrashMaterial->Set("u_DepthThreshold", 0.1f);
			bagtrashMaterial->Set("u_NormalThreshold", 0.4f);
			bagtrashMaterial->Set("u_DepthNormThreshold", 0.4f);
			bagtrashMaterial->Set("u_DepthNormThresholdScale", 4.0f);
			bagtrashMaterial->Set("u_PixelSize", glm::vec2(1.0f) / glm::vec2(128.0f, 128.0f));
		}
		

		//Texture2D::Sptr TutTex1 = ResourceManager::CreateAsset<Texture2D>("textures/Tut1tex.png");

		// 

		////MeshResource::Sptr layoutMesh = ResourceManager::CreateAsset<MeshResource>("layout2.obj");
		//Gameplay::Material::Sptr TutMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader); {
		//	TutMaterial->Name = "Plane";
		//	TutMaterial->Set("u_Material.AlbedoMap", TutTex1);
		//	TutMaterial->Set("u_Material.Shininess", 1.0f);
		//}
	
		//Gameplay::GameObject::Sptr TutM = scene->CreateGameObject("TutImage1");
		//{

		//	TutM->SetPostion(glm::vec3(6.5f, -2.f, 3.0f));
		//	TutM->SetRotation(glm::vec3(180.f, -180.0, -90.0f));

		//	Gameplay::MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>();
		//	tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(3.0f), glm::vec2(3.0f)));
		//	tiledMesh->GenerateMesh();

		//	RenderComponent::Sptr renderer = TutM->Add<RenderComponent>();
		//	renderer->SetMesh(tiledMesh);
		//	renderer->SetMaterial(TutMaterial);

		//}

		{
			Gameplay::GameObject::Sptr trashM = scene->CreateGameObject("Trash1"); //PLACEHOLDER change to any object u deem necessary change the set mesh and set material
			{
				trashM->SetPostion(glm::vec3(8.80f, -8.53f, 0.06f));
				trashM->SetRotation(glm::vec3(90.0f, 0.0f, -62.0f));
				trashM->SetScale(glm::vec3(0.82f, 0.73f, 0.78f));
				// Add a render component
				RenderComponent::Sptr renderer = trashM->Add<RenderComponent>();
				renderer->SetMesh(trashMesh);
				renderer->SetMaterial(trashMaterial);

				// Add a dynamic rigid body to this monkey
				Gameplay::Physics::RigidBody::Sptr physics = trashM->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
				/*Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
				box->SetScale(glm::vec3(0.14f, 0.09f, 0.21f));
				//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
				//box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
				//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
				physics->AddCollider(box);
				//physics->SetMass(0.0f);*/

				Gameplay::Physics::TriggerVolume::Sptr volume = trashM->Add<Gameplay::Physics::TriggerVolume>();
				Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
				box2->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
				box2->SetScale(glm::vec3(0.4f, 0.15f, 0.4f));
				volume->AddCollider(box2);
				CollectTrashBehaviour::Sptr behaviour2 = trashM->Add<CollectTrashBehaviour>();
				behaviour2->type = "Recycle";
				behaviour2->tutorial = true;
			}

			/*Gameplay::GameObject::Sptr trash2 = scene->CreateGameObject("Trash2"); //PLACEHOLDER change to any object u deem necessary change the set mesh and set material
			{
				trash2->SetPostion(glm::vec3(4.140f, -8.530f, 0.06f));
				trash2->SetRotation(glm::vec3(90.0f, 0.0f, -62.0f));
				trash2->SetScale(glm::vec3(0.82f, 0.73f, 0.78f));
				// Add a render component
				RenderComponent::Sptr renderer = trash2->Add<RenderComponent>();
				renderer->SetMesh(trashMesh);
				renderer->SetMaterial(trashMaterial);
				// Add a dynamic rigid body to this monkey
				Gameplay::Physics::RigidBody::Sptr physics = trash2->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
				box->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
				//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
				//box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
				//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
				physics->AddCollider(box);
				//physics->SetMass(0.0f);
				Gameplay::Physics::TriggerVolume::Sptr volume = trash2->Add<Gameplay::Physics::TriggerVolume>();
				Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
				box2->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
				box2->SetScale(glm::vec3(0.4f, 0.15f, 0.4f));
				volume->AddCollider(box2);
				CollectTrashBehaviour::Sptr behaviour2 = trash2->Add<CollectTrashBehaviour>();
			}*/

			Gameplay::GameObject::Sptr trash2 = scene->CreateGameObject("Trash2");
			{
				trash2->SetPostion(glm::vec3(4.140f, -8.530f, 0.0f));
				trash2->SetRotation(glm::vec3(90.0f, 0.0f, -92.0f));
				trash2->SetScale(glm::vec3(0.9f, 0.59f, 0.73f));
				
				RenderComponent::Sptr renderer = trash2->Add<RenderComponent>();
				renderer->SetMesh(bagtrashMesh);
				renderer->SetMaterial(bagtrashMaterial);
				
				Gameplay::Physics::RigidBody::Sptr physics = trash2->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
				/*Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetPosition(glm::vec3(0.00f, 0.16f, -0.08f));
				box->SetScale(glm::vec3(0.44f, 0.3f, 0.38f));
				physics->AddCollider(box);*/

				Gameplay::Physics::TriggerVolume::Sptr volume = trash2->Add<Gameplay::Physics::TriggerVolume>();
				Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
				box2->SetPosition(glm::vec3(0.00f, 0.25f, -0.05f));
				box2->SetRotation(glm::vec3(0.0f, -3.0f, 0.0f));
				box2->SetScale(glm::vec3(0.66f, 0.21f, 0.58f));
				volume->AddCollider(box2);

				CollectTrashBehaviour::Sptr behaviour2 = trash2->Add<CollectTrashBehaviour>();
				behaviour2->tutorial = true;
			}

		}


		//layout
		Gameplay::MeshResource::Sptr layoutMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("models/tutoriallayoutread.obj");
		Texture2D::Sptr layoutTex = ResourceManager::CreateAsset<Texture2D>("textures/therealthing.jpg");
		Gameplay::Material::Sptr layoutMaterial = ResourceManager::CreateAsset<Gameplay::Material>(deferredForward);
		{
			layoutMaterial->Name = "Layout";
			layoutMaterial->Set("u_Material.AlbedoMap", layoutTex);
			layoutMaterial->Set("u_Material.Shininess", 0.0f);
			layoutMaterial->Set("u_Material.NormalMap", normalMapDefault);

		}
		Gameplay::GameObject::Sptr layout = scene->CreateGameObject("Layout");
		{
			layout->SetPostion(glm::vec3(6.33, -6.59f, 0.0f));
			layout->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
			layout->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

			RenderComponent::Sptr renderer = layout->Add<RenderComponent>();
			renderer->SetMesh(layoutMesh);
			renderer->SetMaterial(layoutMaterial);
			GroundBehaviour::Sptr behaviour = layout->Add<GroundBehaviour>();
		}

		spillMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("models/spill.obj");
		Texture2D::Sptr spillTex = ResourceManager::CreateAsset<Texture2D>("textures/goo.png");
		// Create our material
		spillMaterial = ResourceManager::CreateAsset<Gameplay::Material>(rackShader);
		{
			spillMaterial->Name = "Spill";
			spillMaterial->Set("u_Material.AlbedoMap", spillTex);
			spillMaterial->Set("u_Material.Shininess", 0.f);
			spillMaterial->Set("u_Material.NormalMap", normalMapDefault);

		}

		// Tutorial walls walls
		{
			//Walls
			Gameplay::GameObject::Sptr layoutwall1 = scene->CreateGameObject("Layout Wall Front");
			{
				layoutwall1->SetPostion(glm::vec3(7.00f, -13.750, 0.84f));
				layoutwall1->SetScale(glm::vec3(3.04f, 0.27f, 1.17f));
				Gameplay::Physics::RigidBody::Sptr wall1Phys = layoutwall1->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr wall1 = Gameplay::Physics::BoxCollider::Create();
				//wall1->SetPosition(glm::vec3(11.85f, 3.23f, 1.05f));
				wall1->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall1->SetScale(glm::vec3(3.04f, 0.27f, 1.17f));
				wall1->SetExtents(glm::vec3(2.2f, 2.0f, 2.0f));
				wall1Phys->AddCollider(wall1);
			}

			Gameplay::GameObject::Sptr layoutwall2 = scene->CreateGameObject("Layout Wall Back");
			{
				layoutwall2->SetPostion(glm::vec3(6.410f, 2.380f, 0.84f));
				layoutwall2->SetScale(glm::vec3(1.04f, 0.27f, 1.17f));
				Gameplay::Physics::RigidBody::Sptr wall2Phys = layoutwall2->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr wall2 = Gameplay::Physics::BoxCollider::Create();
				//wall1->SetPosition(glm::vec3(11.85f, 3.23f, 1.05f));
				wall2->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall2->SetScale(glm::vec3(2.460f, 0.27f, 1.17f));
				wall2->SetExtents(glm::vec3(2.3f, 2.0f, 2.0f));
				wall2Phys->AddCollider(wall2);
			}

			//Right Bottom
			Gameplay::GameObject::Sptr layoutwall3 = scene->CreateGameObject("Layout Wall Right Bottom");
			{
				layoutwall3->SetPostion(glm::vec3(0.64f, -0.79f, 1.0f));
				layoutwall3->SetScale(glm::vec3(0.3f, 1.54f, 2.37f));
				Gameplay::Physics::RigidBody::Sptr wall3Phys = layoutwall3->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr wall3 = Gameplay::Physics::BoxCollider::Create();
				wall3->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall3->SetScale(glm::vec3(0.15f, 3.020f, 2.37f));
				wall3->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wall3Phys->AddCollider(wall3);
			}

			//Left Bottom
			Gameplay::GameObject::Sptr layoutwall4 = scene->CreateGameObject("Layout Wall Left Bottom");
			{
				layoutwall4->SetPostion(glm::vec3(11.950f, -0.79f, 1.0f));
				layoutwall4->SetScale(glm::vec3(0.3f, 1.54f, 2.37f));
				Gameplay::Physics::RigidBody::Sptr wall4Phys = layoutwall4->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr wall4 = Gameplay::Physics::BoxCollider::Create();
				wall4->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall4->SetScale(glm::vec3(0.15f, 3.020f, 2.37f));
				wall4->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wall4Phys->AddCollider(wall4);
			}
			
			//Center Left
			Gameplay::GameObject::Sptr layoutwall5 = scene->CreateGameObject("Layout Wall Center Left");
			{
				layoutwall5->SetPostion(glm::vec3(10.450f, -5.26f, 1.0f));
				layoutwall5->SetScale(glm::vec3(0.3f, 1.54f, 2.37f));
				Gameplay::Physics::RigidBody::Sptr wall5Phys = layoutwall5->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr wall5 = Gameplay::Physics::BoxCollider::Create();
				wall5->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall5->SetScale(glm::vec3(2.620, 1.56f, 2.02f));
				wall5->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wall5Phys->AddCollider(wall5);
			}
			//Center Right
			Gameplay::GameObject::Sptr layoutwall6 = scene->CreateGameObject("Layout Wall Center Right");
			{
				layoutwall6->SetPostion(glm::vec3(2.060f, -5.33f, 1.0f));
				layoutwall6->SetScale(glm::vec3(0.3f, 1.54f, 2.37f));
				Gameplay::Physics::RigidBody::Sptr wall6Phys = layoutwall6->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr wall6 = Gameplay::Physics::BoxCollider::Create();
				wall6->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall6->SetScale(glm::vec3(2.670, 1.59f, 2.02f));
				wall6->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wall6Phys->AddCollider(wall6);
			}

			Gameplay::GameObject::Sptr layoutwall7 = scene->CreateGameObject("Layout Wall Top Right");
			{
				layoutwall7->SetPostion(glm::vec3(-3.69, -15.28, 1.0f));
				layoutwall7->SetScale(glm::vec3(0.3f, 1.54f, 2.37f));
				Gameplay::Physics::RigidBody::Sptr wall7Phys = layoutwall7->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr wall7 = Gameplay::Physics::BoxCollider::Create();
				wall7->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall7->SetScale(glm::vec3(5.500f, 4.00f, 2.02f));
				wall7->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wall7Phys->AddCollider(wall7);
			}
			Gameplay::GameObject::Sptr layoutwall8 = scene->CreateGameObject("Layout Wall Top Left");
			{
				layoutwall8->SetPostion(glm::vec3(11.1f, -11.12f, 1.0f));
				layoutwall8->SetScale(glm::vec3(0.3f, 1.54f, 2.37f));
				Gameplay::Physics::RigidBody::Sptr wall8Phys = layoutwall8->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr wall8 = Gameplay::Physics::BoxCollider::Create();
				wall8->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall8->SetScale(glm::vec3(0.310, 3.50f, 2.02f));
				wall8->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wall8Phys->AddCollider(wall8);
			}

			Gameplay::GameObject::Sptr layoutwall9 = scene->CreateGameObject("Layout Wall Top Right 2");
			{
				layoutwall9->SetPostion(glm::vec3(-5.39, -7.f, 1.0f));
				layoutwall9->SetScale(glm::vec3(0.3f, 1.54f, 2.37f));
				Gameplay::Physics::RigidBody::Sptr wall9Phys = layoutwall9->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr wall9 = Gameplay::Physics::BoxCollider::Create();
				wall9->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall9->SetScale(glm::vec3(7.200f, 1.50f, 2.02f));
				wall9->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wall9Phys->AddCollider(wall9);
			}

			Gameplay::GameObject::Sptr layoutwallbarrier = scene->CreateGameObject("Layout Wall Top Right Barrier");
			{
				layoutwallbarrier->SetPostion(glm::vec3(2.13f, -9.88f, 1.0f));
				layoutwallbarrier->SetScale(glm::vec3(0.3f, 1.54f, 2.37f));
				Gameplay::Physics::RigidBody::Sptr wallBPhys = layoutwallbarrier->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr wallB = Gameplay::Physics::BoxCollider::Create();
				wallB->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wallB->SetScale(glm::vec3(0.310, 1.50f, 2.02f));
				wallB->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wallBPhys->AddCollider(wallB);
			}

			Gameplay::GameObject::Sptr layoutwall10 = scene->CreateGameObject("Layout Wall Hall Right");
			{
				layoutwall10->SetPostion(glm::vec3(-12.99f, -13.f, 1.0f));
				layoutwall10->SetScale(glm::vec3(0.3f, 1.54f, 2.37f));
				Gameplay::Physics::RigidBody::Sptr wall10Phys = layoutwall10->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr wall10 = Gameplay::Physics::BoxCollider::Create();
				wall10->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall10->SetScale(glm::vec3(0.310, 6.00f, 2.02f));
				wall10->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wall10Phys->AddCollider(wall10);
			}

			Gameplay::GameObject::Sptr layoutwall11 = scene->CreateGameObject("Layout Wall Hall Top");
			{
				layoutwall11->SetPostion(glm::vec3(-5.39, -19.490, 1.0f));
				layoutwall11->SetScale(glm::vec3(0.3f, 1.54f, 2.37f));
				Gameplay::Physics::RigidBody::Sptr wall11Phys = layoutwall11->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr wall11 = Gameplay::Physics::BoxCollider::Create();
				wall11->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall11->SetScale(glm::vec3(7.200f, 1.50f, 2.02f));
				wall11->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wall11Phys->AddCollider(wall11);
			}

		}

		//bin model
		Gameplay::MeshResource::Sptr binMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("models/BigBenClosed_000001.obj");
		Texture2D::Sptr binTex = ResourceManager::CreateAsset<Texture2D>("textures/bigben.png");
		// Create our material
		Gameplay::Material::Sptr binMaterial = ResourceManager::CreateAsset<Gameplay::Material>(deferredForward);
		{
			binMaterial->Name = "Bin";
			binMaterial->Set("u_Material.AlbedoMap", binTex);
			binMaterial->Set("u_Material.Shininess", 0.5f);
			binMaterial->Set("u_Material.NormalMap", normalMapDefault);
		}
		Gameplay::GameObject::Sptr binM = scene->CreateGameObject("Bin");
		{
			binM->SetPostion(glm::vec3(6.410f, -11.510f, 0.106f));
			binM->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
			binM->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
			// Add a render component
			RenderComponent::Sptr renderer = binM->Add<RenderComponent>();
			renderer->SetMesh(binMesh);
			renderer->SetMaterial(binMaterial);
			// Add a dynamic rigid body to this monkey
			Gameplay::Physics::RigidBody::Sptr physics = binM->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
			Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create(glm::vec3(2.0f, 2.23f, 4.25f));
			box->SetPosition(glm::vec3(0.0f, 0.4f, 0.0f));
			box->SetScale(glm::vec3(0.25f, 0.22f, 0.2f));
			//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
			physics->AddCollider(box);
			//heavy
			//physics->SetMass(10.0f);

			Gameplay::Physics::TriggerVolume::Sptr volume = binM->Add<Gameplay::Physics::TriggerVolume>();
			Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create(glm::vec3(2.0f, 2.23f, 4.25f));
			box2->SetPosition(glm::vec3(0.0f, 0.4f, 0.0f));
			box2->SetScale(glm::vec3(0.3f, 0.22f, 0.22f));
			volume->AddCollider(box2);
			SubmittingTrashBehaviour::Sptr behaviour2 = binM->Add<SubmittingTrashBehaviour>();

			//ANIMATION STUFF////
			MorphMeshRenderer::Sptr morph1 = binM->Add<MorphMeshRenderer>();
			morph1->SetMorphMeshRenderer(binMesh, binMaterial);
			MorphAnimator::Sptr morph2 = binM->Add<MorphAnimator>();

			//idle frames
			//std::vector <MeshResource::Sptr> frames2;
			Gameplay::MeshResource::Sptr binMesh8 = ResourceManager::CreateAsset<Gameplay::MeshResource>("models/BigBenClosed_000001.obj");
			std::vector<Gameplay::MeshResource::Sptr> closed;
			closed.push_back(binMesh8);
			behaviour2->getIdle(closed); //send idle frames to behaviour

			morph2->SetInitial();
			morph2->SetFrameTime(0.2f);
			morph2->SetFrames(closed);

		}
		//bin model
		Gameplay::MeshResource::Sptr bin2Mesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("models/recycle bin.obj");
		Texture2D::Sptr bin2Tex = ResourceManager::CreateAsset<Texture2D>("textures/recycle.jpg");
		// Create our material
		Gameplay::Material::Sptr bin2Material = ResourceManager::CreateAsset<Gameplay::Material>(deferredForward);
		{
			bin2Material->Name = "Bin";
			bin2Material->Set("u_Material.AlbedoMap", bin2Tex);
			bin2Material->Set("u_Material.Shininess", 0.5f);
			bin2Material->Set("u_Material.NormalMap", normalMapDefault);

		}
		Gameplay::GameObject::Sptr binM2 = scene->CreateGameObject("Bin Recycle");
		{
			binM2->SetPostion(glm::vec3(8.57f, -11.510f, 0.106f));
			binM2->SetRotation(glm::vec3(90.0f, 0.0f, 180.0f));
			binM2->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
			// Add a render component
			RenderComponent::Sptr renderer = binM2->Add<RenderComponent>();
			renderer->SetMesh(bin2Mesh);
			renderer->SetMaterial(bin2Material);
			// Add a dynamic rigid body to this monkey
			Gameplay::Physics::RigidBody::Sptr physics = binM2->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
			Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create(glm::vec3(2.0f, 2.23f, 4.25f));
			box->SetPosition(glm::vec3(0.0f, 0.37f, 0.1f));
			box->SetScale(glm::vec3(0.2f, 0.19f, -0.08f));
			//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
			physics->AddCollider(box);
			//heavy
			//physics->SetMass(10.0f);

			Gameplay::Physics::TriggerVolume::Sptr volume = binM2->Add<Gameplay::Physics::TriggerVolume>();
			Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create(glm::vec3(2.0f, 2.23f, 4.25f));
			box2->SetPosition(glm::vec3(0.0f, 0.37f, 0.1f));
			box2->SetScale(glm::vec3(0.29f, 0.19f, -0.12f));
		
			volume->AddCollider(box2);
			SubmittingTrashBehaviour::Sptr behaviour2 = binM2->Add<SubmittingTrashBehaviour>();
			behaviour2->type = "Recycle";


		}
		//Bench
		benchMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("models/bench.obj");
		Texture2D::Sptr benchTex = ResourceManager::CreateAsset<Texture2D>("textures/bench.jpg");

		benchMaterial = ResourceManager::CreateAsset<Gameplay::Material>(deferredForward);
		{
			benchMaterial->Name = "Bench";
			benchMaterial->Set("u_Material.AlbedoMap", benchTex);
			benchMaterial->Set("u_Material.Shininess", 0.0f);
			benchMaterial->Set("u_Material.NormalMap", normalMapDefault);
		}

		//----------------------UI STUFF---------------------------------------------

		Font::Sptr junkDogFont = ResourceManager::CreateAsset<Font>("fonts/JunkDog.otf", 35.f); //Font path, font size
		junkDogFont->Bake();

		Gameplay::GameObject::Sptr feedbackUI = scene->CreateGameObject("Feedback UI");
		{
			RectTransform::Sptr feedbackTransform = feedbackUI->Add<RectTransform>();
			feedbackTransform->SetPosition({ 690, 750 });
			feedbackTransform->SetSize({ 35,35 });

			GuiPanel::Sptr feedbackPanel = feedbackUI->Add<GuiPanel>();
			feedbackPanel->SetColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.f));

			//Temporary for Feedback, lets make this prettier later
			Gameplay::GameObject::Sptr pickupFeedback = scene->CreateGameObject("Pickup Feedback");
			{
				pickupFeedback->Add<RectTransform>();

				GuiText::Sptr text = pickupFeedback->Add<GuiText>();
				text->SetText("Press E to Pickup Trash!");
				text->SetFont(junkDogFont);
				text->SetColor(glm::vec4(1.f, 1.f, 1.f, 1.f));
				text->IsEnabled = false;
			}




			Gameplay::GameObject::Sptr submitFeedback = scene->CreateGameObject("Submit Feedback");
			{
				submitFeedback->Add<RectTransform>();

				GuiText::Sptr text = submitFeedback->Add<GuiText>();
				text->SetText("Press E to Dump the Trash!");
				text->SetFont(junkDogFont);
				text->SetColor(glm::vec4(1.f, 1.f, 1.f, 1.f));
				text->IsEnabled = false;
			}

			Gameplay::GameObject::Sptr returnFeedback = scene->CreateGameObject("Return Feedback");
			{
				returnFeedback->Add<RectTransform>()
					->SetMax({ 0, -80.f });

				GuiText::Sptr text = returnFeedback->Add<GuiText>();
				text->SetText("Return to the Dumpster!");
				text->SetFont(junkDogFont);
				text->SetColor(glm::vec4(1.f, 1.f, 1.f, 1.f));
				text->IsEnabled = false;
			}
			//returnFeedback->Get<GuiText>()->IsEnabled = false;
			feedbackUI->AddChild(pickupFeedback);
			feedbackUI->AddChild(submitFeedback);
			feedbackUI->AddChild(returnFeedback);

		}

		Gameplay::GameObject::Sptr tutorialUICanvas = scene->CreateGameObject("Tutorial Canvas");
		{
			RectTransform::Sptr tutorialTransform = tutorialUICanvas->Add<RectTransform>();
			tutorialTransform->SetMax({ 1280, 720 });
			tutorialTransform->SetPosition({ 690, 750 });

			GuiPanel::Sptr tutorialPanel = tutorialUICanvas->Add<GuiPanel>();
			tutorialPanel->SetColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.f));

			Gameplay::GameObject::Sptr dialogue1 = scene->CreateGameObject("Dialogue 1");
			{
				RectTransform::Sptr transform = dialogue1->Add<RectTransform>();
				transform->SetMax({ 1280, 720 });
				transform->SetSize(glm::vec2(90.f, 50.625f));
				transform->SetPosition(glm::vec2(1115.f, 100.f));

				GuiPanel::Sptr Panel = dialogue1->Add<GuiPanel>();
				Panel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/dialoguetut 1.png"));
				//winPanel->SetColor(glm::vec4(1.f, 1.f, 1.f, 0.f));
				Panel->IsEnabled = false;

			}
			Gameplay::GameObject::Sptr dialogue2 = scene->CreateGameObject("Dialogue 2");
			{
				RectTransform::Sptr transform = dialogue2->Add<RectTransform>();
				transform->SetMax({ 1280, 720 });
				transform->SetSize(glm::vec2(90.f, 50.625f));
				transform->SetPosition(glm::vec2(1115.f, 100.f));

				GuiPanel::Sptr Panel = dialogue2->Add<GuiPanel>();
				Panel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/dialoguetut 5.png"));
				//winPanel->SetColor(glm::vec4(1.f, 1.f, 1.f, 0.f));
				Panel->IsEnabled = false;

			}
			Gameplay::GameObject::Sptr dialogue3 = scene->CreateGameObject("Dialogue 3");
			{
				RectTransform::Sptr transform = dialogue3->Add<RectTransform>();
				transform->SetMax({ 1280, 720 });
				transform->SetSize(glm::vec2(90.f, 50.625f));
				transform->SetPosition(glm::vec2(1115.f, 100.f));

				GuiPanel::Sptr Panel = dialogue3->Add<GuiPanel>();
				Panel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/dialoguetut 2.png"));
				//winPanel->SetColor(glm::vec4(1.f, 1.f, 1.f, 0.f));
				Panel->IsEnabled = false;

			}
			
			Gameplay::GameObject::Sptr dialogue4 = scene->CreateGameObject("Dialogue 4");
			{
				RectTransform::Sptr transform = dialogue4->Add<RectTransform>();
				transform->SetMax({ 1280, 720 });
				transform->SetSize(glm::vec2(90.f, 50.625f));
				transform->SetPosition(glm::vec2(1115.f, 100.f));

				GuiPanel::Sptr Panel = dialogue4->Add<GuiPanel>();
				Panel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/dialoguetut 3.png"));
				//winPanel->SetColor(glm::vec4(1.f, 1.f, 1.f, 0.f));
				Panel->IsEnabled = false;

			}

			Gameplay::GameObject::Sptr dialogue5 = scene->CreateGameObject("Dialogue 5");
			{
				RectTransform::Sptr transform = dialogue5->Add<RectTransform>();
				transform->SetMax({ 1280, 720 });
				transform->SetSize(glm::vec2(90.f, 50.625f));
				transform->SetPosition(glm::vec2(1115.f, 100.f));

				GuiPanel::Sptr Panel = dialogue5->Add<GuiPanel>();
				Panel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/dialoguetut 4.png"));
				//winPanel->SetColor(glm::vec4(1.f, 1.f, 1.f, 0.f));
				Panel->IsEnabled = false;

			}

			
			Gameplay::GameObject::Sptr walkTutorial = scene->CreateGameObject("Walk Tutorial UI");
			{
				RectTransform::Sptr transform = walkTutorial->Add<RectTransform>();
				transform->SetMax({ 360, 202.5 });

				GuiPanel::Sptr walkPanel = walkTutorial->Add<GuiPanel>();
				walkPanel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/Tut3tex.png"));
				//winPanel->SetColor(glm::vec4(1.f, 1.f, 1.f, 0.f));
				walkPanel->IsEnabled = false;

			}

			Gameplay::GameObject::Sptr jumpTutorial = scene->CreateGameObject("Jump Tutorial UI");
			{
				RectTransform::Sptr transform = jumpTutorial->Add<RectTransform>();
				transform->SetMax({ 360, 202.5 });

				GuiPanel::Sptr jumpPanel = jumpTutorial->Add<GuiPanel>();
				jumpPanel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/Tut4tex.png"));
				//winPanel->SetColor(glm::vec4(1.f, 1.f, 1.f, 0.f));
				jumpPanel->IsEnabled = false;

			}

			Gameplay::GameObject::Sptr pickupTutorial = scene->CreateGameObject("Pickup Trash Tutorial UI");
			{
				RectTransform::Sptr transform = pickupTutorial->Add<RectTransform>();
				transform->SetMax({ 360, 202.5 });

				GuiPanel::Sptr pickupPanel = pickupTutorial->Add<GuiPanel>();
				pickupPanel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/Tut1tex.png"));
				//winPanel->SetColor(glm::vec4(1.f, 1.f, 1.f, 0.f));
				pickupPanel->IsEnabled = false;

			}

			Gameplay::GameObject::Sptr dumpTutorial = scene->CreateGameObject("Dump Tutorial UI");
			{
				RectTransform::Sptr transform = dumpTutorial->Add<RectTransform>();
				transform->SetMax({ 360, 202.5 });

				GuiPanel::Sptr dumpPanel = dumpTutorial->Add<GuiPanel>();
				dumpPanel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/Tut7tex.png"));
				//winPanel->SetColor(glm::vec4(1.f, 1.f, 1.f, 0.f));
				dumpPanel->IsEnabled = false;

			}

			Gameplay::GameObject::Sptr spillTutorial = scene->CreateGameObject("Spill Tutorial UI");
			{
				RectTransform::Sptr transform = spillTutorial->Add<RectTransform>();
				transform->SetMax({ 360, 202.5 });

				GuiPanel::Sptr spillPanel = spillTutorial->Add<GuiPanel>();
				spillPanel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/Tut5tex.png"));
				//winPanel->SetColor(glm::vec4(1.f, 1.f, 1.f, 0.f));
				spillPanel->IsEnabled = false;

			}
			Gameplay::GameObject::Sptr inventoryUI = scene->CreateGameObject("Inventory UI");
			{
				RectTransform::Sptr transform = inventoryUI->Add<RectTransform>();
				transform->SetMax(app.GetWindowSize());
				transform->SetPosition(glm::vec2(app.GetWindowSize().x - 100.f, app.GetWindowSize().y - 80));
				transform->SetSize(glm::vec2(25.f, 40.f));

				GuiPanel::Sptr invPanel = inventoryUI->Add<GuiPanel>();
				//invPanel->IsEnabled = false;

			}

			tutorialUICanvas->AddChild(walkTutorial);
			tutorialUICanvas->AddChild(jumpTutorial);
			tutorialUICanvas->AddChild(pickupTutorial);
			tutorialUICanvas->AddChild(dumpTutorial);
			tutorialUICanvas->AddChild(spillTutorial);

		}
		////PARTICLES
		//Gameplay::GameObject::Sptr particles = scene->CreateGameObject("Particles");
		//{
		//	particles->SetPostion(glm::vec3(6.318f, -0.788f, 0.106f));
		//	ParticleSystem::Sptr particleManager = particles->Add<ParticleSystem>();
		//	particleManager->AddEmitter(glm::vec3(6.318f, -0.788f, 0.106f), glm::vec3(0.0f, 5.0f, 1.0f), 5.0f, glm::vec4(1.0f, 0.8f, 0.3f, 1.0f));
		//	//particleManager->IsEnabled = false;

		//	//make it move with trashy???
		//	trashyM->AddChild(particles);
		//	
		//}
		//trashyM->AddChild(particles); //makes it move with him?


		GuiBatcher::SetDefaultTexture(ResourceManager::CreateAsset<Texture2D>("textures/ui/ui-sprite.png"));
		GuiBatcher::SetDefaultBorderRadius(8);

		

		// Save the asset manifest for all the resources we just loaded
		ResourceManager::SaveManifest("scene2-manifest.json");
		// Save the scene to a JSON file
		scene->Save("scene2.json");

		// Send the scene to the application
		app.LoadScene(scene);

		scene->trash = 0;
		scene->score = 0;
		scene->held = 0;

	}
}

void TutorialSceneLayer::_UpdateUIPositions() {
	//Gonna put stuff that makes it adjust positions when the window size changes yadayada
	return;
}

void TutorialSceneLayer::_CreateHallway() {
	
	hallwayMat = _tutcurrentScene->FindObjectByName("Layout")->Get<RenderComponent>()->GetMaterial();
	Gameplay::MeshResource::Sptr halllayoutMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("models/extrahall.obj");


	//Bench

	Gameplay::GameObject::Sptr layouthall = _tutcurrentScene->CreateGameObject("Layout2");
	{
			layouthall->SetPostion(glm::vec3(-2.02f, -10.32f, 0.0f));
			layouthall->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
			layouthall->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

			RenderComponent::Sptr renderer = layouthall->Add<RenderComponent>();
			renderer->SetMesh(halllayoutMesh);
			renderer->SetMaterial(hallwayMat);
			GroundBehaviour::Sptr behaviour = layouthall->Add<GroundBehaviour>();
	}

	Gameplay::GameObject::Sptr bench = _tutcurrentScene->CreateGameObject("Bench1");
	{
		bench->SetPostion(glm::vec3(-0.32f, -10.82f, 0.0f));
		bench->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
		bench->SetScale(glm::vec3(2.0f, 1.0f, 1.5f));

		RenderComponent::Sptr renderer = bench->Add<RenderComponent>();
		renderer->SetMesh(benchMesh);
		renderer->SetMaterial(benchMaterial);

		Gameplay::Physics::RigidBody::Sptr physics = bench->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
		Gameplay::Physics::BoxCollider::Sptr boxCollider = Gameplay::Physics::BoxCollider::Create();
		boxCollider->SetScale(glm::vec3(1.25f, 0.35f, 0.35f));
		physics->AddCollider(boxCollider);
	}

	Gameplay::GameObject::Sptr bench2 = _tutcurrentScene->CreateGameObject("Bench2");
	{
		bench2->SetPostion(glm::vec3(-2.86f, -9.f, 0.26f));
		bench2->SetRotation(glm::vec3(180.f, 0.0f, 15.0f));
		bench2->SetScale(glm::vec3(2.0f, 1.0f, 1.5f));

		RenderComponent::Sptr renderer = bench2->Add<RenderComponent>();
		renderer->SetMesh(benchMesh);
		renderer->SetMaterial(benchMaterial);

		Gameplay::Physics::RigidBody::Sptr physics = bench2->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
		Gameplay::Physics::BoxCollider::Sptr boxCollider = Gameplay::Physics::BoxCollider::Create();
		boxCollider->SetScale(glm::vec3(1.25f, 0.35f, 0.35f));
		physics->AddCollider(boxCollider);
	}

	Gameplay::GameObject::Sptr bench3 = _tutcurrentScene->CreateGameObject("Bench3");
	{
		bench3->SetPostion(glm::vec3(-5.820f, -11.030f, 0.260f));
		bench3->SetRotation(glm::vec3(0.f, 0.0f, -16.f));
		bench3->SetScale(glm::vec3(2.0f, 1.0f, 1.5f));

		RenderComponent::Sptr renderer = bench3->Add<RenderComponent>();
		renderer->SetMesh(benchMesh);
		renderer->SetMaterial(benchMaterial);

		Gameplay::Physics::RigidBody::Sptr physics = bench3->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
		Gameplay::Physics::BoxCollider::Sptr boxCollider = Gameplay::Physics::BoxCollider::Create();
		boxCollider->SetScale(glm::vec3(1.25f, 0.35f, 0.35f));
		physics->AddCollider(boxCollider);
	}

	Gameplay::GameObject::Sptr spillM = _tutcurrentScene->CreateGameObject("Spill");
	{
		spillM->SetPostion(glm::vec3(-6.f, -9.5f, 0.03f));
		spillM->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
		spillM->SetScale(glm::vec3(1.25f, 0.7f, 1.f));
		// Add a render component
		RenderComponent::Sptr renderer = spillM->Add<RenderComponent>();
		renderer->SetMesh(spillMesh);
		renderer->SetMaterial(spillMaterial);
		// Add a dynamic rigid body to this monkey
		//Gameplay::Physics::RigidBody::Sptr physics = spillM->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
		Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
		//box->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
		//box->SetScale(glm::vec3(0.22f, 0.37f, 0.24f));
		//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
		box->SetScale(glm::vec3(0.75f, 0.001f, 0.53f));
		//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
		//physics->AddCollider(box);
		//physics->SetMass(0.0f);
		Gameplay::Physics::TriggerVolume::Sptr volume = spillM->Add<Gameplay::Physics::TriggerVolume>();
		Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
		//box2->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
		box2->SetScale(glm::vec3(1.f, 0.001f, 1.f));
		volume->AddCollider(box2);
		SpillBehaviour::Sptr behaviour = spillM->Add<SpillBehaviour>();
		

	}

	/*Gameplay::GameObject::Sptr bench4 = _tutcurrentScene->CreateGameObject("Bench4");
	{
		bench4->SetPostion(glm::vec3(-9.41f, -8.690f, 0.260f));
		bench4->SetRotation(glm::vec3(90.f, 0.0f, -177.f));
		bench4->SetScale(glm::vec3(1.5f, 1.0f, 1.5f));

		RenderComponent::Sptr renderer = bench4->Add<RenderComponent>();
		renderer->SetMesh(benchMesh);
		renderer->SetMaterial(benchMaterial);

		Gameplay::Physics::RigidBody::Sptr physics = bench4->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
		Gameplay::Physics::BoxCollider::Sptr boxCollider = Gameplay::Physics::BoxCollider::Create();
		boxCollider->SetScale(glm::vec3(1.25f, 0.35f, 0.35f));
		physics->AddCollider(boxCollider);
	}*/

	Gameplay::GameObject::Sptr bench5 = _tutcurrentScene->CreateGameObject("Bench5");
	{
		bench5->SetPostion(glm::vec3(-10.85f, -11.37f, 0.f));
		bench5->SetRotation(glm::vec3(90.f, 0.0f, 0.f));
		bench5->SetScale(glm::vec3(2.8f, 1.f, 1.5f));

		RenderComponent::Sptr renderer = bench5->Add<RenderComponent>();
		renderer->SetMesh(benchMesh);
		renderer->SetMaterial(benchMaterial);

		Gameplay::Physics::RigidBody::Sptr physics = bench5->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
		Gameplay::Physics::BoxCollider::Sptr boxCollider = Gameplay::Physics::BoxCollider::Create();
		boxCollider->SetScale(glm::vec3(1.75f, 0.35f, 0.35f));
		physics->AddCollider(boxCollider);
	}

	Gameplay::GameObject::Sptr trash3 = _tutcurrentScene->CreateGameObject("Trash3");
	{
		trash3->SetPostion(glm::vec3(-10.85f, -15.0f, 0.0f));
		trash3->SetRotation(glm::vec3(90.0f, 0.0f, -92.0f));
		trash3->SetScale(glm::vec3(0.9f, 0.59f, 0.73f));

		RenderComponent::Sptr renderer = trash3->Add<RenderComponent>();
		renderer->SetMesh(bagtrashMesh);
		renderer->SetMaterial(bagtrashMaterial);

		Gameplay::Physics::RigidBody::Sptr physics = trash3->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
		/*Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
		box->SetPosition(glm::vec3(0.00f, 0.16f, -0.08f));
		box->SetScale(glm::vec3(0.44f, 0.3f, 0.38f));
		physics->AddCollider(box);*/

		Gameplay::Physics::TriggerVolume::Sptr volume = trash3->Add<Gameplay::Physics::TriggerVolume>();
		Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
		box2->SetPosition(glm::vec3(0.00f, 0.25f, -0.05f));
		box2->SetRotation(glm::vec3(0.0f, -3.0f, 0.0f));
		box2->SetScale(glm::vec3(0.66f, 0.21f, 0.58f));
		volume->AddCollider(box2);

		CollectTrashBehaviour::Sptr behaviour3 = trash3->Add<CollectTrashBehaviour>();
		behaviour3->tutorial = true;
	}


}