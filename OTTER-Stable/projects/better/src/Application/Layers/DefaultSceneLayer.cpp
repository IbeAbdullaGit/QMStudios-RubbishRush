#include "DefaultSceneLayer.h"

// GLM math library
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
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

DefaultSceneLayer::DefaultSceneLayer() :
	ApplicationLayer()
{
	Name = "Default Scene";
	Overrides = AppLayerFunctions::OnAppLoad;
}

DefaultSceneLayer::~DefaultSceneLayer() = default;

void DefaultSceneLayer::OnAppLoad(const nlohmann::json& config) {
	_CreateScene();
}

void DefaultSceneLayer::_CreateScene()
{
	//using namespace Gameplay;
	//using namespace Gameplay::Physics;

	Application& app = Application::Get();
	//all conditions to change between in-game conditions and menus
	bool loadScene = false;
	bool isPressed = false;
	bool timerDone = false;
	bool timeleveltDone = false;
	bool lose = false;
	bool Victory = false;

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
		// This time we'll have 2 different shaders, and share data between both of them using the UBO
		// This shader will handle reflective materials 
		ShaderProgram::Sptr reflectiveShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_environment_reflective.glsl" }
		});

		// This shader handles our basic materials without reflections (cause they expensive)
		ShaderProgram::Sptr basicShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_blinn_phong_textured.glsl" }
		});
		// This shader handles our basic materials without reflections (cause they expensive)
		ShaderProgram::Sptr rackShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_blinn_phong_texturedRACK.glsl" }
		});

		// ANIMATION SHADER??
		ShaderProgram::Sptr animShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/morph.vert" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_blinn_phong_textured.glsl" }
		});

		// This shader handles our basic materials without reflections (cause they expensive)
		ShaderProgram::Sptr specShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/textured_specular.glsl" }
		});

		// This shader handles our foliage vertex shader example
		ShaderProgram::Sptr foliageShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/foliage.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/screendoor_transparency.glsl" }
		});

		// This shader handles our cel shading example
		ShaderProgram::Sptr toonShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/toon_shading.glsl" }
		});


		///////////////////// NEW SHADERS ////////////////////////////////////////////

		// This shader handles our displacement mapping example
		ShaderProgram::Sptr displacementShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/displacement_mapping.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_tangentspace_normal_maps.glsl" }
		});

		// This shader handles our displacement mapping example
		ShaderProgram::Sptr tangentSpaceMapping = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_tangentspace_normal_maps.glsl" }
		});

		// This shader handles our multitexturing example
		ShaderProgram::Sptr multiTextureShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/vert_multitextured.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_multitextured.glsl" }
		});


		// Load in the meshes
		Gameplay::MeshResource::Sptr monkeyMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("Monkey.obj");

		// Load in some textures
		Texture2D::Sptr    boxTexture = ResourceManager::CreateAsset<Texture2D>("textures/box-diffuse.png");
		Texture2D::Sptr    boxSpec = ResourceManager::CreateAsset<Texture2D>("textures/box-specular.png");
		Texture2D::Sptr    monkeyTex = ResourceManager::CreateAsset<Texture2D>("textures/monkey-uvMap.png");
		Texture2D::Sptr    leafTex = ResourceManager::CreateAsset<Texture2D>("textures/leaves.png");
		leafTex->SetMinFilter(MinFilter::Nearest);
		leafTex->SetMagFilter(MagFilter::Nearest);


		// Here we'll load in the cubemap, as well as a special shader to handle drawing the skybox
		TextureCube::Sptr testCubemap = ResourceManager::CreateAsset<TextureCube>("cubemaps/ocean/ocean.jpg");
		ShaderProgram::Sptr      skyboxShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/skybox_vert.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/skybox_frag.glsl" }
		});

		// Create an empty scene
		Gameplay::Scene::Sptr scene = std::make_shared<Gameplay::Scene>();

		// Setting up our enviroment map
		//scene->SetSkyboxTexture(testCubemap);
		//scene->SetSkyboxShader(skyboxShader);
		// Since the skybox I used was for Y-up, we need to rotate it 90 deg around the X-axis to convert it to z-up
		//scene->SetSkyboxRotation(glm::rotate(MAT4_IDENTITY, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)));

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

		scene->Lights[4].Position = glm::vec3(-0.150f, 5.470f, 3.0f);
		scene->Lights[4].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[4].Range = 189.500f;

		scene->Lights[5].Position = glm::vec3(-14.41f, 0.56f, 3.0f);
		scene->Lights[5].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[5].Range = 10.0f;

		scene->Lights[6].Position = glm::vec3(-8.17f, 0.87f, 3.0f);
		scene->Lights[6].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[6].Range = 10.0f;

		scene->Lights[7].Position = glm::vec3(-2.060f, -0.94f, 4.60f);
		scene->Lights[7].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[7].Range = -0.600f;

		// We'll create a mesh that is a simple plane that we can resize later
		Gameplay::MeshResource::Sptr planeMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>();
		planeMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(1.0f)));
		planeMesh->GenerateMesh();

		Gameplay::MeshResource::Sptr sphere = ResourceManager::CreateAsset<Gameplay::MeshResource>();
		sphere->AddParam(MeshBuilderParam::CreateIcoSphere(ZERO, ONE, 5));
		sphere->GenerateMesh();

		// Set up the scene's camera
		Gameplay::GameObject::Sptr camera = scene->CreateGameObject("Main Camera");
		{
			camera->SetPostion(glm::vec3(-1.42f, 4.69f, 5.73f));
			//camera->SetPostion(glm::vec3(-1.42f, 18.67f, 17.420));
			camera->LookAt(glm::vec3(0.0f));
			camera->SetRotation(glm::vec3(59.0f, 0.0f, 177.0f));
			camera->SetScale(glm::vec3(1.0f, 1.0f, 3.1f));

			Gameplay::Camera::Sptr cam = camera->Add<Gameplay::Camera>();
			//cam->SetOrthoEnabled(true);
			//cam->SetOrthoVerticalScale(10.0f);
			// Make sure that the camera is set as the scene's main camera!
			scene->MainCamera = cam;

		}

		// Set up all our sample objects
		//setup trashy
		Gameplay::MeshResource::Sptr trashyMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashy.obj");
		Texture2D::Sptr trashyTex = ResourceManager::CreateAsset<Texture2D>("textures/trashyTEX.png");
		// Create our material
		Gameplay::Material::Sptr trashyMaterial = ResourceManager::CreateAsset<Gameplay::Material>(animShader);
		{
			trashyMaterial->Name = "Trashy";
			trashyMaterial->Set("u_Material.Diffuse", trashyTex);
			trashyMaterial->Set("u_Material.Shininess", 1.0f);


		}
		Gameplay::GameObject::Sptr trashyM = scene->CreateGameObject("Trashy"); //SEARCHBAR TAGS: PLAYERENTITY, PLAYER, TRASHYENTITY, TRASHYOBJECT
		{
			trashyM->SetPostion(glm::vec3(-1.5f, 0.0f, 1.0f));
			trashyM->SetRotation(glm::vec3(90.0f, 0.0f, 270.0f));
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
			//CollectTrashBehaviour::Sptr behaviour2 = trashyM->Add<CollectTrashBehaviour>();

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
		}




		Texture2D::Sptr planeTex = ResourceManager::CreateAsset<Texture2D>("textures/floor.jpg");

		//MeshResource::Sptr layoutMesh = ResourceManager::CreateAsset<MeshResource>("layout2.obj");
		Gameplay::Material::Sptr planeMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader); {
			planeMaterial->Name = "Plane";
			planeMaterial->Set("u_Material.Diffuse", planeTex);
			planeMaterial->Set("u_Material.Shininess", 1.0f);
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


			// Create and attach a RenderComponent to the object to draw our mesh
			/*RenderComponent::Sptr renderer = plane->Add<RenderComponent>();
			renderer->SetMesh(layoutMesh);
			renderer->SetMaterial(planeMaterial);*/

			// Attach a plane collider that extends infinitely along the X/Y axis
			Gameplay::Physics::RigidBody::Sptr physics = plane->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
			Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
			//box->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
			box->SetScale(glm::vec3(50.0f, -0.12f, 50.0f));
			physics->AddCollider(box);
		}

		Gameplay::MeshResource::Sptr enterMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("InvisPlane.obj");
		Texture2D::Sptr enterTex = ResourceManager::CreateAsset<Texture2D>("textures/ENTER.png");
		Gameplay::Material::Sptr enterMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
		{
			enterMaterial->Name = "Enter";
			enterMaterial->Set("u_Material.Diffuse", enterTex);
			enterMaterial->Set("u_Material.Shininess", 0.0f);
		}
		Gameplay::GameObject::Sptr enter = scene->CreateGameObject("Enter");
		{
			enter->SetPostion(glm::vec3(0.67f, -1.6f, 0.11f));
			enter->SetRotation(glm::vec3(0.0f, -74.0f, -90.0f));
			enter->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

			RenderComponent::Sptr renderer = enter->Add<RenderComponent>();
			renderer->SetMesh(enterMesh);
			renderer->SetMaterial(enterMaterial);
		}
		//layout
		Gameplay::MeshResource::Sptr layoutMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("layoutclean.obj");
		Texture2D::Sptr layoutTex = ResourceManager::CreateAsset<Texture2D>("textures/layout.jpg");
		Gameplay::Material::Sptr layoutMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
		{
			layoutMaterial->Name = "Layout";
			layoutMaterial->Set("u_Material.Diffuse", layoutTex);
			layoutMaterial->Set("u_Material.Shininess", 0.0f);
		}
		Gameplay::GameObject::Sptr layout = scene->CreateGameObject("Layout");
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
			Gameplay::GameObject::Sptr layoutwall1 = scene->CreateGameObject("Layout Wall Front");
			{
				layoutwall1->SetPostion(glm::vec3(8.35f, 3.76f, 0.84f));
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
				layoutwall2->SetPostion(glm::vec3(8.35f, -6.69f, 0.84f));
				layoutwall2->SetScale(glm::vec3(3.04f, 0.27f, 1.17f));
				Gameplay::Physics::RigidBody::Sptr wall2Phys = layoutwall2->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr wall2 = Gameplay::Physics::BoxCollider::Create();
				//wall1->SetPosition(glm::vec3(11.85f, 3.23f, 1.05f));
				wall2->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall2->SetScale(glm::vec3(3.04f, 0.27f, 1.17f));
				wall2->SetExtents(glm::vec3(2.2f, 2.0f, 2.0f));
				wall2Phys->AddCollider(wall2);
			}

			//Right Bottom
			Gameplay::GameObject::Sptr layoutwall3 = scene->CreateGameObject("Layout Wall Right Bottom");
			{
				layoutwall3->SetPostion(glm::vec3(1.49f, 1.51f, 0.0f));
				layoutwall3->SetScale(glm::vec3(0.3f, 1.54f, 2.37f));
				Gameplay::Physics::RigidBody::Sptr wall3Phys = layoutwall3->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr wall3 = Gameplay::Physics::BoxCollider::Create();
				wall3->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall3->SetScale(glm::vec3(0.3f, 1.54f, 2.37f));
				wall3->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wall3Phys->AddCollider(wall3);
			}

			//Right Top
			Gameplay::GameObject::Sptr layoutwall4 = scene->CreateGameObject("Layout Wall Right Top");
			{
				layoutwall4->SetPostion(glm::vec3(1.49f, -4.51f, 0.93f));
				layoutwall4->SetScale(glm::vec3(0.3f, 1.54f, 2.37f));
				Gameplay::Physics::RigidBody::Sptr wall4Phys = layoutwall4->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr wall4 = Gameplay::Physics::BoxCollider::Create();
				wall4->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall4->SetScale(glm::vec3(0.3f, 1.54f, 2.37f));
				wall4->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wall4Phys->AddCollider(wall4);
			}

			//Left Wall Corner Top
			Gameplay::GameObject::Sptr layoutwall5 = scene->CreateGameObject("Layout Wall Left Corner");
			{
				layoutwall5->SetPostion(glm::vec3(11.26f, -4.3f, 1.14f));
				layoutwall5->SetScale(glm::vec3(0.19f, 1.54f, 2.4f));
				Gameplay::Physics::RigidBody::Sptr wall5Phys = layoutwall5->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr wall5 = Gameplay::Physics::BoxCollider::Create();
				wall5->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall5->SetScale(glm::vec3(0.19f, 1.54f, 2.4f));
				wall5->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wall5Phys->AddCollider(wall5);
			}
			Gameplay::GameObject::Sptr layoutwall5b = scene->CreateGameObject("Layout Wall Left Corner");
			{
				layoutwall5b->SetPostion(glm::vec3(12.7f, -2.79, 1.1f));
				layoutwall5b->SetScale(glm::vec3(1.68f, 0.18f, 2.44f));
				Gameplay::Physics::RigidBody::Sptr wall5Physb = layoutwall5b->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr wall5b = Gameplay::Physics::BoxCollider::Create();
				wall5b->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall5b->SetScale(glm::vec3(1.68f, 0.18f, 2.44f));
				wall5b->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wall5Physb->AddCollider(wall5b);
			}

			//Left Wall
			Gameplay::GameObject::Sptr layoutwall6 = scene->CreateGameObject("Layout Wall Left");
			{
				layoutwall6->SetPostion(glm::vec3(14.53f, 0.36f, 1.22f));
				layoutwall6->SetScale(glm::vec3(0.32f, 3.16f, 2.28f));
				Gameplay::Physics::RigidBody::Sptr wall6Phys = layoutwall6->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr wall6 = Gameplay::Physics::BoxCollider::Create();
				wall6->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				wall6->SetScale(glm::vec3(0.32f, 3.16f, 2.28f));
				wall6->SetExtents(glm::vec3(1.0f, 1.0f, 1.0f));
				wall6Phys->AddCollider(wall6);
			}
		}
		//placeholder trash object  TAGS: PLACEHOLDER, this is how u create a new object for trash
		//setup trash
		Gameplay::MeshResource::Sptr trashMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("cup.obj");
		Texture2D::Sptr trashTex = ResourceManager::CreateAsset<Texture2D>("textures/acup.jpg");
		// Create our material
		Gameplay::Material::Sptr trashMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
		{
			trashMaterial->Name = "Trash";
			trashMaterial->Set("u_Material.Diffuse", trashTex);
			trashMaterial->Set("u_Material.Shininess", 0.3f);

		}
		//cup collection
		{
			Gameplay::GameObject::Sptr trashM = scene->CreateGameObject("Trash1"); //PLACEHOLDER change to any object u deem necessary change the set mesh and set material
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
			Gameplay::GameObject::Sptr trash2 = scene->CreateGameObject("Trash2"); //Has Cup PLACEHOLDER
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
			Gameplay::GameObject::Sptr trash3 = scene->CreateGameObject("Trash3"); //has Cup PLACEHOLDER
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
			Gameplay::GameObject::Sptr trash4 = scene->CreateGameObject("Trash4"); //has cup PLACEHOLDER
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
		//set up robo toy
		Gameplay::MeshResource::Sptr roboMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("Robo/RoboWalk_000001.obj");
		Texture2D::Sptr roboTex = ResourceManager::CreateAsset<Texture2D>("textures/robo.png");
		Gameplay::Material::Sptr roboMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
		{
			roboMaterial->Name = "Robo";
			roboMaterial->Set("u_Material.Diffuse", roboTex);
			roboMaterial->Set("u_Material.Shininess", 0.5f);
		}
		Gameplay::GameObject::Sptr robo = scene->CreateGameObject("Robo");
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

			Gameplay::MeshResource::Sptr roboMesh2 = ResourceManager::CreateAsset<Gameplay::MeshResource>("Robo/RoboWalk_000001.obj");
			Gameplay::MeshResource::Sptr roboMesh3 = ResourceManager::CreateAsset<Gameplay::MeshResource>("Robo/RoboWalk_000005.obj");
			Gameplay::MeshResource::Sptr roboMesh4 = ResourceManager::CreateAsset<Gameplay::MeshResource>("Robo/RoboWalk_000008.obj");
			Gameplay::MeshResource::Sptr roboMesh5 = ResourceManager::CreateAsset<Gameplay::MeshResource>("Robo/RoboWalk_000010.obj");
			Gameplay::MeshResource::Sptr roboMesh6 = ResourceManager::CreateAsset<Gameplay::MeshResource>("Robo/RoboWalk_000015.obj");
			Gameplay::MeshResource::Sptr roboMesh7 = ResourceManager::CreateAsset<Gameplay::MeshResource>("Robo/RoboWalk_000018.obj");
			Gameplay::MeshResource::Sptr roboMesh8 = ResourceManager::CreateAsset<Gameplay::MeshResource>("Robo/RoboWalk_000020.obj");

			std::vector<Gameplay::MeshResource::Sptr> frames;
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
		Gameplay::MeshResource::Sptr bookMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("Book/AnimBook_000001.obj");
		Texture2D::Sptr bookTex = ResourceManager::CreateAsset<Texture2D>("textures/Book.png");
		Gameplay::Material::Sptr bookMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
		{
			bookMaterial->Name = "Book";
			bookMaterial->Set("u_Material.Diffuse", bookTex);
			bookMaterial->Set("u_Material.Shininess", 0.0f);
		}
		Gameplay::GameObject::Sptr book = scene->CreateGameObject("Book");
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

			Gameplay::MeshResource::Sptr bookMesh2 = ResourceManager::CreateAsset<Gameplay::MeshResource>("Book/AnimBook_000001.obj");
			Gameplay::MeshResource::Sptr bookMesh3 = ResourceManager::CreateAsset<Gameplay::MeshResource>("Book/AnimBook_000004.obj");
			Gameplay::MeshResource::Sptr bookMesh4 = ResourceManager::CreateAsset<Gameplay::MeshResource>("Book/AnimBook_000008.obj");
			Gameplay::MeshResource::Sptr bookMesh5 = ResourceManager::CreateAsset<Gameplay::MeshResource>("Book/AnimBook_000012.obj");
			Gameplay::MeshResource::Sptr bookMesh6 = ResourceManager::CreateAsset<Gameplay::MeshResource>("Book/AnimBook_000016.obj");

			std::vector<Gameplay::MeshResource::Sptr> frames;
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
		Gameplay::MeshResource::Sptr toyMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("toy.obj");
		Texture2D::Sptr toyTex = ResourceManager::CreateAsset<Texture2D>("textures/toy.jpg");
		// Create our material
		Gameplay::Material::Sptr toyMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
		{
			toyMaterial->Name = "Toy";
			toyMaterial->Set("u_Material.Diffuse", toyTex);
			toyMaterial->Set("u_Material.Shininess", 0.0f);

		}
		// rolling toy collection
		{
			Gameplay::GameObject::Sptr toyM = scene->CreateGameObject("Toy");
			{
				toyM->SetPostion(glm::vec3(-0.19f, -4.25f, -0.63f));
				toyM->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
				toyM->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
				// Add a render component
				RenderComponent::Sptr renderer = toyM->Add<RenderComponent>();
				renderer->SetMesh(toyMesh);
				renderer->SetMaterial(toyMaterial);
				// Add a dynamic rigid body to this monkey
				Gameplay::Physics::RigidBody::Sptr physics = toyM->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetPosition(glm::vec3(0.00f, 0.22f, 0.0f));
				box->SetScale(glm::vec3(0.28f * 0.4f, 0.24f * 0.4f, 0.57f * 0.4f));
				physics->AddCollider(box);
				Gameplay::Physics::TriggerVolume::Sptr volume = toyM->Add<Gameplay::Physics::TriggerVolume>();
				Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
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

				Gameplay::MeshResource::Sptr toyMesh2 = ResourceManager::CreateAsset<Gameplay::MeshResource>("RollingToy/toyroll_000001.obj");
				Gameplay::MeshResource::Sptr toyMesh3 = ResourceManager::CreateAsset<Gameplay::MeshResource>("RollingToy/toyroll_000005.obj");
				Gameplay::MeshResource::Sptr toyMesh4 = ResourceManager::CreateAsset<Gameplay::MeshResource>("RollingToy/toyroll_000010.obj");
				Gameplay::MeshResource::Sptr toyMesh5 = ResourceManager::CreateAsset<Gameplay::MeshResource>("RollingToy/toyroll_000015.obj");
				Gameplay::MeshResource::Sptr toyMesh6 = ResourceManager::CreateAsset<Gameplay::MeshResource>("RollingToy/toyroll_000020.obj");

				std::vector<Gameplay::MeshResource::Sptr> frames;
				frames.push_back(toyMesh2);
				frames.push_back(toyMesh3);
				frames.push_back(toyMesh4);
				frames.push_back(toyMesh5);
				frames.push_back(toyMesh6);

				morph2->SetInitial();
				morph2->SetFrameTime(0.1f);
				morph2->SetFrames(frames);
			}
			Gameplay::GameObject::Sptr toyM2 = scene->CreateGameObject("Toy2");
			{
				toyM2->SetPostion(glm::vec3(2.00f, -4.25f, -0.63f));
				toyM2->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
				toyM2->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
				// Add a render component
				RenderComponent::Sptr renderer = toyM2->Add<RenderComponent>();
				renderer->SetMesh(toyMesh);
				renderer->SetMaterial(toyMaterial);
				// Add a dynamic rigid body to this monkey
				Gameplay::Physics::RigidBody::Sptr physics = toyM2->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetPosition(glm::vec3(0.00f, 0.22f, 0.0f));
				box->SetScale(glm::vec3(0.28f * 0.4f, 0.24f * 0.4f, 0.57f * 0.4f));
				physics->AddCollider(box);
				Gameplay::Physics::TriggerVolume::Sptr volume = toyM2->Add<Gameplay::Physics::TriggerVolume>();
				Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
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

				Gameplay::MeshResource::Sptr toyMesh2 = ResourceManager::CreateAsset<Gameplay::MeshResource>("RollingToy/toyroll_000001.obj");
				Gameplay::MeshResource::Sptr toyMesh3 = ResourceManager::CreateAsset<Gameplay::MeshResource>("RollingToy/toyroll_000005.obj");
				Gameplay::MeshResource::Sptr toyMesh4 = ResourceManager::CreateAsset<Gameplay::MeshResource>("RollingToy/toyroll_000010.obj");
				Gameplay::MeshResource::Sptr toyMesh5 = ResourceManager::CreateAsset<Gameplay::MeshResource>("RollingToy/toyroll_000015.obj");
				Gameplay::MeshResource::Sptr toyMesh6 = ResourceManager::CreateAsset<Gameplay::MeshResource>("RollingToy/toyroll_000020.obj");

				std::vector<Gameplay::MeshResource::Sptr> frames;
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
		Gameplay::MeshResource::Sptr spillMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("spill.obj");
		Texture2D::Sptr spillTex = ResourceManager::CreateAsset<Texture2D>("textures/goo.png");
		// Create our material
		Gameplay::Material::Sptr spillMaterial = ResourceManager::CreateAsset<Gameplay::Material>(rackShader);
		{
			spillMaterial->Name = "Spill";
			spillMaterial->Set("u_Material.Diffuse", spillTex);
			spillMaterial->Set("u_Material.Shininess", 1.0f);

		}
		//spill collection
		{
			Gameplay::GameObject::Sptr spillM = scene->CreateGameObject("Spill");
			{
				spillM->SetPostion(glm::vec3(4.15f, 0.39f, 0.03f));
				spillM->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
				spillM->SetScale(glm::vec3(1.11f, 0.7f, 0.7f));
				// Add a render component
				RenderComponent::Sptr renderer = spillM->Add<RenderComponent>();
				renderer->SetMesh(spillMesh);
				renderer->SetMaterial(spillMaterial);
				// Add a dynamic rigid body to this monkey
				Gameplay::Physics::RigidBody::Sptr physics = spillM->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				//box->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
				//box->SetScale(glm::vec3(0.22f, 0.37f, 0.24f));
				//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
				box->SetScale(glm::vec3(0.75f, 0.001f, 0.53f));
				//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
				physics->AddCollider(box);
				//physics->SetMass(0.0f);
				Gameplay::Physics::TriggerVolume::Sptr volume = spillM->Add<Gameplay::Physics::TriggerVolume>();
				Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
				//box2->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
				box2->SetScale(glm::vec3(0.75f, 0.001f, 0.53f));
				volume->AddCollider(box2);
				SpillBehaviour::Sptr behaviour = spillM->Add<SpillBehaviour>();

			}
			Gameplay::GameObject::Sptr spill2 = scene->CreateGameObject("Spill2");
			{
				spill2->SetPostion(glm::vec3(6.32f, -3.39f, 0.03f));
				spill2->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
				spill2->SetScale(glm::vec3(1.35f, 0.7f, 0.7f));
				// Add a render component
				RenderComponent::Sptr renderer = spill2->Add<RenderComponent>();
				renderer->SetMesh(spillMesh);
				renderer->SetMaterial(spillMaterial);
				// Add a dynamic rigid body to this monkey
				Gameplay::Physics::RigidBody::Sptr physics = spill2->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				//box->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
				//box->SetScale(glm::vec3(0.22f, 0.37f, 0.24f));
				//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
				box->SetScale(glm::vec3(1.19f, 0.001f, 0.53f));
				//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
				physics->AddCollider(box);
				//physics->SetMass(0.0f);
				Gameplay::Physics::TriggerVolume::Sptr volume = spill2->Add<Gameplay::Physics::TriggerVolume>();
				Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
				//box2->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
				box2->SetScale(glm::vec3(1.19f, 0.001f, 0.53f));
				volume->AddCollider(box2);
				SpillBehaviour::Sptr behaviour = spill2->Add<SpillBehaviour>();
			}
			Gameplay::GameObject::Sptr spill3 = scene->CreateGameObject("Spill3");
			{
				spill3->SetPostion(glm::vec3(6.38, -0.68f, 0.03f));
				spill3->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
				spill3->SetScale(glm::vec3(1.18f, 0.75f, 0.73f));
				// Add a render component
				RenderComponent::Sptr renderer = spill3->Add<RenderComponent>();
				renderer->SetMesh(spillMesh);
				renderer->SetMaterial(spillMaterial);
				// Add a dynamic rigid body to this monkey
				Gameplay::Physics::RigidBody::Sptr physics = spill3->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				//box->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
				//box->SetScale(glm::vec3(0.22f, 0.37f, 0.24f));
				//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
				box->SetScale(glm::vec3(1.09f, 0.001f, 0.53f));
				//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
				physics->AddCollider(box);
				//physics->SetMass(0.0f);
				Gameplay::Physics::TriggerVolume::Sptr volume = spill3->Add<Gameplay::Physics::TriggerVolume>();
				Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
				//box2->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
				box2->SetScale(glm::vec3(1.09f, 0.001f, 0.53));
				volume->AddCollider(box2);
				SpillBehaviour::Sptr behaviour = spill3->Add<SpillBehaviour>();
			}
			Gameplay::GameObject::Sptr spill4 = scene->CreateGameObject("Spill4");
			{
				spill4->SetPostion(glm::vec3(12.41f, 0.94, 0.03f));
				spill4->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
				spill4->SetScale(glm::vec3(0.7f, 0.7f, 1.43f));
				// Add a render component
				RenderComponent::Sptr renderer = spill4->Add<RenderComponent>();
				renderer->SetMesh(spillMesh);
				renderer->SetMaterial(spillMaterial);
				// Add a dynamic rigid body to this monkey
				Gameplay::Physics::RigidBody::Sptr physics = spill4->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				//box->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
				//box->SetScale(glm::vec3(0.22f, 0.37f, 0.24f));
				//box->SetPosition(glm::vec3(0.02f, 0.5f, 0.0f));
				box->SetScale(glm::vec3(0.5f, 0.001f, 1.61f));
				//box->SetExtents(glm::vec3(0.8, 2.68, 0.83));
				physics->AddCollider(box);
				//physics->SetMass(0.0f);
				Gameplay::Physics::TriggerVolume::Sptr volume = spill4->Add<Gameplay::Physics::TriggerVolume>();
				Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
				//box2->SetPosition(glm::vec3(0.04f, 0.6f, 0.18f));
				box2->SetScale(glm::vec3(0.5f, 0.001f, 1.61f));
				volume->AddCollider(box2);
				SpillBehaviour::Sptr behaviour = spill4->Add<SpillBehaviour>();
			}
		}
		//bin model
		Gameplay::MeshResource::Sptr binMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("BigBenClosed_000001.obj");
		Texture2D::Sptr binTex = ResourceManager::CreateAsset<Texture2D>("textures/bigben.png");
		// Create our material
		Gameplay::Material::Sptr binMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
		{
			binMaterial->Name = "Bin";
			binMaterial->Set("u_Material.Diffuse", binTex);
			binMaterial->Set("u_Material.Shininess", 1.0f);

		}
		Gameplay::GameObject::Sptr binM = scene->CreateGameObject("Bin");
		{
			binM->SetPostion(glm::vec3(-1.5f, 2.0f, 0.07f));
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
			box2->SetScale(glm::vec3(0.25f, 0.22f, 0.2f));
			volume->AddCollider(box2);
			SubmittingTrashBehaviour::Sptr behaviour2 = binM->Add<SubmittingTrashBehaviour>();

			//ANIMATION STUFF////
			MorphMeshRenderer::Sptr morph1 = binM->Add<MorphMeshRenderer>();
			morph1->SetMorphMeshRenderer(binMesh, binMaterial);
			MorphAnimator::Sptr morph2 = binM->Add<MorphAnimator>();

			//idle frames
			//std::vector <MeshResource::Sptr> frames2;
			Gameplay::MeshResource::Sptr binMesh8 = ResourceManager::CreateAsset<Gameplay::MeshResource>("BigBenClosed_000001.obj");
			std::vector<Gameplay::MeshResource::Sptr> closed;
			closed.push_back(binMesh8);
			behaviour2->getIdle(closed); //send idle frames to behaviour

			morph2->SetInitial();
			morph2->SetFrameTime(0.2f);
			morph2->SetFrames(closed);

		}
		//Shelf object
		Gameplay::MeshResource::Sptr shelfMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("shelf.obj");
		Texture2D::Sptr shelfTex = ResourceManager::CreateAsset <Texture2D>("textures/shelf.png");
		//Create Material
		Gameplay::Material::Sptr shelfMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
		{
			shelfMaterial->Name = "Shelf";
			shelfMaterial->Set("u_Material.Diffuse", shelfTex);
			shelfMaterial->Set("u_Material.Shininess", 0.2f);
		}
		//shelf collection
		{
			Gameplay::GameObject::Sptr shelf = scene->CreateGameObject("Shelf1");
			{
				shelf->SetPostion(glm::vec3(4.250f, -4.690f, 0.0f));
				shelf->SetRotation(glm::vec3(90.0, 0.0f, 0.0f));
				shelf->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				RenderComponent::Sptr renderer = shelf->Add<RenderComponent>();
				renderer->SetMesh(shelfMesh);
				renderer->SetMaterial(shelfMaterial);

				Gameplay::Physics::RigidBody::Sptr physics = shelf->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetScale(glm::vec3(1.04f, 1.62f, 1.54f));
				box->SetExtents(glm::vec3(0.5f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
			Gameplay::GameObject::Sptr shelf2 = scene->CreateGameObject("Shelf2");
			{
				shelf2->SetPostion(glm::vec3(6.20f, -5.69f, 0.0f));
				shelf2->SetRotation(glm::vec3(90.0, 0.0f, -90.0f));
				shelf2->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				RenderComponent::Sptr renderer = shelf2->Add<RenderComponent>();
				renderer->SetMesh(shelfMesh);
				renderer->SetMaterial(shelfMaterial);

				Gameplay::Physics::RigidBody::Sptr physics = shelf2->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetScale(glm::vec3(1.04f, 1.62f, 1.54f));
				box->SetExtents(glm::vec3(0.5f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
			Gameplay::GameObject::Sptr shelf3 = scene->CreateGameObject("Shelf3");
			{
				shelf3->SetPostion(glm::vec3(9.26f, -5.69f, 0.0f));
				shelf3->SetRotation(glm::vec3(90.0, 0.0f, -90.0f));
				shelf3->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				RenderComponent::Sptr renderer = shelf3->Add<RenderComponent>();
				renderer->SetMesh(shelfMesh);
				renderer->SetMaterial(shelfMaterial);

				Gameplay::Physics::RigidBody::Sptr physics = shelf3->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetScale(glm::vec3(1.04f, 1.62f, 1.54f));
				box->SetExtents(glm::vec3(0.5f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
			Gameplay::GameObject::Sptr shelf4 = scene->CreateGameObject("Shelf4");
			{
				shelf4->SetPostion(glm::vec3(7.9f, 0.1f, 0.0f));
				shelf4->SetRotation(glm::vec3(90.0, 0.0f, 0.0f));
				shelf4->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				RenderComponent::Sptr renderer = shelf4->Add<RenderComponent>();
				renderer->SetMesh(shelfMesh);
				renderer->SetMaterial(shelfMaterial);

				Gameplay::Physics::RigidBody::Sptr physics = shelf4->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetScale(glm::vec3(1.04f, 1.62f, 1.54f));
				box->SetExtents(glm::vec3(0.5f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
			Gameplay::GameObject::Sptr shelf5 = scene->CreateGameObject("Shelf5");
			{
				shelf5->SetPostion(glm::vec3(8.910f, 0.1f, 0.0f));
				shelf5->SetRotation(glm::vec3(90.0, 0.0f, -180.0f));
				shelf5->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				RenderComponent::Sptr renderer = shelf5->Add<RenderComponent>();
				renderer->SetMesh(shelfMesh);
				renderer->SetMaterial(shelfMaterial);

				Gameplay::Physics::RigidBody::Sptr physics = shelf5->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetScale(glm::vec3(1.04f, 1.62f, 1.54f));
				box->SetExtents(glm::vec3(0.5f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
		}
		//tv box object
		Gameplay::MeshResource::Sptr tvboxMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("tvbox.obj");
		Texture2D::Sptr tvboxTex = ResourceManager::CreateAsset<Texture2D>("textures/tvbox.png");
		//Create Material
		Gameplay::Material::Sptr tvboxMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
		{
			tvboxMaterial->Name = "TvBox";
			tvboxMaterial->Set("u_Material.Diffuse", tvboxTex);
			tvboxMaterial->Set("u_Material.Shininess", 0.0f);
		}
		//tv box collection
		{
			Gameplay::GameObject::Sptr tvbox1 = scene->CreateGameObject("tvbox1");
			{
				tvbox1->SetPostion(glm::vec3(8.37f, 2.1f, 0.0f));
				tvbox1->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
				tvbox1->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));

				RenderComponent::Sptr renderer = tvbox1->Add<RenderComponent>();
				renderer->SetMesh(tvboxMesh);
				renderer->SetMaterial(tvboxMaterial);

				Gameplay::Physics::RigidBody::Sptr physics = tvbox1->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetScale(glm::vec3(0.1f, 0.98f, 0.82f));
				box->SetExtents(glm::vec3(1.7f, 1.0f, 1.0f));
				physics->AddCollider(box);

			}
			Gameplay::GameObject::Sptr tvbox2 = scene->CreateGameObject("tvbox2");
			{
				tvbox2->SetPostion(glm::vec3(8.37f, 2.51f, 0.0f));
				tvbox2->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
				tvbox2->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));

				RenderComponent::Sptr renderer = tvbox2->Add<RenderComponent>();
				renderer->SetMesh(tvboxMesh);
				renderer->SetMaterial(tvboxMaterial);

				Gameplay::Physics::RigidBody::Sptr physics = tvbox2->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetScale(glm::vec3(0.1f, 0.98f, 0.82f));
				box->SetExtents(glm::vec3(1.7f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
			Gameplay::GameObject::Sptr tvbox3 = scene->CreateGameObject("tvbox3");
			{
				tvbox3->SetPostion(glm::vec3(8.37f, 2.92f, 0.0f));
				tvbox3->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
				tvbox3->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));

				RenderComponent::Sptr renderer = tvbox3->Add<RenderComponent>();
				renderer->SetMesh(tvboxMesh);
				renderer->SetMaterial(tvboxMaterial);

				Gameplay::Physics::RigidBody::Sptr physics = tvbox3->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetScale(glm::vec3(0.1f, 0.98f, 0.82f));
				box->SetExtents(glm::vec3(1.7f, 1.0f, 1.0f));
				physics->AddCollider(box);

			}
			Gameplay::GameObject::Sptr tvbox4 = scene->CreateGameObject("tvbox4");
			{
				tvbox4->SetPostion(glm::vec3(5.09f, -0.76f, 0.0f));
				tvbox4->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
				tvbox4->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));

				RenderComponent::Sptr renderer = tvbox4->Add<RenderComponent>();
				renderer->SetMesh(tvboxMesh);
				renderer->SetMaterial(tvboxMaterial);

				Gameplay::Physics::RigidBody::Sptr physics = tvbox4->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetScale(glm::vec3(0.1f, 0.98f, 0.82f));
				box->SetExtents(glm::vec3(1.7f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
			Gameplay::GameObject::Sptr tvbox5 = scene->CreateGameObject("tvbox5");
			{
				tvbox5->SetPostion(glm::vec3(5.09f, 0.98f, 0.0f));
				tvbox5->SetRotation(glm::vec3(90.0f, 0.0f, -180.0f));
				tvbox5->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));

				RenderComponent::Sptr renderer = tvbox5->Add<RenderComponent>();
				renderer->SetMesh(tvboxMesh);
				renderer->SetMaterial(tvboxMaterial);

				Gameplay::Physics::RigidBody::Sptr physics = tvbox5->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetScale(glm::vec3(0.1f, 0.98f, 0.82f));
				box->SetExtents(glm::vec3(1.7f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
			Gameplay::GameObject::Sptr tvbox6 = scene->CreateGameObject("tvbox6");
			{
				tvbox6->SetPostion(glm::vec3(9.97f, -3.45f, 0.0f));
				tvbox6->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
				tvbox6->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));

				RenderComponent::Sptr renderer = tvbox6->Add<RenderComponent>();
				renderer->SetMesh(tvboxMesh);
				renderer->SetMaterial(tvboxMaterial);

				Gameplay::Physics::RigidBody::Sptr physics = tvbox6->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetScale(glm::vec3(0.1f, 0.98f, 0.82f));
				box->SetExtents(glm::vec3(1.7f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
			Gameplay::GameObject::Sptr tvbox7 = scene->CreateGameObject("tvbox7");
			{
				tvbox7->SetPostion(glm::vec3(8.31f, -3.45f, 0.0f));
				tvbox7->SetRotation(glm::vec3(90.0f, 0.0f, -90.0f));
				tvbox7->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));

				RenderComponent::Sptr renderer = tvbox7->Add<RenderComponent>();
				renderer->SetMesh(tvboxMesh);
				renderer->SetMaterial(tvboxMaterial);

				Gameplay::Physics::RigidBody::Sptr physics = tvbox7->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetScale(glm::vec3(0.1f, 0.98f, 0.82f));
				box->SetExtents(glm::vec3(1.7f, 1.0f, 1.0f));
				physics->AddCollider(box);
			}
		}
		//tv object
		Gameplay::MeshResource::Sptr tvMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("TV.obj");
		Texture2D::Sptr tvTex = ResourceManager::CreateAsset<Texture2D>("textures/tvtex.jpg");
		//Create Material
		Gameplay::Material::Sptr tvMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
		{
			tvMaterial->Name = "Tv";
			tvMaterial->Set("u_Material.Diffuse", tvTex);
			tvMaterial->Set("u_Material.Shininess", 0.75f);
		}
		//tv collection
		{
			Gameplay::GameObject::Sptr tv1 = scene->CreateGameObject("tv1");
			{
				tv1->SetPostion(glm::vec3(2.47f, 0.91f, 0.0f));
				tv1->SetRotation(glm::vec3(90.0f, 0.0f, -90.0f));
				tv1->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

				RenderComponent::Sptr renderer = tv1->Add<RenderComponent>();
				renderer->SetMesh(tvMesh);
				renderer->SetMaterial(tvMaterial);

				Gameplay::Physics::RigidBody::Sptr physics = tv1->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetScale(glm::vec3(0.15f, 1.01f, 0.84f));
				physics->AddCollider(box);
			}
			Gameplay::GameObject::Sptr tv2 = scene->CreateGameObject("tv2");
			{
				tv2->SetPostion(glm::vec3(4.27f, -2.89f, 0.0f));
				tv2->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
				tv2->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

				RenderComponent::Sptr renderer = tv2->Add<RenderComponent>();
				renderer->SetMesh(tvMesh);
				renderer->SetMaterial(tvMaterial);

				Gameplay::Physics::RigidBody::Sptr physics = tv2->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetScale(glm::vec3(0.15f, 1.01f, 0.84f));
				physics->AddCollider(box);
			}
			Gameplay::GameObject::Sptr tv3 = scene->CreateGameObject("tv3");
			{
				tv3->SetPostion(glm::vec3(13.31f, 2.27f, 0.0f));
				tv3->SetRotation(glm::vec3(90.0f, 0.0f, -152.0f));
				tv3->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

				RenderComponent::Sptr renderer = tv3->Add<RenderComponent>();
				renderer->SetMesh(tvMesh);
				renderer->SetMaterial(tvMaterial);

				Gameplay::Physics::RigidBody::Sptr physics = tv3->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
				Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
				box->SetScale(glm::vec3(0.15f, 1.01f, 0.84f));
				physics->AddCollider(box);
			}
		}
		//rectangle and trashy images for menu
		//cash counter
		Gameplay::MeshResource::Sptr cashMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("cashcounter.obj");
		Texture2D::Sptr cashTex = ResourceManager::CreateAsset<Texture2D>("textures/cash.png");
		//create Material
		Gameplay::Material::Sptr cashMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
		{
			cashMaterial->Name = "Cash";
			cashMaterial->Set("u_Material.Diffuse", cashTex);
			cashMaterial->Set("u_Material.Shininess", 0.75f);
		}
		//cash counter
		Gameplay::GameObject::Sptr cashcounter = scene->CreateGameObject("CashCounter");
		{
			cashcounter->SetPostion(glm::vec3(12.3f, -1.79f, 0.0f));
			cashcounter->SetRotation(glm::vec3(90.0f, 0.0f, -180.0f));
			cashcounter->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

			RenderComponent::Sptr renderer = cashcounter->Add<RenderComponent>();
			renderer->SetMesh(cashMesh);
			renderer->SetMaterial(cashMaterial);

			Gameplay::Physics::RigidBody::Sptr physics = cashcounter->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
			Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
			box->SetScale(glm::vec3(0.520f, 0.740f, 1.210f));
			physics->AddCollider(box);
		}
		Gameplay::MeshResource::Sptr recMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("RecOBJ.obj");
		Texture2D::Sptr recTex = ResourceManager::CreateAsset<Texture2D>("textures/Rec1.png");
		// Create our material
		Gameplay::Material::Sptr recMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
		{
			recMaterial->Name = "Rec";
			recMaterial->Set("u_Material.Diffuse", recTex);
			recMaterial->Set("u_Material.Shininess", 1.0f);

		}
		Gameplay::GameObject::Sptr recE = scene->CreateGameObject("Rec");
		{
			recE->SetPostion(glm::vec3(-1.36f, 1.22f, 7.0f));
			recE->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
			recE->SetScale(glm::vec3(1.0f, 5.64f, 3.46f));
			// Add a render component
			RenderComponent::Sptr renderer = recE->Add<RenderComponent>();

			renderer->SetMesh(recMesh);
			renderer->SetMaterial(recMaterial);
			//Gameplay::Physics::RigidBody::Sptr physics = recE->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);

		}
		Gameplay::MeshResource::Sptr trashyEMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("trashy2OBJ.obj");
		Texture2D::Sptr trashyETex = ResourceManager::CreateAsset<Texture2D>("textures/Trashy2.png");
		// Create our material
		Gameplay::Material::Sptr trashyEMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
		{
			trashyEMaterial->Name = "trashyE";
			trashyEMaterial->Set("u_Material.Diffuse", trashyETex);
			trashyEMaterial->Set("u_Material.Shininess", 1.0f);

		}
		Gameplay::GameObject::Sptr trashyE = scene->CreateGameObject("TrashyE");
		{
			trashyE->SetPostion(glm::vec3(0.5f, 1.49f, 3.3f));
			trashyE->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
			trashyE->SetScale(glm::vec3(1.0f, 1.46f, 1.090f));
			// Add a render component
			RenderComponent::Sptr renderer = trashyE->Add<RenderComponent>();
			renderer->SetMesh(trashyEMesh);
			renderer->SetMaterial(trashyEMaterial);
			//Gameplay::Physics::RigidBody::Sptr physics = trashyE->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);

		}
		//Modelling Static objects
		{
			//Bench
			Gameplay::MeshResource::Sptr benchMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("bench.obj");
			Texture2D::Sptr benchTex = ResourceManager::CreateAsset<Texture2D>("textures/bench.jpg");
			Gameplay::Material::Sptr benchMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
			{
				benchMaterial->Name = "Bench";
				benchMaterial->Set("u_Material.Diffuse", benchTex);
				benchMaterial->Set("u_Material.Shininess", 0.0f);
			}
			Gameplay::GameObject::Sptr bench = scene->CreateGameObject("Bench");
			{
				bench->SetPostion(glm::vec3(-0.02f, -2.23f, 0.0f));
				bench->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
				bench->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

				RenderComponent::Sptr renderer = bench->Add<RenderComponent>();
				renderer->SetMesh(benchMesh);
				renderer->SetMaterial(benchMaterial);
			}
			//Cash Counter
			Gameplay::GameObject::Sptr cashModel = scene->CreateGameObject("cash Modelling");
			{
				cashModel->SetPostion(glm::vec3(-6.34f, -1.61f, 0.0f));
				cashModel->SetRotation(glm::vec3(90.0f, 0.0f, -90.0f));
				cashModel->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				RenderComponent::Sptr renderer = cashModel->Add<RenderComponent>();
				renderer->SetMesh(cashMesh);
				renderer->SetMaterial(cashMaterial);

			}
			//Computer
			Gameplay::MeshResource::Sptr computerMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("Computer.obj");
			Texture2D::Sptr computerTex = ResourceManager::CreateAsset<Texture2D>("textures/desktoptex.png");
			Gameplay::Material::Sptr computerMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
			{
				computerMaterial->Name = "Computer";
				computerMaterial->Set("u_Material.Diffuse", computerTex);
				computerMaterial->Set("u_Material.Shininess", 0.3f);
			}
			Gameplay::GameObject::Sptr computer = scene->CreateGameObject("Desktop");
			{
				computer->SetPostion(glm::vec3(-2.61f, -1.08f, 0.0f));
				computer->SetRotation(glm::vec3(90.0f, 0.0f, 59.0f));
				computer->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

				RenderComponent::Sptr renderer = computer->Add<RenderComponent>();
				renderer->SetMesh(computerMesh);
				renderer->SetMaterial(computerMaterial);
			}
			//Conveyor
			Gameplay::MeshResource::Sptr conveyorMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("conveyor.obj");
			Texture2D::Sptr conveyorTex = ResourceManager::CreateAsset<Texture2D>("textures/conayortex.png");
			Gameplay::Material::Sptr conveyorMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
			{
				conveyorMaterial->Name = "Conveyor";
				conveyorMaterial->Set("u_Material.Diffuse", conveyorTex);
				conveyorMaterial->Set("u_Material.Shininess", 0.2f);
			}
			Gameplay::GameObject::Sptr conveyor = scene->CreateGameObject("Conveyor");
			{
				conveyor->SetPostion(glm::vec3(-2.13f, -4.49f, 0.0f));
				conveyor->SetRotation(glm::vec3(90.0f, 0.0f, -75.0f));
				conveyor->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

				RenderComponent::Sptr renderer = conveyor->Add<RenderComponent>();
				renderer->SetMesh(conveyorMesh);
				renderer->SetMaterial(conveyorMaterial);
			}
			//Cup
			Gameplay::GameObject::Sptr cupModel = scene->CreateGameObject("cup Modelling");
			{
				cupModel->SetPostion(glm::vec3(-3.76f, -1.52f, 0.0f));
				cupModel->SetRotation(glm::vec3(90.0f, 0.0f, -90.0f));
				cupModel->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

				RenderComponent::Sptr renderer = cupModel->Add<RenderComponent>();
				renderer->SetMesh(trashMesh);
				renderer->SetMaterial(trashMaterial);
			}
			//Mirror
			Gameplay::MeshResource::Sptr mirrorMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("Mirror.obj");
			Texture2D::Sptr mirrorTex = ResourceManager::CreateAsset<Texture2D>("textures/mirror.jpg");
			Gameplay::Material::Sptr mirrorMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
			{
				mirrorMaterial->Name = "Mirror";
				mirrorMaterial->Set("u_Material.Diffuse", mirrorTex);
				mirrorMaterial->Set("u_Material.Shininess", 0.6f);
			}
			Gameplay::GameObject::Sptr mirror = scene->CreateGameObject("Mirror");
			{
				mirror->SetPostion(glm::vec3(-12.1f, -2.65f, 0.0f));
				mirror->SetRotation(glm::vec3(90.0f, -2.0f, 93.0f));
				mirror->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

				RenderComponent::Sptr renderer = mirror->Add<RenderComponent>();
				renderer->SetMesh(mirrorMesh);
				renderer->SetMaterial(mirrorMaterial);
			}
			//Plant
			Gameplay::MeshResource::Sptr plantMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("plant.obj");
			Texture2D::Sptr plantTex = ResourceManager::CreateAsset<Texture2D>("textures/planttex.png");
			Gameplay::Material::Sptr plantMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
			{
				plantMaterial->Name = "Plant";
				plantMaterial->Set("u_Material.Diffuse", plantTex);
				plantMaterial->Set("u_Material.Shininess", 0.0f);
			}
			Gameplay::GameObject::Sptr plant = scene->CreateGameObject("Plant");
			{
				plant->SetPostion(glm::vec3(-3.17f, -1.51f, 0.0f));
				plant->SetRotation(glm::vec3(90.0f, 0.0f, 72.0f));
				plant->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				RenderComponent::Sptr renderer = plant->Add<RenderComponent>();
				renderer->SetMesh(plantMesh);
				renderer->SetMaterial(plantMaterial);
			}
			//Rack
			Gameplay::MeshResource::Sptr rackMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("rack.obj");
			Texture2D::Sptr rackTex = ResourceManager::CreateAsset<Texture2D>("textures/rack.png");
			Gameplay::Material::Sptr rackMaterial = ResourceManager::CreateAsset<Gameplay::Material>(rackShader); //CHANGE THIS SHADER
			{
				rackMaterial->Name = "Rack";
				rackMaterial->Set("u_Material.Diffuse", rackTex);
				rackMaterial->Set("u_Material.Shininess", 0.2f);
			}
			Gameplay::GameObject::Sptr rack = scene->CreateGameObject("Rack");
			{
				rack->SetPostion(glm::vec3(-1.64f, -1.97f, 0.0f));
				rack->SetRotation(glm::vec3(90.0f, 0.0f, 63.0f));
				rack->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

				RenderComponent::Sptr renderer = rack->Add<RenderComponent>();
				renderer->SetMesh(rackMesh);
				renderer->SetMaterial(rackMaterial);
			}
			//Shelf
			Gameplay::GameObject::Sptr shelfModel = scene->CreateGameObject("Shelf Modelling");
			{
				shelfModel->SetPostion(glm::vec3(-9.54f, -1.87f, 0.0f));
				shelfModel->SetRotation(glm::vec3(90.0f, 0.0f, -90.0f));
				shelfModel->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				RenderComponent::Sptr renderer = shelfModel->Add<RenderComponent>();
				renderer->SetMesh(shelfMesh);
				renderer->SetMaterial(shelfMaterial);
			}
			//Shower
			Gameplay::MeshResource::Sptr showerMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("shower.obj");
			Texture2D::Sptr showerTex = ResourceManager::CreateAsset<Texture2D>("textures/shower.png");
			Gameplay::Material::Sptr showerMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
			{
				showerMaterial->Name = "Shower";
				showerMaterial->Set("u_Material.Diffuse", showerTex);
				showerMaterial->Set("u_Material.Shininess", 0.5f);
			}
			Gameplay::GameObject::Sptr shower = scene->CreateGameObject("Shower");
			{
				shower->SetPostion(glm::vec3(-4.5f, -1.84f, 0.0f));
				shower->SetRotation(glm::vec3(90.0f, 0.0f, -180.0f));
				shower->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

				RenderComponent::Sptr renderer = shower->Add<RenderComponent>();
				renderer->SetMesh(showerMesh);
				renderer->SetMaterial(showerMaterial);
			}
			//Sink
			Gameplay::MeshResource::Sptr sinkMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("Sink.obj");
			Texture2D::Sptr sinkTex = ResourceManager::CreateAsset<Texture2D>("textures/sinktex.png");
			Gameplay::Material::Sptr sinkMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
			{
				sinkMaterial->Name = "Sink";
				sinkMaterial->Set("u_Material.Diffuse", sinkTex);
				sinkMaterial->Set("u_Material.Shininess", 0.5f);
			}
			Gameplay::GameObject::Sptr sink = scene->CreateGameObject("Sink");
			{
				sink->SetPostion(glm::vec3(-3.47f, -3.14f, 0.0f));
				sink->SetRotation(glm::vec3(90.0f, 0.0f, -34.0f));
				sink->SetScale(glm::vec3(0.25f, 0.25, 0.25f));

				RenderComponent::Sptr renderer = sink->Add<RenderComponent>();
				renderer->SetMesh(sinkMesh);
				renderer->SetMaterial(sinkMaterial);
			}
			//Tub
			Gameplay::MeshResource::Sptr tubMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("tub.obj");
			Texture2D::Sptr tubTex = ResourceManager::CreateAsset<Texture2D>("textures/tub.png");
			Gameplay::Material::Sptr tubMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
			{
				tubMaterial->Name = "Tub";
				tubMaterial->Set("u_Material.Diffuse", tubTex);
				tubMaterial->Set("u_Material.Shininess", 0.5f);
			}
			Gameplay::GameObject::Sptr tub = scene->CreateGameObject("Tub");
			{
				tub->SetPostion(glm::vec3(-12.25f, -1.78f, 0.0f));
				tub->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
				tub->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

				RenderComponent::Sptr renderer = tub->Add<RenderComponent>();
				renderer->SetMesh(tubMesh);
				renderer->SetMaterial(tubMaterial);
			}
			//Tv
			Gameplay::GameObject::Sptr tvModel = scene->CreateGameObject("Tv Modelling");
			{
				tvModel->SetPostion(glm::vec3(-14.07f, -1.9f, 0.0f));
				tvModel->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
				tvModel->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

				RenderComponent::Sptr renderer = tvModel->Add<RenderComponent>();
				renderer->SetMesh(tvMesh);
				renderer->SetMaterial(tvMaterial);
			}
			//Tv Box
			Gameplay::GameObject::Sptr tvboxModel = scene->CreateGameObject("Tv Box Modelling");
			{
				tvboxModel->SetPostion(glm::vec3(-15.69f, -1.68f, 0.0f));
				tvboxModel->SetRotation(glm::vec3(90.0f, 0.0f, 37.0f));
				tvboxModel->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));

				RenderComponent::Sptr renderer = tvboxModel->Add <RenderComponent>();
				renderer->SetMesh(tvboxMesh);
				renderer->SetMaterial(tvboxMaterial);
			}
			//Wolf statue object
			Gameplay::MeshResource::Sptr statueMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("wolfstatue.obj");
			Texture2D::Sptr statueTex = ResourceManager::CreateAsset <Texture2D>("textures/statue.jpg");
			//Create Material
			Gameplay::Material::Sptr statueMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
			{
				statueMaterial->Name = "Statue";
				statueMaterial->Set("u_Material.Diffuse", statueTex);
				statueMaterial->Set("u_Material.Shininess", 1.0f);
			}
			Gameplay::GameObject::Sptr wolfstatue = scene->CreateGameObject("Wolf Statue");
			{
				wolfstatue->SetPostion(glm::vec3(1.01f, -2.63f, 0.0f));
				wolfstatue->SetRotation(glm::vec3(90.0, 0.0f, -180.f));
				wolfstatue->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				RenderComponent::Sptr renderer = wolfstatue->Add<RenderComponent>();
				renderer->SetMesh(statueMesh);
				renderer->SetMaterial(statueMaterial);
			}
		}

		/////////////////////////// UI //////////////////////////////
		//Font: Junk Dog
		Font::Sptr junkDogFont = ResourceManager::CreateAsset<Font>("fonts/JunkDog.otf", 35.f); //Font path, font size
		junkDogFont->Bake();

		Gameplay::GameObject::Sptr MenuUI = scene->CreateGameObject("Menu UI Canvas");
		{
			RectTransform::Sptr transform = MenuUI->Add<RectTransform>();

			Gameplay::GameObject::Sptr start = scene->CreateGameObject("Start");
			{
				RectTransform::Sptr transform = start->Add<RectTransform>();
				transform->SetMin({ 0, 0 });
				transform->SetMax({ 1280, 720 });

				GuiPanel::Sptr startPanel = start->Add<GuiPanel>();
				startPanel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/start_Screen.png"));
			}

			Gameplay::GameObject::Sptr pause = scene->CreateGameObject("Pause");
			{
				RectTransform::Sptr transform = pause->Add<RectTransform>();
				transform->SetMin({ 0, 0 });
				transform->SetMax({ 1280, 720 });

				GuiPanel::Sptr pausePanel = pause->Add<GuiPanel>();
				pausePanel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/pause.png"));
				//pausePanel->SetColor(glm::vec4(1.f, 1.f, 1.f, 0.f));
				pausePanel->IsEnabled = false;
			}

			Gameplay::GameObject::Sptr win = scene->CreateGameObject("Win");
			{
				RectTransform::Sptr transform = win->Add<RectTransform>();
				transform->SetMin({ 0, 0 });
				transform->SetMax({ 1280, 720 });

				GuiPanel::Sptr winPanel = win->Add<GuiPanel>();
				winPanel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/WIN.png"));
				//winPanel->SetColor(glm::vec4(1.f, 1.f, 1.f, 0.f));
				winPanel->IsEnabled = false;
			}

			Gameplay::GameObject::Sptr end = scene->CreateGameObject("Fail");
			{
				RectTransform::Sptr transform = end->Add<RectTransform>();
				transform->SetMin({ 0, 0 });
				transform->SetMax({ 1280, 720 });

				GuiPanel::Sptr losePanel = end->Add<GuiPanel>();
				losePanel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/fail.png"));
				//losePanel->SetColor(glm::vec4(1.f, 1.f, 1.f, 0.f));
				losePanel->IsEnabled = false;
			}

			MenuUI->AddChild(start);
		}


		Gameplay::GameObject::Sptr objectiveUI = scene->CreateGameObject("Objective UI Canvas"); //UI for Time and Garbage Remaining
		{
			RectTransform::Sptr transform = objectiveUI->Add<RectTransform>();
			transform->SetMin({ 10, 10 });
			transform->SetMax({ 200, 200 });
			transform->SetPosition({ 640, 50 });
			transform->SetSize({ 35,35 });

			GuiPanel::Sptr canPanel = objectiveUI->Add<GuiPanel>();
			canPanel->SetColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.f));
			canPanel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/ui/ui-clock.png"));


			Gameplay::GameObject::Sptr timeText = scene->CreateGameObject("Time Text");
			{
				RectTransform::Sptr timetransform = timeText->Add<RectTransform>();
				timetransform->SetMax({ 130, 240 });

				GuiText::Sptr text = timeText->Add<GuiText>();
				text->SetText("0:00");
				text->SetFont(junkDogFont);
				text->SetColor(glm::vec4(1.f, 1.f, 1.f, 1.f));
				text->SetTextScale(3.0f);

			}
			objectiveUI->AddChild(timeText);

			Gameplay::GameObject::Sptr trashRemain = scene->CreateGameObject("Trash Remaining");
			{
				RectTransform::Sptr remainTransform = trashRemain->Add<RectTransform>();
				remainTransform->SetMax({ 130,293 });
				//remainTransform->SetSize({ 0,0 });

				GuiText::Sptr text = trashRemain->Add<GuiText>();
				text->SetText("0 Trash Remaining!");
				text->SetFont(junkDogFont);
				text->SetColor(glm::vec4(1.f, 1.f, 1.f, 1.f));
				text->SetTextScale(1.0f);
				text->IsEnabled = false;
			}
			//trashRemain->Get<GuiText>()->IsEnabled = false;
			objectiveUI->AddChild(trashRemain);
		}

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

		GuiBatcher::SetDefaultTexture(ResourceManager::CreateAsset<Texture2D>("textures/ui/ui-sprite.png"));
		GuiBatcher::SetDefaultBorderRadius(8);

		// Save the asset manifest for all the resources we just loaded
		ResourceManager::SaveManifest("scene-manifest.json");
		// Save the scene to a JSON file
		scene->Save("scene.json");

		// Send the scene to the application
		app.LoadScene(scene);
	}
}
