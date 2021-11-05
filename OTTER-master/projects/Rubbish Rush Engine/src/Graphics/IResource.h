#pragma once
#include "Utils/GUID.hpp"
#include "json.hpp"

/// <summary>
/// Base class for graphics that the resource manager may want to manage
/// (ex: textures, models, shaders, materials, etc...)
/// </summary>
class IResource {
public:
	virtual ~IResource() = default;

	/// <summary>
	/// Gets the unique ID of this resource
	/// </summary>
	Guid GetGUID() const { return _guid; }
	/// <summary>
	/// Overrides the GUID of this resource with a new value
	/// Only use this if you know what you're doing!
	/// </summary>
	/// <param name="newValue">The new GUID for the object</param>
	void OverrideGUID(Guid newValue) { _guid = newValue; }

	/// <summary>
	/// Populates this resource from a JSON blob of data
	/// </summary>
	/// <param name="data">The data to initialize this resource with</param>
	virtual void LoadFromJson(const nlohmann::json& data) {};
	/// <summary>
	/// Converts this resource into it's JSON manifest format
	/// Should contain all the data required to reconstruct the
	/// resource via the LoadFromJson command
	/// </summary>
	/// <returns>The JSON blob for the resource</returns>
	virtual nlohmann::json ToJson() { return {}; };

protected:
	Guid _guid;
	IResource() : _guid(Guid::New()){}
};