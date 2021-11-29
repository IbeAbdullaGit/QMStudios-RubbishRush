#pragma once
#include <btBulletDynamicsCommon.h>
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

#include "Gameplay/Components/Camera.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Light.h"

#include "Physics/BulletDebugDraw.h"

#include "Graphics/UniformBuffer.h"
struct GLFWwindow;

class TextureCube;
class Shader;

const int LIGHT_UBO_BINDING_SLOT = 0;

namespace Gameplay {
	namespace Physics {
		class RigidBody;
	}

	class MeshResource;
	class Material;

	/// <summary>
	/// Main class for our game structure
	/// Stores game objects, lights, the camera,
	/// and other top level state for our game
	/// </summary>
	class Scene {
	public:
		typedef std::shared_ptr<Scene> Sptr;

		static const int MAX_LIGHTS = 8;
		static const int LIGHT_UBO_BINDING = 2;

		// Stores all the lights in our scene
		std::vector<Light>         Lights;
		// The camera for our scene
		Camera::Sptr               MainCamera;

		// Instead of a "base shader", we can specify a default material
		std::shared_ptr<Material>  DefaultMaterial;

		GLFWwindow*                Window; // another place that can use improvement

		// Whether the application is in "play mode", lets us leverage editors!
		bool                       IsPlaying;


		Scene();
		~Scene();

		void SetPhysicsDebugDrawMode(BulletDebugMode mode);
		std::shared_ptr<TextureCube> GetSkyboxTexture() const;

		/**
		 * Gets whether the scene has already called Awake()
		 */
		bool GetIsAwake() const { return _isAwake; }

		/// <summary>
		/// Creates a game object with the given name
		/// CreateGameObject is the only way to create game objects
		/// </summary>
		/// <param name="name">The name of the gameobject to create</param>
		/// <returns>A new gameobject with the given name</returns>
		GameObject::Sptr CreateGameObject(const std::string& name);

		/// <summary>
		/// Queues a game object for deletion at the call of the next Update function
		/// </summary>
		/// <param name="object">The gameobject to delete</param>
		void RemoveGameObject(const GameObject::Sptr& object);

		/// <summary>
		/// Searches all objects in the scene and returns the first
		/// one who's name matches the one given, or nullptr if no object
		/// is found
		/// </summary>
		/// <param name="name">The name of the object to find</param>
		GameObject::Sptr FindObjectByName(const std::string name);
		/// <summary>
		/// Searches all render objects in the scene and returns the first
		/// one who's guid matches the one given, or nullptr if no object
		/// is found
		/// </summary>
		/// <param name="id">The guid of the object to find</param>
		GameObject::Sptr FindObjectByGUID(Guid id);

		/// <summary>
		/// Sets the ambient light color for this scene
		/// </summary>
		/// <param name="value">The new value for the ambient light, should be in the 0-1 range</param>
		void SetAmbientLight(const glm::vec3& value);
		/// <summary>
		/// Gets the current ambient lighting factor for this scene
		/// </summary>
		const glm::vec3& GetAmbientLight() const;

		/// <summary>
		/// Gets the file path that this scene was saved to or loaded from
		/// </summary>
		const std::string& GetFilePath() const { return _filePath; }

		/// <summary>
		/// Calls awake on all objects in the scene,
		/// call this after loading or creating a new scene
		/// </summary>
		void Awake();

		/// <summary>
		/// Performs physics updates for all physics bodies in this scene,
		/// should be called after Update in the main loop
		/// 
		/// Only invokes events if IsPlaying is true
		/// </summary>
		/// <param name="dt">The time in seconds since the last frame</param>
		void DoPhysics(float dt);

		/// <summary>
		/// Performs updates on all enabled components and gameobjects in the
		/// scene
		/// 
		/// Only invokes events if IsPlaying is true
		/// </summary>
		/// <param name="dt">The time in seconds since the last frame</param>
		void Update(float dt);

		/// <summary>
		/// Performs setup before rendering
		/// </summary>
		void PreRender();

		//delete game object
		void DeleteGameObject(const std::shared_ptr<GameObject>& object);

		/// <summary>
		/// Handles setting the shader uniforms for our light structure in our array of lights
		/// </summary>
		/// <param name="shader">The pointer to the shader</param>
		/// <param name="uniformName">The name of the uniform (ex: u_Lights)</param>
		/// <param name="index">The index of the light to set</param>
		/// <param name="light">The light data to copy over</param>
		void SetShaderLight(int index, bool update = true);
		/// <summary>
		/// Creates the shader and sets up all the lights
		/// </summary>
		void SetupShaderAndLights();

		void DrawSkybox();

		/// <summary>
		/// Draws all GUI objects in the scene
		/// </summary>
		void RenderGUI();

		/// <summary>
		/// Draws ImGui stuff for all gameobjects in the scene
		/// </summary>
		void DrawAllGameObjectGUIs();

		/// <summary>
		/// Gets the scene's Bullet physics world
		/// </summary>
		btDynamicsWorld* GetPhysicsWorld() const;

		/// <summary>
		/// Loads a scene from a JSON blob
		/// </summary>
		static Scene::Sptr FromJson(const nlohmann::json& data);
		/// <summary>
		/// Converts this object into it's JSON representation for storage
		/// </summary>
		nlohmann::json ToJson() const;

		/// <summary>
		/// Saves this scene to an output JSON file
		/// </summary>
		/// <param name="path">The path of the file to write to</param>
		void Save(const std::string& path);
		/// <summary>
		/// Loads a scene from an input JSON file
		/// </summary>
		/// <param name="path">The path of the file to read from</param>
		/// <returns>A new scene loaded from the file</returns>
		static Scene::Sptr Load(const std::string& path);


		int NumObjects() const;
		GameObject::Sptr GetObjectByIndex(int index) const;

		int brick_count;
		int trash;
		bool need_update = false;
		//std::vector<GameObject::Sptr> getBricks();
		//void addBricks(GameObject::Sptr b);

		

	protected:
		// Bullet physics stuff world
		btDynamicsWorld*          _physicsWorld;
		// Our bullet physics configuration
		btCollisionConfiguration* _collisionConfig; 
		// Handles dispatching collisions between objects
		btCollisionDispatcher*    _collisionDispatcher;
		// Provides rough broadphase (AABB) checks to improve performance
		btBroadphaseInterface*    _broadphaseInterface;
		// Resolves contraints (ex: hinge constraints, angle axis, etc...)
		btConstraintSolver*       _constraintSolver;
		// this is what allows us to get our pairs from the trigger volumes
		btGhostPairCallback*      _ghostCallback;

		BulletDebugDraw* _bulletDebugDraw;

		// The path that we've saved or loaded this scene from
		std::string             _filePath;

		// Our physics scene's global gravity, default matches earth's gravity (m/s^2)
		glm::vec3 _gravity;

		// Stores all the objects in our scene
		std::vector<GameObject::Sptr>  Objects;
		//std::vector<GameObject::Sptr> bricks;
		
		// Info for rendering our skybox will be stored in the scene itself
		std::shared_ptr<Shader>       _skyboxShader;
		std::shared_ptr<MeshResource> _skyboxMesh;
		std::shared_ptr<TextureCube>  _skyboxTexture;
		glm::mat3                     _skyboxRotation;

		
		std::vector<GameObject::Sptr> _deletionQueue;

		/// <summary>
		/// Represents a c++ struct layout that matches that of
		/// our multiple light uniform buffer
		/// 
		/// Note that we have to do some weirdness since OpenGl has a
		/// thing for packing structures to sizeof(vec4)
		/// </summary>
		struct LightingUboStruct {
			struct Light {
				// This lets us continue to access Position as a vec3, but also allocates space for the
				// pack at the end (since objects are vec4 aligned)
				union {
					glm::vec3 Position;
					glm::vec4 Position4;
				};
				// Since these are tightly packed, will match the vec4 in light
				glm::vec3 Color;
				float     Attenuation;
			};

			// Since these are tightly packed, will match the vec4 in the UBO
			glm::vec3 AmbientCol;
			float     NumLights;

			Light     Lights[MAX_LIGHTS];
			// NOTE: our shaders expect a mat3, but due to the STD140 layout, each column of the
			// vec3 needs to be padded to the size of a vec4, hence the use of a mat4 here
			glm::mat4 EnvironmentRotation;
		};
		UniformBuffer<LightingUboStruct>::Sptr _lightingUbo;
		bool                       _isAwake;

	

		/// <summary>
		/// Handles configuring our bullet physics stuff
		/// </summary>
		void _InitPhysics();
		/// <summary>
		/// Handles cleaning up bullet physics for this scene
		/// </summary>
		void _CleanupPhysics();

		void _FlushDeleteQueue();
	};
}