#version 430

layout(location = 0) in vec2 inUV;
layout(location = 0) out vec3 outColor;

uniform layout(binding = 0) sampler2D s_Image;

uniform float iTime;

//help from https://www.shadertoy.com/view/3sGGRz
void main() {
    //calculate noise
    float mdf = 0.1; // increase for noise amount 
    float noise = (fract(sin(dot(inUV, vec2(12.9898,78.233)*2.0)) * 43758.5453));
    vec3 color = texture(s_Image, inUV).rgb;

    mdf *= sin(iTime) + 1.0; // animate the effect's strength

    //the output
    outColor = color - noise*mdf;
    
}
