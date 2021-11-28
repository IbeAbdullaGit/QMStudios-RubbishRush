#pragma once
#include <string>

// Utils
#include "Utils/GUID.hpp"

// GLM
#define GLM_ENABLE_EXPERIMENTAL
#include "GLM/gtx/common.hpp"

// Others
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Components/ComponentManager.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace Gameplay {
// Predeclaration for Scene
	class Scene;

	namespace Physics {
		class TriggerVolume;
		class RigidBody;
	}

	/// <summary>
	/// Represents an object in our scene with a transformation and a collection
	/// of components. Components provide gameobject's with behaviours
	/// </summary>
	struct GameObject {
		typedef std::shared_ptr<GameObject> Sptr;

		// Human readable name for the object
		std::string             Name;
		// Unique ID for the object
		Guid                    GUID;

		/// <summary>
		/// Rotates this object to look at the given point in world coordinates
		/// </summary>
		void LookAt(const glm::vec3& point);

		// LERP function
		template<typename T>
		static T LERP(const T& p0, const T& p1, float t)
		{
			//adjust for ease-in and ease-out
			t = (sin(t - M_PI / 2) + 1) / 2;
			return (1.0f - t) * p0 + t * p1;
		}
		//2nd LERP function
		template<typename T>
		static T LERPLINEAR(const T& p0, const T& p1, float t)
		{
			return (1.0f - t) * p0 + t * p1;
		}
		//catmull rom spline function
		template<typename T>
		static T CatmullRomm(const T& p0, const T& p1, const T& p2, const T& p3, float t)
		{
			return 0.5f * (2.f * p1 + t * (-p0 + p2)
				+ t * t * (2.f * p0 - 5.f * p1 + 4.f * p2 - p3)
				+ t * t * t * (-p0 + 3.f * p1 - 3.f * p2 + p3));
		}
		void UpdateLerp(std::vector<glm::vec3> points, float deltaTime);
		void UpdateScale(std::vector<glm::vec3> points, std::vector<glm::vec3> points2, float deltaTime);
		void UpdateCAT(std::vector<glm::vec3> points, float deltaTima);

		/// <summary>
		/// Invoked when the rigidbody attached to this game object (if any) enters
		/// a trigger volume for the first time
		/// </summary>
		/// <param name="trigger">The trigger volume that was entered</param>
		void OnEnteredTrigger(const std::shared_ptr<Physics::TriggerVolume>& trigger);
		/// <summary>
		/// Invoked when the rigidbody attached to this game object (if any) leaves
		/// a trigger volume
		/// </summary>
		/// <param name="trigger">The trigger volume that was left</param>
		void OnLeavingTrigger(const std::shared_ptr<Physics::TriggerVolume>& trigger);

		/// <summary>
		/// Invoked when a rigidbody enters the trigger volume attached to this
		/// game object (if any)
		/// </summary>
		/// <param name="body">The body that has entered our trigger volume</param>
		void OnTriggerVolumeEntered(const std::shared_ptr<Physics::RigidBody>& body);
		/// <summary>
		/// Invoked when a rigidbody leaves the trigger volume attached to this
		/// game object (if any)
		/// </summary>
		/// <param name="body">The body that has left our trigger volume</param>
		void OnTriggerVolumeLeaving(const std::shared_ptr<Physics::RigidBody>& body);

		/// <summary>
		/// Sets the game object's world position
		/// </summary>
		/// <param name="position">The new position for the object in world space</param>
		void SetPostion(const glm::vec3& position);
		void SetPostionZ(float val);
		/// <summary>
		/// Gets the object's position in world space
		/// </summary>
		const glm::vec3& GetPosition() const;

		/// <summary>
		/// Sets the rotation of this object to a quaternion value
		/// </summary>
		/// <param name="value">The rotation quaternion for the object</param>
		void SetRotation(const glm::quat& value);
		/// <summary>
		/// Gets the object's rotation as a quaternion value
		/// </summary>
		const glm::quat& GetRotation() const;

		/// <summary>
		/// Sets the rotation of the object in euler degrees (yaw, pitch, roll)
		/// </summary>
		/// <param name="eulerAngles">The angles in degrees</param>
		void SetRotation(const glm::vec3& eulerAngles);
		/// <summary>
		/// Gets the euler angles from this object in degrees
		/// </summary>
		const glm::vec3& GetRotationEuler() const;

		/// <summary>
		/// Sets the scaling factor for the game object, should be non-zero
		/// </summary>
		/// <param name="value">The new scaling factor for the game object</param>
		void SetScale(const glm::vec3& value);
		/// <summary>
		/// Gets the scaling factor for the game object
		/// </summary>
		const glm::vec3& GetScale() const;

		/// <summary>
		/// Gets or recalculates and gets the object's world transform
		/// </summary>
		const glm::mat4& GetTransform() const;
		/// <summary>
		/// Gets or recalculates the inverse of this object's world transform
		/// This matrix transforms points from world space to local space
		/// </summary>
		const glm::mat4& GetInverseTransform() const;

		/// <summary>
		/// Returns a pointer to the scene that this GameObject belongs to
		/// </summary>
		Scene* GetScene() const;

		/// <summary>
		/// Notify all enabled components in this gameObject that the scene has been loaded
		/// </summary>
		void Awake();

		/// <summary>
		/// Calls update on all enabled components in this object
		/// </summary>
		/// <param name="deltaTime">The time since the last frame, in seconds</param>
		void Update(float dt);

		/// <summary>
		/// Checks whether this gameobject has a component of the given type
		/// </summary>
		/// <typeparam name="T">The type of component to search for</typeparam>
		template <typename T, typename = typename std::enable_if<std::is_base_of<IComponent, T>::value>::type>
		bool Has() {
			// Iterate over all the pointers in the components list
			for (const auto& ptr : _components) {
				// If the pointer type matches T, we return true
				if (std::type_index(typeid(*ptr.get())) == std::type_index(typeid(T))) {
					return true;
				}
			}
			return false;
		}
		bool Has(const std::type_index& type);

		/// <summary>
		/// Gets the component of the given type from this gameobject, or nullptr if it does not exist
		/// </summary>
		/// <typeparam name="T">The type of component to search for</typeparam>
		template <typename T, typename = typename std::enable_if<std::is_base_of<IComponent, T>::value>::type>
		std::shared_ptr<T> Get() {
			// Iterate over all the pointers in the binding list
			for (const auto& ptr : _components) {
				// If the pointer type matches T, we return that behaviour, making sure to cast it back to the requested type
				if (std::type_index(typeid(*ptr.get())) == std::type_index(typeid(T))) {
					return std::dynamic_pointer_cast<T>(ptr);
				}
			}
			return nullptr;
		}
		std::shared_ptr<IComponent> Get(const std::type_index& type);

		/// <summary>
		/// Adds a component of the given type to this gameobject. Note that only one component
		/// of a given type may be attached to a gameobject
		/// </summary>
		/// <typeparam name="T">The type of component to add</typeparam>
		/// <typeparam name="TArgs">The arguments to forward to the component constructor</typeparam>
		template <typename T, typename ... TArgs>
		std::shared_ptr<T> Add(TArgs&&... args) {
			static_assert(is_valid_component<T>(), "Type is not a valid component type!");
			LOG_ASSERT(!Has<T>(), "Cannot add 2 instances of a component type to a game object");

			// Make a new component, forwarding the arguments
			std::shared_ptr<T> component = ComponentManager::Create<T>(std::forward<TArgs>(args)...);
			// Let the component know we are the parent
			component->_context = this;

			// Append it to the binding component's storage, and invoke the OnLoad
			_components.push_back(component);
			component->OnLoad();

			if (_scene->GetIsAwake()) {
				component->Awake();
			}

			return component;
		}
		std::shared_ptr<IComponent> Add(const std::type_index& type);

		/// <summary>
		/// Draws the ImGui window for this game object and all nested components
		/// </summary>
		void DrawImGui(float indent = 0.0f);

		std::shared_ptr<GameObject> SelfRef();

		/// <summary>
		/// Loads a render object from a JSON blob
		/// </summary>
		static GameObject::Sptr FromJson(const nlohmann::json& data, Scene* scene);
		/// <summary>
		/// Converts this object into it's JSON representation for storage
		/// </summary>
		nlohmann::json ToJson() const;

	private:
		friend class Scene;

		//lerp variables
		float m_segmentIndex = 0;
		float m_segmentTimer = 0.0f;
		float m_segmentTravelTime = 1.0f;

		float m_segmentIndex2 = 0;
		float m_segmentTimer2 = 0.0f;


		// Rotation of the object as a quaternion
		glm::quat _rotation;
		// Position of the object
		glm::vec3 _position;
		// The scale of the object
		glm::vec3 _scale;

		// The object's world transform
		mutable glm::mat4 _transform;
		mutable glm::mat4 _inverseTransform;
		mutable bool _isTransformDirty;

		// The components that this game object has attached to it
		std::vector<IComponent::Sptr> _components;
		std::weak_ptr<GameObject> _selfRef;
		// Pointer to the scene, we use raw pointers since 
		// this will always be set by the scene on creation
		// or load, we don't need to worry about ref counting
		Scene* _scene;

		/// <summary>
		/// Only scenes will be allowed to create gameobjects
		/// </summary>
		GameObject();

		// Recalculates the transform matrix for the object when required
		void _RecalcTransform() const;
	};
}