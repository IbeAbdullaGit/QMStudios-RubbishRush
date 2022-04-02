#include "MotionblurEffect.h"
#include "Utils/ResourceManager/ResourceManager.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include "Application/Application.h"

MotionblurEffect::MotionblurEffect() :
	MotionblurEffect(true) { }

MotionblurEffect::MotionblurEffect(bool defaultLut) :
	PostProcessingLayer::Effect(),
	motionscale(1.0f), _shader2(nullptr), fixedsampling(0), texVelocity(nullptr)
{
	Name = "Motion Blur Effect";
	_format = RenderTargetType::ColorRgb8;

	_shader2 = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
		{ ShaderPartType::Vertex, "shaders/vertex_shaders/new_fullscreen_quad.glsl" },
		{ ShaderPartType::Fragment, "shaders/fragment_shaders/post_effects/motion_blur.glsl" }
	});
	Application& app = Application::Get();

	Texture2DDescription properties;
	properties.Width = app.GetWindowSize().x;
	properties.Height = app.GetWindowSize().y;
	properties.Format = InternalFormat::RG8;
	properties.MagnificationFilter = MagFilter::Nearest;
	properties.HorizontalWrap = WrapMode::ClampToEdge;


	texVelocity = ResourceManager::CreateAsset<Texture2D>(properties);
	
}

MotionblurEffect::~MotionblurEffect() = default;

void MotionblurEffect::Apply(const Framebuffer::Sptr& gBuffer, VertexArrayObject::Sptr _quadVAO)
{
	_shader2->Bind();
	texVelocity->Bind(1);
	//_shader2->SetUniform("uMotionScale", (1.0f)); //1/targetFPS/dt
	//_shader2->SetUniform("uMaxMotionBlurSamples", 128);
	//_shader2->SetUniform("uDoMotionBlur", 1);
	//_shader2->SetUniform("uDoFixedSampling", 0);

	//gBuffer->BindAttachment(RenderTargetAttachment::Depth, 1);
	

}

void MotionblurEffect::RenderImGui()
{
	//LABEL_LEFT(ImGui::LabelText, "LUT", Lut ? Lut->GetDebugName().c_str() : "none");
	LABEL_LEFT(ImGui::SliderFloat, "Motion Scale", &motionscale, 0, 10);
	LABEL_LEFT(ImGui::SliderInt, "Fixed Sampling", &fixedsampling, 0, 1);
}

MotionblurEffect::Sptr MotionblurEffect::FromJson(const nlohmann::json& data)
{
	MotionblurEffect::Sptr result = std::make_shared<MotionblurEffect>(false);
	result->Enabled = JsonGet(data, "enabled", true);
	result->motionscale = JsonGet(data, "motion scale", result->motionscale);
	result->fixedsampling = JsonGet(data, "fixed sampling", result->fixedsampling);
	//result->Lut = ResourceManager::Get<Texture3D>(Guid(data["lut"].get<std::string>()));
	return result;
}

nlohmann::json MotionblurEffect::ToJson() const
{
	return {
		{ "enabled", Enabled },
		//{ "lut", Lut != nullptr ? Lut->GetGUID().str() : "null" }, 
		{ "motion scale", motionscale },
		{"fixed sampling", fixedsampling}
	};
}
