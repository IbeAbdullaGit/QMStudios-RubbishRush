#pragma once
#include "../ApplicationLayer.h"
#include "Graphics/UniformBuffer.h"

class RenderLayer final : public ApplicationLayer {
public:
	MAKE_PTRS(RenderLayer);

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

	// Structure for our instance-level uniforms, matches layout from
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

	RenderLayer();
	virtual ~RenderLayer();

	bool IsBlitEnabled() const;
	void SetBlitEnabled(bool value);

	const glm::vec4& GetClearColor() const;
	void SetClearColor(const glm::vec4& value);

	// Inherited from ApplicationLayer

	virtual void OnAppLoad(const nlohmann::json& config) override;
	virtual void OnRender() override;
	virtual void OnWindowResize(const glm::ivec2& oldSize, const glm::ivec2& newSize) override;

protected:
	bool              _blitFbo;
	glm::vec4         _clearColor;

	const int FRAME_UBO_BINDING = 0;
	UniformBuffer<FrameLevelUniforms>::Sptr _frameUniforms;

	const int INSTANCE_UBO_BINDING = 1;
	UniformBuffer<InstanceLevelUniforms>::Sptr _instanceUniforms;
};