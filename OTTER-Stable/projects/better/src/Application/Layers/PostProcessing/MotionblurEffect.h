#pragma once
#include "Application/Layers/PostProcessingLayer.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Textures/Texture2D.h"

class MotionblurEffect : public PostProcessingLayer::Effect {
public:
	MAKE_PTRS(MotionblurEffect);
	
	MotionblurEffect();
	MotionblurEffect(bool defaultLut);
	virtual ~MotionblurEffect();

	virtual void Apply(const Framebuffer::Sptr& gBuffer, VertexArrayObject::Sptr _quadVAO) override;
	virtual void RenderImGui() override;

	// Inherited from IResource

	MotionblurEffect::Sptr FromJson(const nlohmann::json& data);
	virtual nlohmann::json ToJson() const override;

	Texture2D::Sptr texVelocity;

protected:
	ShaderProgram::Sptr _shader2;
	float motionscale;
	int fixedsampling;
};

