#version 430

#include "../fragments/fs_common_inputs.glsl"

// We output a single color to the color buffer
layout(location = 0) out vec4 frag_color;

// Represents a collection of attributes that would define a material
// For instance, you can think of this like material settings in 
// Unity
struct Material {
	sampler2D Diffuse;
	float     Shininess;
    int       Steps;
};
// Create a uniform for the material
uniform Material u_Material;

uniform sampler1D s_ToonTerm;

#include "../fragments/multiple_point_lights.glsl"
#include "../fragments/frame_uniforms.glsl"

// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
void main() {
	// Normalize our input normal
	vec3 normal = normalize(inNormal);

	// Use the lighting calculation that we included from our partial file
	vec3 lightAccumulation = CalcAllLightContribution(inWorldPos, normal, u_CamPos.xyz, u_Material.Shininess);

	// Get the albedo from the diffuse / albedo map
	vec4 textureColor = texture(u_Material.Diffuse, inUV);

	// combine for the final result
	vec3 result = lightAccumulation  * inColor * textureColor.rgb;

    // Using a LUT to allow artists to tweak toon shading settings
    result.r = texture(s_ToonTerm, result.r).r;
    result.g = texture(s_ToonTerm, result.g).g;
    result.b = texture(s_ToonTerm, result.b).b;

	frag_color = vec4(result, textureColor.a);
}