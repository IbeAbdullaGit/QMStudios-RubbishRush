//Abdalla Mohamed - 100795120, Angelina Ratchkov – 100740576

#include <Logging.h>
#include <iostream>
#include <fstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <json.hpp>
#include <fstream>
#include <sstream>
#include <typeindex>
#include <optional>
#include <string>

// GLM math library
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/gtx/common.hpp> // for fmod (floating modulus)

// Graphics
#include "Graphics/IndexBuffer.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/VertexArrayObject.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture2D.h"
#include "Graphics/VertexTypes.h"

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
#include "Gameplay/Components/DeleteObjectBehaviour.h"

// Physics
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/Colliders/PlaneCollider.h"
#include "Gameplay/Physics/Colliders/SphereCollider.h"
#include "Gameplay/Physics/Colliders/ConvexMeshCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Graphics/DebugDraw.h"

//#define LOG_GL_NOTIFICATIONS

/*
	Handles debug messages from OpenGL
	https://www.khronos.org/opengl/wiki/Debug_Output#Message_Components
	@param source    Which part of OpenGL dispatched the message
	@param type      The type of message (ex: error, performance issues, deprecated behavior)
	@param id        The ID of the error or message (to distinguish between different types of errors, like nullref or index out of range)
	@param severity  The severity of the message (from High to Notification)
	@param length    The length of the message
	@param message   The human readable message from OpenGL
	@param userParam The pointer we set with glDebugMessageCallback (should be the game pointer)
*/
void GlDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::string sourceTxt;
	switch (source) {
		case GL_DEBUG_SOURCE_API: sourceTxt = "DEBUG"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceTxt = "WINDOW"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceTxt = "SHADER"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: sourceTxt = "THIRD PARTY"; break;
		case GL_DEBUG_SOURCE_APPLICATION: sourceTxt = "APP"; break;
		case GL_DEBUG_SOURCE_OTHER: default: sourceTxt = "OTHER"; break;
	}
	switch (severity) {
		case GL_DEBUG_SEVERITY_LOW:          LOG_INFO("[{}] {}", sourceTxt, message); break;
		case GL_DEBUG_SEVERITY_MEDIUM:       LOG_WARN("[{}] {}", sourceTxt, message); break;
		case GL_DEBUG_SEVERITY_HIGH:         LOG_ERROR("[{}] {}", sourceTxt, message); break;
			#ifdef LOG_GL_NOTIFICATIONS
		case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_INFO("[{}] {}", sourceTxt, message); break;
			#endif
		default: break;
	}
}

// Stores our GLFW window in a global variable for now
GLFWwindow* window;
// The current size of our window in pixels
glm::ivec2 windowSize = glm::ivec2(800, 800);
// The title of our GLFW window
std::string windowTitle = "INFR-1350U";


//PLAYER SCORE
int score = 0;

// using namespace should generally be avoided, and if used, make sure it's ONLY in cpp files
using namespace Gameplay;
using namespace Gameplay::Physics;

// The scene that we will be rendering
Scene::Sptr scene = nullptr;

void GlfwWindowResizedCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	windowSize = glm::ivec2(width, height);
	if (windowSize.x * windowSize.y > 0) {
		scene->MainCamera->ResizeWindow(width, height);
	}
}

/// <summary>
/// Handles intializing GLFW, should be called before initGLAD, but after Logger::Init()
/// Also handles creating the GLFW window
/// </summary>
/// <returns>True if GLFW was initialized, false if otherwise</returns>
bool initGLFW() {
	// Initialize GLFW
	if (glfwInit() == GLFW_FALSE) {
		LOG_ERROR("Failed to initialize GLFW");
		return false;
	}

	//Create a new GLFW window and make it current
	window = glfwCreateWindow(windowSize.x, windowSize.y, windowTitle.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent(window);
	
	// Set our window resized callback
	glfwSetWindowSizeCallback(window, GlfwWindowResizedCallback);

	return true;
}

/// <summary>
/// Handles initializing GLAD and preparing our GLFW window for OpenGL calls
/// </summary>
/// <returns>True if GLAD is loaded, false if there was an error</returns>
bool initGLAD() {
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		LOG_ERROR("Failed to initialize Glad");
		return false;
	}
	return true;
}

/// <summary>
/// Draws a widget for saving or loading our scene
/// </summary>
/// <param name="scene">Reference to scene pointer</param>
/// <param name="path">Reference to path string storage</param>
/// <returns>True if a new scene has been loaded</returns>
bool DrawSaveLoadImGui(Scene::Sptr& scene, std::string& path) {
	// Since we can change the internal capacity of an std::string,
	// we can do cool things like this!
	ImGui::InputText("Path", path.data(), path.capacity());

	// Draw a save button, and save when pressed
	if (ImGui::Button("Save")) {
		scene->Save(path);
		std::string newFilename = std::filesystem::path(path).stem().string() + "-manifest.json";
		ResourceManager::SaveManifest(newFilename);
		//save brick count
		std::ofstream myFile;
		myFile.open("bricks.txt");
		myFile << scene->brick_count <<"\n";
		myFile.close();

	}
	ImGui::SameLine();
	// Load scene from file button
	if (ImGui::Button("Load")) {
		// Since it's a reference to a ptr, this will
		// overwrite the existing scene!
		std::string newFilename = std::filesystem::path(path).stem().string() + "-manifest.json";
		ResourceManager::LoadManifest(newFilename);
		scene = Scene::Load(path);
		std::string line;
		std::ifstream myFile("bricks.txt");
		if (myFile.is_open())
		{
			while (std::getline(myFile, line))
			{
				scene->brick_count = std::stoi(line);
			}
			myFile.close();
		}
		else
		{
			scene->brick_count = 0;
		}

		return true;
	}
	return false;
}

/// <summary>
/// Draws some ImGui controls for the given light
/// </summary>
/// <param name="title">The title for the light's header</param>
/// <param name="light">The light to modify</param>
/// <returns>True if the parameters have changed, false if otherwise</returns>
bool DrawLightImGui(const Scene::Sptr& scene, const char* title, int ix) {
	bool isEdited = false;
	bool result = false;
	Light& light = scene->Lights[ix];
	ImGui::PushID(&light); // We can also use pointers as numbers for unique IDs
	if (ImGui::CollapsingHeader(title)) {
		isEdited |= ImGui::DragFloat3("Pos", &light.Position.x, 0.01f);
		isEdited |= ImGui::ColorEdit3("Col", &light.Color.r);
		isEdited |= ImGui::DragFloat("Range", &light.Range, 0.1f);

		result = ImGui::Button("Delete");
	}
	if (isEdited) {
		scene->SetShaderLight(ix);
	}

	ImGui::PopID();
	return result;
}
GLfloat movX = 0.0f;

void keyboard() {

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		movX += 0.1f;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		movX -= 0.1f;
}


//RenderObject createBrick(glm::vec3 pos)
//{
//	RenderObject blockM = RenderObject();
//	blockM.Position = pos;
//	blockM.Rotation.z = 180.0f;
//	blockM.Rotation.x = 45.0f;
//	blockM.Name = "Block" + std::to_string(count);
//	blockM.Scale = glm::vec3(0.3f, 0.3f, 0.3f);
//	count += 1;
//	return blockM;
//
//}

int main() {
	//how many balls we have left
	int balls = 3;
	
	Logger::Init(); // We'll borrow the logger from the toolkit, but we need to initialize it

	//Initialize GLFW
	if (!initGLFW())
		return 1;

	//Initialize GLAD
	if (!initGLAD())
		return 1;

	// Let OpenGL know that we want debug output, and route it to our handler function
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GlDebugMessage, nullptr);

	// Initialize our ImGui helper
	ImGuiHelper::Init(window);

	// Initialize our resource manager
	ResourceManager::Init();

	// Register all our resource types so we can load them from manifest files
	ResourceManager::RegisterType<Texture2D>();
	ResourceManager::RegisterType<Shader>();
	ResourceManager::RegisterType<Material>();
	ResourceManager::RegisterType<MeshResource>();

	// Register all of our component types so we can load them from files
	ComponentManager::RegisterType<Camera>();
	ComponentManager::RegisterType<RenderComponent>();
	ComponentManager::RegisterType<RigidBody>();
	ComponentManager::RegisterType<TriggerVolume>();
	ComponentManager::RegisterType<RotatingBehaviour>();
	ComponentManager::RegisterType<JumpBehaviour>();
	ComponentManager::RegisterType<MaterialSwapBehaviour>();
	ComponentManager::RegisterType<TriggerVolumeEnterBehaviour>();
	ComponentManager::RegisterType<DeleteObjectBehaviour>();

	// GL states, we'll enable depth testing and backface fulling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	bool loadScene = false;
	// For now we can use a toggle to generate our scene vs load from file
	if (loadScene) {
		ResourceManager::LoadManifest("Brick Brea-manifest.json");
		scene = Scene::Load("Brick Breaker.json");
		std::string line;
		std::ifstream myFile("bricks.txt");
		if (myFile.is_open())
		{
			while (std::getline(myFile, line))
			{
				scene->brick_count = std::stoi(line);
			}
			myFile.close();
		}
		else
		{
			scene->brick_count = 0;
		}

	}
	else {
		// Create our OpenGL resources
		Shader::Sptr uboShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shader.glsl" }, 
			{ ShaderPartType::Fragment, "shaders/frag_blinn_phong_textured.glsl" }
		}); 

		MeshResource::Sptr playerMesh = ResourceManager::CreateAsset<MeshResource>("Player.obj");
		Texture2D::Sptr playerTex = ResourceManager::CreateAsset<Texture2D>("textures/Player.png");

		MeshResource::Sptr ballMesh = ResourceManager::CreateAsset<MeshResource>("ball.obj");
		Texture2D::Sptr ballTex = ResourceManager::CreateAsset<Texture2D>("textures/Player.png");

		MeshResource::Sptr wallMesh = ResourceManager::CreateAsset<MeshResource>("Wall.obj");
		Texture2D::Sptr wallTex = ResourceManager::CreateAsset<Texture2D>("textures/Wall.png");
		

		
		// Create an empty scene
		scene = std::make_shared<Scene>();

		// I hate this
		scene->BaseShader = uboShader;

		// Create our material
		Material::Sptr playerMaterial = ResourceManager::CreateAsset<Material>();
		{
			playerMaterial->Name = "Player";
			playerMaterial->MatShader = scene->BaseShader;
			playerMaterial->Texture = playerTex;
			playerMaterial->Shininess = 1.0f;
			
		}
		Material::Sptr ballMaterial = ResourceManager::CreateAsset<Material>();
		{
			ballMaterial->Name = "Ball";
			ballMaterial->MatShader = scene->BaseShader;
			ballMaterial->Texture =ballTex;
			ballMaterial->Shininess = 1.0f;

		}
		
		Material::Sptr wallMaterial = ResourceManager::CreateAsset<Material>(); {
			wallMaterial->Name = "Wall";
			wallMaterial->MatShader = scene->BaseShader;
			wallMaterial->Texture = wallTex;
			wallMaterial->Shininess = 1.0f;
		}
		
		// Create some lights for our scene
		scene->Lights.resize(3);
		scene->Lights[0].Position = glm::vec3(0.0f, 8.50f, 3.0f);
		scene->Lights[0].Color = glm::vec3(0.5f, 0.0f, 0.7f);
		scene->Lights[0].Range = 10.0f;

		scene->Lights[1].Position = glm::vec3(1.0f, 5.0f, 3.0f);
		scene->Lights[1].Color = glm::vec3(0.2f, 0.8f, 0.1f);

		scene->Lights[2].Position = glm::vec3(0.0f, 1.0f, 3.0f);
		scene->Lights[2].Color = glm::vec3(1.0f, 0.2f, 0.1f);

		
		// Set up the scene's camera
		GameObject::Sptr camera = scene->CreateGameObject("Main Camera");
		{
			camera->SetPostion(glm::vec3(0, 4, 4));
			camera->LookAt(glm::vec3(0.0f));

			Camera::Sptr cam = camera->Add<Camera>();
			cam->SetOrthoEnabled(true);
			cam->SetOrthoVerticalScale(10.0f);
			// Make sure that the camera is set as the scene's main camera!
			scene->MainCamera = cam;
			
		}

		

		//GameObject::Sptr square = scene->CreateGameObject("Square");
		//{
		//	// Set position in the scene
		//	square->SetPostion(glm::vec3(0.0f, 0.0f, 2.0f));
		//	// Scale down the plane
		//	square->SetScale(glm::vec3(0.5f));

		//	// Create and attach a render component
		//	RenderComponent::Sptr renderer = square->Add<RenderComponent>();
		//	renderer->SetMesh(planeMesh);
		//	renderer->SetMaterial(boxMaterial);

		//	// This object is a renderable only, it doesn't have any behaviours or
		//	// physics bodies attached!
		//}

		//GameObject::Sptr monkey1 = scene->CreateGameObject("Monkey 1");
		//{
		//	// Set position in the scene
		//	monkey1->SetPostion(glm::vec3(1.5f, 0.0f, 1.0f));

		//	// Add some behaviour that relies on the physics body
		//	monkey1->Add<JumpBehaviour>();

		//	// Create and attach a renderer for the monkey
		//	RenderComponent::Sptr renderer = monkey1->Add<RenderComponent>();
		//	renderer->SetMesh(monkeyMesh);
		//	renderer->SetMaterial(monkeyMaterial);

		//	// Add a dynamic rigid body to this monkey
		//	RigidBody::Sptr physics = monkey1->Add<RigidBody>(RigidBodyType::Dynamic);
		//	physics->AddCollider(ConvexMeshCollider::Create());


		//	// We'll add a behaviour that will interact with our trigger volumes
		//	MaterialSwapBehaviour::Sptr triggerInteraction = monkey1->Add<MaterialSwapBehaviour>();
		//	triggerInteraction->EnterMaterial = boxMaterial;
		//	triggerInteraction->ExitMaterial = monkeyMaterial;
		//}

		//GameObject::Sptr monkey2 = scene->CreateGameObject("Complex Object");
		//{
		//	// Set and rotation position in the scene
		//	monkey2->SetPostion(glm::vec3(-1.5f, 0.0f, 1.0f));
		//	monkey2->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));

		//	// Add a render component
		//	RenderComponent::Sptr renderer = monkey2->Add<RenderComponent>();
		//	renderer->SetMesh(monkeyMesh);
		//	renderer->SetMaterial(boxMaterial);



		//	// This is an example of attaching a component and setting some parameters
		//	RotatingBehaviour::Sptr behaviour = monkey2->Add<RotatingBehaviour>();
		//	behaviour->RotationSpeed = glm::vec3(0.0f, 0.0f, -90.0f);
		//}

		GameObject::Sptr playerM = scene->CreateGameObject("Player");
		{
			playerM->SetPostion(glm::vec3(-1.5f, 0.0f, 1.0f));
			playerM->SetRotation(glm::vec3(135.0f, 0.0f, 180.0f));
			playerM->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
			// Add a render component
			RenderComponent::Sptr renderer = playerM->Add<RenderComponent>();
			renderer->SetMesh(playerMesh);
			renderer->SetMaterial(playerMaterial);
			// Add a dynamic rigid body to this monkey
			RigidBody::Sptr physics = playerM->Add<RigidBody>(RigidBodyType::Dynamic);
			BoxCollider::Sptr box = BoxCollider::Create(glm::vec3(6.7f, 1.14f, 0.179f));
			//BoxCollider::Sptr box = BoxCollider::Create(glm::vec3(6.7f, 0.179f, 1.14f));
			//box->SetPosition(playerM->GetPosition());
			box->SetScale(glm::vec3(0.2f, 1.5f, 0.2f));
			physics->AddCollider(box);
			//physics->AddCollider(BoxCollider::Create());
			physics->SetMass(0.0f);
		}
		//TriggerVolume::Sptr volume = playerM->Add<TriggerVolume>();
		GameObject::Sptr ballM = scene->CreateGameObject("Ball");
		{
			ballM->SetPostion(glm::vec3(-1.0f, 0.0f, 2.0f));
			ballM->SetRotation(glm::vec3(0.0f, 0.0f, 180.0f));
			ballM->SetScale(glm::vec3(0.6f, 0.6f, 0.6f));
			// Add a render component
			RenderComponent::Sptr renderer = ballM->Add<RenderComponent>();
			renderer->SetMesh(ballMesh);
			renderer->SetMaterial(ballMaterial);
			// Add a dynamic rigid body to this monkey
			RigidBody::Sptr physics = ballM->Add<RigidBody>(RigidBodyType::Dynamic);
			SphereCollider::Sptr sphere = SphereCollider::Create(0.413f);
			//sphere->SetPosition(ballM->GetPosition());
			sphere->SetScale(glm::vec3(0.6f, 2.0f, 0.6f));
			physics->AddCollider(sphere);
			
			//physics->ApplyImpulse(wForce);
			//physics->ApplyForce(wForce);

		}

		GameObject::Sptr WallM = scene->CreateGameObject("Wall1"); {
			WallM->SetPostion(glm::vec3(5.410f, 0.370f, 2.0f));
			WallM->SetRotation(glm::vec3(-113.f, -91.f, 113.f));
			WallM->SetScale(glm::vec3(0.210f, 2.64f, 5.0f));
			RenderComponent::Sptr renderer = WallM->Add<RenderComponent>();
			renderer->SetMesh(wallMesh);
			renderer->SetMaterial(wallMaterial);

			RigidBody::Sptr physics = WallM->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr box = BoxCollider::Create(glm::vec3(189.f, 80.f, 0.166f));
			//BoxCollider::Sptr box = BoxCollider::Create(glm::vec3(189.f, 0.166f, 80.f));
			box->SetScale(glm::vec3(0.210f, 2.64f, 5.0f));
			box->SetPosition(glm::vec3(0.0f, 0.0f, -0.5f));
			//box->SetPosition(WallM1->GetPosition());
			physics->AddCollider(box);
			physics->SetMass(0.0f);
			
		}

		GameObject::Sptr WallM1 = scene->CreateGameObject("Wall2"); {
			WallM1->SetPostion(glm::vec3(-4.410f, 0.370f, 2.0f));
			WallM1->SetRotation(glm::vec3(-113.f, -91.f, 113.f));
			WallM1->SetScale(glm::vec3(0.210f, 2.64f, 5.0f));
			RenderComponent::Sptr renderer = WallM1->Add<RenderComponent>();
			renderer->SetMesh(wallMesh);
			renderer->SetMaterial(wallMaterial);

			RigidBody::Sptr physics = WallM1->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr box = BoxCollider::Create(glm::vec3(189.f, 80.f, 0.166f));
			//BoxCollider::Sptr box = BoxCollider::Create(glm::vec3(189.f, 0.166f, 80.f));
			box->SetScale(glm::vec3(0.210f, 2.64f, 5.0f));
			box->SetPosition(glm::vec3(0.0f, 0.0f, 1.5f));
			//box->SetPosition(WallM1->GetPosition());
			physics->AddCollider(box);
			physics->SetMass(0.0f);

		}
		GameObject::Sptr WallM2 = scene->CreateGameObject("Cieling"); {
			WallM2->SetPostion(glm::vec3(5.410f, -4.350f, 2.0f));
			WallM2->SetRotation(glm::vec3(-166.f, 137.f, 110.f));
			WallM2->SetScale(glm::vec3(0.210f, 2.64f, 5.0f));
			RenderComponent::Sptr renderer = WallM2->Add<RenderComponent>();
			renderer->SetMesh(wallMesh);
			renderer->SetMaterial(wallMaterial);

			RigidBody::Sptr physics = WallM2->Add<RigidBody>(RigidBodyType::Dynamic);
			BoxCollider::Sptr box = BoxCollider::Create(glm::vec3(189.f, 80.f, 0.166f));
			//BoxCollider::Sptr box = BoxCollider::Create(glm::vec3(189.f, 0.166f, 80.f));
			box->SetScale(glm::vec3(0.210f, 2.64f, 5.0f));
			box->SetPosition(glm::vec3(0.0, 0.0f, 1.0f));
			//box->SetPosition(WallM1->GetPosition());
			physics->AddCollider(box);
			physics->SetMass(0.0f);

		}




		TriggerVolume::Sptr volume2 = ballM->Add<TriggerVolume>();
		// This is an example of attaching a component and setting some parameters
		TriggerVolumeEnterBehaviour::Sptr behaviour = ballM->Add<TriggerVolumeEnterBehaviour>();

		
		
		//volume2->OnEnteredTrigger(volume);
		//volume2->OnLeavingTrigger(volume);
		//volume2->OnTriggerVolumeEntered(playerM->Get<RigidBody>());
		
		// Kinematic rigid bodies are those controlled by some outside controller
		// and ONLY collide with dynamic objects
		// Create a trigger volume for testing how we can detect collisions with objects!

		//GameObject::Sptr trigger = scene->CreateGameObject("Trigger"); 
		//{
		//	TriggerVolume::Sptr volume = trigger->Add<TriggerVolume>();
		//	SphereCollider::Sptr collider = SphereCollider::Create();
		//	collider->SetPosition(ballM->GetPosition());
		//	//Add a dynamic rigid body to ball
		//	volume->AddCollider(collider);
		//}

		//GameObject::Sptr trigger2 = scene->CreateGameObject("Trigger");
		//{
		//	TriggerVolume::Sptr volume = trigger2->Add<TriggerVolume>();
		//	BoxCollider::Sptr collider = BoxCollider::Create();
		//	collider->SetPosition(playerM->GetPosition());
		//	//Add a dynamic rigid body to ball
		//	volume->AddCollider(collider);
		//}

		// Save the asset manifest for all the resources we just loaded
		ResourceManager::SaveManifest("manifest.json");
		// Save the scene to a JSON file
		scene->Save("scene.json");
		scene->brick_count = 0;
	}
	//our score
	scene->score = 0;

	Texture2D::Sptr planeTex = ResourceManager::CreateAsset<Texture2D>("textures/L3P0.png");
	// We'll create a mesh that is a simple plane that we can resize later
	MeshResource::Sptr planeMesh = ResourceManager::CreateAsset<MeshResource>();
	planeMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(1.0f)));
	planeMesh->GenerateMesh();
	Material::Sptr planeMaterial = ResourceManager::CreateAsset<Material>(); {
		planeMaterial->Name = "Plane";
		planeMaterial->MatShader = scene->BaseShader;
		planeMaterial->Texture = planeTex;
		planeMaterial->Shininess = 1.0f;
	}


	// Set up all our sample objects
	GameObject::Sptr plane = scene->CreateGameObject("Plane");
	{
		// Scale up the plane
		plane->SetScale(glm::vec3(10.0F));
		plane->SetRotation(glm::vec3(45.0f, 0.0f, 180.0f));
		plane->SetPostion(glm::vec3(0.0f, -3.75f, -3.92f));

		// Create and attach a RenderComponent to the object to draw our mesh
		RenderComponent::Sptr renderer = plane->Add<RenderComponent>();
		renderer->SetMesh(planeMesh);
		renderer->SetMaterial(planeMaterial);

		// Attach a plane collider that extends infinitely along the X/Y axis
		RigidBody::Sptr physics = plane->Add<RigidBody>(/*static by default*/);
		physics->AddCollider(PlaneCollider::Create());
	}

	// Call scene awake to start up all of our components
	scene->Window = window;
	scene->Awake();

	// We'll use this to allow editing the save/load path
	// via ImGui, note the reserve to allocate extra space
	// for input!
	std::string scenePath = "scene.json"; 
	scenePath.reserve(256); 

	bool isRotating = true;
	float rotateSpeed = 90.0f;

	// Our high-precision timer
	double lastFrame = glfwGetTime();


	BulletDebugMode physicsDebugMode = BulletDebugMode::None;
	float playbackSpeed = 1.0f;

	nlohmann::json editorSceneState;

	//block stuff 
	MeshResource::Sptr blockMesh = ResourceManager::CreateAsset<MeshResource>("Brick.obj");
	Texture2D::Sptr blockTex = ResourceManager::CreateAsset<Texture2D>("textures/Brick1.png");
	Texture2D::Sptr blockTex2 = ResourceManager::CreateAsset<Texture2D>("textures/Brick2.png");
	//create material 
	Material::Sptr blockMaterial = ResourceManager::CreateAsset<Material>();
	{
		blockMaterial->Name = "Block";
		blockMaterial->MatShader = scene->BaseShader;
		blockMaterial->Texture = blockTex;
		blockMaterial->Shininess = 1.0f;
	}
	//create material2 
	Material::Sptr blockMaterial2 = ResourceManager::CreateAsset<Material>();
	{
		blockMaterial2->Name = "Block2";
		blockMaterial2->MatShader = scene->BaseShader;
		blockMaterial2->Texture = blockTex2;
		blockMaterial2->Shininess = 1.0f;
	}
	Texture2D::Sptr ballTex = ResourceManager::CreateAsset<Texture2D>("textures/Player.png");
	Material::Sptr ballMaterial = ResourceManager::CreateAsset<Material>();
	{
		ballMaterial->Name = "Ball";
		ballMaterial->MatShader = scene->BaseShader;
		ballMaterial->Texture = ballTex;
		ballMaterial->Shininess = 1.0f;

	}
	ResourceManager::SaveManifest("manifest.json");
	GameObject::Sptr playerM = scene->FindObjectByName("Player");
	GameObject::Sptr ballM = scene->FindObjectByName("Ball");
	//limit rotation
	ballM->Get<RigidBody>()->SetAngularFactor(glm::vec3(0.0f, 0.0f, 0.0f));
	ballM->Get<RigidBody>()->SetAngularDamping(0.0f);

	//add light to ball
	Light ballLight = Light();
	ballLight.Color = glm::vec3(0.0f, 1.0f, 0.0f);
	ballLight.Range = 5.0f;
	ballLight.Position = ballM->GetPosition();
	scene->Lights.push_back(ballLight);
	scene->SetupShaderAndLights();

	//add light to player
	Light pLight = Light();
	pLight.Color = glm::vec3(1.0f, 0.0f, 0.0f);
	pLight.Range = 5.0f;
	pLight.Position = playerM->GetPosition();
	scene->Lights.push_back(pLight);
	scene->SetupShaderAndLights();

	

	///// Game loop /////
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		ImGuiHelper::StartFrame();

		// Calculate the time since our last frame (dt)
		double thisFrame = glfwGetTime();
		float dt = static_cast<float>(thisFrame - lastFrame);

		// Showcasing how to use the imGui library!
		bool isDebugWindowOpen = ImGui::Begin("Debugging");
		if (isDebugWindowOpen)
		{
			if (ImGui::Button("Add brick"))
			{
				//count += 1;
				scene->brick_count += 1;
				
				GameObject::Sptr blockM = scene->CreateGameObject("Block" + std::to_string(scene->brick_count));
				{
					blockM->SetPostion(glm::vec3(-1.5f, 0.0f, 1.0f));
					blockM->SetRotation(glm::vec3(45.0f, 0.0f, 180.0f));
					blockM->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));
					// Add a render component
					RenderComponent::Sptr renderer = blockM->Add<RenderComponent>();
					renderer->SetMesh(blockMesh);
					renderer->SetMaterial(blockMaterial);
					// Add a dynamic rigid body to this block
					RigidBody::Sptr physics = blockM->Add<RigidBody>(RigidBodyType::Dynamic);
					BoxCollider::Sptr box = BoxCollider::Create(glm::vec3(3.24f, 1.14f, 1.0f));
					box->SetScale(glm::vec3(0.15f, 0.7f, 0.15f));
					physics->AddCollider(box);
					physics->SetMass(0.0f);
					
					// We'll add a behaviour that will interact with our trigger volumes
					TriggerVolume::Sptr volume = blockM->Add<TriggerVolume>();
					// This is an example of attaching a component and setting some parameters
					DeleteObjectBehaviour::Sptr behaviour = blockM->Add<DeleteObjectBehaviour>();
					behaviour->setEnter(blockMaterial2);
					behaviour->setExit(blockMaterial);
					
					//scene->addBricks(blockM);
					
				}
				
			}
			ImGui::Separator();
			if (ImGui::Button("Add brick2"))
			{
				//count += 1;
				scene->brick_count += 1;
				GameObject::Sptr blockM = scene->CreateGameObject("Block" + std::to_string(scene->brick_count));
				{
					blockM->SetPostion(glm::vec3(-1.5f, 0.0f, 1.0f));
					blockM->SetRotation(glm::vec3(45.0f, 0.0f, 180.0f));
					blockM->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));
					// Add a render component
					RenderComponent::Sptr renderer = blockM->Add<RenderComponent>();
					renderer->SetMesh(blockMesh);
					renderer->SetMaterial(blockMaterial2);
					// Add a dynamic rigid body to this block
					RigidBody::Sptr physics = blockM->Add<RigidBody>(RigidBodyType::Dynamic);
					BoxCollider::Sptr box = BoxCollider::Create(glm::vec3(3.24f, 1.14f, 1.0f));
					box->SetScale(glm::vec3(0.15f, 0.7f, 0.15f));
					physics->AddCollider(box);
					physics->SetMass(0.0f);
					// We'll add a behaviour that will interact with our trigger volumes
					TriggerVolume::Sptr volume = blockM->Add<TriggerVolume>();
					// This is an example of attaching a component and setting some parameters
					DeleteObjectBehaviour::Sptr behaviour = blockM->Add<DeleteObjectBehaviour>();
					behaviour->setEnter(blockMaterial2);
					behaviour->setExit(blockMaterial);

					//scene->addBricks(blockM);
				}
				
			}
			ImGui::Separator();
		}
		if (isDebugWindowOpen) {
			// Draws a button to control whether or not the game is currently playing
			static char buttonLabel[64];
			sprintf_s(buttonLabel, "%s###playmode", scene->IsPlaying ? "Exit Play Mode" : "Enter Play Mode");
			if (ImGui::Button(buttonLabel)) {
				// Save scene so it can be restored when exiting play mode
				if (!scene->IsPlaying) {
					editorSceneState = scene->ToJson();
				}

				// Toggle state
				scene->IsPlaying = !scene->IsPlaying;

				// If we've gone from playing to not playing, restore the state from before we started playing
				if (!scene->IsPlaying) {
					scene = nullptr;
					// We reload to scene from our cached state
					scene = Scene::FromJson(editorSceneState);
					// Don't forget to reset the scene's window and wake all the objects!
					scene->Window = window;
					scene->Awake();
				}
			}

			// Make a new area for the scene saving/loading
			ImGui::Separator();
			if (DrawSaveLoadImGui(scene, scenePath)) {
				// C++ strings keep internal lengths which can get annoying
				// when we edit it's underlying datastore, so recalcualte size
				scenePath.resize(strlen(scenePath.c_str()));

				// We have loaded a new scene, call awake to set
				// up all our components
				scene->Window = window;
				scene->Awake();
			}
			ImGui::Separator();
			// Draw a dropdown to select our physics debug draw mode
			if (BulletDebugDraw::DrawModeGui("Physics Debug Mode:", physicsDebugMode)) {
				scene->SetPhysicsDebugDrawMode(physicsDebugMode);
			}
			LABEL_LEFT(ImGui::SliderFloat, "Playback Speed:    ", &playbackSpeed, 0.0f, 10.0f);
			ImGui::Separator();
		}

		// Clear the color and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update our application level uniforms every frame

		// Draw some ImGui stuff for the lights
		if (isDebugWindowOpen) {
			for (int ix = 0; ix < scene->Lights.size(); ix++) {
				char buff[256];
				sprintf_s(buff, "Light %d##%d", ix, ix);
				// DrawLightImGui will return true if the light was deleted
				if (DrawLightImGui(scene, buff, ix)) {
					// Remove light from scene, restore all lighting data
					scene->Lights.erase(scene->Lights.begin() + ix);
					scene->SetupShaderAndLights();
					// Move back one element so we don't skip anything!
					ix--;
				}
			}
			// As long as we don't have max lights, draw a button
			// to add another one
			if (scene->Lights.size() < scene->MAX_LIGHTS) {
				if (ImGui::Button("Add Light")) {
					scene->Lights.push_back(Light());
					scene->SetupShaderAndLights();
				}
			}
			// Split lights from the objects in ImGui
			ImGui::Separator();
		}
		//ballM->SetPostion(glm::vec3(ballM->GetPosition().x, 0.0f, ballM->GetPosition().z));
		ballM->SetPostionZ(0.0f);
	//	glm::vec3 test = playerM->Get<RigidBody>()->GetLinearVelocity();
		//ballM->Get<RigidBody>()->SetLinearVelocity(test);
		dt *= playbackSpeed;
		//change plane background based on players current number of balls and score
	
		if (scene->need_update)
		{
			
			plane->Get<RenderComponent>()->GetMaterial()->Texture = ResourceManager::CreateAsset<Texture2D>("textures/L" + std::to_string(balls) + "P" + std::to_string(scene->score) + ".png");
			
			scene->need_update = false;
		}
		//move player
		keyboard();
		playerM->SetPostion(glm::vec3(movX, 0.0f, -5.0f));
		if (ballM->GetPosition().z < -6.0f)
		{
			balls -= 1;
			scene->need_update = true;
			//reset ball's position
			ballM->SetPostion(glm::vec3(0.0f, 0.0f, 1.0f));
			ballM->Get<RigidBody>()->SetLinearVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
		}


		//Makes sure it doesn't go overbound
		if (ballM->GetPosition().z >= 7.0f) {
			ballM->SetPostion(glm::vec3(ballM->GetPosition().x, ballM->GetPosition().y, 6.9f));
			ballM->Get<RigidBody>()->SetLinearVelocity(glm::vec3(2.0f, 0.0f, -2.0f));
			
		}
		if (balls <= 0)
		{
			//exit game
			exit(0);
		}
		//set lights
		ballLight.Position = ballM->GetPosition();
		pLight.Position = playerM->GetPosition();
		
		//check for collisions?


		// Perform updates for all components
		scene->Update(dt);

		// Grab shorthands to the camera and shader from the scene
		Camera::Sptr camera = scene->MainCamera;

		// Cache the camera's viewprojection
		glm::mat4 viewProj = camera->GetViewProjection();
		DebugDrawer::Get().SetViewProjection(viewProj);

		// Update our worlds physics!
		scene->DoPhysics(dt);


		// Draw object GUIs
		if (isDebugWindowOpen) {
			scene->DrawAllGameObjectGUIs();
		}
		
		// The current material that is bound for rendering
		Material::Sptr currentMat = nullptr;
		Shader::Sptr shader = nullptr;

		// Render all our objects
		ComponentManager::Each<RenderComponent>([&](const RenderComponent::Sptr& renderable) {

			// If the material has changed, we need to bind the new shader and set up our material and frame data
			// Note: This is a good reason why we should be sorting the render components in ComponentManager
			if (renderable->GetMaterial() != currentMat) {
				currentMat = renderable->GetMaterial();
				shader = currentMat->MatShader;

				shader->Bind();
				shader->SetUniform("u_CamPos", scene->MainCamera->GetGameObject()->GetPosition());
				currentMat->Apply();
			}

			// Grab the game object so we can do some stuff with it
			GameObject* object = renderable->GetGameObject();

			// Set vertex shader parameters
			shader->SetUniformMatrix("u_ModelViewProjection", viewProj * object->GetTransform());
			shader->SetUniformMatrix("u_Model", object->GetTransform());
			shader->SetUniformMatrix("u_NormalMatrix", glm::mat3(glm::transpose(glm::inverse(object->GetTransform()))));

			// Draw the object
			renderable->GetMesh()->Draw();
		});


		// End our ImGui window
		ImGui::End();

		VertexArrayObject::Unbind();

		lastFrame = thisFrame;
		ImGuiHelper::EndFrame();
		glfwSwapBuffers(window);
	}

	// Clean up the ImGui library
	ImGuiHelper::Cleanup();

	// Clean up the resource manager
	ResourceManager::Cleanup();

	// Clean up the toolkit logger so we don't leak memory
	Logger::Uninitialize();
	return 0;
}

