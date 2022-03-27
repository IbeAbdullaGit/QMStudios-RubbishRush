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



layout(location = 6) in vec3 inPos_1;

layout(location = 7) in vec3 inNorm_1;


uniform float t;


void main()
{
    //World-space vertex - LERP the verts!
    //(The final LERPed vert is, as always, transformed
    //by the model matrix.)
    vec3 new_pos = (mix(inPosition, inPos_1, t));
    //OLD
    outViewPos = (u_ModelView * vec4(new_pos, 1.0)).xyz;
    // Pass vertex pos in world space to frag shader
	//outViewPos = (u_ModelView * vec4(inPosition, 1.0)).xyz;

    //World-space normal - LERP the normals!
    //(And transform by the normal matrix.)
    //OLD
    //outNormal = mat3(u_NormalMatrix) * mix(inNormal, inNorm_1, t);
    // Normals
	outNormal = (u_View * vec4(mat3(u_NormalMatrix) * mix(inNormal,inNorm_1, t), 0)).xyz;

    
    //Output position - our viewprojection matrix
    //multiplied by world-space position.
    gl_Position = u_ModelViewProjection * vec4(inPosition, 1.0);

    
    // We use a TBN matrix for tangent space normal mapping
    vec3 T = normalize((u_View * vec4(mat3(u_NormalMatrix) * inTangent, 0)).xyz);
    vec3 B = normalize((u_View * vec4(mat3(u_NormalMatrix) * inBiTangent, 0)).xyz);
    vec3 N = normalize((u_View * vec4(mat3(u_NormalMatrix) * inNormal, 0)).xyz);
    mat3 TBN = mat3(T, B, N);

    // We can pass the TBN matrix to the fragment shader to save computation
    outTBN = TBN;

    // Pass our UV coords to the fragment shader
	outUV = inUV;

	///////////
	outColor = inColor;

   
}