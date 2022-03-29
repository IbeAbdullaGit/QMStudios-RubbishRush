#pragma once
#include "Application/Layers/PostProcessingLayer.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Textures/Texture3D.h"

class BloomEffect : public PostProcessingLayer::Effect {
public:
	MAKE_PTRS(BloomEffect);
	
	BloomEffect();
	BloomEffect(bool defaultLut);
	virtual ~BloomEffect();

	virtual void Apply(const Framebuffer::Sptr& gBuffer) override;
	virtual void RenderImGui() override;

	// Inherited from IResource

	BloomEffect::Sptr FromJson(const nlohmann::json& data);
	virtual nlohmann::json ToJson() const override;

protected:
	ShaderProgram::Sptr _shader;
	ShaderProgram::Sptr _shader2;
	float _strength;
	float threshold;
};

