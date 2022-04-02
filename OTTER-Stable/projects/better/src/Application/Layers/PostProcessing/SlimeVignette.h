#pragma once

#include "Application/Layers/PostProcessingLayer.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Textures/Texture3D.h"
#include "Graphics/Framebuffer.h"


class SlimeVignette : public PostProcessingLayer::Effect 
{
public:
	MAKE_PTRS(SlimeVignette);

	SlimeVignette();
	virtual ~SlimeVignette();

	virtual void Apply(const Framebuffer::Sptr& gBuffer, VertexArrayObject::Sptr _quadVAO) override;
	virtual void RenderImGui() override;

	// Inherited from IResource

	SlimeVignette::Sptr FromJson(const nlohmann::json& data);
	virtual nlohmann::json ToJson() const override;

protected:
	ShaderProgram::Sptr _shader;

	float timer = 1.0f;
};
