#version 430

#include "../fragments/fs_common_inputs.glsl"

// We output a single color to the color buffer
layout(location = 0) out vec4 albedo_specPower;
layout(location = 1) out vec4 normal_metallic;
layout(location = 2) out vec4 emissive;
layout(location = 3) out vec3 view_pos;

// Represents a collection of attributes that would define a material
// For instance, you can think of this like material settings in 
// Unity
struct Material {
	sampler2D AlbedoMap;
	sampler2D EmissiveMap;
	sampler2D NormalMap;
	sampler2D MetallicShininessMap;
    sampler2D s_Depth;
	float     DiscardThreshold;
};
// Create a uniform for the material
uniform Material u_Material;
const float offset = 1.0 / 128.0;

uniform vec4  u_OutlineColor;
uniform float u_Scale;
uniform float u_DepthThreshold;
uniform float u_NormalThreshold;
uniform float u_DepthNormThreshold;
uniform float u_DepthNormThresholdScale;
uniform vec2  u_PixelSize;


#include "../fragments/frame_uniforms.glsl"
//#include "../fragments/color_correction.glsl"

// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
void main() {
	
	
	// Get albedo from the material
	vec4 albedoColor = texture(u_Material.AlbedoMap, inUV);

	//apply color correction?
	//albedoColor =vec4(ColorCorrect(albedoColor.rgb), albedoColor.a);

	// We can use another texture to store things like our lighting settings
	vec4 lightingParams = texture(u_Material.MetallicShininessMap, inUV);
	

	// Discarding fragments who's alpha is below the material's threshold
	if (albedoColor.a < u_Material.DiscardThreshold) {
		discard;
	}

	
	// Normalize our input normal
    // Read our tangent from the map, and convert from the [0,1] range to [-1,1] range
    vec3 normal = texture(u_Material.NormalMap, inUV).rgb;
    normal = normal * 2.0 - 1.0;

    // Here we apply the TBN matrix to transform the normal from tangent space to view space
    normal = normalize(inTBN * normal);
	
	// Map [-1, 1] to [0, 1]
	normal = clamp((normal + 1) / 2.0, 0, 1);
	normal_metallic = vec4(normal, lightingParams.y);

	// Extract emissive from the material
	emissive = texture(u_Material.EmissiveMap, inUV);
	
	view_pos = inViewPos;
	//outline effect calculation
	float depth = texture(u_Material.s_Depth, inUV).r;
    vec3 norm = normal;

    float halfScale = u_Scale * 0.5f;

    // We calculate an x shape around our UV that we'll sample the corners of
    vec2 u0 = inUV + vec2(-u_PixelSize.x, -u_PixelSize.y) * floor(halfScale);
    vec2 u1 = inUV + vec2( u_PixelSize.x,  u_PixelSize.y) * ceil(halfScale);
    vec2 u2 = inUV + vec2( u_PixelSize.x, -u_PixelSize.y) * floor(halfScale);
    vec2 u3 = inUV + vec2(-u_PixelSize.x,  u_PixelSize.y) * ceil(halfScale);

    // Grab our depth samples
    float d0 = texture(u_Material.s_Depth, u0).r;
    float d1 = texture(u_Material.s_Depth, u1).r;
    float d2 = texture(u_Material.s_Depth, u2).r;
    float d3 = texture(u_Material.s_Depth, u3).r;

    // Grab normals
    vec3 n0 = texture(u_Material.NormalMap, u0).rgb * 2 - 1;
    vec3 n1 = texture(u_Material.NormalMap, u1).rgb * 2 - 1;
    vec3 n2 = texture(u_Material.NormalMap, u2).rgb * 2 - 1;
    vec3 n3 = texture(u_Material.NormalMap, u3).rgb * 2 - 1;

    // Compute a threshold term based on the dot product between the camera and the normal
    float nDotV = 1 - dot(norm, -inViewPos);
    float normalThreshold = clamp((nDotV - u_DepthNormThreshold) / (1 - u_DepthNormThreshold), 0, 1);
    normalThreshold = normalThreshold * u_DepthNormThresholdScale + 1;

    // Robert's cross depth
    float dDiff0 = d1 - d0;
    float dDiff1 = d3 - d2;

    float edgeDepth = sqrt(pow(dDiff0, 2) + pow(dDiff1, 2)) * 64;
    edgeDepth = edgeDepth > u_DepthThreshold * normalThreshold * depth ? 1 : 0;
    

    // Robert's cross normals
    vec3 nDiff0 = n1 - n0;
    vec3 nDiff1 = n3 - n2;

    float edgeNorm = sqrt(dot(nDiff0, nDiff0) + dot(nDiff1, nDiff1));
    edgeNorm = edgeNorm > u_NormalThreshold ? 1 : 0;

    float edgeFactor = max(edgeDepth, edgeNorm);

    vec3 result = (u_OutlineColor.rgb * u_OutlineColor.a * edgeFactor) + (1 - edgeFactor) * albedoColor.rgb;

    albedo_specPower = vec4(result, 1.0f);//lightingParams.x);
	

}