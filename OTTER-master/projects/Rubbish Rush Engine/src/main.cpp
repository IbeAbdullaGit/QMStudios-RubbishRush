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
#include "Gameplay/Components/CollectTrashBehaviour.h"
#include "Gameplay/Components/SubmittingTrashBehaviour.h"
#include "Gameplay/Components/PlayerMovementBehavior.h"

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
std::string windowTitle = "RubbishRush";



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
		
	}
	ImGui::SameLine();
	// Load scene from file button
	if (ImGui::Button("Load")) {
		// Since it's a reference to a ptr, this will
		// overwrite the existing scene!
		std::string newFilename = std::filesystem::path(path).stem().string() + "-manifest.json";
		ResourceManager::LoadManifest(newFilename);
		scene = Scene::Load(path);
		

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
GLfloat movZ = 0.0f;

//void keyboard(RigidBody::Sptr& body) {
//
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//	{
//		movX += 1.0f;
//		if (movX > 3)
//		{
//			movX = 3;
//		}
//		body->SetLinearVelocity(glm::vec3(1.0f, 0.0f, 0.0f) * movX);
//	}
//	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//	{
//		movX += 1.0f;
//		if (movX > 3)
//		{
//			movX = 3;
//		}
//		body->SetLinearVelocity(glm::vec3(-1.0f, 0.0f, 0.0f) * movX);
//	}
//	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//	{
//		movZ += 1.0f;
//		if (movZ > 3)
//		{
//			movZ = 3;
//		}
//		body->SetLinearVelocity(glm::vec3(0.0f, -1.0f, 0.0f) * movZ);
//	}
//	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//	{
//		movZ += 1.0f;
//		if (movZ > 3)
//		{
//			movZ = 3;
//		}
//		body->SetLinearVelocity(glm::vec3(0.0f, 1.0f, 0.0f) * movZ);
//	}
//	else
//	{
//		if (movX > 0)
//		{
//			movX -= 1; //second slope
//		}
//		if (movZ > 0)
//		{
//			movZ -= 1; //second slope
//		}
//	}
//
//	
//	
//}


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
	ComponentManager::RegisterType<CollectTrashBehaviour>();
	ComponentManager::RegisterType<SubmittingTrashBehaviour>();
	ComponentManager::RegisterType<PlayerMovementBehavior>();

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

	}
	else {
		// Create our OpenGL resources
		Shader::Sptr uboShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shader.glsl" }, 
			{ ShaderPartType::Fragment, "shaders/frag_blinn_phong_textured.glsl" }
		}); 

		
		// Create an empty scene
		scene = std::make_shared<Scene>();

		// I hate this
		scene->BaseShader = uboShader;

		
		
		// Create some lights for our scene
		scene->Lights.resize(3);
		scene->Lights[0].Position = glm::vec3(0.0f, 8.50f, 3.0f);
		scene->Lights[0].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[0].Range = 10.0f;

		scene->Lights[1].Position = glm::vec3(1.0f, 5.0f, 3.0f);
		scene->Lights[1].Color = glm::vec3(1.0f, 1.0f, 1.1f);

		scene->Lights[2].Position = glm::vec3(0.0f, 1.0f, 3.0f);
		scene->Lights[2].Color = glm::vec3(1.0f, 1.0f, 1.0f);

		
		// Set up the scene's camera
		GameObject::Sptr camera = scene->CreateGameObject("Main Camera");
		{
			camera->SetPostion(glm::vec3(-1.42f, 4.69f, 5.73f));
			//camera->SetPostion(glm::vec3(-1.42f, 18.67f, 17.420));
			camera->LookAt(glm::vec3(0.0f));
			camera->SetRotation(glm::vec3(59.0f, 0.0f, 177.0f));
			camera->SetScale(glm::vec3(1.0f, 1.0f, 3.1f));

			Camera::Sptr cam = camera->Add<Camera>();
			//cam->SetOrthoEnabled(true);
			//cam->SetOrthoVerticalScale(10.0f);
			// Make sure that the camera is set as the scene's main camera!
			scene->MainCamera = cam;
			
		}
		//setup trashy
		MeshResource::Sptr trashyMesh = ResourceManager::CreateAsset<MeshResource>("trashy.obj");
		Texture2D::Sptr trashyTex = ResourceManager::CreateAsset<Texture2D>("textures/trashyTEX.png");
		// Create our material
		Material::Sptr trashyMaterial = ResourceManager::CreateAsset<Material>();
		{
			trashyMaterial->Name = "Trashy";
			trashyMaterial->MatShader = scene->BaseShader;
			trashyMaterial->Texture = trashyTex;
			trashyMaterial->Shininess = 1.0f;

		}
		GameObject::Sptr trashyM = scene->CreateGameObject("Trashy");
		{
			trashyM->SetPostion(glm::vec3(-1.5f, 0.0f, 2.0f));
			trashyM->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			trashyM->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
			// Add a render component
			RenderComponent::Sptr renderer = trashyM->Add<RenderComponent>();
			renderer->SetMesh(trashyMesh);
			renderer->SetMaterial(trashyMaterial);
			// Add a dynamic rigid body to this monkey
			RigidBody::Sptr physics = trashyM->Add<RigidBody>(RigidBodyType::Dynamic);
			BoxCollider::Sptr box = BoxCollider::Create(glm::vec3(1.51f, 2.68f, 0.831f));
			
			box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
			box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
			box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
			physics->AddCollider(box);
			//physics->AddCollider(BoxCollider::Create());
			//physics->SetMass(0.0f);
			//add trigger for collisions and behaviours
			TriggerVolume::Sptr volume = trashyM->Add<TriggerVolume>();
			BoxCollider::Sptr box2 = BoxCollider::Create(glm::vec3(1.51f, 2.68f, 0.831f));

			box2->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
			box2->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
			box2->SetExtents(glm::vec3(0.8, 2.68, 0.83));
			volume->AddCollider(box2);
			JumpBehaviour::Sptr behaviour = trashyM->Add<JumpBehaviour>();
			//CollectTrashBehaviour::Sptr behaviour2 = trashyM->Add<CollectTrashBehaviour>();

			PlayerMovementBehavior::Sptr movement = trashyM->Add<PlayerMovementBehavior>();
		}
		

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
			plane->SetRotation(glm::vec3(9.0f, 0.0f, 0.0f));
			// Make a big tiled mesh
			MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(100.0f), glm::vec2(20.0f)));
			tiledMesh->GenerateMesh();
			

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = plane->Add<RenderComponent>();
			renderer->SetMesh(tiledMesh);
			renderer->SetMaterial(planeMaterial);

			// Attach a plane collider that extends infinitely along the X/Y axis
			RigidBody::Sptr physics = plane->Add<RigidBody>(RigidBodyType::Kinematic);
			physics->AddCollider(BoxCollider::Create(glm::vec3(50.0f, 50.0f, 1.0f)))->SetPosition({ 0,0,-1 });
		}
		//placeholder trash object
		//setup trash
		MeshResource::Sptr trashMesh = ResourceManager::CreateAsset<MeshResource>("plant.obj");
		Texture2D::Sptr trashTex = ResourceManager::CreateAsset<Texture2D>("textures/planttex.png");
		// Create our material
		Material::Sptr trashMaterial = ResourceManager::CreateAsset<Material>();
		{
			trashMaterial->Name = "Trash";
			trashMaterial->MatShader = scene->BaseShader;
			trashMaterial->Texture = trashTex;
			trashMaterial->Shininess = 1.0f;

		}
		GameObject::Sptr trashM = scene->CreateGameObject("Trash");
		{
			trashM->SetPostion(glm::vec3(-1.5f, -2.0f, -0.16f));
			trashM->SetRotation(glm::vec3(120.0f, 0.0f, 0.0f));
			trashM->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
			// Add a render component
			RenderComponent::Sptr renderer = trashM->Add<RenderComponent>();
			renderer->SetMesh(trashMesh);
			renderer->SetMaterial(trashMaterial);
			// Add a dynamic rigid body to this monkey
			RigidBody::Sptr physics = trashM->Add<RigidBody>(RigidBodyType::Kinematic);
			BoxCollider::Sptr box = BoxCollider::Create();
			box->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
			box->SetScale(glm::vec3(0.22f, 0.37f, 0.24f));
			//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
			//box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
			//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
			physics->AddCollider(box);
			//physics->SetMass(0.0f);
			TriggerVolume::Sptr volume = trashM->Add<TriggerVolume>();
			BoxCollider::Sptr box2 = BoxCollider::Create();
			box2->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
			box2->SetScale(glm::vec3(0.22f, 0.37f, 0.24f));
			volume->AddCollider(box2);
			CollectTrashBehaviour::Sptr behaviour2 = trashM->Add<CollectTrashBehaviour>();

		}
		//bin model
		MeshResource::Sptr binMesh = ResourceManager::CreateAsset<MeshResource>("Big_Bin2.obj");
		Texture2D::Sptr binTex = ResourceManager::CreateAsset<Texture2D>("textures/big_bintex2.png");
		// Create our material
		Material::Sptr binMaterial = ResourceManager::CreateAsset<Material>();
		{
			binMaterial->Name = "Bin";
			binMaterial->MatShader = scene->BaseShader;
			binMaterial->Texture = binTex;
			binMaterial->Shininess = 1.0f;

		}
		GameObject::Sptr binM = scene->CreateGameObject("Bin");
		{
			binM->SetPostion(glm::vec3(-1.5f, 2.0f, 0.76f));
			binM->SetRotation(glm::vec3(90.0f, -36.0f, 90.0f));
			binM->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
			// Add a render component
			RenderComponent::Sptr renderer = binM->Add<RenderComponent>();
			renderer->SetMesh(binMesh);
			renderer->SetMaterial(binMaterial);
			// Add a dynamic rigid body to this monkey
			RigidBody::Sptr physics = binM->Add<RigidBody>(RigidBodyType::Kinematic);
			BoxCollider::Sptr box = BoxCollider::Create(glm::vec3(2.0f, 2.23f, 4.25f));
			box->SetPosition(glm::vec3(0.0f, 0.4f, 0.0f));
			box->SetScale(glm::vec3(0.25f, 0.22f, 0.2f));
			//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
			physics->AddCollider(box);
			//heavy
			//physics->SetMass(10.0f);
			TriggerVolume::Sptr volume = binM->Add<TriggerVolume>();
			BoxCollider::Sptr box2 = BoxCollider::Create(glm::vec3(2.0f, 2.23f, 4.25f));
			box2->SetPosition(glm::vec3(0.0f, 0.4f, 0.0f));
			box2->SetScale(glm::vec3(0.25f, 0.22f, 0.2f));
			volume->AddCollider(box2);
			SubmittingTrashBehaviour::Sptr behaviour2 = binM->Add<SubmittingTrashBehaviour>();
			
		}

		//// Set up all our sample objects
		//GameObject::Sptr plane = scene->CreateGameObject("Plane");
		//{
		//	// Scale up the plane
		//	plane->SetScale(glm::vec3(10.0F, 26.10f, 10.0f));
		//	plane->SetRotation(glm::vec3(40.0f, -1.0f, -1.0f));
		//	plane->SetPostion(glm::vec3(-2.25f, -0.82f, -1.880f));

		//	// Create and attach a RenderComponent to the object to draw our mesh
		//	RenderComponent::Sptr renderer = plane->Add<RenderComponent>();
		//	renderer->SetMesh(planeMesh);
		//	renderer->SetMaterial(planeMaterial);

		//	// Attach a plane collider that extends infinitely along the X/Y axis
		//	RigidBody::Sptr physics = plane->Add<RigidBody>(Kinematic);
		//	BoxCollider::Sptr box = BoxCollider::Create(/*glm::vec3(0.401f, 2.03f, 1.99f)*/);
		//	box->SetRotation(glm::vec3(-89.750f, 117.0f, 89.0f));
		//	box->SetPosition(glm::vec3(0.75f, 1.89f, 0.0f));
		//	box->SetScale(glm::vec3(0.010f, 11.97f, 4.430f));

		//	//box->SetScale(glm::vec3(10.f, 10.0f, 10.f));
		//	physics->AddCollider(box);
		//}

		// Save the asset manifest for all the resources we just loaded
		ResourceManager::SaveManifest("manifest.json");
		// Save the scene to a JSON file
		scene->Save("scene.json");
		scene->brick_count = 0;
	}
	//our score
	scene->trash = 0;


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

	//fetch resources
	GameObject::Sptr trashyM = scene->FindObjectByName("Trashy");
	GameObject::Sptr binM = scene->FindObjectByName("Bin");
	//limit rotation
	trashyM->Get<RigidBody>()->SetAngularFactor(glm::vec3(0.0f, 0.0f, 0.0f));
	trashyM->Get<RigidBody>()->SetLinearDamping(0.9f);
	//trashyM->Get<RigidBody>()->SetAngularDamping(0.8f);
	//trashyM->Get<RigidBody>()->damp
	//binM->Get<RigidBody>()->SetAngularFactor(glm::vec3(0.0f, 0.0f, 0.0f));
	//binM->Get<RigidBody>()->SetLinearDamping(1.0f);
	
	//limit pos of the bin??

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
				{

				}
				
			}
			ImGui::Separator();
			if (ImGui::Button("Add brick2"))
			{
				{

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
		//movement update
		//keyboard(trashyM->Get<RigidBody>());

		// Grab shorthands to the camera and shader from the scene
		Camera::Sptr camera = scene->MainCamera;

		camera->GetGameObject()->SetPostion(trashyM->GetPosition() + glm::vec3(0.0f, 4.00f, 5.7f));
		camera->GetGameObject()->LookAt(trashyM->GetPosition() + glm::vec3(0.0f, -4.0f, -2.0f));

		// Cache the camera's viewprojection
		glm::mat4 viewProj = camera->GetViewProjection();
		DebugDrawer::Get().SetViewProjection(viewProj);


		// Perform updates for all components
		scene->Update(dt);
		

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

