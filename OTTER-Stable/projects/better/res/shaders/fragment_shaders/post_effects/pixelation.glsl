#version 430

layout(location = 0) in vec2 inUV;
layout(location = 0) out vec3 outColor;

uniform layout(binding = 0) sampler2D s_Image;

//help from https://godotshaders.com/shader/pixelate/
uniform int amount = 40;

//help from https://www.shadertoy.com/view/3sGGRz
void main() {
    
    

    //convert to grid
    vec2 grid_uv = round(inUV*float(amount))/float(amount);
   
    vec3 color = texture(s_Image, grid_uv).rgb;

    //the output
    outColor = color;
    
}
