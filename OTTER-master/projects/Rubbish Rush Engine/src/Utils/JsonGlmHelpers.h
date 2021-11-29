#pragma once
#include <GLM/glm.hpp>
#include "json.hpp"
#include "GLM/gtc/quaternion.hpp"

inline glm::vec2 ParseJsonVec2(const nlohmann::json& blob) {
	glm::vec2 result;
	result.x = blob["x"].get<float>();
	result.y = blob["y"].get<float>();
	return result;
}
inline glm::vec3 ParseJsonVec3(const nlohmann::json& blob) {
	glm::vec3 result;
	result.x = blob["x"].get<float>();
	result.y = blob["y"].get<float>();
	result.z = blob["z"].get<float>();
	return result;
}
inline glm::vec4 ParseJsonVec4(const nlohmann::json& blob) {
	glm::vec4 result;
	result.x = blob["x"].get<float>();
	result.y = blob["y"].get<float>();
	result.z = blob["z"].get<float>();
	result.w = blob["w"].get<float>();
	return result;
}
inline glm::quat ParseJsonQuat(const nlohmann::json& blob) {
	glm::quat result;
	result.x = blob["x"].get<float>();
	result.y = blob["y"].get<float>();
	result.z = blob["z"].get<float>();
	result.w = blob["w"].get<float>();
	return result;
}
template <int C, typename T>
inline glm::vec<C, T> ParseJsonVec(const nlohmann::json& blob) {
	glm::vec<C, T> result;
	for (int ix = 0; ix < C; ix++) {
		result[ix] = blob["" + ('x' + ix)].get<T>();
	}
	return result;
}

template <int R, int C, typename T>
inline glm::mat<R, C, T> ParseJsonMat(const nlohmann::json& blob) {
	glm::mat<R, C, T> result = glm::mat<R, C, T>(1.0f);
	if (blob.is_array() && blob.size() == R) {
		for (int ix = 0; ix < R; ix++) {
			result[ix] = ParseJsonVec<C, T>(blob[ix]);
		}
	}
	else {
		LOG_WARN("Failed to parse a matrix from JSON, value was not an array of the correct length");
	}
	return result;
}

template <typename T>
inline nlohmann::json GlmToJson(const glm::vec<2, T>& value) {
	return {
		{"x", value.x},
		{"y", value.y}
	};
}
template <typename T>
inline nlohmann::json GlmToJson(const glm::vec<3, T>& value) {
	return {
		{"x", value.x},
		{"y", value.y},
		{"z", value.z}
	};
}
template <typename T>
inline nlohmann::json GlmToJson(const glm::vec<4, T>& value) {
	return {
		{"x", value.x},
		{"y", value.y},
		{"z", value.z},
		{"w", value.w}
	};
}
inline nlohmann::json GlmToJson(const glm::quat& value) {
	return {
		{"x", value.x},
		{"y", value.y},
		{"z", value.z},
		{"w", value.w}
	};
}

template <int R, int C, typename T, enum glm::qualifier Q = glm::qualifier::packed_highp>
inline nlohmann::json GlmToJsonMat(const glm::mat<R, C, T, Q>& mat) {
	nlohmann::json result = std::vector<nlohmann::json>();
	for (int ix = 0; ix < R; ix++) {
		result[ix] = GlmToJson(mat[ix]);
	}
	return result;
}

template <typename T>
T JsonGet(const nlohmann::json& blob, const std::string& key, const T& defaultRet = T()) {
	if (blob.find(key) != blob.end()) {
		return blob[key].get<T>();
	}
	else {
		return defaultRet;
	}
}

#define JsonParseEnum(typeName, blob, key, defaultVal) (blob.contains(key) ? Parse##typeName(blob[key].get<std::string>(), defaultVal) : defaultVal)