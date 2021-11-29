#include "Scene.h"

#include <GLFW/glfw3.h>

#include "Utils/FileHelpers.h"
#include "Utils/GlmBulletConversions.h"

#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Gameplay/MeshResource.h"

#include "Graphics/DebugDraw.h"
#include "Graphics/TextureCube.h"
#include "Graphics/VertexArrayObject.h"



namespace Gameplay {
	Scene::Scene() :
		Objects(std::vector<GameObject::Sptr>()),
		//bricks(std::vector<GameObject::Sptr>()),
		Lights(std::vector<Light>()),
		IsPlaying(false),
		MainCamera(nullptr),
		DefaultMaterial(nullptr),
		_isAwake(false),
		_filePath(""),
		_gravity(glm::vec3(0.0f, 0.0f, -9.81f))
	{
		_lightingUbo = std::make_shared<UniformBuffer<LightingUboStruct>>();
		_lightingUbo->GetData().AmbientCol = glm::vec3(0.1f);
		_lightingUbo->Update();
		_lightingUbo->Bind(LIGHT_UBO_BINDING_SLOT);

		_InitPhysics();
	}

	Scene::~Scene() {
		Objects.clear();
		//bricks.clear();
		_CleanupPhysics();
	}

	void Scene::SetPhysicsDebugDrawMode(BulletDebugMode mode) {
		_bulletDebugDraw->setDebugMode((btIDebugDraw::DebugDrawModes)mode);
	}

	GameObject::Sptr Scene::CreateGameObject(const std::string& name)
	{
		GameObject::Sptr result(new GameObject());
		result->Name = name;
		result->_scene = this;
		Objects.push_back(result);
		return result;
	}

	void Scene::RemoveGameObject(const GameObject::Sptr& object) {
		_deletionQueue.push_back(object);
	}

	GameObject::Sptr Scene::FindObjectByName(const std::string name) {
		auto it = std::find_if(Objects.begin(), Objects.end(), [&](const GameObject::Sptr& obj) {
			return obj->Name == name;
		});
		return it == Objects.end() ? nullptr : *it;
	}

	GameObject::Sptr Scene::FindObjectByGUID(Guid id) {
		auto it = std::find_if(Objects.begin(), Objects.end(), [&](const GameObject::Sptr& obj) {
			return obj->GUID == id;
		});
		return it == Objects.end() ? nullptr : *it;
	}

	void Scene::SetAmbientLight(const glm::vec3& value) {
		_lightingUbo->GetData().AmbientCol = glm::vec3(0.1f);
		_lightingUbo->Update();
	}

	const glm::vec3& Scene::GetAmbientLight() const { 
		return _lightingUbo->GetData().AmbientCol;
	}

	void Scene::Awake() {
		// Not a huge fan of this, but we need to get window size to notify our camera
		// of the current screen size
		int width, height;
		glfwGetWindowSize(Window, &width, &height);
		MainCamera->ResizeWindow(width, height);

		// Call awake on all gameobjects
		for (auto& obj : Objects) {
			obj->Awake();
		}
		// Set up our lighting 
		SetupShaderAndLights();

		_isAwake = true;
	}

	void Scene::DoPhysics(float dt) {
		if (IsPlaying) {
			ComponentManager::Each<Gameplay::Physics::RigidBody>([=](const std::shared_ptr<Gameplay::Physics::RigidBody>& body) {
				body->PhysicsPreStep(dt);
			});
			ComponentManager::Each<Gameplay::Physics::TriggerVolume>([=](const std::shared_ptr<Gameplay::Physics::TriggerVolume>& body) {
				body->PhysicsPreStep(dt);
			}); 

			_physicsWorld->stepSimulation(dt, 15);

			ComponentManager::Each<Gameplay::Physics::RigidBody>([=](const std::shared_ptr<Gameplay::Physics::RigidBody>& body) {
				body->PhysicsPostStep(dt);
			});
			ComponentManager::Each<Gameplay::Physics::TriggerVolume>([=](const std::shared_ptr<Gameplay::Physics::TriggerVolume>& body) {
				body->PhysicsPostStep(dt);
			});
			if (_bulletDebugDraw->getDebugMode() != btIDebugDraw::DBG_NoDebug) {
				_physicsWorld->debugDrawWorld();
				DebugDrawer::Get().FlushAll();
			}
		}
	}

	void Scene::Update(float dt) {
		if (IsPlaying) {
			for (auto& obj : Objects) {
				obj->Update(dt);
			}
			//iterate over deletion queue
			for (int i = 0; i < _deletionQueue.size(); i++)
			{
				//delete from objects
				//remove(Objects.begin(), Objects.end(), _deletionQueue[i]);
				Objects.erase(remove(Objects.begin(), Objects.end(), _deletionQueue[i]), Objects.end());
				this->brick_count -= 1;
			}
			//empty queue each frame
			_deletionQueue.clear();
		}
	}

	void Scene::DeleteGameObject(const std::shared_ptr<GameObject>& object)
	{
		_deletionQueue.push_back(object);
	}

	void Scene::SetShaderLight(int index, bool update /*= true*/) {
		if (index >= 0 && index < Lights.size() && index < MAX_LIGHTS) {
			// Get a reference to the light UBO data so we can update it
			LightingUboStruct& data = _lightingUbo->GetData();
			Light& light = Lights[index];

			// Copy to the ubo data
			data.Lights[index].Position = light.Position;
			data.Lights[index].Color = light.Color;
			data.Lights[index].Attenuation = 1.0f / (1.0f + light.Range);

			// If requested, send the new data to the UBO
			if (update)	_lightingUbo->Update();
		}
	}

	void Scene::SetupShaderAndLights() {
		// Get a reference to the light UBO data so we can update it
		LightingUboStruct& data = _lightingUbo->GetData();
		// Send in how many active lights we have and the global lighting settings
		data.AmbientCol = glm::vec3(0.1f);
		data.NumLights = Lights.size();

		// Iterate over all lights that are enabled and configure them
		for (int ix = 0; ix < Lights.size(); ix++) {
			SetShaderLight(ix, false);
		}

		// Send updated data to OpenGL
		_lightingUbo->Update();
	}

	btDynamicsWorld* Scene::GetPhysicsWorld() const {
		return _physicsWorld;
	}

	Scene::Sptr Scene::FromJson(const nlohmann::json& data)
	{
		Scene::Sptr result = std::make_shared<Scene>();
		result->DefaultMaterial = ResourceManager::Get<Material>(Guid(data["default_material"]));

		// Make sure the scene has objects, then load them all in!
		LOG_ASSERT(data["objects"].is_array(), "Objects not present in scene!");
		for (auto& object : data["objects"]) {
			result->Objects.push_back(GameObject::FromJson(object, result.get()));
		}
		//// Make sure the scene has bricks, then load them all in!
		//LOG_ASSERT(data["Bricks"].is_array(), "bricks not present in scene!");
		//for (auto& object : data["Bricks"]) {
		//	result->bricks.push_back(GameObject::FromJson(object, result.get()));
		//}

		// Make sure the scene has lights, then load all
		LOG_ASSERT(data["lights"].is_array(), "Lights not present in scene!");
		for (auto& light : data["lights"]) {
			result->Lights.push_back(Light::FromJson(light));
		}

		// Create and load camera config
		result->MainCamera = ComponentManager::GetComponentByGUID<Camera>(Guid(data["main_camera"]));
	
		return result;
	}

	nlohmann::json Scene::ToJson() const
	{
		nlohmann::json blob;
		// Save the default shader (really need a material class)
		blob["default_material"] = DefaultMaterial ? DefaultMaterial->GetGUID().str() : "null";

		// Save renderables
		std::vector<nlohmann::json> objects;
		objects.resize(Objects.size());
		for (int ix = 0; ix < Objects.size(); ix++) {
			objects[ix] = Objects[ix]->ToJson();
		}
		blob["objects"] = objects;

		// Save renderables 2
		/*std::vector<nlohmann::json> Bricks;
		objects.resize(bricks.size());
		for (int ix = 0; ix < bricks.size(); ix++) {
			Bricks[ix] = bricks[ix]->ToJson();
		}
		blob["Bricks"] = Bricks;*/

		// Save lights
		std::vector<nlohmann::json> lights;
		lights.resize(Lights.size());
		for (int ix = 0; ix < Lights.size(); ix++) {
			lights[ix] = Lights[ix].ToJson();
		}
		blob["lights"] = lights;

		// Save camera info
		blob["main_camera"] = MainCamera != nullptr ? MainCamera->GetGUID().str() : "null";

		return blob;
	}

	void Scene::Save(const std::string& path) {
		_filePath = path;
		// Save data to file
		FileHelpers::WriteContentsToFile(path, ToJson().dump(1, '\t'));
		LOG_INFO("Saved scene to \"{}\"", path);
	}

	Scene::Sptr Scene::Load(const std::string& path)
	{
		LOG_INFO("Loading scene from \"{}\"", path);
		std::string content = FileHelpers::ReadFile(path);
		nlohmann::json blob = nlohmann::json::parse(content);
		Scene::Sptr result = FromJson(blob);
		result->_filePath = path;
		return result;
	}

	int Scene::NumObjects() const {
		return Objects.size();
	}

	GameObject::Sptr Scene::GetObjectByIndex(int index) const {
		return Objects[index];
	}

	/*std::vector<GameObject::Sptr> Scene::getBricks()
	{
		return bricks;
	}*/


	//void Scene::addBricks(GameObject::Sptr b)
	//{
	//	bricks.push_back(b);
	//}

	void Scene::_InitPhysics() {
		_collisionConfig = new btDefaultCollisionConfiguration();
		_collisionDispatcher = new btCollisionDispatcher(_collisionConfig);
		_broadphaseInterface = new btDbvtBroadphase();
		_ghostCallback = new btGhostPairCallback();
		_broadphaseInterface->getOverlappingPairCache()->setInternalGhostPairCallback(_ghostCallback);
		_constraintSolver = new btSequentialImpulseConstraintSolver();
		_physicsWorld = new btDiscreteDynamicsWorld(
			_collisionDispatcher,
			_broadphaseInterface,
			_constraintSolver,
			_collisionConfig
		);
		_physicsWorld->setGravity(ToBt(_gravity));
		// TODO bullet debug drawing
		_bulletDebugDraw = new BulletDebugDraw();
		_physicsWorld->setDebugDrawer(_bulletDebugDraw);
		_bulletDebugDraw->setDebugMode(btIDebugDraw::DBG_NoDebug);
	}

	void Scene::_CleanupPhysics() {
		delete _physicsWorld;
		delete _constraintSolver;
		delete _broadphaseInterface;
		delete _ghostCallback;
		delete _collisionDispatcher;
		delete _collisionConfig;
	}

	void Scene::DrawAllGameObjectGUIs()
	{
		for (auto& object : Objects) {
			object->DrawImGui();
		}

		static char buffer[256];
		ImGui::InputText("", buffer, 256);
		ImGui::SameLine();
		if (ImGui::Button("Add Object")) {
			CreateGameObject(buffer);
			memset(buffer, 0, 256);
		}
	}

	/*void Scene::_FlushDeleteQueue() {
		for (std::weakPtr : _deletionQueue) {
			if (weakPtr.expired()) continue;
			auto& it = std::find(Objects.begin(), Objects.end(), weakPtr.lock());
			if (it != Objects.end()) {
				Objects.erase(it);
			}
		}
		_deletionQueue.clear();
	}*/
}