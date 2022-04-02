#pragma once
#include "Application/Layers/PostProcessingLayer.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Textures/Texture3D.h"
#include "Graphics/Textures/Texture1D.h"

class ToonEffect : public PostProcessingLayer::Effect {
public:
	MAKE_PTRS(ToonEffect);
	Texture1D::Sptr toonterm;

	ToonEffect();
	ToonEffect(bool defaultLut);
	virtual ~ToonEffect();

	virtual void Apply(const Framebuffer::Sptr& gBuffer, VertexArrayObject::Sptr _quadVAO) override;
	virtual void RenderImGui() override;

	// Inherited from IResource

	ToonEffect::Sptr FromJson(const nlohmann::json& data);
	virtual nlohmann::json ToJson() const override;

protected:
	ShaderProgram::Sptr _shader;
	int _strength;
	
};

