#pragma once
#include "json.hpp"
#include "Utils/JsonGlmHelpers.h"

namespace Gameplay {
	/// <summary>
	/// Represents information for a single light within the scene
	/// </summary>
	struct Light {
		/// <summary>
		/// The position of the light in the world
		/// </summary>
		glm::vec3 Position = glm::vec3(0.0f);
		/// <summary>
		/// The color of the light in RGB
		/// </summary>
		glm::vec3 Color = glm::vec3(1.0f);
		/// <summary>
		/// The approximate range of our light in world units (meters)
		/// </summary>
		float Range = 4.0f;

		/// <summary>
		/// Loads a light from a JSON blob
		/// </summary>
		inline static Light FromJson(const nlohmann::json& data) {
			Light result;
			result.Position = data["position"];
			result.Color = data["color"];
			result.Range = data["range"].get<float>();
			return result;
		}

		/// <summary>
		/// Converts this object into it's JSON representation for storage
		/// </summary>
		inline nlohmann::json ToJson() const {
			return {
				{ "position", Position },
				{ "color", Color },
				{ "range", Range },
			};
		}

	};
}
