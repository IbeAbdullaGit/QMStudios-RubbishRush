#pragma once
#include "../ApplicationLayer.h"
#include "Graphics/Framebuffer.h"
#include "Graphics/Buffers/UniformBuffer.h"

ENUM_FLAGS(RenderFlags, uint32_t,
	None = 0,
	EnableColorCorrection = 1 << 0
);

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
		// The time in seconds since the previous frame
		float u_DeltaTime;
		// Bitfield representing up to 32 bool values to enable/disable stuff
		RenderFlags u_RenderFlags;
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

	/// <summary>
	/// Gets the primary framebuffer that is being rendered to
	/// </summary>
	const Framebuffer::Sptr& GetPrimaryFBO() const;

	bool IsBlitEnabled() const;
	void SetBlitEnabled(bool value);

	const glm::vec4& GetClearColor() const;
	void SetClearColor(const glm::vec4& value);

	void SetRenderFlags(RenderFlags value);
	RenderFlags GetRenderFlags() const;

	// Inherited from ApplicationLayer

	virtual void OnAppLoad(const nlohmann::json& config) override;
	virtual void OnRender(const Framebuffer::Sptr& prevLayer) override;
	virtual void OnWindowResize(const glm::ivec2& oldSize, const glm::ivec2& newSize) override;
	virtual Framebuffer::Sptr GetRenderOutput() override;

protected:
	Framebuffer::Sptr _primaryFBO;
	bool              _blitFbo;
	glm::vec4         _clearColor;
	RenderFlags       _renderFlags;

	const int FRAME_UBO_BINDING = 0;
	UniformBuffer<FrameLevelUniforms>::Sptr _frameUniforms;

	const int INSTANCE_UBO_BINDING = 1;
	UniformBuffer<InstanceLevelUniforms>::Sptr _instanceUniforms;
};