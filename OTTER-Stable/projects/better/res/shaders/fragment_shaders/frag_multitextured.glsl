#version 440

#include "../fragments/fs_common_inputs.glsl"

layout(location = 7) in vec2 inTextureWeights;

// We output a single color to the color buffer
layout(location = 0) out vec4 frag_color;

////////////////////////////////////////////////////////////////
/////////////// Frame Level Uniforms ///////////////////////////
////////////////////////////////////////////////////////////////

#include "../fragments/frame_uniforms.glsl"

////////////////////////////////////////////////////////////////
/////////////// Instance Level Uniforms ////////////////////////
////////////////////////////////////////////////////////////////

// Represents a collection of attributes that would define a material
// For instance, you can think of this like material settings in 
// Unity
struct Material {
	sampler2D DiffuseA;
	sampler2D DiffuseB;
	float     Shininess;
};
// Create a uniform for the material
uniform Material u_Material;

////////////////////////////////////////////////////////////////
///////////// Application Level Uniforms ///////////////////////
////////////////////////////////////////////////////////////////

#include "../fragments/multiple_point_lights.glsl"
#include "../fragments/color_correction.glsl"

const float LOG_MAX = 2.40823996531;

// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
void main() {
	// Normalize our input normal
	vec3 normal = normalize(inNormal);

	// Will accumulate the contributions of all lights on this fragment
	// This is defined in the fragment file "multiple_point_lights.glsl"
	vec3 lightAccumulation = CalcAllLightContribution(inWorldPos, normal, u_CamPos.xyz, u_Material.Shininess);

    // By we can use this lil trick to divide our weight by the sum of all components
    // This will make all of our texture weights add up to one! 
    vec2 texWeight = inTextureWeights / dot(inTextureWeights, vec2(1,1));

	// Perform our texture mixing, we'll calculate our albedo as the sum of the texture and it's weight
	vec4 textureColor = 
        texture(u_Material.DiffuseA, inUV) * texWeight.x + 
        texture(u_Material.DiffuseB, inUV) * texWeight.y;

	// combine for the final result
	vec3 result = lightAccumulation  * inColor * textureColor.rgb;

	frag_color = vec4(ColorCorrect(result), textureColor.a);
}