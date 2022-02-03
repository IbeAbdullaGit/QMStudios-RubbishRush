#version 440

// Include our common vertex shader attributes and uniforms
#include "../fragments/vs_common.glsl"

#include "../fragments/math_constants.glsl"

layout(location = 5) out vec2 outTexWeights;

uniform float u_Scale;

void main() {

	gl_Position = u_ModelViewProjection * vec4(inPosition, 1.0);

	// Pass vertex pos in world space to frag shader
	outWorldPos = (u_Model * vec4(inPosition, 1.0)).xyz;
	// Normals
	outNormal = mat3(u_NormalMatrix) * inNormal;
	// Pass our UV coords to the fragment shader
	outUV = inUV;
	///////////
	outColor = inColor;

    // We have some calculation to determine the texture weights
    // In this case, we are going to use cos and sin to generate texture
    // weights based on the z coord of the model in world coords
    outTexWeights = vec2(
        (sin(outWorldPos.z / u_Scale + M_PI) + 1) / 2,
        (sin(outWorldPos.z / u_Scale) + 1) / 2
    );
}

