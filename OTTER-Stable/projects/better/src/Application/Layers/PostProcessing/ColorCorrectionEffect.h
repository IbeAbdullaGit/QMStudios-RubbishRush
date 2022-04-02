#pragma once
#include "Application/Layers/PostProcessingLayer.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Textures/Texture3D.h"

class ColorCorrectionEffect : public PostProcessingLayer::Effect {
public:
	MAKE_PTRS(ColorCorrectionEffect);
	Texture3D::Sptr Lut;

	ColorCorrectionEffect();
	ColorCorrectionEffect(bool defaultLut);
	virtual ~ColorCorrectionEffect();

	virtual void Apply(const Framebuffer::Sptr& gBuffer, VertexArrayObject::Sptr _quadVAO) override;
	virtual void ChangeLut(Texture3D::Sptr new_lut) override;

	virtual void RenderImGui() override;

	// Inherited from IResource

	ColorCorrectionEffect::Sptr FromJson(const nlohmann::json& data);
	virtual nlohmann::json ToJson() const override;

protected:
	ShaderProgram::Sptr _shader;
	float _strength;
};

