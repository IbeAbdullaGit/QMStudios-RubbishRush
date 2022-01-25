/*
NOU Framework - Created for INFR 2310 at Ontario Tech.
(c) Samantha Stahlke 2020

morph.vert
Vertex shader.
Basic morph-based blending of vertex position and normals.
*/

#version 440

// Stores uniforms that change every frame (ex: time, camera data)
layout (std140, binding = 0) uniform b_FrameLevelUniforms {
    // The camera's view matrix
    uniform mat4 u_View;
    // The camera's projection matrix
    uniform mat4 u_Projection;
    // The combined viewProject matrix
    uniform mat4 u_ViewProjection;
    // The position of the camera in world space
    uniform vec4  u_CamPos;
    // The time in seconds since the start of the application
    uniform float u_Time;    
};

// Stores uniforms that change every object/instance
layout (std140, binding = 1) uniform b_InstanceLevelUniforms {
    // Complete MVP
    uniform mat4 u_ModelViewProjection;
    // Just the model transform, we'll do worldspace lighting
    uniform mat4 u_Model;
    // Normal Matrix for transforming normals
    uniform mat4 u_NormalMatrix;
};
//
////Keyframe 0 vertex position.
//layout(location = 0) in vec3 inPos_0;
////Keyframe 1 vertex position.
//layout(location = 1) in vec3 inPos_1;
////Keyframe 0 vertex normal.
//layout(location = 2) in vec3 inNorm_0;
////Keyframe 1 vertex normal.
//layout(location = 3) in vec3 inNorm_1;
//
//Keyframe 0 vertex position.
layout(location = 0) in vec3 inPos_0;
//Keyframe 1 vertex position.
layout(location = 1) in vec3 inColor;
//Keyframe 0 vertex normal.
layout(location = 2) in vec3 inNorm_0;
//Keyframe 1 vertex normal.
layout(location = 3) in vec2 inUV;

layout(location = 6) in vec3 inPos_1;

layout(location = 7) in vec3 inNorm_1;

layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBiTangent;

// Standard vertex shader outputs
layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outColor;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec2 outUV;
layout(location = 4) out mat3 outTBN;


uniform float t;

void main()
{
    //World-space vertex - LERP the verts!
    //(The final LERPed vert is, as always, transformed
    //by the model matrix.)
    vec3 new_pos = (mix(inPos_0, inPos_1, t));
    outWorldPos = (u_Model * vec4(new_pos, 1.0)).xyz;

    //World-space normal - LERP the normals!
    //(And transform by the normal matrix.)
    outNormal = mat3(u_NormalMatrix) * mix(inNorm_0, inNorm_1, t);
    
    //Output position - our viewprojection matrix
    //multiplied by world-space position.
    gl_Position = u_ViewProjection * vec4(outWorldPos, 1.0);

    // We use a TBN matrix for tangent space normal mapping
    vec3 T = normalize(vec3(mat3(u_NormalMatrix) * inTangent));
    vec3 B = normalize(vec3(mat3(u_NormalMatrix) * inBiTangent));
    vec3 N = normalize(vec3(mat3(u_NormalMatrix) * inNorm_0));
    mat3 TBN = mat3(T, B, N);

    // We can pass the TBN matrix to the fragment shader to save computation
    outTBN = TBN;

    // Pass our UV coords to the fragment shader
	outUV = inUV;

	///////////
	outColor = inColor;
}