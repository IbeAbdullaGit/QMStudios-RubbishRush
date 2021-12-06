//Abdalla Mohamed - 100795120, Angelina Ratchkov � 100740576

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
#include "Gameplay/Components/MorphAnimator.h"
#include "Gameplay/Components/MorphMeshRenderer.h"

#include "CMorphMeshRenderer.h"
#include "CMorphAnimator.h"


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
	GuiBatcher::SetWindowSize({ -width, -height });
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

	// Pass the window to the input engine and let it initialize itself
	InputEngine::Init(window);

	GuiBatcher::SetWindowSize(-windowSize);

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

std::string TimeCountdown(float DisplayTime) { //Timer Function
	float minutes = floorf(DisplayTime / 60);
	float seconds = floorf(fmodf(DisplayTime, 60));
	std::string minString;

	std::stringstream testm; //Minutes
	testm << minutes << ":";
	std::stringstream tests;
	tests << seconds;
	std::stringstream tests10;
	tests10 << "0" << seconds;

	//std::cout << testm.str(); //print minutes

	if (seconds < 10) {
		minString = tests10.str();
	}
	else {
		minString = tests.str();
	}

	return (testm.str() + minString); //Adds the minute and seconds strings together

	//std::cout << timeValue;

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
	ComponentManager::RegisterType<MorphAnimator>();
	ComponentManager::RegisterType<MorphMeshRenderer>();

	
	ComponentManager::RegisterType<RectTransform>();
	ComponentManager::RegisterType<GuiPanel>();
	ComponentManager::RegisterType<GuiText>();

	//ComponentManager::RegisterType<CMorphMeshRenderer>();
	//ComponentManager::RegisterType<CMorphAnimator>();
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
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	bool loadScene = false;
	bool isPressed = false;
	bool timerDone = false;
	bool timeleveltDone = false;
	bool lose = false;
	bool Victory = false;

	std::vector <MeshResource::Sptr> walking;
	std::vector <MeshResource::Sptr> idle;
	std::vector <MeshResource::Sptr> jumping;
	std::vector <MeshResource::Sptr> interaction;

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
		Shader::Sptr rackShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_blinn_phong_texturedRACK.glsl" }
		});

		// ANIMATION SHADER??
		Shader::Sptr animShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/morph.vert" },
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
		scene->Lights.resize(8);
		scene->Lights[0].Position = glm::vec3(11.77f, 0.21f, 3.0f);
		scene->Lights[0].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[0].Range = 10.0f;

		scene->Lights[1].Position = glm::vec3(7.9f, -4.89f, 3.0f);
		scene->Lights[1].Color = glm::vec3(1.0f, 1.0f, 1.1f);
		scene->Lights[1].Range = 10.0f;

		scene->Lights[2].Position = glm::vec3(2.58f, -6.29f, 3.0f);
		scene->Lights[2].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[2].Range = 10.0f;

		scene->Lights[3].Position = glm::vec3(3.83f, 3.13f, 3.0f);
		scene->Lights[3].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[3].Range = 10.0f;

		scene->Lights[4].Position = glm::vec3(0.0f, 1.0f, 3.0f);
		scene->Lights[4].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[4].Range = 10.0f;

		scene->Lights[5].Position = glm::vec3(-14.41f, 0.56f, 3.0f);
		scene->Lights[5].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[5].Range = 10.0f;

		scene->Lights[6].Position = glm::vec3(-8.17f, 0.87f, 3.0f);
		scene->Lights[6].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[6].Range = 10.0f;

		scene->Lights[7].Position = glm::vec3(-2.2f, 0.5f, 3.0f);
		scene->Lights[7].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[7].Range = 10.0f;


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
		Material::Sptr trashyMaterial = ResourceManager::CreateAsset<Material>(animShader);
		{
			trashyMaterial->Name = "Trashy";
			trashyMaterial->Set("u_Material.Diffuse", trashyTex);
			trashyMaterial->Set("u_Material.Shininess", 1.0f);


		}
		GameObject::Sptr trashyM = scene->CreateGameObject("Trashy"); //SEARCHBAR TAGS: PLAYERENTITY, PLAYER, TRASHYENTITY, TRASHYOBJECT
		{
			trashyM->SetPostion(glm::vec3(-1.5f, 0.0f, 1.0f));
			trashyM->SetRotation(glm::vec3(90.0f, 0.0f, 270.0f));
			trashyM->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
			// Add a render component
			RenderComponent::Sptr renderer = trashyM->Add<RenderComponent>();
			renderer->SetMesh(trashyMesh);
			renderer->SetMaterial(trashyMaterial);
			// Add a dynamic rigid body to this monkey
			RigidBody::Sptr physics = trashyM->Add<RigidBody>(RigidBodyType::Dynamic);
			BoxCollider::Sptr box = BoxCollider::Create();

			box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
			box->SetScale(glm::vec3(0.33f, 0.580f, 0.22f));
			//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
			physics->AddCollider(box);
			//physics->AddCollider(BoxCollider::Create());
			//physics->SetMass(0.0f);
			//add trigger for collisions and behaviours
			TriggerVolume::Sptr volume = trashyM->Add<TriggerVolume>();
			BoxCollider::Sptr box2 = BoxCollider::Create();

			box2->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
			box2->SetScale(glm::vec3(0.33f, 0.58f, 0.22f));
			//box2->SetExtents(glm::vec3(0.8, 2.68, 0.83));
			volume->AddCollider(box2);
			JumpBehaviour::Sptr behaviour = trashyM->Add<JumpBehaviour>();
			//CollectTrashBehaviour::Sptr behaviour2 = trashyM->Add<CollectTrashBehaviour>();

			PlayerMovementBehavior::Sptr movement = trashyM->Add<PlayerMovementBehavior>();

			//ANIMATION STUFF////
			MorphMeshRenderer::Sptr morph1 = trashyM->Add<MorphMeshRenderer>();
			morph1->SetMorphMeshRenderer(trashyMesh, trashyMaterial);
			MorphAnimator::Sptr morph2 = trashyM->Add<MorphAnimator>();

			//walking frames
			//std::vector <MeshResource::Sptr> frames;
			//MeshResource::Sptr trashyMesh1 = ResourceManager::CreateAsset<MeshResource>("trashyWalk/trashywalk_000001.obj");
			MeshResource::Sptr trashyMesh2 = ResourceManager::CreateAsset<MeshResource>("trashyWalk/trashywalk_000002.obj");
			MeshResource::Sptr trashyMesh7 = ResourceManager::CreateAsset<MeshResource>("trashyWalk/trashywalk_000010.obj");
			MeshResource::Sptr trashyMesh3 = ResourceManager::CreateAsset<MeshResource>("trashyWalk/trashywalk_000016.obj");
			MeshResource::Sptr trashyMesh4 = ResourceManager::CreateAsset<MeshResource>("trashyWalk/trashywalk_000020.obj");
			MeshResource::Sptr trashyMesh5 = ResourceManager::CreateAsset<MeshResource>("trashyWalk/trashywalk_000025.obj");
			MeshResource::Sptr trashyMesh6 = ResourceManager::CreateAsset<MeshResource>("trashyWalk/trashywalk_000030.obj");
			//idle frames
			//std::vector <MeshResource::Sptr> frames2;
			MeshResource::Sptr trashyMesh8 = ResourceManager::CreateAsset<MeshResource>("trashyIdle/trashyidle_000001.obj");
			MeshResource::Sptr trashyMesh11 = ResourceManager::CreateAsset<MeshResource>("trashyIdle/trashyidle_000011.obj");
			MeshResource::Sptr trashyMesh10 = ResourceManager::CreateAsset<MeshResource>("trashyIdle/trashyidle_000020.obj");
			MeshResource::Sptr trashyMesh12 = ResourceManager::CreateAsset<MeshResource>("trashyIdle/trashyidle_000030.obj");
			MeshResource::Sptr trashyMesh9 = ResourceManager::CreateAsset<MeshResource>("trashyIdle/trashyidle_000040.obj");
			
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
		}

		
		

		Texture2D::Sptr planeTex = ResourceManager::CreateAsset<Texture2D>("textures/floor.jpg");

		//MeshResource::Sptr layoutMesh = ResourceManager::CreateAsset<MeshResource>("layout2.obj");
		Material::Sptr planeMaterial = ResourceManager::CreateAsset<Material>(basicShader); {
			planeMaterial->Name = "Plane";
			planeMaterial->Set("u_Material.Diffuse", planeTex);
			planeMaterial->Set("u_Material.Shininess", 1.0f);
		}

		// Set up all our sample objects
		GameObject::Sptr plane = scene->CreateGameObject("Plane");
		{
			plane->SetPostion(glm::vec3(0.0f, 0.0f, -0.08));
			plane->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			// Make a big tiled mesh
			MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(100.0f), glm::vec2(20.0f)));
			tiledMesh->GenerateMesh();


			// Create and attach a RenderComponent to the object to draw our mesh
			/*RenderComponent::Sptr renderer = plane->Add<RenderComponent>();
			renderer->SetMesh(layoutMesh);
			renderer->SetMaterial(planeMaterial);*/

			// Attach a plane collider that extends infinitely along the X/Y axis
			RigidBody::Sptr physics = plane->Add<RigidBody>(RigidBodyType::Kinematic);
			BoxCollider::Sptr box = BoxCollider::Create();
			//box->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
			box->SetScale(glm::vec3(50.0f, -0.12f, 50.0f));
			physics->AddCollider(box);
		}

		MeshResource::Sptr enterMesh = ResourceManager::CreateAsset<MeshResource>("InvisPlane.obj");
		Texture2D::Sptr enterTex = ResourceManager::CreateAsset<Texture2D>("textures/ENTER.png");
		Material::Sptr enterMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			enterMaterial->Name = "Enter";
			enterMaterial->Set("u_Material.Diffuse", enterTex);
			enterMaterial->Set("u_Material.Shininess", 0.0f);
		}
		GameObject::Sptr enter = scene->CreateGameObject("Enter");
		{
			enter->SetPostion(glm::vec3(0.67f, -1.6f, 0.11f));
			enter->SetRotation(glm::vec3(0.0f, -74.0f, -90.0f));
			enter->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
			
			RenderComponent::Sptr renderer = enter->Add<RenderComponent>();
			renderer->SetMesh(enterMesh);
			renderer->SetMaterial(enterMaterial);
		}
		//layout
		MeshResource::Sptr layoutMesh = ResourceManager::CreateAsset<MeshResource>("layoutclean.obj");
		Texture2D::Sptr layoutTex = ResourceManager::CreateAsset<Texture2D>("textures/floor.jpg");
		Material::Sptr layoutMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			layoutMaterial->Name = "Layout";
			layoutMaterial->Set("u_Material.Diffuse", layoutTex);
			layoutMaterial->Set("u_Material.Shininess", 0.0f);
		}
		GameObject::Sptr layout = scene->CreateGameObject("Layout");
		{
			layout->SetPostion(glm::vec3(6.33, -1.44f, 0.0f));
			layout->SetRotation(glm::vec3(90.0f, 0.0f, -90.0f));
			layout->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

			RenderComponent::Sptr renderer = layout->Add<RenderComponent>();
			renderer->SetMesh(layoutMesh);
			renderer->SetMaterial(layoutMaterial);
		}
		// Eletronics store walls
		{
			//Walls
			GameObject::Sptr layoutwall1 = scene->CreateGameObject("Layout Wall Front");
			{
				layoutwall1->SetPostion(glm::vec3(8.35f, 3.76f, 0.84f));
				layoutwall1->SetScale(glm::vec3(3.04f, 0.27f, 1.17f));
				RigidBody::Sptr wall1Phys = layoutwall1->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr wall1 = BoxCollider::Create();
				//wall1->SetPosition(glm::vec3(11.85f, 3.23f, 1.05f));
				wall1->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall1->SetScale(glm::vec3(3.04f, 0.27f, 1.17f));
				wall1->SetExtents(glm::vec3(2.2f, 2.0f, 2.0f));
				wall1Phys->AddCollider(wall1);
			}

			GameObject::Sptr layoutwall2 = scene->CreateGameObject("Layout Wall Back");
			{
				layoutwall2->SetPostion(glm::vec3(8.35f, -6.69f, 0.84f));
				layoutwall2->SetScale(glm::vec3(3.04f, 0.27f, 1.17f));
				RigidBody::Sptr wall2Phys = layoutwall2->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr wall2 = BoxCollider::Create();
				//wall1->SetPosition(glm::vec3(11.85f, 3.23f, 1.05f));
				wall2->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall2->SetScale(glm::vec3(3.04f, 0.27f, 1.17f));
				wall2->SetExtents(glm::vec3(2.2f, 2.0f, 2.0f));
				wall2Phys->AddCollider(wall2);
			}
			
			//Right Bottom
			GameObject::Sptr layoutwall3 = scene->CreateGameObject("Layout Wall Right Bottom");
			{
				layoutwall3->SetPostion(glm::vec3(1.49f, 1.51f, 0.0f));
				layoutwall3->SetScale(glm::vec3(0.3f, 1.54f, 2.37f));
				RigidBody::Sptr wall3Phys = layoutwall3->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr wall3 = BoxCollider::Create();
				wall3->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall3->SetScale(glm::vec3(0.3f, 1.54f, 2.37f));
				wall3->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wall3Phys->AddCollider(wall3);
			}

			//Right Top
			GameObject::Sptr layoutwall4 = scene->CreateGameObject("Layout Wall Right Top");
			{
				layoutwall4->SetPostion(glm::vec3(1.49f, -4.51f, 0.93f));
				layoutwall4->SetScale(glm::vec3(0.3f, 1.54f, 2.37f));
				RigidBody::Sptr wall4Phys = layoutwall4->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr wall4 = BoxCollider::Create();
				wall4->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall4->SetScale(glm::vec3(0.3f, 1.54f, 2.37f));
				wall4->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wall4Phys->AddCollider(wall4);
			}

			//Left Wall Corner Top
			GameObject::Sptr layoutwall5 = scene->CreateGameObject("Layout Wall Left Corner");
			{
				layoutwall5->SetPostion(glm::vec3(11.26f, -4.3f, 1.14f));
				layoutwall5->SetScale(glm::vec3(0.19f, 1.54f, 2.4f));
				RigidBody::Sptr wall5Phys = layoutwall5->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr wall5 = BoxCollider::Create();
				wall5->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall5->SetScale(glm::vec3(0.19f, 1.54f, 2.4f));
				wall5->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wall5Phys->AddCollider(wall5);
			}
			GameObject::Sptr layoutwall5b = scene->CreateGameObject("Layout Wall Left Corner");
			{
				layoutwall5b->SetPostion(glm::vec3(12.7f, -2.79, 1.1f));
				layoutwall5b->SetScale(glm::vec3(1.68f, 0.18f, 2.44f));
				RigidBody::Sptr wall5Physb = layoutwall5b->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr wall5b = BoxCollider::Create();
				wall5b->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall5b->SetScale(glm::vec3(1.68f, 0.18f, 2.44f));
				wall5b->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wall5Physb->AddCollider(wall5b);
			}
			
			//Left Wall
			GameObject::Sptr layoutwall6 = scene->CreateGameObject("Layout Wall Left");
			{
				layoutwall6->SetPostion(glm::vec3(14.53f, 0.36f, 1.22f));
				layoutwall6->SetScale(glm::vec3(0.32f, 3.16f, 2.28f));
				RigidBody::Sptr wall6Phys = layoutwall6->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr wall6 = BoxCollider::Create();
				wall6->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall6->SetScale(glm::vec3(0.32f, 3.16f, 2.28f));
				wall6->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wall6Phys->AddCollider(wall6);
			}
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
			trashMaterial->Set("u_Material.Shininess", 0.3f);

		}
		//cup collection
		{
			GameObject::Sptr trashM = scene->CreateGameObject("Trash1");
			{
				trashM->SetPostion(glm::vec3(2.75f, 2.27f, 0.0f));
				trashM->SetRotation(glm::vec3(90.0f, 0.0f, -62.0f));
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
			GameObject::Sptr trash2 = scene->CreateGameObject("Trash2");
			{
				trash2->SetPostion(glm::vec3(6.36f, 2.64f, 0.0f));
				trash2->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
				trash2->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
				// Add a render component
				RenderComponent::Sptr renderer = trash2->Add<RenderComponent>();
				renderer->SetMesh(trashMesh);
				renderer->SetMaterial(trashMaterial);
				// Add a dynamic rigid body to this monkey
				RigidBody::Sptr physics = trash2->Add<RigidBody>(RigidBodyType::Kinematic);
				BoxCollider::Sptr box = BoxCollider::Create();
				box->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
				box->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
				//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
				//box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
				//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
				physics->AddCollider(box);
				//physics->SetMass(0.0f);
				TriggerVolume::Sptr volume = trash2->Add<TriggerVolume>();
				BoxCollider::Sptr box2 = BoxCollider::Create();
				box2->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
				box2->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
				volume->AddCollider(box2);
				CollectTrashBehaviour::Sptr behaviour2 = trash2->Add<CollectTrashBehaviour>();

			}
			GameObject::Sptr trash3 = scene->CreateGameObject("Trash3");
			{
				trash3->SetPostion(glm::vec3(10.08f, -4.97f, 0.0f));
				trash3->SetRotation(glm::vec3(120.0f, 0.0f, 0.0f));
				trash3->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
				// Add a render component
				RenderComponent::Sptr renderer = trash3->Add<RenderComponent>();
				renderer->SetMesh(trashMesh);
				renderer->SetMaterial(trashMaterial);
				// Add a dynamic rigid body to this monkey
				RigidBody::Sptr physics = trash3->Add<RigidBody>(RigidBodyType::Kinematic);
				BoxCollider::Sptr box = BoxCollider::Create();
				box->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
				box->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
				//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
				//box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
				//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
				physics->AddCollider(box);
				//physics->SetMass(0.0f);
				TriggerVolume::Sptr volume = trash3->Add<TriggerVolume>();
				BoxCollider::Sptr box2 = BoxCollider::Create();
				box2->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
				box2->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
				volume->AddCollider(box2);
				CollectTrashBehaviour::Sptr behaviour2 = trash3->Add<CollectTrashBehaviour>();

			}
			GameObject::Sptr trash4 = scene->CreateGameObject("Trash4");
			{
				trash4->SetPostion(glm::vec3(13.680, -1.67f, 0.0f));
				trash4->SetRotation(glm::vec3(120.0f, 0.0f, 0.0f));
				trash4->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
				// Add a render component
				RenderComponent::Sptr renderer = trash4->Add<RenderComponent>();
				renderer->SetMesh(trashMesh);
				renderer->SetMaterial(trashMaterial);
				// Add a dynamic rigid body to this monkey
				RigidBody::Sptr physics = trash4->Add<RigidBody>(RigidBodyType::Kinematic);
				BoxCollider::Sptr box = BoxCollider::Create();
				box->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
				box->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
				//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
				//box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
				//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
				physics->AddCollider(box);
				//physics->SetMass(0.0f);
				TriggerVolume::Sptr volume = trash4->Add<TriggerVolume>();
				BoxCollider::Sptr box2 = BoxCollider::Create();
				box2->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
				box2->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
				volume->AddCollider(box2);
				CollectTrashBehaviour::Sptr behaviour2 = trash4->Add<CollectTrashBehaviour>();

			}
		}
		//set up robo toy
		MeshResource::Sptr roboMesh = ResourceManager::CreateAsset<MeshResource>("Robo/RoboWalk_000001.obj");
		Texture2D::Sptr roboTex = ResourceManager::CreateAsset<Texture2D>("textures/robo.png");
		Material::Sptr roboMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			roboMaterial->Name = "Robo";
			roboMaterial->Set("u_Material.Diffuse", roboTex);
			roboMaterial->Set("u_Material.Shininess", 0.5f);
		}
		GameObject::Sptr robo = scene->CreateGameObject("Robo");
		{
			robo->SetPostion(glm::vec3(0.0f, 0.0f, 0.0f));
			robo->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			robo->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

			RenderComponent::Sptr renderer = robo->Add<RenderComponent>();
			renderer->SetMesh(roboMesh);
			renderer->SetMaterial(roboMaterial);

			MorphMeshRenderer::Sptr morph1 = robo->Add<MorphMeshRenderer>();
			morph1->SetMorphMeshRenderer(roboMesh, roboMaterial);
			MorphAnimator::Sptr morph2 = robo->Add<MorphAnimator>();

			//moving

			MeshResource::Sptr roboMesh2 = ResourceManager::CreateAsset<MeshResource>("Robo/RoboWalk_000001.obj");
			MeshResource::Sptr roboMesh3 = ResourceManager::CreateAsset<MeshResource>("Robo/RoboWalk_000005.obj");
			MeshResource::Sptr roboMesh4 = ResourceManager::CreateAsset<MeshResource>("Robo/RoboWalk_000008.obj");
			MeshResource::Sptr roboMesh5 = ResourceManager::CreateAsset<MeshResource>("Robo/RoboWalk_000010.obj");
			MeshResource::Sptr roboMesh6 = ResourceManager::CreateAsset<MeshResource>("Robo/RoboWalk_000015.obj");
			MeshResource::Sptr roboMesh7 = ResourceManager::CreateAsset<MeshResource>("Robo/RoboWalk_000018.obj");
			MeshResource::Sptr roboMesh8 = ResourceManager::CreateAsset<MeshResource>("Robo/RoboWalk_000020.obj");
			
			std::vector<MeshResource::Sptr> frames;
			frames.push_back(roboMesh2);
			frames.push_back(roboMesh3);
			frames.push_back(roboMesh4);
			frames.push_back(roboMesh5);
			frames.push_back(roboMesh6);
			frames.push_back(roboMesh7);
			frames.push_back(roboMesh8);

			morph2->SetInitial();
			morph2->SetFrameTime(0.1f);
			morph2->SetFrames(frames);
		}
		//set up book
		MeshResource::Sptr bookMesh = ResourceManager::CreateAsset<MeshResource>("Book/AnimBook_000001.obj");
		Texture2D::Sptr bookTex = ResourceManager::CreateAsset<Texture2D>("textures/Book.png");
		Material::Sptr bookMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			bookMaterial->Name = "Book";
			bookMaterial->Set("u_Material.Diffuse", bookTex);
			bookMaterial->Set("u_Material.Shininess", 0.0f);
		}
		GameObject::Sptr book = scene->CreateGameObject("Book");
		{
			book->SetPostion(glm::vec3(1.0f, 1.0f, 0.0f));
			book->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			book->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

			RenderComponent::Sptr renderer = book->Add<RenderComponent>();
			renderer->SetMesh(bookMesh);
			renderer->SetMaterial(bookMaterial);

			MorphMeshRenderer::Sptr morph1 = book->Add<MorphMeshRenderer>();
			morph1->SetMorphMeshRenderer(bookMesh, bookMaterial);
			MorphAnimator::Sptr morph2 = book->Add<MorphAnimator>();

			//moving

			MeshResource::Sptr bookMesh2 = ResourceManager::CreateAsset<MeshResource>("Book/AnimBook_000001.obj");
			MeshResource::Sptr bookMesh3 = ResourceManager::CreateAsset<MeshResource>("Book/AnimBook_000004.obj");
			MeshResource::Sptr bookMesh4 = ResourceManager::CreateAsset<MeshResource>("Book/AnimBook_000008.obj");
			MeshResource::Sptr bookMesh5 = ResourceManager::CreateAsset<MeshResource>("Book/AnimBook_000012.obj");
			MeshResource::Sptr bookMesh6 = ResourceManager::CreateAsset<MeshResource>("Book/AnimBook_000016.obj");
			
			std::vector<MeshResource::Sptr> frames;
			frames.push_back(bookMesh2);
			frames.push_back(bookMesh3);
			frames.push_back(bookMesh4);
			frames.push_back(bookMesh5);
			frames.push_back(bookMesh6);

			morph2->SetInitial();
			morph2->SetFrameTime(0.2f);
			morph2->SetFrames(frames);
		}
		//setup moving toy
		MeshResource::Sptr toyMesh = ResourceManager::CreateAsset<MeshResource>("toy.obj");
		Texture2D::Sptr toyTex = ResourceManager::CreateAsset<Texture2D>("textures/toy.jpg");
		// Create our material
		Material::Sptr toyMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			toyMaterial->Name = "Toy";
			toyMaterial->Set("u_Material.Diffuse", toyTex);
			toyMaterial->Set("u_Material.Shininess", 0.0f);

		}
		// rolling toy collection
		{
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
				box->SetScale(glm::vec3(0.28f * 0.4f, 0.24f * 0.4f, 0.57f * 0.4f));
				physics->AddCollider(box);
				TriggerVolume::Sptr volume = toyM->Add<TriggerVolume>();
				BoxCollider::Sptr box2 = BoxCollider::Create();
				box2->SetPosition(glm::vec3(0.00f, 0.22f, 0.0f));
				box2->SetScale(glm::vec3(0.28f * 0.4f, 0.24f * 0.4f, 0.57f * 0.4f));
				volume->AddCollider(box2);
				SteeringBehaviour::Sptr behaviour2 = toyM->Add<SteeringBehaviour>();
				std::vector<glm::vec3> points;
				//points for catmull movement
				points.push_back(glm::vec3(-0.19f, -4.25f, -0.63f));
				points.push_back(glm::vec3(-7.18f, -4.25f, -0.63f));
				points.push_back(glm::vec3(-4.25f, -7.010f, -0.63f));
				points.push_back(glm::vec3(-0.67f, -7.32f, -0.63f));
				behaviour2->SetPoints(points);

				/////
				//ANIMATION STUFF////
				MorphMeshRenderer::Sptr morph1 = toyM->Add<MorphMeshRenderer>();
				morph1->SetMorphMeshRenderer(toyMesh, toyMaterial);
				MorphAnimator::Sptr morph2 = toyM->Add<MorphAnimator>();

				//moving

				MeshResource::Sptr toyMesh2 = ResourceManager::CreateAsset<MeshResource>("RollingToy/toyroll_000001.obj");
				MeshResource::Sptr toyMesh3 = ResourceManager::CreateAsset<MeshResource>("RollingToy/toyroll_000005.obj");
				MeshResource::Sptr toyMesh4 = ResourceManager::CreateAsset<MeshResource>("RollingToy/toyroll_000010.obj");
				MeshResource::Sptr toyMesh5 = ResourceManager::CreateAsset<MeshResource>("RollingToy/toyroll_000015.obj");
				MeshResource::Sptr toyMesh6 = ResourceManager::CreateAsset<MeshResource>("RollingToy/toyroll_000020.obj");

				std::vector<MeshResource::Sptr> frames;
				frames.push_back(toyMesh2);
				frames.push_back(toyMesh3);
				frames.push_back(toyMesh4);
				frames.push_back(toyMesh5);
				frames.push_back(toyMesh6);

				morph2->SetInitial();
				morph2->SetFrameTime(0.1f);
				morph2->SetFrames(frames);
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
				box->SetScale(glm::vec3(0.28f * 0.4f, 0.24f * 0.4f, 0.57f * 0.4f));
				physics->AddCollider(box);
				TriggerVolume::Sptr volume = toyM2->Add<TriggerVolume>();
				BoxCollider::Sptr box2 = BoxCollider::Create();
				box2->SetPosition(glm::vec3(0.00f, 0.22f, 0.0f));
				box2->SetScale(glm::vec3(0.28f * 0.4f, 0.24f * 0.4f, 0.57f * 0.4f));
				volume->AddCollider(box2);
				FollowBehaviour::Sptr behaviour2 = toyM2->Add<FollowBehaviour>();
				behaviour2->SetTarget(toyM);

				//ANIMATION STUFF////
				MorphMeshRenderer::Sptr morph1 = toyM2->Add<MorphMeshRenderer>();
				morph1->SetMorphMeshRenderer(toyMesh, toyMaterial);
				MorphAnimator::Sptr morph2 = toyM2->Add<MorphAnimator>();

				//moving
			
				MeshResource::Sptr toyMesh2 = ResourceManager::CreateAsset<MeshResource>("RollingToy/toyroll_000001.obj");
				MeshResource::Sptr toyMesh3 = ResourceManager::CreateAsset<MeshResource>("RollingToy/toyroll_000005.obj");
				MeshResource::Sptr toyMesh4 = ResourceManager::CreateAsset<MeshResource>("RollingToy/toyroll_000010.obj");
				MeshResource::Sptr toyMesh5 = ResourceManager::CreateAsset<MeshResource>("RollingToy/toyroll_000015.obj");
				MeshResource::Sptr toyMesh6 = ResourceManager::CreateAsset<MeshResource>("RollingToy/toyroll_000020.obj");
		
				std::vector<MeshResource::Sptr> frames;
				frames.push_back(toyMesh2);
				frames.push_back(toyMesh3);
				frames.push_back(toyMesh4);
				frames.push_back(toyMesh5);
				frames.push_back(toyMesh6);

				morph2->SetInitial();
				morph2->SetFrameTime(0.1f);
				morph2->SetFrames(frames);

			}
		}
		//spill object
		MeshResource::Sptr spillMesh = ResourceManager::CreateAsset<MeshResource>("spill.obj");
		Texture2D::Sptr spillTex = ResourceManager::CreateAsset<Texture2D>("textures/goo.png");
		// Create our material
		Material::Sptr spillMaterial = ResourceManager::CreateAsset<Material>(rackShader);
		{
			spillMaterial->Name = "Spill";
			spillMaterial->Set("u_Material.Diffuse", spillTex);
			spillMaterial->Set("u_Material.Shininess", 1.0f);

		}
		//spill collection
		{
			GameObject::Sptr spillM = scene->CreateGameObject("Spill");
			{
				spillM->SetPostion(glm::vec3(4.15f, 0.39f, 0.03f));
				spillM->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
				spillM->SetScale(glm::vec3(1.11f, 0.7f, 0.7f));
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
				box->SetScale(glm::vec3(0.75f, 0.001f, 0.53f));
				//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
				physics->AddCollider(box);
				//physics->SetMass(0.0f);
				TriggerVolume::Sptr volume = spillM->Add<TriggerVolume>();
				BoxCollider::Sptr box2 = BoxCollider::Create();
				//box2->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
				box2->SetScale(glm::vec3(0.75f, 0.001f, 0.53f));
				volume->AddCollider(box2);
				SpillBehaviour::Sptr behaviour = spillM->Add<SpillBehaviour>();

			}
			GameObject::Sptr spill2 = scene->CreateGameObject("Spill2");
			{
				spill2->SetPostion(glm::vec3(6.32f, -3.39f, 0.03f));
				spill2->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
				spill2->SetScale(glm::vec3(1.35f, 0.7f, 0.7f));
				// Add a render component
				RenderComponent::Sptr renderer = spill2->Add<RenderComponent>();
				renderer->SetMesh(spillMesh);
				renderer->SetMaterial(spillMaterial);
				// Add a dynamic rigid body to this monkey
				RigidBody::Sptr physics = spill2->Add<RigidBody>(RigidBodyType::Kinematic);
				BoxCollider::Sptr box = BoxCollider::Create();
				//box->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
				//box->SetScale(glm::vec3(0.22f, 0.37f, 0.24f));
				//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
				box->SetScale(glm::vec3(1.19f, 0.001f, 0.53f));
				//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
				physics->AddCollider(box);
				//physics->SetMass(0.0f);
				TriggerVolume::Sptr volume = spill2->Add<TriggerVolume>();
				BoxCollider::Sptr box2 = BoxCollider::Create();
				//box2->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
				box2->SetScale(glm::vec3(1.19f, 0.001f, 0.53f));
				volume->AddCollider(box2);
				SpillBehaviour::Sptr behaviour = spill2->Add<SpillBehaviour>();
			}
			GameObject::Sptr spill3 = scene->CreateGameObject("Spill3");
			{
				spill3->SetPostion(glm::vec3(6.38, -0.68f, 0.03f));
				spill3->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
				spill3->SetScale(glm::vec3(1.18f, 0.75f, 0.73f));
				// Add a render component
				RenderComponent::Sptr renderer = spill3->Add<RenderComponent>();
				renderer->SetMesh(spillMesh);
				renderer->SetMaterial(spillMaterial);
				// Add a dynamic rigid body to this monkey
				RigidBody::Sptr physics = spill3->Add<RigidBody>(RigidBodyType::Kinematic);
				BoxCollider::Sptr box = BoxCollider::Create();
				//box->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
				//box->SetScale(glm::vec3(0.22f, 0.37f, 0.24f));
				//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
				box->SetScale(glm::vec3(1.09f, 0.001f, 0.53f));
				//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
				physics->AddCollider(box);
				//physics->SetMass(0.0f);
				TriggerVolume::Sptr volume = spill3->Add<TriggerVolume>();
				BoxCollider::Sptr box2 = BoxCollider::Create();
				//box2->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
				box2->SetScale(glm::vec3(1.09f, 0.001f, 0.53));
				volume->AddCollider(box2);
				SpillBehaviour::Sptr behaviour = spill3->Add<SpillBehaviour>();
			}
			GameObject::Sptr spill4 = scene->CreateGameObject("Spill4");
			{
				spill4->SetPostion(glm::vec3(12.41f, 0.94, 0.03f));
				spill4->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
				spill4->SetScale(glm::vec3(0.7f, 0.7f, 1.43f));
				// Add a render component
				RenderComponent::Sptr renderer = spill4->Add<RenderComponent>();
				renderer->SetMesh(spillMesh);
				renderer->SetMaterial(spillMaterial);
				// Add a dynamic rigid body to this monkey
				RigidBody::Sptr physics = spill4->Add<RigidBody>(RigidBodyType::Kinematic);
				BoxCollider::Sptr box = BoxCollider::Create();
				//box->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
				//box->SetScale(glm::vec3(0.22f, 0.37f, 0.24f));
				//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
				box->SetScale(glm::vec3(0.5f, 0.001f, 1.61f));
				//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
				physics->AddCollider(box);
				//physics->SetMass(0.0f);
				TriggerVolume::Sptr volume = spill4->Add<TriggerVolume>();
				BoxCollider::Sptr box2 = BoxCollider::Create();
				//box2->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
				box2->SetScale(glm::vec3(0.5f, 0.001f, 1.61f));
				volume->AddCollider(box2);
				SpillBehaviour::Sptr behaviour = spill4->Add<SpillBehaviour>();
			}
		}
		//bin model
		MeshResource::Sptr binMesh = ResourceManager::CreateAsset<MeshResource>("BigBenClosed_000001.obj");
		Texture2D::Sptr binTex = ResourceManager::CreateAsset<Texture2D>("textures/bigben.png");
		// Create our material
		Material::Sptr binMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			binMaterial->Name = "Bin";
			binMaterial->Set("u_Material.Diffuse", binTex);
			binMaterial->Set("u_Material.Shininess", 1.0f);

		}
		GameObject::Sptr binM = scene->CreateGameObject("Bin");
		{
			binM->SetPostion(glm::vec3(-1.5f, 2.0f, 0.07f));
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

			//ANIMATION STUFF////
			MorphMeshRenderer::Sptr morph1 =binM->Add<MorphMeshRenderer>();
			morph1->SetMorphMeshRenderer(binMesh, binMaterial);
			MorphAnimator::Sptr morph2 = binM->Add<MorphAnimator>();

			//idle frames
			//std::vector <MeshResource::Sptr> frames2;
			MeshResource::Sptr binMesh8 = ResourceManager::CreateAsset<MeshResource>("BigBenClosed_000001.obj");
			std::vector<MeshResource::Sptr> closed;
			closed.push_back(binMesh8);
			behaviour2->getIdle(closed); //send idle frames to behaviour
			
			morph2->SetInitial();
			morph2->SetFrameTime(0.2f);
			morph2->SetFrames(closed);
			
		}
		//Shelf object
		MeshResource::Sptr shelfMesh = ResourceManager::CreateAsset<MeshResource>("shelf.obj");
		Texture2D::Sptr shelfTex = ResourceManager::CreateAsset <Texture2D>("textures/shelf.png");
		//Create Material
		Material::Sptr shelfMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			shelfMaterial->Name = "Shelf";
			shelfMaterial->Set("u_Material.Diffuse", shelfTex);
			shelfMaterial->Set("u_Material.Shininess", 0.2f);
		}
		//shelf collection
		{
			GameObject::Sptr shelf = scene->CreateGameObject("Shelf1");
			{
				shelf->SetPostion(glm::vec3(4.250f, -4.690f, 0.0f));
				shelf->SetRotation(glm::vec3(90.0, 0.0f, 0.0f));
				shelf->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				RenderComponent::Sptr renderer = shelf->Add<RenderComponent>();
				renderer->SetMesh(shelfMesh);
				renderer->SetMaterial(shelfMaterial);

				RigidBody::Sptr physics = shelf->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr box = BoxCollider::Create();
				box->SetScale(glm::vec3(1.04f, 1.62f, 1.54f));
				box->SetExtents(glm::vec3(0.5f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
			GameObject::Sptr shelf2 = scene->CreateGameObject("Shelf2");
			{
				shelf2->SetPostion(glm::vec3(6.20f, -5.69f, 0.0f));
				shelf2->SetRotation(glm::vec3(90.0, 0.0f, -90.0f));
				shelf2->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				RenderComponent::Sptr renderer = shelf2->Add<RenderComponent>();
				renderer->SetMesh(shelfMesh);
				renderer->SetMaterial(shelfMaterial);

				RigidBody::Sptr physics = shelf2->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr box = BoxCollider::Create();
				box->SetScale(glm::vec3(1.04f, 1.62f, 1.54f));
				box->SetExtents(glm::vec3(0.5f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
			GameObject::Sptr shelf3 = scene->CreateGameObject("Shelf3");
			{
				shelf3->SetPostion(glm::vec3(9.26f, -5.69f, 0.0f));
				shelf3->SetRotation(glm::vec3(90.0, 0.0f, -90.0f));
				shelf3->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				RenderComponent::Sptr renderer = shelf3->Add<RenderComponent>();
				renderer->SetMesh(shelfMesh);
				renderer->SetMaterial(shelfMaterial);

				RigidBody::Sptr physics = shelf3->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr box = BoxCollider::Create();
				box->SetScale(glm::vec3(1.04f, 1.62f, 1.54f));
				box->SetExtents(glm::vec3(0.5f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
			GameObject::Sptr shelf4 = scene->CreateGameObject("Shelf4");
			{
				shelf4->SetPostion(glm::vec3(7.9f, 0.1f, 0.0f));
				shelf4->SetRotation(glm::vec3(90.0, 0.0f, 0.0f));
				shelf4->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				RenderComponent::Sptr renderer = shelf4->Add<RenderComponent>();
				renderer->SetMesh(shelfMesh);
				renderer->SetMaterial(shelfMaterial);

				RigidBody::Sptr physics = shelf4->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr box = BoxCollider::Create();
				box->SetScale(glm::vec3(1.04f, 1.62f, 1.54f));
				box->SetExtents(glm::vec3(0.5f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
			GameObject::Sptr shelf5 = scene->CreateGameObject("Shelf5");
			{
				shelf5->SetPostion(glm::vec3(8.910f, 0.1f, 0.0f));
				shelf5->SetRotation(glm::vec3(90.0, 0.0f, -180.0f));
				shelf5->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				RenderComponent::Sptr renderer = shelf5->Add<RenderComponent>();
				renderer->SetMesh(shelfMesh);
				renderer->SetMaterial(shelfMaterial);

				RigidBody::Sptr physics = shelf5->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr box = BoxCollider::Create();
				box->SetScale(glm::vec3(1.04f, 1.62f, 1.54f));
				box->SetExtents(glm::vec3(0.5f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
		}
		//tv box object
		MeshResource::Sptr tvboxMesh = ResourceManager::CreateAsset<MeshResource>("tvbox.obj");
		Texture2D::Sptr tvboxTex = ResourceManager::CreateAsset<Texture2D>("textures/tvbox.png");
		//Create Material
		Material::Sptr tvboxMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			tvboxMaterial->Name = "TvBox";
			tvboxMaterial->Set("u_Material.Diffuse", tvboxTex);
			tvboxMaterial->Set("u_Material.Shininess", 0.0f);
		}
		//tv box collection
		{
			GameObject::Sptr tvbox1 = scene->CreateGameObject("tvbox1");
			{
				tvbox1->SetPostion(glm::vec3(8.37f, 2.1f, 0.0f));
				tvbox1->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
				tvbox1->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));
				
				RenderComponent::Sptr renderer = tvbox1->Add<RenderComponent>();
				renderer->SetMesh(tvboxMesh);
				renderer->SetMaterial(tvboxMaterial);

				RigidBody::Sptr physics = tvbox1->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr box = BoxCollider::Create();
				box->SetScale(glm::vec3(0.1f, 0.98f, 0.82f));
				box->SetExtents(glm::vec3(1.7f, 1.0f, 1.0f));
				physics->AddCollider(box);

			}
			GameObject::Sptr tvbox2 = scene->CreateGameObject("tvbox2");
			{
				tvbox2->SetPostion(glm::vec3(8.37f, 2.51f, 0.0f));
				tvbox2->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
				tvbox2->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));

				RenderComponent::Sptr renderer = tvbox2->Add<RenderComponent>();
				renderer->SetMesh(tvboxMesh);
				renderer->SetMaterial(tvboxMaterial);

				RigidBody::Sptr physics = tvbox2->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr box = BoxCollider::Create();
				box->SetScale(glm::vec3(0.1f, 0.98f, 0.82f));
				box->SetExtents(glm::vec3(1.7f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
			GameObject::Sptr tvbox3 = scene->CreateGameObject("tvbox3");
			{
				tvbox3->SetPostion(glm::vec3(8.37f, 2.92f, 0.0f));
				tvbox3->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
				tvbox3->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));

				RenderComponent::Sptr renderer = tvbox3->Add<RenderComponent>();
				renderer->SetMesh(tvboxMesh);
				renderer->SetMaterial(tvboxMaterial);

				RigidBody::Sptr physics = tvbox3->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr box = BoxCollider::Create();
				box->SetScale(glm::vec3(0.1f, 0.98f, 0.82f));
				box->SetExtents(glm::vec3(1.7f, 1.0f, 1.0f));
				physics->AddCollider(box);

			}
			GameObject::Sptr tvbox4 = scene->CreateGameObject("tvbox4");
			{
				tvbox4->SetPostion(glm::vec3(5.09f, -0.76f, 0.0f));
				tvbox4->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
				tvbox4->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));

				RenderComponent::Sptr renderer = tvbox4->Add<RenderComponent>();
				renderer->SetMesh(tvboxMesh);
				renderer->SetMaterial(tvboxMaterial);

				RigidBody::Sptr physics = tvbox4->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr box = BoxCollider::Create();
				box->SetScale(glm::vec3(0.1f, 0.98f, 0.82f));
				box->SetExtents(glm::vec3(1.7f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
			GameObject::Sptr tvbox5 = scene->CreateGameObject("tvbox5");
			{
				tvbox5->SetPostion(glm::vec3(5.09f, 0.98f, 0.0f));
				tvbox5->SetRotation(glm::vec3(90.0f, 0.0f, -180.0f));
				tvbox5->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));

				RenderComponent::Sptr renderer = tvbox5->Add<RenderComponent>();
				renderer->SetMesh(tvboxMesh);
				renderer->SetMaterial(tvboxMaterial);

				RigidBody::Sptr physics = tvbox5->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr box = BoxCollider::Create();
				box->SetScale(glm::vec3(0.1f, 0.98f, 0.82f));
				box->SetExtents(glm::vec3(1.7f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
			GameObject::Sptr tvbox6 = scene->CreateGameObject("tvbox6");
			{
				tvbox6->SetPostion(glm::vec3(9.97f, -3.45f, 0.0f));
				tvbox6->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
				tvbox6->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));

				RenderComponent::Sptr renderer = tvbox6->Add<RenderComponent>();
				renderer->SetMesh(tvboxMesh);
				renderer->SetMaterial(tvboxMaterial);

				RigidBody::Sptr physics = tvbox6->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr box = BoxCollider::Create();
				box->SetScale(glm::vec3(0.1f, 0.98f, 0.82f));
				box->SetExtents(glm::vec3(1.7f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
			GameObject::Sptr tvbox7 = scene->CreateGameObject("tvbox7");
			{
				tvbox7->SetPostion(glm::vec3(8.31f, -3.45f, 0.0f));
				tvbox7->SetRotation(glm::vec3(90.0f, 0.0f, -90.0f));
				tvbox7->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));

				RenderComponent::Sptr renderer = tvbox7->Add<RenderComponent>();
				renderer->SetMesh(tvboxMesh);
				renderer->SetMaterial(tvboxMaterial);

				RigidBody::Sptr physics = tvbox7->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr box = BoxCollider::Create();
				box->SetScale(glm::vec3(0.1f, 0.98f, 0.82f));
				box->SetExtents(glm::vec3(1.7f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
		}
		//tv object
		MeshResource::Sptr tvMesh = ResourceManager::CreateAsset<MeshResource>("TV.obj");
		Texture2D::Sptr tvTex = ResourceManager::CreateAsset<Texture2D>("textures/tvtex.jpg");
		//Create Material
		Material::Sptr tvMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			tvMaterial->Name = "Tv";
			tvMaterial->Set("u_Material.Diffuse", tvTex);
			tvMaterial->Set("u_Material.Shininess", 0.75f);
		}
		//tv collection
		{
			GameObject::Sptr tv1 = scene->CreateGameObject("tv1");
			{
				tv1->SetPostion(glm::vec3(2.47f, 0.91f, 0.0f));
				tv1->SetRotation(glm::vec3(90.0f, 0.0f, -90.0f));
				tv1->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

				RenderComponent::Sptr renderer = tv1->Add<RenderComponent>();
				renderer->SetMesh(tvMesh);
				renderer->SetMaterial(tvMaterial);

				RigidBody::Sptr physics = tv1->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr box = BoxCollider::Create();
				box->SetScale(glm::vec3(0.15f, 1.01f, 0.84f));
				physics->AddCollider(box);
			}
			GameObject::Sptr tv2 = scene->CreateGameObject("tv2");
			{
				tv2->SetPostion(glm::vec3(4.27f, -2.89f, 0.0f));
				tv2->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
				tv2->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

				RenderComponent::Sptr renderer = tv2->Add<RenderComponent>();
				renderer->SetMesh(tvMesh);
				renderer->SetMaterial(tvMaterial);

				RigidBody::Sptr physics = tv2->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr box = BoxCollider::Create();
				box->SetScale(glm::vec3(0.15f, 1.01f, 0.84f));
				physics->AddCollider(box);
			}
			GameObject::Sptr tv3 = scene->CreateGameObject("tv3");
			{
				tv3->SetPostion(glm::vec3(13.31f, 2.27f, 0.0f));
				tv3->SetRotation(glm::vec3(90.0f, 0.0f, -152.0f));
				tv3->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

				RenderComponent::Sptr renderer = tv3->Add<RenderComponent>();
				renderer->SetMesh(tvMesh);
				renderer->SetMaterial(tvMaterial);

				RigidBody::Sptr physics = tv3->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr box = BoxCollider::Create();
				box->SetScale(glm::vec3(0.15f, 1.01f, 0.84f));
				physics->AddCollider(box);
			}
		}
		//rectangle and trashy images for menu
		//cash counter
		MeshResource::Sptr cashMesh = ResourceManager::CreateAsset<MeshResource>("cashcounter.obj");
		Texture2D::Sptr cashTex = ResourceManager::CreateAsset<Texture2D>("textures/cash.png");
		//create Material
		Material::Sptr cashMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			cashMaterial->Name = "Cash";
			cashMaterial->Set("u_Material.Diffuse", cashTex);
			cashMaterial->Set("u_Material.Shininess", 0.75f);
		}
		//cash counter
		GameObject::Sptr cashcounter = scene->CreateGameObject("CashCounter");
		{
			cashcounter->SetPostion(glm::vec3(12.3f, -1.79f, 0.0f));
			cashcounter->SetRotation(glm::vec3(90.0f, 0.0f, -180.0f));
			cashcounter->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

			RenderComponent::Sptr renderer = cashcounter->Add<RenderComponent>();
			renderer->SetMesh(cashMesh);
			renderer->SetMaterial(cashMaterial);

			RigidBody::Sptr physics = cashcounter->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr box = BoxCollider::Create();
			box->SetScale(glm::vec3(0.520f, 0.740f, 1.210f));
			physics->AddCollider(box);
		}
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
		//Modelling Static objects
		{
			//Bench
			MeshResource::Sptr benchMesh = ResourceManager::CreateAsset<MeshResource>("bench.obj");
			Texture2D::Sptr benchTex = ResourceManager::CreateAsset<Texture2D>("textures/bench.jpg");
			Material::Sptr benchMaterial = ResourceManager::CreateAsset<Material>(basicShader);
			{
				benchMaterial->Name = "Bench";
				benchMaterial->Set("u_Material.Diffuse", benchTex);
				benchMaterial->Set("u_Material.Shininess", 0.0f);
			}
			GameObject::Sptr bench = scene->CreateGameObject("Bench");
			{
				bench->SetPostion(glm::vec3(-0.02f, -2.23f, 0.0f));
				bench->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
				bench->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
				
				RenderComponent::Sptr renderer = bench->Add<RenderComponent>();
				renderer->SetMesh(benchMesh);
				renderer->SetMaterial(benchMaterial);
			}
			//Cash Counter
			GameObject::Sptr cashModel = scene->CreateGameObject("cash Modelling");
			{
				cashModel->SetPostion(glm::vec3(-6.34f, -1.61f, 0.0f));
				cashModel->SetRotation(glm::vec3(90.0f, 0.0f, -90.0f));
				cashModel->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
				
				RenderComponent::Sptr renderer = cashModel->Add<RenderComponent>();
				renderer->SetMesh(cashMesh);
				renderer->SetMaterial(cashMaterial);

			}
			//Computer
			MeshResource::Sptr computerMesh = ResourceManager::CreateAsset<MeshResource>("Computer.obj");
			Texture2D::Sptr computerTex = ResourceManager::CreateAsset<Texture2D>("textures/desktoptex.png");
			Material::Sptr computerMaterial = ResourceManager::CreateAsset<Material>(basicShader);
			{
				computerMaterial->Name = "Computer";
				computerMaterial->Set("u_Material.Diffuse", computerTex);
				computerMaterial->Set("u_Material.Shininess", 0.3f);
			}
			GameObject::Sptr computer = scene->CreateGameObject("Desktop");
			{
				computer->SetPostion(glm::vec3(-2.61f, -1.08f, 0.0f));
				computer->SetRotation(glm::vec3(90.0f, 0.0f, 59.0f));
				computer->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));
				
				RenderComponent::Sptr renderer = computer->Add<RenderComponent>();
				renderer->SetMesh(computerMesh);
				renderer->SetMaterial(computerMaterial);
			}
			//Conveyor
			MeshResource::Sptr conveyorMesh = ResourceManager::CreateAsset<MeshResource>("conveyor.obj");
			Texture2D::Sptr conveyorTex = ResourceManager::CreateAsset<Texture2D>("textures/conayortex.png");
			Material::Sptr conveyorMaterial = ResourceManager::CreateAsset<Material>(basicShader);
			{
				conveyorMaterial->Name = "Conveyor";
				conveyorMaterial->Set("u_Material.Diffuse", conveyorTex);
				conveyorMaterial->Set("u_Material.Shininess", 0.2f);
			}
			GameObject::Sptr conveyor = scene->CreateGameObject("Conveyor");
			{
				conveyor->SetPostion(glm::vec3(-2.13f, -4.49f, 0.0f));
				conveyor->SetRotation(glm::vec3(90.0f, 0.0f, -75.0f));
				conveyor->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

				RenderComponent::Sptr renderer = conveyor->Add<RenderComponent>();
				renderer->SetMesh(conveyorMesh);
				renderer->SetMaterial(conveyorMaterial);
			}
			//Cup
			GameObject::Sptr cupModel = scene->CreateGameObject("cup Modelling");
			{
				cupModel->SetPostion(glm::vec3(-3.76f, -1.52f, 0.0f));
				cupModel->SetRotation(glm::vec3(90.0f, 0.0f, -90.0f));
				cupModel->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
				
				RenderComponent::Sptr renderer = cupModel->Add<RenderComponent>();
				renderer->SetMesh(trashMesh);
				renderer->SetMaterial(trashMaterial);
			}
			//Mirror
			MeshResource::Sptr mirrorMesh = ResourceManager::CreateAsset<MeshResource>("Mirror.obj");
			Texture2D::Sptr mirrorTex = ResourceManager::CreateAsset<Texture2D>("textures/mirror.jpg");
			Material::Sptr mirrorMaterial = ResourceManager::CreateAsset<Material>(basicShader);
			{
				mirrorMaterial->Name = "Mirror";
				mirrorMaterial->Set("u_Material.Diffuse", mirrorTex);
				mirrorMaterial->Set("u_Material.Shininess", 0.6f);
			}
			GameObject::Sptr mirror = scene->CreateGameObject("Mirror");
			{
				mirror->SetPostion(glm::vec3(-12.1f, -2.65f, 0.0f));
				mirror->SetRotation(glm::vec3(90.0f, -2.0f, 93.0f));
				mirror->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

				RenderComponent::Sptr renderer = mirror->Add<RenderComponent>();
				renderer->SetMesh(mirrorMesh);
				renderer->SetMaterial(mirrorMaterial);
			}
			//Plant
			MeshResource::Sptr plantMesh = ResourceManager::CreateAsset<MeshResource>("plant.obj");
			Texture2D::Sptr plantTex = ResourceManager::CreateAsset<Texture2D>("textures/planttex.png");
			Material::Sptr plantMaterial = ResourceManager::CreateAsset<Material>(basicShader);
			{
				plantMaterial->Name = "Plant";
				plantMaterial->Set("u_Material.Diffuse", plantTex);
				plantMaterial->Set("u_Material.Shininess", 0.0f);
			}
			GameObject::Sptr plant = scene->CreateGameObject("Plant");
			{
				plant->SetPostion(glm::vec3(-3.17f, -1.51f, 0.0f));
				plant->SetRotation(glm::vec3(90.0f, 0.0f, 72.0f));
				plant->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
				
				RenderComponent::Sptr renderer = plant->Add<RenderComponent>();
				renderer->SetMesh(plantMesh);
				renderer->SetMaterial(plantMaterial);
			}
			//Rack
			MeshResource::Sptr rackMesh = ResourceManager::CreateAsset<MeshResource>("rack.obj");
			Texture2D::Sptr rackTex = ResourceManager::CreateAsset<Texture2D>("textures/rack.png");
			Material::Sptr rackMaterial = ResourceManager::CreateAsset<Material>(rackShader); //CHANGE THIS SHADER
			{
				rackMaterial->Name = "Rack";
				rackMaterial->Set("u_Material.Diffuse", rackTex);
				rackMaterial->Set("u_Material.Shininess", 0.2f);
			}
			GameObject::Sptr rack = scene->CreateGameObject("Rack");
			{
				rack->SetPostion(glm::vec3(-1.64f, -1.97f, 0.0f));
				rack->SetRotation(glm::vec3(90.0f, 0.0f, 63.0f));
				rack->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));
				
				RenderComponent::Sptr renderer = rack->Add<RenderComponent>();
				renderer->SetMesh(rackMesh);
				renderer->SetMaterial(rackMaterial);
			}
			//Shelf
			GameObject::Sptr shelfModel = scene->CreateGameObject("Shelf Modelling");
			{
				shelfModel ->SetPostion(glm::vec3(-9.54f, -1.87f, 0.0f));
				shelfModel->SetRotation(glm::vec3(90.0f, 0.0f, -90.0f));
				shelfModel->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				RenderComponent::Sptr renderer = shelfModel->Add<RenderComponent>();
				renderer->SetMesh(shelfMesh);
				renderer->SetMaterial(shelfMaterial);
			}
			//Shower
			MeshResource::Sptr showerMesh = ResourceManager::CreateAsset<MeshResource>("shower.obj");
			Texture2D::Sptr showerTex = ResourceManager::CreateAsset<Texture2D>("textures/shower.png");
			Material::Sptr showerMaterial = ResourceManager::CreateAsset<Material>(basicShader);
			{
				showerMaterial->Name = "Shower";
				showerMaterial->Set("u_Material.Diffuse", showerTex);
				showerMaterial->Set("u_Material.Shininess", 0.5f);
			}
			GameObject::Sptr shower = scene->CreateGameObject("Shower");
			{
				shower->SetPostion(glm::vec3(-4.5f, -1.84f, 0.0f));
				shower->SetRotation(glm::vec3(90.0f, 0.0f, -180.0f));
				shower->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

				RenderComponent::Sptr renderer = shower->Add<RenderComponent>();
				renderer->SetMesh(showerMesh);
				renderer->SetMaterial(showerMaterial);
			}
			//Sink
			MeshResource::Sptr sinkMesh = ResourceManager::CreateAsset<MeshResource>("Sink.obj");
			Texture2D::Sptr sinkTex = ResourceManager::CreateAsset<Texture2D>("textures/sinktex.png");
			Material::Sptr sinkMaterial = ResourceManager::CreateAsset<Material>(basicShader);
			{
				sinkMaterial->Name = "Sink";
				sinkMaterial->Set("u_Material.Diffuse", sinkTex);
				sinkMaterial->Set("u_Material.Shininess", 0.5f);
			}
			GameObject::Sptr sink = scene->CreateGameObject("Sink");
			{
				sink->SetPostion(glm::vec3(-3.47f, -3.14f, 0.0f));
				sink->SetRotation(glm::vec3(90.0f, 0.0f, -34.0f));
				sink->SetScale(glm::vec3(0.25f, 0.25, 0.25f));

				RenderComponent::Sptr renderer = sink->Add<RenderComponent>();
				renderer->SetMesh(sinkMesh);
				renderer->SetMaterial(sinkMaterial);
			}
			//Tub
			MeshResource::Sptr tubMesh = ResourceManager::CreateAsset<MeshResource>("tub.obj");
			Texture2D::Sptr tubTex = ResourceManager::CreateAsset<Texture2D>("textures/tub.png");
			Material::Sptr tubMaterial = ResourceManager::CreateAsset<Material>(basicShader);
			{
				tubMaterial->Name = "Tub";
				tubMaterial->Set("u_Material.Diffuse", tubTex);
				tubMaterial->Set("u_Material.Shininess", 0.5f);
			}
			GameObject::Sptr tub = scene->CreateGameObject("Tub");
			{
				tub->SetPostion(glm::vec3(-12.25f, -1.78f, 0.0f));
				tub->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
				tub->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
				
				RenderComponent::Sptr renderer = tub->Add<RenderComponent>();
				renderer->SetMesh(tubMesh);
				renderer->SetMaterial(tubMaterial);
			}
			//Tv
			GameObject::Sptr tvModel = scene->CreateGameObject("Tv Modelling");
			{
				tvModel->SetPostion(glm::vec3(-14.07f, -1.9f, 0.0f));
				tvModel->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
				tvModel->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

				RenderComponent::Sptr renderer = tvModel->Add<RenderComponent>();
				renderer->SetMesh(tvMesh);
				renderer->SetMaterial(tvMaterial);
			}
			//Tv Box
			GameObject::Sptr tvboxModel = scene->CreateGameObject("Tv Box Modelling");
			{
				tvboxModel->SetPostion(glm::vec3(-15.69f, -1.68f, 0.0f));
				tvboxModel->SetRotation(glm::vec3(90.0f, 0.0f, 37.0f));
				tvboxModel->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));

				RenderComponent::Sptr renderer = tvboxModel->Add <RenderComponent>();
				renderer->SetMesh(tvboxMesh);
				renderer->SetMaterial(tvboxMaterial);
			}
			//Wolf statue object
			MeshResource::Sptr statueMesh = ResourceManager::CreateAsset<MeshResource>("wolfstatue.obj");
			Texture2D::Sptr statueTex = ResourceManager::CreateAsset <Texture2D>("textures/statue.jpg");
			//Create Material
			Material::Sptr statueMaterial = ResourceManager::CreateAsset<Material>(basicShader);
			{
				statueMaterial->Name = "Statue";
				statueMaterial->Set("u_Material.Diffuse", statueTex);
				statueMaterial->Set("u_Material.Shininess", 1.0f);
			}
			GameObject::Sptr wolfstatue = scene->CreateGameObject("Wolf Statue");
			{
				wolfstatue->SetPostion(glm::vec3(1.01f, -2.63f, 0.0f));
				wolfstatue->SetRotation(glm::vec3(90.0, 0.0f, -180.f));
				wolfstatue->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				RenderComponent::Sptr renderer = wolfstatue->Add<RenderComponent>();
				renderer->SetMesh(statueMesh);
				renderer->SetMaterial(statueMaterial);
			}
		}
		//start menu
		Texture2D::Sptr startTex = ResourceManager::CreateAsset<Texture2D>("textures/start_Screen.png");

		//MeshResource::Sptr layoutMesh = ResourceManager::CreateAsset<MeshResource>("layout2.obj");
		Material::Sptr startMaterial = ResourceManager::CreateAsset<Material>(basicShader); {
			startMaterial->Name = "StartMenu";
			startMaterial->Set("u_Material.Diffuse",startTex);
			startMaterial->Set("u_Material.Shininess", 1.0f);
		}

		// Set up all our sample objects
		GameObject::Sptr start = scene->CreateGameObject("Start");
		{
			start->SetPostion(glm::vec3(-1.44f, -0.11f, 2.91f));
			start->SetRotation(glm::vec3(-138.0f, -180.0f, 0.0f));
			start->SetScale(glm::vec3(0.21f, 0.19f, 0.42f));
			// Make a big tiled mesh
			MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(20.0f), glm::vec2(1.0f)));
			tiledMesh->GenerateMesh();


			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = start->Add<RenderComponent>();
			renderer->SetMesh(tiledMesh);
			renderer->SetMaterial(startMaterial);
		}
		//pause menu
		Texture2D::Sptr pauseTex = ResourceManager::CreateAsset<Texture2D>("textures/pause.png");
		//MeshResource::Sptr layoutMesh = ResourceManager::CreateAsset<MeshResource>("layout2.obj");
		Material::Sptr pauseMaterial = ResourceManager::CreateAsset<Material>(basicShader); {
			pauseMaterial->Name = "PauseMenu";
			pauseMaterial->Set("u_Material.Diffuse", pauseTex);
			pauseMaterial->Set("u_Material.Shininess", 1.0f);
		}

		// Set up all our sample objects
		GameObject::Sptr pause = scene->CreateGameObject("Pause");
		{
			pause->SetPostion(glm::vec3(-0.99f, -0.23f, 1.92f));
			pause->SetRotation(glm::vec3(-138.0f, -180.0f, 0.0f));
			pause->SetScale(glm::vec3(0.26f, 0.21f, 0.42f));
			// Make a big tiled mesh
			MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(20.0f), glm::vec2(1.0f)));
			tiledMesh->GenerateMesh();


			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = pause->Add<RenderComponent>();
			renderer->SetMesh(tiledMesh);
			renderer->SetMaterial(pauseMaterial);
			renderer->IsEnabled = false;
		}
		//end menu
		Texture2D::Sptr endTex = ResourceManager::CreateAsset<Texture2D>("textures/fail.png");
		//MeshResource::Sptr layoutMesh = ResourceManager::CreateAsset<MeshResource>("layout2.obj");
		Material::Sptr endMaterial = ResourceManager::CreateAsset<Material>(basicShader); {
			endMaterial->Name = "FailMenu";
			endMaterial->Set("u_Material.Diffuse", endTex);
			endMaterial->Set("u_Material.Shininess", 1.0f);
		}

		// Set up all our sample objects
		GameObject::Sptr end = scene->CreateGameObject("Fail");
		{
			end->SetPostion(glm::vec3(-0.99f, -0.23f, 1.92f));
			end->SetRotation(glm::vec3(-138.0f, -180.0f, 0.0f));
			end->SetScale(glm::vec3(0.20f, 0.21f, 0.42f));
			// Make a big tiled mesh
			MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(20.0f), glm::vec2(1.0f)));
			tiledMesh->GenerateMesh();


			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = end->Add<RenderComponent>();
			renderer->SetMesh(tiledMesh);
			renderer->SetMaterial(endMaterial);
			renderer->IsEnabled = false;
		}
		//victory menu
		Texture2D::Sptr winTex = ResourceManager::CreateAsset<Texture2D>("textures/WIN.png");
		//MeshResource::Sptr layoutMesh = ResourceManager::CreateAsset<MeshResource>("layout2.obj");
		Material::Sptr winMaterial = ResourceManager::CreateAsset<Material>(basicShader); {
			winMaterial->Name = "WinMenu";
			winMaterial->Set("u_Material.Diffuse", winTex);
			winMaterial->Set("u_Material.Shininess", 1.0f);
		}

		// Set up all our sample objects
		GameObject::Sptr win = scene->CreateGameObject("Win");
		{
			win->SetPostion(glm::vec3(-0.99f, -0.23f, 1.92f));
			win->SetRotation(glm::vec3(-138.0f, -180.0f, 0.0f));
			win ->SetScale(glm::vec3(0.20f, 0.21f, 0.42f));
			// Make a big tiled mesh
			MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(20.0f), glm::vec2(1.0f)));
			tiledMesh->GenerateMesh();


			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = win->Add<RenderComponent>();
			renderer->SetMesh(tiledMesh);
			renderer->SetMaterial(winMaterial);
			renderer->IsEnabled = false;
		}

		///////////////////////////// UI //////////////////////////////


		//Font: Junk Dog
		Font::Sptr junkDogFont = ResourceManager::CreateAsset<Font>("fonts/JunkDog.otf", 35.f); //Font path, font size
		junkDogFont->Bake();


		GameObject::Sptr objectiveUI = scene->CreateGameObject("Objective UI Canvas"); //UI for Time and Garbage Remaining
		{
			RectTransform::Sptr transform = objectiveUI->Add<RectTransform>();
			transform->SetMin({ 10, 10 });
			transform->SetMax({ 200, 200 });
			transform->SetPosition({ 400, 50 });
			transform->SetSize({ 35,35 });

			GuiPanel::Sptr canPanel = objectiveUI->Add<GuiPanel>();
			canPanel->SetColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.f));
			canPanel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/ui/ui-clock.png"));

			GameObject::Sptr timeText = scene->CreateGameObject("Time Text");
			{
				RectTransform::Sptr timetransform = timeText->Add<RectTransform>();
				timetransform->SetMax({ 130, 240 });

				GuiText::Sptr text = timeText->Add<GuiText>();
				text->SetText("0:00");
				text->SetFont(junkDogFont);
				text->SetColor(glm::vec4(0.8f, 0.8f, 0.8f, 1.f));
				text->SetTextScale(3.0f);

			}
			objectiveUI->AddChild(timeText);

			GameObject::Sptr trashRemain = scene->CreateGameObject("Trash Remaining");
			{
				RectTransform::Sptr remainTransform = trashRemain->Add<RectTransform>();
				remainTransform->SetMax({ 130,240 });
				remainTransform->SetPosition({ 0,0 });
				remainTransform->SetSize({ 0,0 });
				
				GuiText::Sptr text = trashRemain->Add<GuiText>();
				text->SetText("Trash Remaining");
				text->SetFont(junkDogFont);
				text->SetColor(glm::vec4(0.8f, 0.8f, 0.8f, 1.f));
				text->SetTextScale(3.0f);
			}
		}

		GuiBatcher::SetDefaultTexture(ResourceManager::CreateAsset<Texture2D>("textures/ui/ui-sprite.png"));
		GuiBatcher::SetDefaultBorderRadius(8);

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
	GameObject::Sptr startMenu = scene->FindObjectByName("Start");
	GameObject::Sptr pauseMenu = scene->FindObjectByName("Pause");
	GameObject::Sptr failMenu = scene->FindObjectByName("Fail");
	GameObject::Sptr winMenu = scene->FindObjectByName("Win");
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
	float timelevelt = 65.f;
	bool playMenu = true;

	bool start = false;
	bool spressed = false;
	bool isPaused = false;

	//save trash elements for when the victory is there
	//setup trash
	MeshResource::Sptr trashMesh = scene->FindObjectByName("Trash1")->Get<RenderComponent>()->GetMeshResource();
	
	// Create our material
	Material::Sptr trashMaterial = scene->FindObjectByName("Trash1")->Get<RenderComponent>()->GetMaterial();
	

	GameObject::Sptr UIText = scene->FindObjectByName("Time Text");
	GameObject::Sptr trashRemainder = scene->FindObjectByName("Trash Remaining");
	
	///// Game loop /////
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		ImGuiHelper::StartFrame();
		if (!start)
		{
			if (glfwGetKey(scene->Window, GLFW_KEY_ENTER))
			{
				spressed = true;
			}
			if (glfwGetKey(scene->Window, GLFW_KEY_ESCAPE)) //exit
			{
				exit(0);

			}
			//scene->IsPlaying = false;
			playMenu = false;
			//FREEZE TRASHY
			trashyM->Get<RigidBody>()->IsEnabled = false;


			if (spressed)
			{
				start = true;
				startMenu->Get<RenderComponent>()->IsEnabled = false;
				trashyM->Get<RigidBody>()->IsEnabled = true;
				scene->RemoveGameObject(startMenu);
				playMenu = true;
				spressed = false;

			}

		}
		// Calculate the time since our last frame (dt)
		double thisFrame = glfwGetTime();
		float dt = static_cast<float>(thisFrame - lastFrame);

		//start screen?

		//MENU ANIMATED UPDATED
		if (scene->IsPlaying && !timerDone && playMenu && start)
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
			//trashyM->Get<RigidBody>()->IsEnabled = false;
			//trashyM->Get<RigidBody>()->SetMass(0.0f);
		}
		else if (scene->IsPlaying && timerDone && playMenu && start)
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
				//scene->RemoveGameObject(TrashyE); //DONT DELETE WE NEED TO REUSE
				//scene->RemoveGameObject(RectangleE); //DONT DELETE WE NEED TO REUSE
				std::cout << "should be deleted\n";
				playMenu = false;
				//trashyM->Get<RigidBody>()->IsEnabled = true;
				//trashyM->SetDirty(true);
			}
		}

		else if (scene->IsPlaying && !playMenu && !timeleveltDone && start)
		{
			//put gui here
			if (scene->score == 4)
			{
				//trashRemainder->Get<GuiText>()->SetText(3 - scene->score);
				timeleveltDone = true;
				Victory = true;
			}
			else
			{
				if (scene->trash == 0) {
					timerDone = true;
				}
				else {

				}
			}
			if (!isPaused) 
			{

				if (timelevelt > 0 && !timeleveltDone) {
					timelevelt -= dt;
					UIText->Get<GuiText>()->SetText(TimeCountdown(timelevelt));
				}
				else if(timelevelt <= 0 )
				{
					timeleveltDone = true;
					lose = true;

				}
			}
			else 
			{

			}
			
		}
		else if (timeleveltDone && scene->IsPlaying && start)
		{
			if (lose)
			{
				
				//lose = false;
				//end menu
				failMenu->SetPostion(trashyM->GetPosition() + glm::vec3(0.07f, 0.14f, 1.81f)); //offset from player
				trashyM->Get<RigidBody>()->IsEnabled = false;
				failMenu->Get<RenderComponent>()->IsEnabled = true;
				//pause the timer*****
				if (glfwGetKey(scene->Window, GLFW_KEY_SPACE)) //return to game
				{
					//trashyM->Get<RigidBody>()->IsEnabled = true; 
					failMenu->Get<RenderComponent>()->IsEnabled = false; //dont show win menu
					startMenu->Get<RenderComponent>()->IsEnabled = true;
					//reset variables
					lose = false;
					start = false;
					playMenu = false;
					timeLoop = 7.0f;
					timelevelt = 65.f;
					timerDone = false;
					timeleveltDone = false;
					trashyM->SetPostion(glm::vec3(-1.5f, 0.0f, 1.0f)); //reset position to start
					scene->score = 0;
					scene->trash = 0;
					//create trash objects again
					//delete any remaining trash objects
					GameObject::Sptr trash1 = scene->FindObjectByName("Trash1");
					GameObject::Sptr trash2 = scene->FindObjectByName("Trash2");
					GameObject::Sptr trash3 = scene->FindObjectByName("Trash3");
					GameObject::Sptr trash4 = scene->FindObjectByName("Trash4");
					if (trash1 != nullptr)
					{
						scene->RemoveGameObject(trash1);
					}
					if (trash2 != nullptr)
					{
						scene->RemoveGameObject(trash2);
					}
					if (trash3 != nullptr)
					{
						scene->RemoveGameObject(trash3);
					}
					if (trash4 != nullptr)
					{
						scene->RemoveGameObject(trash4);
					}
					//cup collection
					{
						GameObject::Sptr trashM = scene->CreateGameObject("Trash1");
						{
							trashM->SetPostion(glm::vec3(2.75f, 2.27f, 0.0f));
							trashM->SetRotation(glm::vec3(90.0f, 0.0f, -62.0f));
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
						GameObject::Sptr trash2 = scene->CreateGameObject("Trash2");
						{
							trash2->SetPostion(glm::vec3(6.36f, 2.64f, 0.0f));
							trash2->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
							trash2->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
							// Add a render component
							RenderComponent::Sptr renderer = trash2->Add<RenderComponent>();
							renderer->SetMesh(trashMesh);
							renderer->SetMaterial(trashMaterial);
							// Add a dynamic rigid body to this monkey
							RigidBody::Sptr physics = trash2->Add<RigidBody>(RigidBodyType::Kinematic);
							BoxCollider::Sptr box = BoxCollider::Create();
							box->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
							box->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
							//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
							//box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
							//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
							physics->AddCollider(box);
							//physics->SetMass(0.0f);
							TriggerVolume::Sptr volume = trash2->Add<TriggerVolume>();
							BoxCollider::Sptr box2 = BoxCollider::Create();
							box2->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
							box2->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
							volume->AddCollider(box2);
							CollectTrashBehaviour::Sptr behaviour2 = trash2->Add<CollectTrashBehaviour>();

						}
						GameObject::Sptr trash3 = scene->CreateGameObject("Trash3");
						{
							trash3->SetPostion(glm::vec3(10.08f, -4.97f, 0.0f));
							trash3->SetRotation(glm::vec3(120.0f, 0.0f, 0.0f));
							trash3->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
							// Add a render component
							RenderComponent::Sptr renderer = trash3->Add<RenderComponent>();
							renderer->SetMesh(trashMesh);
							renderer->SetMaterial(trashMaterial);
							// Add a dynamic rigid body to this monkey
							RigidBody::Sptr physics = trash3->Add<RigidBody>(RigidBodyType::Kinematic);
							BoxCollider::Sptr box = BoxCollider::Create();
							box->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
							box->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
							//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
							//box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
							//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
							physics->AddCollider(box);
							//physics->SetMass(0.0f);
							TriggerVolume::Sptr volume = trash3->Add<TriggerVolume>();
							BoxCollider::Sptr box2 = BoxCollider::Create();
							box2->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
							box2->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
							volume->AddCollider(box2);
							CollectTrashBehaviour::Sptr behaviour2 = trash3->Add<CollectTrashBehaviour>();

						}
						GameObject::Sptr trash4 = scene->CreateGameObject("Trash4");
						{
							trash4->SetPostion(glm::vec3(13.680, -1.67f, 0.0f));
							trash4->SetRotation(glm::vec3(120.0f, 0.0f, 0.0f));
							trash4->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
							// Add a render component
							RenderComponent::Sptr renderer = trash4->Add<RenderComponent>();
							renderer->SetMesh(trashMesh);
							renderer->SetMaterial(trashMaterial);
							// Add a dynamic rigid body to this monkey
							RigidBody::Sptr physics = trash4->Add<RigidBody>(RigidBodyType::Kinematic);
							BoxCollider::Sptr box = BoxCollider::Create();
							box->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
							box->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
							//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
							//box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
							//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
							physics->AddCollider(box);
							//physics->SetMass(0.0f);
							TriggerVolume::Sptr volume = trash4->Add<TriggerVolume>();
							BoxCollider::Sptr box2 = BoxCollider::Create();
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
				//Victory = false;
				winMenu->SetPostion(trashyM->GetPosition() + glm::vec3(0.07f, 0.14f, 1.81f)); //offset from player
				trashyM->Get<RigidBody>()->IsEnabled = false;
				winMenu->Get<RenderComponent>()->IsEnabled = true;
				//pause the timer*****
				if (glfwGetKey(scene->Window, GLFW_KEY_SPACE)) //return to game
				{
					//trashyM->Get<RigidBody>()->IsEnabled = true; 
					winMenu->Get<RenderComponent>()->IsEnabled = false; //dont show win menu
					startMenu->Get<RenderComponent>()->IsEnabled = true;
					//reset variables
					Victory = false;
					start = false;
					playMenu = false;
					timeLoop = 7.0f;
					timelevelt = 65.f;
					timerDone = false;
					timeleveltDone = false;
					trashyM->SetPostion(glm::vec3(-1.5f, 0.0f, 1.0f)); //reset position to start
					scene->score = 0;
					scene->trash = 0;
					//create trash objects again
					
					//cup collection
					{
						GameObject::Sptr trashM = scene->CreateGameObject("Trash1");
						{
							trashM->SetPostion(glm::vec3(2.75f, 2.27f, 0.0f));
							trashM->SetRotation(glm::vec3(90.0f, 0.0f, -62.0f));
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
						GameObject::Sptr trash2 = scene->CreateGameObject("Trash2");
						{
							trash2->SetPostion(glm::vec3(6.36f, 2.64f, 0.0f));
							trash2->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
							trash2->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
							// Add a render component
							RenderComponent::Sptr renderer = trash2->Add<RenderComponent>();
							renderer->SetMesh(trashMesh);
							renderer->SetMaterial(trashMaterial);
							// Add a dynamic rigid body to this monkey
							RigidBody::Sptr physics = trash2->Add<RigidBody>(RigidBodyType::Kinematic);
							BoxCollider::Sptr box = BoxCollider::Create();
							box->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
							box->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
							//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
							//box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
							//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
							physics->AddCollider(box);
							//physics->SetMass(0.0f);
							TriggerVolume::Sptr volume = trash2->Add<TriggerVolume>();
							BoxCollider::Sptr box2 = BoxCollider::Create();
							box2->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
							box2->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
							volume->AddCollider(box2);
							CollectTrashBehaviour::Sptr behaviour2 = trash2->Add<CollectTrashBehaviour>();

						}
						GameObject::Sptr trash3 = scene->CreateGameObject("Trash3");
						{
							trash3->SetPostion(glm::vec3(10.08f, -4.97f, 0.0f));
							trash3->SetRotation(glm::vec3(120.0f, 0.0f, 0.0f));
							trash3->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
							// Add a render component
							RenderComponent::Sptr renderer = trash3->Add<RenderComponent>();
							renderer->SetMesh(trashMesh);
							renderer->SetMaterial(trashMaterial);
							// Add a dynamic rigid body to this monkey
							RigidBody::Sptr physics = trash3->Add<RigidBody>(RigidBodyType::Kinematic);
							BoxCollider::Sptr box = BoxCollider::Create();
							box->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
							box->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
							//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
							//box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
							//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
							physics->AddCollider(box);
							//physics->SetMass(0.0f);
							TriggerVolume::Sptr volume = trash3->Add<TriggerVolume>();
							BoxCollider::Sptr box2 = BoxCollider::Create();
							box2->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
							box2->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
							volume->AddCollider(box2);
							CollectTrashBehaviour::Sptr behaviour2 = trash3->Add<CollectTrashBehaviour>();

						}
						GameObject::Sptr trash4 = scene->CreateGameObject("Trash4");
						{
							trash4->SetPostion(glm::vec3(13.680, -1.67f, 0.0f));
							trash4->SetRotation(glm::vec3(120.0f, 0.0f, 0.0f));
							trash4->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
							// Add a render component
							RenderComponent::Sptr renderer = trash4->Add<RenderComponent>();
							renderer->SetMesh(trashMesh);
							renderer->SetMaterial(trashMaterial);
							// Add a dynamic rigid body to this monkey
							RigidBody::Sptr physics = trash4->Add<RigidBody>(RigidBodyType::Kinematic);
							BoxCollider::Sptr box = BoxCollider::Create();
							box->SetPosition(glm::vec3(0.00f, 0.05f, 0.0f));
							box->SetScale(glm::vec3(0.06f, 0.09f, 0.12f));
							//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
							//box->SetScale(glm::vec3(0.3f, 0.210f, 0.130f));
							//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
							physics->AddCollider(box);
							//physics->SetMass(0.0f);
							TriggerVolume::Sptr volume = trash4->Add<TriggerVolume>();
							BoxCollider::Sptr box2 = BoxCollider::Create();
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
				if (glfwGetKey(scene->Window, GLFW_KEY_ESCAPE)) //exit
				{
					exit(0);

				}

				//victory menu?

			}
		}
		//PAUSE MENU
		if (start && !playMenu)
		{
			if (glfwGetKey(scene->Window, GLFW_KEY_ESCAPE) && !isPaused)
			{
				isPaused = true;
				pauseMenu->SetPostion(trashyM->GetPosition() + glm::vec3(0.07f, 0.14f, 1.81f)); //offset from player
			}
			if (isPaused)
			{
				trashyM->Get<RigidBody>()->IsEnabled = false;
				pauseMenu->Get<RenderComponent>()->IsEnabled = true;
				//pause the timer*****
				if (glfwGetKey(scene->Window, GLFW_KEY_ENTER)) //return to game
				{
					trashyM->Get<RigidBody>()->IsEnabled = true;
					pauseMenu->Get<RenderComponent>()->IsEnabled = false;
					isPaused = false;
				}
				if (glfwGetKey(scene->Window, GLFW_KEY_ESCAPE)) //exit
				{
					exit(0);

				}

			}
		}
		if (trashyM->Get<PlayerMovementBehavior>()->is_moving)
		{
			trashyM->Get<MorphAnimator>()->SetFrames(walking);
		}
		else
		{
			trashyM->Get<MorphAnimator>()->SetFrames(idle);
		}


		// Draw our material properties window!
		//DrawMaterialsWindow();
		// Showcasing how to use the imGui library!
		bool isDebugWindowOpen = ImGui::Begin("Debugging");
		if (isDebugWindowOpen)
		{
			if (ImGui::Button("Add brick"))
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

			/// <summary>
			/// /ANIMATION STUFF
			/// </summary>
			/// <returns></returns>
			//trashyM->Get<MorphAnimator>()->Update(dt);
			//trashyM->Get<MorphMeshRenderer>()->Draw();


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

			glfwSwapInterval(1); //vsynch

			////////UI UPDATING////////

			//// Disable culling
			glDisable(GL_CULL_FACE);
			//// Disable depth testing, we're going to use order-dependant layering
			glDisable(GL_DEPTH_TEST);
			//// Disable depth writing
			glDepthMask(GL_FALSE);

			// Enable alpha blending
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			//// Enable the scissor test;
			//glEnable(GL_SCISSOR_TEST);

			// Our projection matrix will be our entire window for now
			glm::mat4 proj = glm::ortho(0.0f, (float)windowSize.x, (float)windowSize.y, 0.0f, -1.0f, 1.0f);
			GuiBatcher::SetProjection(proj);

			// Iterate over and render all the GUI objects
			scene->RenderGUI();

			// Flush the Gui Batch renderer
			GuiBatcher::Flush();

			//// Disable alpha blending
			//glDisable(GL_BLEND);
			//// Disable scissor testing
			glDisable(GL_SCISSOR_TEST);

			glEnable(GL_DEPTH_TEST);

			//// Re-enable depth writing
			glDepthMask(GL_TRUE);

			// End our ImGui window
			ImGui::End();

			VertexArrayObject::Unbind();

			lastFrame = thisFrame;
			ImGuiHelper::EndFrame();
			glfwSwapBuffers(window);
		}
	}
			// Clean up the ImGui library
			ImGuiHelper::Cleanup();

			// Clean up the resource manager
			ResourceManager::Cleanup();

			// Clean up the toolkit logger so we don't leak memory
			Logger::Uninitialize();
			return 0;
	}
	

