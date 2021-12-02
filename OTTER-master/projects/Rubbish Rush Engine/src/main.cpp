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
#include "Graphics/TextureCube.h"
#include "Graphics/VertexTypes.h"
#include "Graphics/Font.h"
#include "Graphics/GuiBatcher.h"

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
#include "Gameplay/InputEngine.h"

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
#include "Gameplay/Components/ConveyorBeltBehaviour.h"
#include "Gameplay/Components/SpillBehaviour.h"
#include "Gameplay/Components/SteeringBehaviour.h"
#include "Gameplay/Components/FollowBehaviour.h"


// Physics
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/Colliders/PlaneCollider.h"
#include "Gameplay/Physics/Colliders/SphereCollider.h"
#include "Gameplay/Physics/Colliders/ConvexMeshCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Graphics/DebugDraw.h"
#include "Gameplay/Components/SimpleCameraControl.h"
#include "Gameplay/Physics/Colliders/CylinderCollider.h"

// GUI
#include "Gameplay/Components/GUI/RectTransform.h"
#include "Gameplay/Components/GUI/GuiPanel.h"
#include "Gameplay/Components/GUI/GuiText.h"
#include "Gameplay/InputEngine.h"

#include "CPathAnimator.h"
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

void TimeCountdown(float DisplayTime) {
	if (DisplayTime < 0) {
		DisplayTime = 0;
	}
	else if (DisplayTime > 0) {
		DisplayTime += 1;
	}

	float minutes = floorf(DisplayTime / 60);
	float seconds = floorf(fmodf(DisplayTime, 60));

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
	ComponentManager::RegisterType<ConveyorBeltBehaviour>();
	ComponentManager::RegisterType<SpillBehaviour>();
	ComponentManager::RegisterType<SteeringBehaviour>();
	ComponentManager::RegisterType<FollowBehaviour>();
	ComponentManager::RegisterType<SimpleCameraControl>();

	ComponentManager::RegisterType<RectTransform>();
	ComponentManager::RegisterType<GuiPanel>();
	ComponentManager::RegisterType<GuiText>();
	// Structure for our frame-level uniforms, matches layout from
	// fragments/frame_uniforms.glsl
	// For use with a UBO.
	struct FrameLevelUniforms {
		// The camera's view matrix
		glm::mat4 u_View;
		// The camera's projection matrix
		glm::mat4 u_Projection;
		// The combined viewProject matrix
		glm::mat4 u_ViewProjection;
		// The camera's position in world space
		glm::vec4 u_CameraPos;
		// The time in seconds since the start of the application
		float u_Time;
	};
	// This uniform buffer will hold all our frame level uniforms, to be shared between shaders
	UniformBuffer<FrameLevelUniforms>::Sptr frameUniforms = std::make_shared<UniformBuffer<FrameLevelUniforms>>(BufferUsage::DynamicDraw);
	// The slot that we'll bind our frame level UBO to
	const int FRAME_UBO_BINDING = 0;

	// Structure for our isntance-level uniforms, matches layout from
	// fragments/frame_uniforms.glsl
	// For use with a UBO.
	struct InstanceLevelUniforms {
		// Complete MVP
		glm::mat4 u_ModelViewProjection;
		// Just the model transform, we'll do worldspace lighting
		glm::mat4 u_Model;
		// Normal Matrix for transforming normals
		glm::mat4 u_NormalMatrix;
	};


	// This uniform buffer will hold all our instance level uniforms, to be shared between shaders
	UniformBuffer<InstanceLevelUniforms>::Sptr instanceUniforms = std::make_shared<UniformBuffer<InstanceLevelUniforms>>(BufferUsage::DynamicDraw);

	// The slot that we'll bind our instance level UBO to
	const int INSTANCE_UBO_BINDING = 1;

	// GL states, we'll enable depth testing and backface fulling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	bool loadScene = false;
	bool isPressed = false;
	bool timerDone = false;
	bool timeleveltDone = false;
	// For now we can use a toggle to generate our scene vs load from file
	if (loadScene) {
		ResourceManager::LoadManifest("Brick Brea-manifest.json");
		scene = Scene::Load("Brick Breaker.json");

	}
	else {
		// This time we'll have 2 different shaders, and share data between both of them using the UBO
		// This shader will handle reflective materials 
		Shader::Sptr reflectiveShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_environment_reflective.glsl" }
		});

		// This shader handles our basic materials without reflections (cause they expensive)
		Shader::Sptr basicShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_blinn_phong_textured.glsl" }
		});

		// This shader handles our basic materials without reflections (cause they expensive)
		Shader::Sptr specShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/textured_specular.glsl" }
		});

		// This shader handles our foliage vertex shader example
		Shader::Sptr foliageShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/foliage.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/screendoor_transparency.glsl" }
		});

		// This shader handles our cel shading example
		Shader::Sptr toonShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/toon_shading.glsl" }
		});


		///////////////////// NEW SHADERS ////////////////////////////////////////////

		// This shader handles our displacement mapping example
		Shader::Sptr displacementShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/displacement_mapping.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_tangentspace_normal_maps.glsl" }
		});

		// This shader handles our displacement mapping example
		Shader::Sptr tangentSpaceMapping = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_tangentspace_normal_maps.glsl" }
		});

		// This shader handles our multitexturing example
		Shader::Sptr multiTextureShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/vert_multitextured.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_multitextured.glsl" }
		});

		
		// Create an empty scene
		scene = std::make_shared<Scene>();

		// I hate this
		/*scene->BaseShader = uboShader;*/

		
		
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
		Material::Sptr trashyMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			trashyMaterial->Name = "Trashy";
			trashyMaterial->Set("u_Material.Diffuse", trashyTex);
			trashyMaterial->Set("u_Material.Shininess", 1.0f);
			

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
		

		Texture2D::Sptr planeTex = ResourceManager::CreateAsset<Texture2D>("textures/floor.jpg");
		
		MeshResource::Sptr layoutMesh = ResourceManager::CreateAsset<MeshResource>("layout2.obj");
		Material::Sptr planeMaterial = ResourceManager::CreateAsset<Material>(basicShader); {
			planeMaterial->Name = "Plane";
			planeMaterial->Set("u_Material.Diffuse", planeTex);
			planeMaterial->Set("u_Material.Shininess", 1.0f);
		}

		// Set up all our sample objects
		GameObject::Sptr plane = scene->CreateGameObject("Plane");
		{
			plane->SetRotation(glm::vec3(100.0f, 0.0f, 0.0f));
			// Make a big tiled mesh
			MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(100.0f), glm::vec2(20.0f)));
			tiledMesh->GenerateMesh();
			

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = plane->Add<RenderComponent>();
			renderer->SetMesh(layoutMesh);
			renderer->SetMaterial(planeMaterial);

			// Attach a plane collider that extends infinitely along the X/Y axis
			RigidBody::Sptr physics = plane->Add<RigidBody>(RigidBodyType::Kinematic);
			BoxCollider::Sptr box = BoxCollider::Create();
			//box->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
			box->SetScale(glm::vec3(11.98f, -0.12f, 12.88f));
			physics->AddCollider(box);
		}
		//placeholder trash object
		//setup trash
		MeshResource::Sptr trashMesh = ResourceManager::CreateAsset<MeshResource>("cup.obj");
		Texture2D::Sptr trashTex = ResourceManager::CreateAsset<Texture2D>("textures/acup.jpg");
		// Create our material
		Material::Sptr trashMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			trashMaterial->Name = "Trash";
			trashMaterial->Set("u_Material.Diffuse", trashTex);
			trashMaterial->Set("u_Material.Shininess", 1.0f);

		}
		GameObject::Sptr trashM = scene->CreateGameObject("Trash");
		{
			trashM->SetPostion(glm::vec3(0.43f, -1.67f, -0.12f));
			trashM->SetRotation(glm::vec3(120.0f, 0.0f, 0.0f));
			trashM->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
			// Add a render component
			RenderComponent::Sptr renderer = trashM->Add<RenderComponent>();
			renderer->SetMesh(trashMesh);
			renderer->SetMaterial(trashMaterial);
			// Add a dynamic rigid body to this monkey
			RigidBody::Sptr physics = trashM->Add<RigidBody>(RigidBodyType::Kinematic);
			BoxCollider::Sptr box = BoxCollider::Create();
			box->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
			box->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
			//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
			//box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
			//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
			physics->AddCollider(box);
			//physics->SetMass(0.0f);
			TriggerVolume::Sptr volume = trashM->Add<TriggerVolume>();
			BoxCollider::Sptr box2 = BoxCollider::Create();
			box2->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
			box2->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
			volume->AddCollider(box2);
			CollectTrashBehaviour::Sptr behaviour2 = trashM->Add<CollectTrashBehaviour>();

		}
		//setup moving toy
		MeshResource::Sptr toyMesh = ResourceManager::CreateAsset<MeshResource>("toy.obj");
		Texture2D::Sptr toyTex = ResourceManager::CreateAsset<Texture2D>("textures/toy.jpg");
		// Create our material
		Material::Sptr toyMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			toyMaterial->Name = "Toy";
			toyMaterial->Set("u_Material.Diffuse", toyTex);
			toyMaterial->Set("u_Material.Shininess", 1.0f);

		}
		GameObject::Sptr toyM = scene->CreateGameObject("Toy");
		{
			toyM->SetPostion(glm::vec3(-0.19f, -4.25f, -0.63f));
			toyM->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			toyM->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
			// Add a render component
			RenderComponent::Sptr renderer = toyM->Add<RenderComponent>();
			renderer->SetMesh(toyMesh);
			renderer->SetMaterial(toyMaterial);
			// Add a dynamic rigid body to this monkey
			RigidBody::Sptr physics = toyM->Add<RigidBody>(RigidBodyType::Kinematic);
			BoxCollider::Sptr box = BoxCollider::Create();
			box->SetPosition(glm::vec3(0.00f, 0.22f, 0.0f));
			box->SetScale(glm::vec3(0.28f*0.4f, 0.24f*0.4f, 0.57f*0.4f));
			physics->AddCollider(box);
			TriggerVolume::Sptr volume = toyM->Add<TriggerVolume>();
			BoxCollider::Sptr box2 = BoxCollider::Create();
			box2->SetPosition(glm::vec3(0.00f, 0.22f, 0.0f));
			box2->SetScale(glm::vec3(0.28f*0.4f, 0.24f*0.4f, 0.57f*0.4f));
			volume->AddCollider(box2);
			SteeringBehaviour::Sptr behaviour2 = toyM->Add<SteeringBehaviour>();
			std::vector<glm::vec3> points;
			//points for catmull movement
			points.push_back(glm::vec3(-0.19f, -4.25f, -0.63f));
			points.push_back(glm::vec3(-7.18f, -4.25f, -0.63f));
			points.push_back(glm::vec3(-4.25f, -7.010f, -0.63f));
			points.push_back(glm::vec3(-0.67f, -7.32f, -0.63f));
			behaviour2->SetPoints(points);
		}
		GameObject::Sptr toyM2 = scene->CreateGameObject("Toy2");
		{
			toyM2->SetPostion(glm::vec3(2.00f, -4.25f, -0.63f));
			toyM2->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			toyM2->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
			// Add a render component
			RenderComponent::Sptr renderer = toyM2->Add<RenderComponent>();
			renderer->SetMesh(toyMesh);
			renderer->SetMaterial(toyMaterial);
			// Add a dynamic rigid body to this monkey
			RigidBody::Sptr physics = toyM2->Add<RigidBody>(RigidBodyType::Kinematic);
			BoxCollider::Sptr box = BoxCollider::Create();
			box->SetPosition(glm::vec3(0.00f, 0.22f, 0.0f));
			box->SetScale(glm::vec3(0.28f*0.4f, 0.24f*0.4f, 0.57f*0.4f));
			physics->AddCollider(box);
			TriggerVolume::Sptr volume = toyM2->Add<TriggerVolume>();
			BoxCollider::Sptr box2 = BoxCollider::Create();
			box2->SetPosition(glm::vec3(0.00f, 0.22f, 0.0f));
			box2->SetScale(glm::vec3(0.28f*0.4f, 0.24f*0.4f, 0.57f*0.4f));
			volume->AddCollider(box2);
			FollowBehaviour::Sptr behaviour2 = toyM2->Add<FollowBehaviour>();
			behaviour2->SetTarget(toyM);
			
		}
		//spill object
		MeshResource::Sptr spillMesh = ResourceManager::CreateAsset<MeshResource>("spill.obj");
		Texture2D::Sptr spillTex = ResourceManager::CreateAsset<Texture2D>("textures/goo.png");
		// Create our material
		Material::Sptr spillMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			spillMaterial->Name = "Spill";
			spillMaterial->Set("u_Material.Diffuse", spillTex);
			spillMaterial->Set("u_Material.Shininess", 1.0f);

		}
		GameObject::Sptr spillM = scene->CreateGameObject("Spill");
		{
			spillM->SetPostion(glm::vec3(1.27f, -2.88f, -0.40f));
			spillM->SetRotation(glm::vec3(100.0f, 0.0f, 0.0f));
			spillM->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
			// Add a render component
			RenderComponent::Sptr renderer = spillM->Add<RenderComponent>();
			renderer->SetMesh(spillMesh);
			renderer->SetMaterial(spillMaterial);
			// Add a dynamic rigid body to this monkey
			RigidBody::Sptr physics = spillM->Add<RigidBody>(RigidBodyType::Kinematic);
			BoxCollider::Sptr box = BoxCollider::Create();
			//box->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
			//box->SetScale(glm::vec3(0.22f, 0.37f, 0.24f));
			//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
			box->SetScale(glm::vec3(0.5f, 0.001f, 0.53f));
			//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
			physics->AddCollider(box);
			//physics->SetMass(0.0f);
			TriggerVolume::Sptr volume = spillM->Add<TriggerVolume>();
			BoxCollider::Sptr box2 = BoxCollider::Create();
			//box2->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
			box2->SetScale(glm::vec3(0.5f, 0.001f, 0.53f));
			volume->AddCollider(box2);
			SpillBehaviour::Sptr behaviour = spillM->Add<SpillBehaviour>();
			
		}
		//bin model
		MeshResource::Sptr binMesh = ResourceManager::CreateAsset<MeshResource>("Big_Bin2.obj");
		Texture2D::Sptr binTex = ResourceManager::CreateAsset<Texture2D>("textures/big_bintex2.png");
		// Create our material
		Material::Sptr binMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			binMaterial->Name = "Bin";
			binMaterial->Set("u_Material.Diffuse", binTex);
			binMaterial->Set("u_Material.Shininess", 1.0f);

		}
		GameObject::Sptr binM = scene->CreateGameObject("Bin");
		{
			binM->SetPostion(glm::vec3(-1.5f, 2.0f, 0.76f));
			binM->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
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
		//rectangle and trashy images for menu
		MeshResource::Sptr recMesh = ResourceManager::CreateAsset<MeshResource>("RecOBJ.obj");
		Texture2D::Sptr recTex = ResourceManager::CreateAsset<Texture2D>("textures/Rec1.png");
		// Create our material
		Material::Sptr recMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			recMaterial->Name = "Rec";
			recMaterial->Set("u_Material.Diffuse", recTex);
			recMaterial->Set("u_Material.Shininess", 1.0f);

		}
		GameObject::Sptr recE = scene->CreateGameObject("Rec");
		{
			recE->SetPostion(glm::vec3(-1.36f, 1.22f, 7.0f));
			recE->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
			recE->SetScale(glm::vec3(1.0f, 5.64f, 3.46f));
			// Add a render component
			RenderComponent::Sptr renderer = recE->Add<RenderComponent>();
			
			renderer->SetMesh(recMesh);
			renderer->SetMaterial(recMaterial);
			//RigidBody::Sptr physics = recE->Add<RigidBody>(RigidBodyType::Kinematic);
			
		}
		MeshResource::Sptr trashyEMesh = ResourceManager::CreateAsset<MeshResource>("trashy2OBJ.obj");
		Texture2D::Sptr trashyETex = ResourceManager::CreateAsset<Texture2D>("textures/Trashy2.png");
		// Create our material
		Material::Sptr trashyEMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			trashyEMaterial->Name = "trashyE";
			trashyEMaterial->Set("u_Material.Diffuse", trashyETex);
			trashyEMaterial->Set("u_Material.Shininess", 1.0f);

		}
		GameObject::Sptr trashyE = scene->CreateGameObject("TrashyE");
		{
			trashyE->SetPostion(glm::vec3(0.5f, 1.49f, 3.3f));
			trashyE->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
			trashyE->SetScale(glm::vec3(1.0f, 1.46f, 1.090f));
			// Add a render component
			RenderComponent::Sptr renderer = trashyE->Add<RenderComponent>();
			renderer->SetMesh(trashyEMesh);
			renderer->SetMaterial(trashyEMaterial);
			//RigidBody::Sptr physics = trashyE->Add<RigidBody>(RigidBodyType::Kinematic);
			
		}
		///////////////////////////// UI //////////////////////////////
		//GameObject::Sptr canvas = scene->CreateGameObject("UI Canvas");
		//{
		//	RectTransform::Sptr transform = canvas->Add<RectTransform>();
		//	transform->SetMin({ 16, 16 });
		//	transform->SetMax({ 256, 256 });

		//	GuiPanel::Sptr canPanel = canvas->Add<GuiPanel>();

		//	GameObject::Sptr subPanel = scene->CreateGameObject("Sub Item");
		//	{
		//		RectTransform::Sptr transform = subPanel->Add<RectTransform>();
		//		transform->SetMin({ 10, 10 });
		//		transform->SetMax({ 128, 128 });

		//		GuiPanel::Sptr panel = subPanel->Add<GuiPanel>();
		//		panel->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

		//		Font::Sptr font = ResourceManager::CreateAsset<Font>("fonts/Roboto-Medium.ttf", 16.0f);
		//		font->Bake();

		//		GuiText::Sptr text = subPanel->Add<GuiText>();
		//		text->SetText("Hello world!");
		//		text->SetFont(font);
		//	}

		//	canvas->AddChild(subPanel);
		//}

		//GuiBatcher::SetDefaultTexture(ResourceManager::CreateAsset<Texture2D>("textures/ui-sprite.png"));
		//GuiBatcher::SetDefaultBorderRadius(8);

		// Call scene awake to start up all of our components
		scene->Window = window;
		scene->Awake();

		// Save the asset manifest for all the resources we just loaded
		ResourceManager::SaveManifest("manifest.json");

		// Save the scene to a JSON file
		scene->Save("scene.json");
		scene->brick_count = 0;
	}
	//our score
	scene->trash = 0;


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
	GameObject::Sptr RectangleE = scene->FindObjectByName("Rec");
	GameObject::Sptr TrashyE = scene->FindObjectByName("TrashyE");
	//limit rotation
	trashyM->Get<RigidBody>()->SetAngularFactor(glm::vec3(0.0f, 0.0f, 0.0f));
	trashyM->Get<RigidBody>()->SetLinearDamping(0.9f);
	
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
	
	float timeLoop = 7.0f;
	float timelevelt = 180.f;
	bool playMenu = true;
	///// Game loop /////
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		ImGuiHelper::StartFrame();

		// Calculate the time since our last frame (dt)
		double thisFrame = glfwGetTime();
		float dt = static_cast<float>(thisFrame - lastFrame);


		//MENU ANIMATED UPDATED
		if (scene->IsPlaying && !timerDone && playMenu)
		{
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
				trashyM->Get<RigidBody>()->SetLinearVelocity(glm::vec3(0.0f));
			}
			//trashyM->Get<RigidBody>()->SetMass(0.0f);
		}
		else if (scene->IsPlaying && timerDone && playMenu)
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
				trashyM->Get<RigidBody>()->SetLinearVelocity(glm::vec3(0.0f));
			}
			//FREEZE TRASHY
			//trashyM->Get<RigidBody>()->SetLinearVelocity(glm::vec3(0.0f));
			if (TrashyE->GetPosition().x >= 0.4f && RectangleE->GetPosition().z >= 6.9f)
			{
				TrashyE->SetPostionZ(-10.0f);
				RectangleE->SetPostionZ(-10.0f);
				//TrashyE->SetDirty(true);
				//RectangleE->SetDirty(true);
				//TrashyE->GetScene()->DeleteGameObject(TrashyE->GetScene()->FindObjectByGUID(TrashyE->GUID));
				//RectangleE->GetScene()->DeleteGameObject(TrashyE->GetScene()->FindObjectByGUID(RectangleE->GUID));
				scene->RemoveGameObject(TrashyE);
				scene->RemoveGameObject(RectangleE);
				std::cout << "should be deleted\n";
				playMenu = false;
				//trashyM->SetDirty(true);
			}
		}
		else if (scene->IsPlaying && !playMenu) {

			if (scene->trash == 0) {
				timerDone = true;
			}
			else {

			}

			if (timelevelt > 0 && !timeleveltDone) {
				timelevelt -= dt;
			}
			else {
				timeleveltDone = true;
			}

			std::cout << timelevelt;
		}
		// Draw our material properties window!
		//DrawMaterialsWindow();
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

		dt *= playbackSpeed;

		// Perform updates for all components
		scene->Update(dt);
		// Grab shorthands to the camera and shader from the scene
		Camera::Sptr camera = scene->MainCamera;

		camera->GetGameObject()->SetPostion(trashyM->GetPosition() + glm::vec3(0.0f, 4.00f, 5.7f));
		camera->GetGameObject()->LookAt(trashyM->GetPosition() + glm::vec3(0.0f, -4.0f, -2.0f));

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

		// Bind the skybox texture to a reserved texture slot
		// See Material.h and Material.cpp for how we're reserving texture slots
		TextureCube::Sptr environment = scene->GetSkyboxTexture();
		if (environment) environment->Bind(0);

		// Here we'll bind all the UBOs to their corresponding slots
		scene->PreRender();
		frameUniforms->Bind(FRAME_UBO_BINDING);
		instanceUniforms->Bind(INSTANCE_UBO_BINDING);

		// Upload frame level uniforms
		auto& frameData = frameUniforms->GetData();
		frameData.u_Projection = camera->GetProjection();
		frameData.u_View = camera->GetView();
		frameData.u_ViewProjection = camera->GetViewProjection();
		frameData.u_CameraPos = glm::vec4(camera->GetGameObject()->GetPosition(), 1.0f);
		frameData.u_Time = static_cast<float>(thisFrame);
		frameUniforms->Update();

		// Render all our objects
		ComponentManager::Each<RenderComponent>([&](const RenderComponent::Sptr& renderable) {
			// Early bail if mesh not set
			if (renderable->GetMesh() == nullptr) {
				return;
			}

			// If we don't have a material, try getting the scene's fallback material
			// If none exists, do not draw anything
			if (renderable->GetMaterial() == nullptr) {
				if (scene->DefaultMaterial != nullptr) {
					renderable->SetMaterial(scene->DefaultMaterial);
				}
				else {
					return;
				}
			}

			// If the material has changed, we need to bind the new shader and set up our material and frame data
			// Note: This is a good reason why we should be sorting the render components in ComponentManager
			if (renderable->GetMaterial() != currentMat) {
				currentMat = renderable->GetMaterial();
				shader = currentMat->GetShader();

				shader->Bind();
				currentMat->Apply();
			}

			// Grab the game object so we can do some stuff with it
			GameObject* object = renderable->GetGameObject();

			// Use our uniform buffer for our instance level uniforms
			auto& instanceData = instanceUniforms->GetData();
			instanceData.u_Model = object->GetTransform();
			instanceData.u_ModelViewProjection = viewProj * object->GetTransform();
			instanceData.u_NormalMatrix = glm::mat3(glm::transpose(glm::inverse(object->GetTransform())));
			instanceUniforms->Update();

			// Draw the object
			renderable->GetMesh()->Draw();
			});

		// Use our cubemap to draw our skybox
		scene->DrawSkybox();

		////////UNCOMMENT THIS FOR UI////////

		//// Disable culling
		//glDisable(GL_CULL_FACE);
		//// Disable depth testing, we're going to use order-dependant layering
		//glDisable(GL_DEPTH_TEST);
		//// Disable depth writing
		//glDepthMask(GL_FALSE);

		//// Enable alpha blending
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//// Enable the scissor test;
		//glEnable(GL_SCISSOR_TEST);

		//// Our projection matrix will be our entire window for now
		//glm::mat4 proj = glm::ortho(0.0f, (float)windowSize.x, (float)windowSize.y, 0.0f, -1.0f, 1.0f);
		//GuiBatcher::SetProjection(proj);

		//// Iterate over and render all the GUI objects
		//scene->RenderGUI();

		//// Flush the Gui Batch renderer
		//GuiBatcher::Flush();

		//// Disable alpha blending
		//glDisable(GL_BLEND);
		//// Disable scissor testing
		//glDisable(GL_SCISSOR_TEST);
		//// Re-enable depth writing
		//glDepthMask(GL_TRUE);

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

