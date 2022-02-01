#version 450

layout (location = 1) in vec3  inPosition;
layout (location = 3) in vec4  inColor;

layout (location = 0) out vec4 fragColor;

#include "../fragments/frame_uniforms.glsl"

void main() {
    gl_Position = u_ViewProjection * vec4(inPosition, 1);
    fragColor = inColor;
    gl_PointSize = 10.0; 
}