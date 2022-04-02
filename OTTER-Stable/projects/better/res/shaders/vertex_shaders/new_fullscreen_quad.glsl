#version 440

layout (location = 0) in vec2 inPos;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 outViewDir;
layout(location =2) out vec4 sides;
#include "../fragments/frame_uniforms.glsl"


void main() {
    mat4 clipToView = inverse(u_Projection);

    gl_Position = vec4(inPos, 0, 1);
    sides = gl_Position;
    sides.x = sides.x - 128.0;

    outUV = (inPos + 1) / 2;
    outViewDir = (clipToView * vec4(inPos, 0, 1)).xyz;


}