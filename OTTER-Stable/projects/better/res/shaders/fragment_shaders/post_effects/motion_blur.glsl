#version 430

layout(location = 0) in vec2 inUV;

layout(location = 0) out vec3 outColor;

uniform layout(binding = 0) sampler2D s_Image;
//uniform layout(binding = 1) sampler2D s_Depth;
uniform layout(binding=1) sampler2D uVelocityTex; //velocity buffer

noperspective in vec2 vTexcoord;

const int MAX_MOTION_SAMPLES = 128;
float uMotionScale = 1.0; // current fps / target fps
int uMaxMotionBlurSamples = MAX_MOTION_SAMPLES;
int uDoMotionBlur = 1;
int uDoFixedSampling = 0;


#define VELOCITY_GLSL_
#define VELOCITY_BUFFER_UINT
#ifdef VELOCITY_BUFFER_UINT
	const float kVelocityPower = 3.0;
#endif
vec2 powv2(in vec2 b, in float e) {
	return vec2(pow(b.x, e), pow(b.y, e));
}
vec2 decodeVelocity(in vec2 velocity) {
#ifdef VELOCITY_BUFFER_UINT
	vec2 result = powv2(velocity, 1.0 / kVelocityPower);
	result = result * 2.0 - 1.0;
	return result;
#else
	return velocity;
#endif
}
void main() {
    //vec3 color = texture(s_Image, inUV).rgb;
    //outColor = mix(color, texture(s_Lut, color).rgb, clamp(u_Strength, 0, 1));

    vec2 velocity = decodeVelocity(texture(uVelocityTex, vTexcoord).rg);
	//debugging
	velocity = vec2(1.0, 1.0);

	velocity *= uMotionScale;
	
//	compute number of blur samples to take:
	vec2 texelSize = 1.0 / vec2(textureSize(s_Image, 0));
	float speed = length(velocity / texelSize);
   int nSamples = clamp(int(speed), 1, uMaxMotionBlurSamples);
	vec4 result = vec4(0.0);
	
	if (bool(uDoFixedSampling)) {
	/*	This approach samples at 1-texel intervals along the velocity direction. 
		This limits the maximum achievable blur to uMaxMotionBlurSamples, which
		under-blurs when speed > uMaxMotionBlurSamples. */
		velocity = normalize(velocity) * texelSize;
		float hlim = float(-nSamples) * 0.5 + 0.5;
		for (int i = 0; i < nSamples; ++i) {
			vec2 offset = velocity * (hlim + float(i));
			result += texture(s_Image, vTexcoord + offset);
		}
	} else {
	/*	This approach samples at even intervals along the velocity direction. This
		ensures the proper amount of blur, but causes a degredation in blur 
		quality (banding aretfacts) when when speed > uMaxMotionBlurSamples. */
		result = texture(s_Image, vTexcoord);
		for (int i = 1; i < nSamples; ++i) {
			vec2 offset = velocity * (float(i) / float(nSamples - 1) - 0.5);
			result += texture(s_Image, vTexcoord + offset);
		}
	}
	
	result /= float(nSamples);
	//return
	outColor = result.rgb;
}
