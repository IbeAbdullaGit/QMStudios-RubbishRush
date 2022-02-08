/*
NOU Framework - Created for INFR 2310 at Ontario Tech.
(c) Samantha Stahlke 2020

morph.vert
Vertex shader.
Basic morph-based blending of vertex position and normals.
*/

#version 440

// Include our common vertex shader attributes and uniforms
#include "../fragments/vs_common.glsl"


////Keyframe 0 vertex position.
//layout(location = 0) in vec3 inPos_0;
////Keyframe 1 vertex position.
//layout(location = 1) in vec3 inPos_1;
////Keyframe 0 vertex normal.
//layout(location = 2) in vec3 inNorm_0;
////Keyframe 1 vertex normal.
//layout(location = 3) in vec3 inNorm_1;
//

layout(location = 6) in vec3 inPos_1;

layout(location = 7) in vec3 inNorm_1;


uniform float t;

void main()
{
    //World-space vertex - LERP the verts!
    //(The final LERPed vert is, as always, transformed
    //by the model matrix.)
    vec3 new_pos = (mix(inPosition, inPos_1, t));
    outWorldPos = (u_Model * vec4(new_pos, 1.0)).xyz;

    //World-space normal - LERP the normals!
    //(And transform by the normal matrix.)
    outNormal = mat3(u_NormalMatrix) * mix(inNormal, inNorm_1, t);
    
    //Output position - our viewprojection matrix
    //multiplied by world-space position.
    gl_Position = u_ViewProjection * vec4(outWorldPos, 1.0);

    // We use a TBN matrix for tangent space normal mapping
    vec3 T = normalize(vec3(mat3(u_NormalMatrix) * inTangent));
    vec3 B = normalize(vec3(mat3(u_NormalMatrix) * inBiTangent));
    vec3 N = normalize(vec3(mat3(u_NormalMatrix) * inNormal));
    mat3 TBN = mat3(T, B, N);

    // We can pass the TBN matrix to the fragment shader to save computation
    outTBN = TBN;

    // Pass our UV coords to the fragment shader
	outUV = inUV;

	///////////
	outColor = inColor;
}