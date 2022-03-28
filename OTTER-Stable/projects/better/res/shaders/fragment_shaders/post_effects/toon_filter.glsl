#version 430

layout(location = 0) in vec2 inUV;
layout(location = 0) out vec3 outColor;

uniform layout(binding = 0) sampler2D s_Image;
uniform layout(binding = 1) sampler1D s_Toon;

uniform int Steps;
void main() {
    vec3 color = texture(s_Image, inUV).rgb;

    // Using a LUT to allow artists to tweak toon shading settings
    outColor.r = texture(s_Toon, color.r).r;
    outColor.g = texture(s_Toon, color.g).g;
    outColor.b = texture(s_Toon, color.b).b;

    //control strength?
    outColor = round(outColor*Steps)/Steps;
   // outColor = mix(color, texture(s_Lut, color).rgb, clamp(u_Strength, 0, 1));
}
