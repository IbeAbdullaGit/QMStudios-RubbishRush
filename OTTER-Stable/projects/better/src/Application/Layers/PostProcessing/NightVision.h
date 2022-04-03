#pragma once
#include "Application/Layers/PostProcessingLayer.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Textures/Texture3D.h"
#include "Graphics/Framebuffer.h"

class NightVision : public PostProcessingLayer::Effect
{
public:
	MAKE_PTRS(NightVision);

	Texture2D::Sptr _noise;
	Texture2D::Sptr _mask;

	NightVision();
	NightVision(bool activate);
	virtual ~NightVision();

	virtual void Apply(const Framebuffer::Sptr& gBuffer, VertexArrayObject::Sptr _quadVAO) override;
	virtual void RenderImGui() override;

	// Inherited from IResource

	NightVision::Sptr FromJson(const nlohmann::json& data);
	virtual nlohmann::json ToJson() const override;

protected:
	ShaderProgram::Sptr _shader;

	float timer = 1.0f;
	float light = 0.2f;
	float color = 4.0f;
};