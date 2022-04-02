#pragma once
#include "Application/Layers/PostProcessingLayer.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Textures/Texture3D.h"

class RimLightEffect : public PostProcessingLayer::Effect {
public:
	MAKE_PTRS(RimLightEffect);

	RimLightEffect();
	virtual ~RimLightEffect();

	virtual void Apply(const Framebuffer::Sptr& gBuffer, VertexArrayObject::Sptr _quadVAO) override;
	virtual void RenderImGui() override;

	// Inherited from IResource

	RimLightEffect::Sptr FromJson(const nlohmann::json& data);
	virtual nlohmann::json ToJson() const override;

protected:
	ShaderProgram::Sptr _shader;
	glm::vec4           _rimColor;
	float strength;
	//float               _scale;
	//float               _depthThreshold;
	//float               _normalThreshold;
	//float               _depthNormalThreshold;
	//float               _depthNormalThresholdScale;
	

};


