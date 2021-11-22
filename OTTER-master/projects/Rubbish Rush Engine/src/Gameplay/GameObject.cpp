#include "GameObject.h"

// Utilities
#include "Utils/JsonGlmHelpers.h"

// GLM
#define GLM_ENABLE_EXPERIMENTAL
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/glm.hpp"
#include "Utils/GlmDefines.h"
#include "Utils/ImGuiHelper.h"

#include "Gameplay/Scene.h"

namespace Gameplay {
	GameObject::GameObject() :
		Name("Unknown"),
		GUID(Guid::New()),
		_components(std::vector<IComponent::Sptr>()),
		_scene(nullptr),
		_position(ZERO),
		_rotation(glm::quat(glm::vec3(0.0f))),
		_scale(ONE),
		_transform(MAT4_IDENTITY),
		_isTransformDirty(true)
	{ }

	void GameObject::LookAt(const glm::vec3& point) {
		glm::mat3 rot = glm::lookAt(_position, point, glm::vec3(0.0f, 0.0f, 1.0f));
		SetRotation(glm::quat(rot));
	}

	void GameObject::UpdateLerp(std::vector<glm::vec3> points, float deltaTime)
	{
		if (points.size() <= m_segmentIndex)
		{
			m_segmentTimer = 0;
			return;
		}
		m_segmentTimer += 1.0f * deltaTime;
		if (m_segmentIndex + 1 >= points.size())
		{
			m_segmentIndex = 0;
		}

		m_segmentTimer *= m_segmentTravelTime;

		if (points.size() < 2) {
			m_segmentTimer = 0;

			return;
		}

		_position = glm::vec3(LERP(points[m_segmentIndex], points[m_segmentIndex + 1], m_segmentTimer));
		_isTransformDirty = true;
	}

	void GameObject::UpdateScale(std::vector<glm::vec3> points, std::vector<glm::vec3> points2, float deltaTime)
	{
		if (points.size() == 0 || m_segmentTravelTime == 0)
			return;

		else
		{
			m_segmentTimer2 += 1.0f * deltaTime;

			//Ensure we are not "over time" and move to the next segment
				//if necessary.
			while (m_segmentTimer2 > m_segmentTravelTime)
			{
				m_segmentTimer2 -= m_segmentTravelTime;

				m_segmentIndex2 += 1;

				if (m_segmentIndex2 >= points.size())
					m_segmentIndex2 = 0;
			}

			float t = m_segmentTimer2 / m_segmentTravelTime;

			// LERP

			// Need at least 2 points for 1 segment
			if (points.size() < 2)
			{
				_scale = points[0];
				return;
			}

			glm::vec3 p0, p1;
			size_t p0_index, p1_index;

			p1_index = m_segmentIndex2;
			p0_index = (p1_index == 0) ? points.size() - 1 : p1_index - 1;

			p0 = points[p0_index];
			p1 = points[p1_index];

			_scale = glm::vec3(LERPLINEAR(p0, p1, t));
			_rotation = glm::quat(LERPLINEAR(glm::radians(points2[p0_index]), glm::radians(points2[p1_index]), t));

		}
		_isTransformDirty = true;
	}

	void GameObject::UpdateCAT(std::vector<glm::vec3> points, float deltaTima)
	{
		if (points.size() == 0 || m_segmentTravelTime == 0)
			return;
		else {
			m_segmentTimer += 0.05f;

			//Ensure we are not "over time" and move to the next segment
			//if necessary.
			while (m_segmentTimer > m_segmentTravelTime)
			{
				m_segmentTimer -= m_segmentTravelTime;

				m_segmentIndex += 1;

				if (m_segmentIndex >= points.size())
					m_segmentIndex = 0;
			}

			float t = m_segmentTimer / m_segmentTravelTime;

			// Neither Catmull nor Bezier make sense with less than 4 points.
			if (points.size() < 4)
			{
				_position = points[0];
				return;
			}

			size_t p0_index, p1_index, p2_index, p3_index;
			glm::vec3 p0, p1, p2, p3;

			// TODO: Complete this function
			p1_index = m_segmentIndex;

			if (p1_index == 0) {
				p0_index = points.size() - 1;
			}
			else {
				p0_index = p1_index - 1;
			}

			p2_index = (p1_index + 1) % points.size();
			p3_index = (p2_index + 1) % points.size();

			p0 = points[p0_index];
			p1 = points[p1_index];
			p2 = points[p2_index];
			p3 = points[p3_index];

			_position = CatmullRomm(p0, p1, p2, p3, t);
		}
		_isTransformDirty = true;
	}

	void GameObject::SetDirty(bool val)
	{
		_isTransformDirty = val;
	}


	void GameObject::OnEnteredTrigger(const std::shared_ptr<Physics::TriggerVolume>& trigger) {
		for (auto& component : _components) {
			component->OnEnteredTrigger(trigger);
		}
	}

	void GameObject::OnLeavingTrigger(const std::shared_ptr<Physics::TriggerVolume>& trigger) {
		for (auto& component : _components) {
			component->OnLeavingTrigger(trigger);
		}
	}

	void GameObject::OnTriggerVolumeEntered(const std::shared_ptr<Physics::RigidBody>& trigger) {
		for (auto& component : _components) {
			component->OnTriggerVolumeEntered(trigger);
		}
	}

	void GameObject::OnTriggerVolumeLeaving(const std::shared_ptr<Physics::RigidBody>& trigger) {
		for (auto& component : _components) {
			component->OnTriggerVolumeLeaving(trigger);
		}
	}

	void GameObject::SetPostion(const glm::vec3& position) {
		_position = position;
		_isTransformDirty = true;

	}
	void GameObject::SetPostionZ(float val)
	{
		_position.y = val;
		_isTransformDirty = true;
	}

	const glm::vec3& GameObject::GetPosition() const {
		return _position;
	}

	void GameObject::SetRotation(const glm::quat& value) {
		_rotation = value;
		_isTransformDirty = true;
	}

	const glm::quat& GameObject::GetRotation() const {
		return _rotation;
	}

	void GameObject::SetRotation(const glm::vec3& eulerAngles) {
		_rotation = glm::quat(glm::radians(eulerAngles));
		_isTransformDirty = true;
	}

	const glm::vec3& GameObject::GetRotationEuler() const {
		return glm::degrees(glm::eulerAngles(_rotation));
	}

	void GameObject::SetScale(const glm::vec3& value) {
		_scale = value;
		_isTransformDirty = true;
	}

	const glm::vec3& GameObject::GetScale() const {
		return _scale;
	}

	const glm::mat4& GameObject::GetTransform() const
	{
		if (_isTransformDirty) {
			_transform = glm::translate(MAT4_IDENTITY, _position) * glm::mat4_cast(_rotation) * glm::scale(MAT4_IDENTITY, _scale);
			_isTransformDirty = false;
		}
		return _transform;
	}


	Scene* GameObject::GetScene() const {
		return _scene;
	}

	void GameObject::Awake() {
		for (auto& component : _components) {
			component->Awake();
		}
	}

	void GameObject::Update(float dt) {
		for (auto& component : _components) {
			if (component->IsEnabled) {
				component->Update(dt);
			}
		}
	}

	void GameObject::DrawImGui(float indent) {
		ImGui::PushID(this); // Push a new ImGui ID scope for this object
		if (ImGui::CollapsingHeader(Name.c_str())) {
			ImGui::Indent();

			// Render position label
			_isTransformDirty |= LABEL_LEFT(ImGui::DragFloat3, "Position", &_position.x, 0.01f);
			
			// Get the ImGui storage state so we can avoid gimbal locking issues by storing euler angles in the editor
			glm::vec3 euler = GetRotationEuler();
			ImGuiStorage* guiStore = ImGui::GetStateStorage();

			// Extract the angles from the storage, note that we're only using the address of the position for unique IDs
			euler.x = guiStore->GetFloat(ImGui::GetID(&_position.x), euler.x);
			euler.y = guiStore->GetFloat(ImGui::GetID(&_position.y), euler.y);
			euler.z = guiStore->GetFloat(ImGui::GetID(&_position.z), euler.z);

			//Draw the slider for angles
			if (LABEL_LEFT(ImGui::DragFloat3, "Rotation", &euler.x, 1.0f)) {
				// Wrap to the -180.0f to 180.0f range for safety
				euler = Wrap(euler, -180.0f, 180.0f);

				// Update the editor state with our new values
				guiStore->SetFloat(ImGui::GetID(&_position.x), euler.x);
				guiStore->SetFloat(ImGui::GetID(&_position.y), euler.y);
				guiStore->SetFloat(ImGui::GetID(&_position.z), euler.z);

				//Send new rotation to the gameobject
				SetRotation(euler);
			}
			
			// Draw the scale
			_isTransformDirty |= LABEL_LEFT(ImGui::DragFloat3, "Scale   ", &_scale.x, 0.01f, 0.0f);

			ImGui::Separator();
			ImGui::TextUnformatted("Components");
			ImGui::Separator();

			// Render each component under it's own header
			for (auto& component : _components) {
				if (ImGui::CollapsingHeader(component->ComponentTypeName().c_str())) {
					ImGui::PushID(component.get()); 
					component->RenderImGui();
					ImGui::PopID();
				}
			}
			ImGui::Unindent();
		}
		ImGui::PopID(); // Pop the ImGui ID scope for the object
	}

	GameObject::Sptr GameObject::FromJson(const nlohmann::json& data, Scene* scene)
	{
		// We need to manually construct since the GameObject constructor is
		// protected. We can call it here since Scene is a friend class of GameObjects
		GameObject::Sptr result(new GameObject());
		result->_scene = scene;

		// Load in basic info
		result->Name = data["name"];
		result->GUID = Guid(data["guid"]);
		result->_position = ParseJsonVec3(data["position"]);
		result->_rotation = ParseJsonQuat(data["rotation"]);
		result->_scale    = ParseJsonVec3(data["scale"]);
		result->_isTransformDirty = true;

		// Since our components are stored based on the type name, we iterate
		// on the keys and values from the components object
		nlohmann::json components = data["components"];
		for (auto& [typeName, value] : components.items()) {
			// We need to reference the component registry to load our components
			// based on the type name (note that all component types need to be
			// registered at the start of the application)
			IComponent::Sptr component = ComponentManager::Load(typeName, value);
			component->_context = result.get();

			// Add component to object and allow it to perform self initialization
			result->_components.push_back(component);
			component->OnLoad();
		}
		return result;
	}

	nlohmann::json GameObject::ToJson() const {
		nlohmann::json result = {
			{ "name", Name },
			{ "guid", GUID.str() },
			{ "position", GlmToJson(_position) },
			{ "rotation", GlmToJson(_rotation) },
			{ "scale",    GlmToJson(_scale) },
		};
		result["components"] = nlohmann::json();
		for (auto& component : _components) {
			result["components"][component->ComponentTypeName()] = component->ToJson();
			IComponent::SaveBaseJson(component, result["components"][component->ComponentTypeName()]);
		}
		return result;
	}
}
