#version 440

layout (location = 0) in vec2 inPos;

layout (location = 0) out vec2 outUV;

void main() {
    gl_Position = vec4(inPos, 0, 1);
    outUV = (inPos + 1) / 2;
}