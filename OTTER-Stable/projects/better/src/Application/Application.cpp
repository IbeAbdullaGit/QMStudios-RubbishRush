#include "Application/Application.h"

#include <Windows.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Logging.h"
#include "Gameplay/InputEngine.h"
#include "Application/Timing.h"
#include <filesystem>
#include "Layers/GLAppLayer.h"
#include "Utils/FileHelpers.h"
#include "Utils/ResourceManager/ResourceManager.h"
#include "Utils/ImGuiHelper.h"

// Graphics
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/VertexArrayObject.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Texture2D.h"
#include "Graphics/TextureCube.h"
#include "Graphics/VertexTypes.h"
#include "Graphics/Font.h"
#include "Graphics/GuiBatcher.h"
#include "Graphics/Framebuffer.h"

// Gameplay
#include "Gameplay/Material.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"


#include "Gameplay/Physics/Colliders/BoxCollider.h"

// Components
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Components/Camera.h"
#include "Gameplay/Components/RotatingBehaviour.h"
#include "Gameplay/Components/JumpBehaviour.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/MaterialSwapBehaviour.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/SimpleCameraControl.h"
#include "Gameplay/Components/ParticleSystem.h"
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

// GUI
#include "Gameplay/Components/GUI/RectTransform.h"
#include "Gameplay/Components/GUI/GuiPanel.h"
#include "Gameplay/Components/GUI/GuiText.h"
#include "Gameplay/Components/ComponentManager.h"

// Layers
#include "Layers/RenderLayer.h"
#include "Layers/InterfaceLayer.h"
#include "Layers/DefaultSceneLayer.h"
#include "Layers/LogicUpdateLayer.h"
#include "Layers/ImGuiDebugLayer.h"
#include "Layers/InstancedRenderingTestLayer.h"
#include "Layers/ParticleLayer.h"

Application* Application::_singleton = nullptr;
std::string Application::_applicationName = "Rubbish Rush";

#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720

Application::Application() :
	_window(nullptr),
	_windowSize({DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT}),
	_isRunning(false),
	_isEditor(true),
	_windowTitle("Rubbish Rush"),
	_currentScene(nullptr),
	_targetScene(nullptr),
	_renderOutput(nullptr)
{ }

Application::~Application() = default; 

Application& Application::Get() {
	LOG_ASSERT(_singleton != nullptr, "Failed to get application! Get was called before the application was started!");
	return *_singleton;
}

void Application::Start(int argCount, char** arguments) {
	LOG_ASSERT(_singleton == nullptr, "Application has already been started!");
	_singleton = new Application();
	_singleton->_Run();
}

GLFWwindow* Application::GetWindow() { return _window; }

const glm::ivec2& Application::GetWindowSize() const { return _windowSize; }


const glm::uvec4& Application::GetPrimaryViewport() const {
	return _primaryViewport;
}

void Application::SetPrimaryViewport(const glm::uvec4& value) {
	_primaryViewport = value;
}

void Application::ResizeWindow(const glm::ivec2& newSize)
{
	_HandleWindowSizeChanged(newSize);
}

void Application::Quit() {
	_isRunning = false;
}

bool Application::LoadScene(const std::string& path) {
	if (std::filesystem::exists(path)) {

		std::string manifestPath = std::filesystem::path(path).stem().string() + "-manifest.json";
		if (std::filesystem::exists(manifestPath)) {
			LOG_INFO("Loading manifest from \"{}\"", manifestPath);
			ResourceManager::LoadManifest(manifestPath);
		}

		Gameplay::Scene::Sptr scene = Gameplay::Scene::Load(path);
		LoadScene(scene);
		return scene != nullptr;
	}
	return false;
}

void Application::LoadScene(const Gameplay::Scene::Sptr& scene) {
	_targetScene = scene;
}

void Application::SaveSettings()
{
	std::filesystem::path appdata = getenv("APPDATA");
	std::filesystem::path settingsPath = appdata / _applicationName / "app-settings.json";

	if (!std::filesystem::exists(appdata / _applicationName)) {
		std::filesystem::create_directory(appdata / _applicationName);
	}

	FileHelpers::WriteContentsToFile(settingsPath.string(), _appSettings.dump(1, '\t'));
}

std::string TimeCountdown(float DisplayTime) { //Timer Function
	float minutes = floorf(DisplayTime / 60);
	float seconds = floorf(fmodf(DisplayTime, 60));
	//Use string and stringstream so it can work with the GUI
	std::string minString;

	std::stringstream testm; //Minutes
	testm << minutes << ":";
	std::stringstream tests;
	tests << seconds;
	std::stringstream tests10;
	tests10 << "0" << seconds; // Seconds

	//std::cout << testm.str(); //print minutes

	//if secolnds is lower than 10, uses tests10 so 9 - 0 has a 0 to their left else it will print out 59-10 normally
	if (seconds < 10) {
		minString = tests10.str();
	}
	else {
		minString = tests.str();
	}

	return (testm.str() + minString); //Adds the minute and seconds strings together

	//std::cout << timeValue;

}

void Application::_Run()
{
	// TODO: Register layers
	_layers.push_back(std::make_shared<GLAppLayer>());
	_layers.push_back(std::make_shared<DefaultSceneLayer>());
	_layers.push_back(std::make_shared<LogicUpdateLayer>());
	_layers.push_back(std::make_shared<RenderLayer>());
	//_layers.push_back(std::make_shared<ParticleLayer>());
	//_layers.push_back(std::make_shared<InstancedRenderingTestLayer>());
	_layers.push_back(std::make_shared<InterfaceLayer>());

	// If we're in editor mode, we add all the editor layers
	if (_isEditor) {
		_layers.push_back(std::make_shared<ImGuiDebugLayer>());
	}

	// Either load the settings, or use the defaults
	_ConfigureSettings();

	// We'll grab these since we'll need them!
	_windowSize.x = JsonGet(_appSettings, "window_width", DEFAULT_WINDOW_WIDTH);
	_windowSize.y = JsonGet(_appSettings, "window_height", DEFAULT_WINDOW_HEIGHT);

	// By default, we want our viewport to be the whole screen
	_primaryViewport = { 0, 0, _windowSize.x, _windowSize.y };

	// Register all component and resource types
	_RegisterClasses();
	//all conditions to change between in-game conditions and menus
	bool loadScene = false;
	bool isPressed = false;
	bool timerDone = false;
	bool timeleveltDone = false;
	bool lose = false;
	bool Victory = false;


	// Load all layers
	_Load();
	

	// Grab current time as the previous frame
	double lastFrame =  glfwGetTime();

	// Done loading, app is now running!
	_isRunning = true;

	//limit pos of the bin??

	//create points we need for lerping
	std::vector<glm::vec3> pointsPos;
	pointsPos.push_back(glm::vec3(-1.36f, 1.22f, 7.1f));
	pointsPos.push_back(glm::vec3(-1.36f, 1.22f, 3.27f));

	std::vector<glm::vec3> pointsPos2;
	pointsPos2.push_back(glm::vec3(0.5f, 1.49f, 3.29f));
	pointsPos2.push_back(glm::vec3(0.0f, 1.49f, 3.29f));
	pointsPos2.push_back(glm::vec3(-0.5f, 1.49f, 3.29f));
	pointsPos2.push_back(glm::vec3(-1.0f, 1.49f, 3.29f));
	pointsPos2.push_back(glm::vec3(-1.18f, 1.49f, 3.29f));

	std::vector<glm::vec3> pointsS;
	pointsS.push_back(glm::vec3(1.0f, 0.8f, 0.8f));
	pointsS.push_back(glm::vec3(1.0f, 1.8f, 1.4f));
	std::vector<glm::vec3> pointsR;
	pointsR.push_back(glm::vec3(62.0f, 0.0f, 90.0f));
	pointsR.push_back(glm::vec3(100.0f, 0.0f, 90.0f));

	std::vector<glm::vec3> pointsS2;
	pointsS2.push_back(glm::vec3(1.0f, 1.46f, 1.0f));
	pointsS2.push_back(glm::vec3(1.0f, 1.0f, 0.4f));
	std::vector<glm::vec3> pointsR2;
	pointsR2.push_back(glm::vec3(90.0f, 0.0f, 90.0f));
	pointsR2.push_back(glm::vec3(90.0f, 0.0f, 90.0f));

	//Variables for changing stuff in the game
	float timeLoop = 7.0f; //The seconds between start time and the menu that plays in the beginning
	float timelevelt = 65.f; //The time needed to collect all trash
	bool playMenu = true;

	bool start = false;
	bool spressed = false;
	bool isPaused = false;

	//save trash elements for when the victory is there
	
	//_currentScene->CreateGameObject("Submit Feedback");

	//returnUI->Get<GuiText>()->IsEnabled = false;
	//objective->Get<GuiPanel>()->IsEnabled = false;
	//UIText->Get<GuiText>()->IsEnabled = false;
	//trashRemainder->Get<GuiText>()->IsEnabled = false;



	// Infinite loop as long as the application is running
	while (_isRunning) {
		// Handle scene switching
		if (_targetScene != nullptr) {
			_HandleSceneChange();
		}

		// Receive events like input and window position/size changes from GLFW
		glfwPollEvents();



		// Handle closing the app via the close button
		if (glfwWindowShouldClose(_window)) {
			_isRunning = false;
		}

		// Grab the timing singleton instance as a reference
		Timing& timing = Timing::_singleton;

		// Figure out the current time, and the time since the last frame
		double thisFrame = glfwGetTime();
		float dt = static_cast<float>(thisFrame - lastFrame);
		float scaledDt = dt * timing._timeScale;

		// Update all timing values
		timing._unscaledDeltaTime = dt;
		timing._deltaTime = scaledDt;
		timing._timeSinceAppLoad += scaledDt;
		timing._unscaledTimeSinceAppLoad += dt;
		timing._timeSinceSceneLoad += scaledDt;
		timing._unscaledTimeSinceSceneLoad += dt;

		//InputEngine::EndFrame();
		ImGuiHelper::StartFrame();
		
		// Core update loop
		if (_currentScene != nullptr) {
			_Update();
			_LateUpdate();
			_PreRender();
			_RenderScene();
			_PostRender();
			//fetch resources
			Gameplay::GameObject::Sptr trashyM = _currentScene->FindObjectByName("Trashy");
			Gameplay::GameObject::Sptr binM = _currentScene->FindObjectByName("Bin");
			Gameplay::GameObject::Sptr RectangleE = _currentScene->FindObjectByName("Rec");
			Gameplay::GameObject::Sptr TrashyE = _currentScene->FindObjectByName("TrashyE");
			//limit rotation
			trashyM->Get<Gameplay::Physics::RigidBody>()->SetAngularFactor(glm::vec3(0.0f, 0.0f, 0.0f));
			trashyM->Get<Gameplay::Physics::RigidBody>()->SetLinearDamping(0.9f);
			//setup trash
			Gameplay::MeshResource::Sptr trashMesh = _currentScene->FindObjectByName("Trash1")->Get<RenderComponent>()->GetMeshResource();

			// Create our material
			Gameplay::Material::Sptr trashMaterial = _currentScene->FindObjectByName("Trash1")->Get<RenderComponent>()->GetMaterial();

			//UI
			Gameplay::GameObject::Sptr startMenu = _currentScene->FindObjectByName("Start");
			Gameplay::GameObject::Sptr pauseMenu = _currentScene->FindObjectByName("Pause");
			Gameplay::GameObject::Sptr failMenu = _currentScene->FindObjectByName("Fail");
			Gameplay::GameObject::Sptr winMenu = _currentScene->FindObjectByName("Win");
			Gameplay::GameObject::Sptr UIText = _currentScene->FindObjectByName("Time Text");
			Gameplay::GameObject::Sptr trashRemainder = _currentScene->FindObjectByName("Trash Remaining");
			Gameplay::GameObject::Sptr objective = _currentScene->FindObjectByName("Objective UI Canvas");
			Gameplay::GameObject::Sptr returnUI = _currentScene->FindObjectByName("Return Feedback");
			Gameplay::GameObject::Sptr submitUI = _currentScene->FindObjectByName("Submit Feedback");
			if (!start)
			{

				if (glfwGetKey(GetWindow(), GLFW_KEY_ENTER))
				{
					spressed = true;
				}
				if (glfwGetKey(GetWindow(), GLFW_KEY_ESCAPE)) //exit
				{
					exit(0);

				}
				//_currentScene->IsPlaying = false;
				playMenu = false;
				//FREEZE TRASHY
				trashyM->Get<Gameplay::Physics::RigidBody>()->IsEnabled = false;
				submitUI->Get<GuiText>()->IsEnabled = false;

				if (spressed)
				{
					//allows the game to start when enter is pressed, trashy becomes unfrozen so the player can control them
					start = true;
					startMenu->Get<GuiPanel>()->IsEnabled = false;
					trashyM->Get<Gameplay::Physics::RigidBody>()->IsEnabled = true;
					//startMenu->GetScene()->DeleteGameObject
					playMenu = true;
					spressed = false;

				}

			}

			// Core update loop
			if (_currentScene != nullptr) {
				//MENU ANIMATED UPDATED
				if (_currentScene->IsPlaying && !timerDone && playMenu && start)
				{
					//When the game starts a menu plays, this just ensure the menu plays in that amount of time
					if (timeLoop > 0) {
						timerDone = false;
						timeLoop -= dt;
					}
					else {
						timerDone = true;
					}

					if (RectangleE->GetPosition().z > 3.3f)
					{
						RectangleE->UpdateLerp(pointsPos, dt);
					}
					else
					{
						if (TrashyE->GetPosition().x >= -1.22f)
						{
							TrashyE->UpdateCAT(pointsPos2, dt);
						}
						else
						{
							TrashyE->UpdateScale(pointsS, pointsR, dt);
						}
					}
					//FREEZE TRASHY
					if (trashyM->GetPosition().z < 1.0f)
					{
						trashyM->Get<Gameplay::Physics::RigidBody>()->SetLinearVelocity(glm::vec3(0.0f));
					}
					//trashyM->Get<RigidBody>()->IsEnabled = false;
					//trashyM->Get<RigidBody>()->SetMass(0.0f);
				}
				else if (_currentScene->IsPlaying && timerDone && playMenu && start)
				{
					if (TrashyE->GetPosition().x < 0.5f)
					{
						TrashyE->UpdateCAT(pointsPos2, dt);
						TrashyE->UpdateScale(pointsS2, pointsR2, dt);
					}
					else
					{
						if (RectangleE->GetPosition().z < 7.0f)
						{
							RectangleE->UpdateLerp(pointsPos, dt);
						}
					}
					//FREEZE TRASHY
					if (trashyM->GetPosition().z < 1.0f)
					{
						trashyM->Get<Gameplay::Physics::RigidBody>()->SetLinearVelocity(glm::vec3(0.0f));
					}
					if (TrashyE->GetPosition().x >= 0.4f && RectangleE->GetPosition().z >= 6.9f)
					{
						TrashyE->SetPostionZ(-10.0f);
						RectangleE->SetPostionZ(-10.0f);
						//TrashyE->SetDirty(true);
						//RectangleE->SetDirty(true);
						//TrashyE->GetScene()->DeleteGameObject(TrashyE->GetScene()->FindObjectByGUID(TrashyE->GUID));
						//RectangleE->GetScene()->DeleteGameObject(TrashyE->GetScene()->FindObjectByGUID(RectangleE->GUID));
						TrashyE->Get<RenderComponent>()->IsEnabled = false;
						RectangleE->Get<RenderComponent>()->IsEnabled = false;
						//_currentScene->RemoveGameObject(TrashyE); //DONT DELETE WE NEED TO REUSE
						//_currentScene->RemoveGameObject(RectangleE); //DONT DELETE WE NEED TO REUSE
						std::cout << "should be deleted\n";
						playMenu = false;
						//trashyM->Get<RigidBody>()->IsEnabled = true;
						//trashyM->SetDirty(true);

						//enable timer stuff
						UIText->Get<GuiText>()->IsEnabled = true;
						trashRemainder->Get<GuiText>()->IsEnabled = true;
						objective->Get<GuiPanel>()->IsEnabled = true;
					}
				}

				else if (_currentScene->IsPlaying && !playMenu && !timeleveltDone && start)
				{
					//put gui here
					//if all trash is collected, the timer stops, and victory is set to true to show of the victory screen
					if (_currentScene->score == 4)
					{
						//trashRemainder->Get<GuiText>()->SetText(3 - _currentScene->score);
						timeleveltDone = true;
						Victory = true;
					}
					else
					{
						//just make sure the condition of the timer of the menu is completed and to ensure no complications happen
						if (_currentScene->trash == 0) {
							timerDone = true;
						}
						else {

						}
					}
					//If the player does not pause the game, the timer will keep reducing till 0
					if (!isPaused)
					{
						//Will reduce as long as it is greater than 0 and is not completed, also shows off how much trash is remaining
						if (timelevelt > 0 && !timeleveltDone) {
							timelevelt -= dt;
							UIText->Get<GuiText>()->SetText(TimeCountdown(timelevelt));
							trashRemainder->Get<GuiText>()->SetText(std::to_string(4 - _currentScene->score - _currentScene->trash) + " Trash Remaining!");

							//GUI disappears when all trash is collected
							if (4 - _currentScene->score - _currentScene->trash == 0)
							{
								returnUI->Get<GuiText>()->IsEnabled = true;
							}
						}
						//checks if timer reaches 0 if it does the lose screen will get activated
						else if (timelevelt <= 0)
						{
							timeleveltDone = true;
							lose = true;

						}
					}
					else
					{

					}

				}
				else if (timeleveltDone && _currentScene->IsPlaying && start)
				{
					if (lose)
					{


						//end menu
						trashyM->Get<Gameplay::Physics::RigidBody>()->IsEnabled = false;
						failMenu->Get<GuiPanel>()->IsEnabled = true;
						UIText->Get<GuiText>()->IsEnabled = false;
						trashRemainder->Get<GuiText>()->IsEnabled = false;
						objective->Get<GuiPanel>()->IsEnabled = false;
						//pause the timer*****
						if (glfwGetKey(GetWindow(), GLFW_KEY_SPACE)) //return to game
						{
							//trashyM->Get<RigidBody>()->IsEnabled = true; 
							failMenu->Get<GuiPanel>()->IsEnabled = false; //dont show lose menu
							startMenu->Get<GuiPanel>()->IsEnabled = true;

							//reset variables
							lose = false;
							start = false;
							playMenu = false;
							timeLoop = 7.0f;
							timelevelt = 65.f;
							timerDone = false;
							timeleveltDone = false;
							trashyM->SetPostion(glm::vec3(-1.5f, 0.0f, 1.0f)); //reset position to start
							_currentScene->score = 0;
							_currentScene->trash = 0;
							//create trash objects again
							//delete any remaining trash objects
							Gameplay::GameObject::Sptr trash1 = _currentScene->FindObjectByName("Trash1");
							Gameplay::GameObject::Sptr trash2 = _currentScene->FindObjectByName("Trash2");
							Gameplay::GameObject::Sptr trash3 = _currentScene->FindObjectByName("Trash3");
							Gameplay::GameObject::Sptr trash4 = _currentScene->FindObjectByName("Trash4");
							if (trash1 != nullptr)
							{
								_currentScene->RemoveGameObject(trash1);
							}
							if (trash2 != nullptr)
							{
								_currentScene->RemoveGameObject(trash2);
							}
							if (trash3 != nullptr)
							{
								_currentScene->RemoveGameObject(trash3);
							}
							if (trash4 != nullptr)
							{
								_currentScene->RemoveGameObject(trash4);
							}
							//cup collection
							{
								Gameplay::GameObject::Sptr trashM = _currentScene->CreateGameObject("Trash1");
								{
									trashM->SetPostion(glm::vec3(2.75f, 2.27f, 0.0f));
									trashM->SetRotation(glm::vec3(90.0f, 0.0f, -62.0f));
									trashM->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
									// Add a render component
									RenderComponent::Sptr renderer = trashM->Add<RenderComponent>();
									renderer->SetMesh(trashMesh);
									renderer->SetMaterial(trashMaterial);
									// Add a dynamic rigid body to this monkey
									Gameplay::Physics::RigidBody::Sptr physics = trashM->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
									Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
									box->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
									box->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
									//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
									//box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
									//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
									physics->AddCollider(box);
									//physics->SetMass(0.0f);
									Gameplay::Physics::TriggerVolume::Sptr volume = trashM->Add<Gameplay::Physics::TriggerVolume>();
									Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
									box2->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
									box2->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
									volume->AddCollider(box2);
									CollectTrashBehaviour::Sptr behaviour2 = trashM->Add<CollectTrashBehaviour>();



								}
								Gameplay::GameObject::Sptr trash2 = _currentScene->CreateGameObject("Trash2");
								{
									trash2->SetPostion(glm::vec3(6.36f, 2.64f, 0.0f));
									trash2->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
									trash2->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
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
									box2->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
									volume->AddCollider(box2);
									CollectTrashBehaviour::Sptr behaviour2 = trash2->Add<CollectTrashBehaviour>();

								}
								Gameplay::GameObject::Sptr trash3 = _currentScene->CreateGameObject("Trash3");
								{
									trash3->SetPostion(glm::vec3(10.08f, -4.97f, 0.0f));
									trash3->SetRotation(glm::vec3(120.0f, 0.0f, 0.0f));
									trash3->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
									// Add a render component
									RenderComponent::Sptr renderer = trash3->Add<RenderComponent>();
									renderer->SetMesh(trashMesh);
									renderer->SetMaterial(trashMaterial);
									// Add a dynamic rigid body to this monkey
									Gameplay::Physics::RigidBody::Sptr physics = trash3->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
									Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
									box->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
									box->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
									//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
									//box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
									//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
									physics->AddCollider(box);
									//physics->SetMass(0.0f);
									Gameplay::Physics::TriggerVolume::Sptr volume = trash3->Add<Gameplay::Physics::TriggerVolume>();
									Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
									box2->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
									box2->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
									volume->AddCollider(box2);
									CollectTrashBehaviour::Sptr behaviour2 = trash3->Add<CollectTrashBehaviour>();

								}
								Gameplay::GameObject::Sptr trash4 = _currentScene->CreateGameObject("Trash4");
								{
									trash4->SetPostion(glm::vec3(13.680, -1.67f, 0.0f));
									trash4->SetRotation(glm::vec3(120.0f, 0.0f, 0.0f));
									trash4->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
									// Add a render component
									RenderComponent::Sptr renderer = trash4->Add<RenderComponent>();
									renderer->SetMesh(trashMesh);
									renderer->SetMaterial(trashMaterial);
									// Add a dynamic rigid body to this monkey
									Gameplay::Physics::RigidBody::Sptr physics = trash4->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
									Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
									box->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
									box->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
									//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
									//box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
									//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
									physics->AddCollider(box);
									//physics->SetMass(0.0f);
									Gameplay::Physics::TriggerVolume::Sptr volume = trash4->Add<Gameplay::Physics::TriggerVolume>();
									Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
									box2->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
									box2->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
									volume->AddCollider(box2);
									CollectTrashBehaviour::Sptr behaviour2 = trash4->Add<CollectTrashBehaviour>();

								}
							}
							//put menus back in
							TrashyE->Get<RenderComponent>()->IsEnabled = true;
							RectangleE->Get<RenderComponent>()->IsEnabled = true;
							TrashyE->SetPostion(glm::vec3(0.5f, 1.49f, 3.3f));
							RectangleE->SetPostion(glm::vec3(-1.36f, 1.22f, 7.0f));


						}

					}
					if (Victory)
					{
						//winMenu->SetPostion(trashyM->GetPosition() + glm::vec3(0.07f, 0.14f, 1.81f)); //offset from player
						trashyM->Get<Gameplay::Physics::RigidBody>()->IsEnabled = false;
						winMenu->Get<GuiPanel>()->IsEnabled = true;
						UIText->Get<GuiText>()->IsEnabled = false;
						trashRemainder->Get<GuiText>()->IsEnabled = false;
						objective->Get<GuiPanel>()->IsEnabled = false;
						returnUI->Get<GuiText>()->IsEnabled = false;
						//pause the timer*****
						if (glfwGetKey(GetWindow(), GLFW_KEY_SPACE)) //return to game
						{
							//trashyM->Get<RigidBody>()->IsEnabled = true; 
							winMenu->Get<GuiPanel>()->IsEnabled = false; //dont show win menu
							startMenu->Get<GuiPanel>()->IsEnabled = true;
							//reset variables
							Victory = false;
							start = false;
							playMenu = false;
							timeLoop = 7.0f;
							timelevelt = 65.f;
							timerDone = false;
							timeleveltDone = false;
							trashyM->SetPostion(glm::vec3(-1.5f, 0.0f, 1.0f)); //reset position to start
							_currentScene->score = 0;
							_currentScene->trash = 0;
							//create trash objects again

							//cup collection
							{
								Gameplay::GameObject::Sptr trashM = _currentScene->CreateGameObject("Trash1");
								{
									trashM->SetPostion(glm::vec3(2.75f, 2.27f, 0.0f));
									trashM->SetRotation(glm::vec3(90.0f, 0.0f, -62.0f));
									trashM->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
									// Add a render component
									RenderComponent::Sptr renderer = trashM->Add<RenderComponent>();
									renderer->SetMesh(trashMesh);
									renderer->SetMaterial(trashMaterial);
									// Add a dynamic rigid body to this monkey
									Gameplay::Physics::RigidBody::Sptr physics = trashM->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
									Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
									box->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
									box->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
									//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
									//box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
									//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
									physics->AddCollider(box);
									//physics->SetMass(0.0f);
									Gameplay::Physics::TriggerVolume::Sptr volume = trashM->Add<Gameplay::Physics::TriggerVolume>();
									Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
									box2->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
									box2->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
									volume->AddCollider(box2);
									CollectTrashBehaviour::Sptr behaviour2 = trashM->Add<CollectTrashBehaviour>();



								}
								Gameplay::GameObject::Sptr trash2 = _currentScene->CreateGameObject("Trash2");
								{
									trash2->SetPostion(glm::vec3(6.36f, 2.64f, 0.0f));
									trash2->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
									trash2->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
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
									box2->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
									volume->AddCollider(box2);
									CollectTrashBehaviour::Sptr behaviour2 = trash2->Add<CollectTrashBehaviour>();

								}
								Gameplay::GameObject::Sptr trash3 = _currentScene->CreateGameObject("Trash3");
								{
									trash3->SetPostion(glm::vec3(10.08f, -4.97f, 0.0f));
									trash3->SetRotation(glm::vec3(120.0f, 0.0f, 0.0f));
									trash3->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
									// Add a render component
									RenderComponent::Sptr renderer = trash3->Add<RenderComponent>();
									renderer->SetMesh(trashMesh);
									renderer->SetMaterial(trashMaterial);
									// Add a dynamic rigid body to this monkey
									Gameplay::Physics::RigidBody::Sptr physics = trash3->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
									Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
									box->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
									box->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
									//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
									//box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
									//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
									physics->AddCollider(box);
									//physics->SetMass(0.0f);
									Gameplay::Physics::TriggerVolume::Sptr volume = trash3->Add<Gameplay::Physics::TriggerVolume>();
									Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
									box2->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
									box2->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
									volume->AddCollider(box2);
									CollectTrashBehaviour::Sptr behaviour2 = trash3->Add<CollectTrashBehaviour>();

								}
								Gameplay::GameObject::Sptr trash4 = _currentScene->CreateGameObject("Trash4");
								{
									trash4->SetPostion(glm::vec3(13.680, -1.67f, 0.0f));
									trash4->SetRotation(glm::vec3(120.0f, 0.0f, 0.0f));
									trash4->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
									// Add a render component
									RenderComponent::Sptr renderer = trash4->Add<RenderComponent>();
									renderer->SetMesh(trashMesh);
									renderer->SetMaterial(trashMaterial);
									// Add a dynamic rigid body to this monkey
									Gameplay::Physics::RigidBody::Sptr physics = trash4->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
									Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
									box->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
									box->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
									//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
									//box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
									//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
									physics->AddCollider(box);
									//physics->SetMass(0.0f);
									Gameplay::Physics::TriggerVolume::Sptr volume = trash4->Add<Gameplay::Physics::TriggerVolume>();
									Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
									box2->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
									box2->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
									volume->AddCollider(box2);
									CollectTrashBehaviour::Sptr behaviour2 = trash4->Add<CollectTrashBehaviour>();

								}
							}
							//put menus back in
							TrashyE->Get<RenderComponent>()->IsEnabled = true;
							RectangleE->Get<RenderComponent>()->IsEnabled = true;
							TrashyE->SetPostion(glm::vec3(0.5f, 1.49f, 3.3f));
							RectangleE->SetPostion(glm::vec3(-1.36f, 1.22f, 7.0f));


						}
						if (glfwGetKey(GetWindow(), GLFW_KEY_ESCAPE)) //exit
						{
							exit(0);

						}



					}
				}
				//PAUSE MENU
				if (start && !playMenu)
				{
					if (glfwGetKey(GetWindow(), GLFW_KEY_ESCAPE) && !isPaused)
					{
						isPaused = true;
						//pauseMenu->SetPostion(trashyM->GetPosition() + glm::vec3(0.07f, 0.14f, 1.81f)); //offset from player
						UIText->Get<GuiText>()->IsEnabled = false;
						trashRemainder->Get<GuiText>()->IsEnabled = false;
						objective->Get<GuiPanel>()->IsEnabled = false;
						trashyM->Get<Gameplay::Physics::RigidBody>()->IsEnabled = false;
						pauseMenu->Get<GuiPanel>()->IsEnabled = true;

					}
					if (isPaused)
					{

						//pause the timer*****
						if (glfwGetKey(GetWindow(), GLFW_KEY_ENTER)) //return to game
						{
							trashyM->Get<Gameplay::Physics::RigidBody>()->IsEnabled = true;
							pauseMenu->Get<GuiPanel>()->IsEnabled = false;
							isPaused = false;
							UIText->Get<GuiText>()->IsEnabled = true;
							trashRemainder->Get<GuiText>()->IsEnabled = true;
							objective->Get<GuiPanel>()->IsEnabled = true;
						}
					}
				}


				//PROBABLY FIND A BETTER WAY TO DO THIS
			// Grab shorthands to the camera and shader from the _currentScene
				Gameplay::Camera::Sptr camera = _currentScene->MainCamera;

				camera->GetGameObject()->SetPostion(trashyM->GetPosition() + glm::vec3(0.0f, 4.00f, 5.7f));
				camera->GetGameObject()->LookAt(trashyM->GetPosition() + glm::vec3(0.0f, -4.0f, -2.0f));
			}

		}

		// Store timing for next loop
		lastFrame = thisFrame;

		InputEngine::EndFrame();
		ImGuiHelper::EndFrame();

		glfwSwapBuffers(_window);

	}

	// Unload all our layers
	_Unload();
}

void Application::_RegisterClasses()
{
	//using namespace Gameplay;
	//using namespace Gameplay::Physics;

	// Initialize our resource manager
	ResourceManager::Init();

	// Register all our resource types so we can load them from manifest files
	ResourceManager::RegisterType<Texture2D>();
	ResourceManager::RegisterType<TextureCube>();
	ResourceManager::RegisterType<ShaderProgram>();
	ResourceManager::RegisterType<Gameplay::Material>();
	ResourceManager::RegisterType<Gameplay::MeshResource>();
	ResourceManager::RegisterType<Font>();
	ResourceManager::RegisterType<Framebuffer>();

	// Register all of our component types so we can load them from files
	Gameplay::ComponentManager::RegisterType<Gameplay::Camera>();
	Gameplay::ComponentManager::RegisterType<RenderComponent>();
	Gameplay::ComponentManager::RegisterType<Gameplay::Physics::RigidBody>();
	Gameplay::ComponentManager::RegisterType<Gameplay::Physics::TriggerVolume>();
	Gameplay::ComponentManager::RegisterType<RotatingBehaviour>();
	Gameplay::ComponentManager::RegisterType<JumpBehaviour>();
	Gameplay::ComponentManager::RegisterType<MaterialSwapBehaviour>();
	Gameplay::ComponentManager::RegisterType<TriggerVolumeEnterBehaviour>();
	Gameplay::ComponentManager::RegisterType<SimpleCameraControl>();
	Gameplay::ComponentManager::RegisterType<RectTransform>();
	Gameplay::ComponentManager::RegisterType<GuiPanel>();
	Gameplay::ComponentManager::RegisterType<GuiText>();
	Gameplay::ComponentManager::RegisterType<ParticleSystem>();

	Gameplay::ComponentManager::RegisterType<DeleteObjectBehaviour>();
	Gameplay::ComponentManager::RegisterType<CollectTrashBehaviour>();
	Gameplay::ComponentManager::RegisterType<SubmittingTrashBehaviour>();
	Gameplay::ComponentManager::RegisterType<PlayerMovementBehavior>();
	Gameplay::ComponentManager::RegisterType<ConveyorBeltBehaviour>();
	Gameplay::ComponentManager::RegisterType<SpillBehaviour>();
	Gameplay::ComponentManager::RegisterType<SteeringBehaviour>();
	Gameplay::ComponentManager::RegisterType<FollowBehaviour>();
	Gameplay::ComponentManager::RegisterType<MorphAnimator>();
	Gameplay::ComponentManager::RegisterType<MorphMeshRenderer>();

}

void Application::_Load() {
	for (const auto& layer : _layers) {
		if (layer->Enabled && *(layer->Overrides & AppLayerFunctions::OnAppLoad)) {
			layer->OnAppLoad(_appSettings);
		}
	}

	// Pass the window to the input engine and let it initialize itself
	InputEngine::Init(_window);
	
	// Initialize our ImGui helper
	ImGuiHelper::Init(_window);

	GuiBatcher::SetWindowSize(_windowSize);
}

void Application::_Update() {
	for (const auto& layer : _layers) {
		if (layer->Enabled && *(layer->Overrides & AppLayerFunctions::OnUpdate)) {
			layer->OnUpdate();
		}
	}
}

void Application::_LateUpdate() {
	for (const auto& layer : _layers) {
		if (layer->Enabled && *(layer->Overrides & AppLayerFunctions::OnLateUpdate)) {
			layer->OnLateUpdate();
		}
	}
}

void Application::_PreRender()
{
	glm::ivec2 size ={ 0, 0 };
	glfwGetWindowSize(_window, &size.x, &size.y);
	glViewport(0, 0, size.x, size.y);
	glScissor(0, 0, size.x, size.y);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	for (const auto& layer : _layers) {
		if (layer->Enabled && *(layer->Overrides & AppLayerFunctions::OnPreRender)) {
			layer->OnPreRender();
		}
	}
}

void Application::_RenderScene() {

	Framebuffer::Sptr result = nullptr;
	for (const auto& layer : _layers) {
		if (layer->Enabled && *(layer->Overrides & AppLayerFunctions::OnRender)) {
			layer->OnRender(result);
			Framebuffer::Sptr layerResult = layer->GetRenderOutput();
			result = layerResult != nullptr ? layerResult : result;
		}
	}
	_renderOutput = result;

}

void Application::_PostRender() {
	// Note that we use a reverse iterator for post render
	for (auto it = _layers.crbegin(); it != _layers.crend(); it++) {
		const auto& layer = *it;
		if (layer->Enabled && *(layer->Overrides & AppLayerFunctions::OnPostRender)) {
			layer->OnPostRender();
			Framebuffer::Sptr layerResult = layer->GetPostRenderOutput();
			_renderOutput = layerResult != nullptr ? layerResult : _renderOutput;
		}
	}

	// We can use the application's viewport to set our OpenGL viewport, as well as clip rendering to that area
	const glm::uvec4& viewport = GetPrimaryViewport();
	glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
	glScissor(viewport.x, viewport.y, viewport.z, viewport.w);

	// If we have a final output, blit it to the screen
	if (_renderOutput != nullptr) {
		_renderOutput->Unbind();

		glm::ivec2 windowSize = _windowSize;
		if (_isEditor) {
			glfwGetWindowSize(_window, &windowSize.x, &windowSize.y);
		}
		//glViewport(0, 0, windowSize.x, windowSize.y);
		glm::ivec4 viewportMinMax ={ viewport.x, viewport.y, viewport.x + viewport.z, viewport.y + viewport.w };

		_renderOutput->Bind(FramebufferBinding::Read);
		glBindFramebuffer(*FramebufferBinding::Write, 0);
		Framebuffer::Blit({ 0, 0, _renderOutput->GetWidth(), _renderOutput->GetHeight() }, viewportMinMax, BufferFlags::All, MagFilter::Nearest);
	}
}

void Application::_Unload() {
	// Note that we use a reverse iterator for unloading
	for (auto it = _layers.crbegin(); it != _layers.crend(); it++) {
		const auto& layer = *it;
		if (layer->Enabled && *(layer->Overrides & AppLayerFunctions::OnAppUnload)) {
			layer->OnAppUnload();
		}
	}

	// Clean up ImGui
	ImGuiHelper::Cleanup();
}

void Application::_HandleSceneChange() {
	// If we currently have a current scene, let the layers know it's being unloaded
	if (_currentScene != nullptr) {
		// Note that we use a reverse iterator, so that layers are unloaded in the opposite order that they were loaded
		for (auto it = _layers.crbegin(); it != _layers.crend(); it++) {
			const auto& layer = *it;
			if (layer->Enabled && *(layer->Overrides & AppLayerFunctions::OnSceneUnload)) {
				layer->OnSceneUnload();
			}
		}
	}

	_currentScene = _targetScene;
	
	// Let the layers know that we've loaded in a new scene
	for (const auto& layer : _layers) {
		if (layer->Enabled && *(layer->Overrides & AppLayerFunctions::OnSceneLoad)) {
			layer->OnSceneLoad();
		}
	}

	// Wake up all game objects in the scene
	_currentScene->Awake();

	// If we are not in editor mode, scenes play by default
	if (!_isEditor) {
		_currentScene->IsPlaying = true;
	}

	_targetScene = nullptr;
}

void Application::_HandleWindowSizeChanged(const glm::ivec2& newSize) {
	for (const auto& layer : _layers) {
		if (layer->Enabled && *(layer->Overrides & AppLayerFunctions::OnWindowResize)) {
			layer->OnWindowResize(_windowSize, newSize);
		}
	}
	_windowSize = newSize;
	_primaryViewport = { 0, 0, newSize.x, newSize.y };
}

void Application::_ConfigureSettings() {
	// Start with the defaul application settings
	_appSettings = _GetDefaultAppSettings();

	// We'll store our settings in the %APPDATA% directory, under our application name
	std::filesystem::path appdata = getenv("APPDATA");
	std::filesystem::path settingsPath = appdata / _applicationName / "app-settings.json";

	// If the settings file exists, we can load it in!
	if (std::filesystem::exists(settingsPath)) {
		// Read contents and parse into a JSON blob
		std::string content = FileHelpers::ReadFile(settingsPath.string());
		nlohmann::json blob = nlohmann::json::parse(content);

		// We use merge_patch so that we can keep our defaults if they are missing from the file!
		_appSettings.merge_patch(blob);
	}
	// If the file does not exist, save the default application settings to the path
	else {
		SaveSettings();
	}
}

nlohmann::json Application::_GetDefaultAppSettings()
{
	nlohmann::json result ={};

	for (const auto& layer : _layers) {
		if (!layer->Name.empty()) {
			result[layer->Name] = layer->GetDefaultConfig();
		}
		else {
			LOG_WARN("Unnamed layer! Injecting settings into global namespace, may conflict with other layers!");
			result.merge_patch(layer->GetDefaultConfig());
		}
	}

	result["window_width"]  = DEFAULT_WINDOW_WIDTH;
	result["window_height"] = DEFAULT_WINDOW_HEIGHT;
	return result;
}

