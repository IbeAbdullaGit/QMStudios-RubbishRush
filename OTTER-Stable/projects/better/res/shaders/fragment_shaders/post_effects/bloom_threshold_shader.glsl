#version 430

layout(location = 0) in vec2 inUV;
//bright color
layout(location = 0) out vec3 outColor;
uniform layout(binding = 0) sampler2D s_Image;

uniform float threshold;
uniform float strength;

void main() {
    vec3 color = texture(s_Image, inUV).rgb;
    //convert rgb to grayscale/brightness
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > threshold)
        outColor = strength*color;
    else
        outColor = vec3(0.0);

  
   // outColor = mix(color, texture(s_Lut, color).rgb, clamp(u_Strength, 0, 1));
}