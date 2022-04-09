#version 430
#pragma once
layout(location = 0) in vec2 inUV;
layout(location = 0) out vec4 outColor;

uniform layout(binding = 0) sampler2D s_Albedo;
uniform layout(binding = 1) sampler2D s_NormalsMetallic;
uniform layout(binding = 2) sampler2D s_DiffuseAccumulation;
uniform layout(binding = 3) sampler2D s_SpecularAccumulation;
uniform layout(binding = 4) sampler2D s_Emissive;

#include "../fragments/frame_uniforms.glsl"
#include "../fragments/multiple_point_lights.glsl"
#include "../fragments/light_correction.glsl"

void main() {
    
    vec3 albedo = texture(s_Albedo, inUV).rgb;
    vec3 diffuse = texture(s_DiffuseAccumulation, inUV).rgb;
    vec3 specular = texture(s_SpecularAccumulation, inUV).rgb;
 
    vec4 emissive = texture(s_Emissive, inUV);
   

    //emissive = 1-emissive;
    //can customize how these work
    //albedo = LightCorrect(albedo);
    diffuse = LightCorrect(diffuse);
    specular = LightCorrect(specular);
    emissive.rgb = LightCorrect(emissive.rgb);

     //enable/disable specular
     //send in original value, so we can do specular-only
    specular = SpecularCorrect(texture(s_SpecularAccumulation, inUV).rgb);
    //diffuse = AmbientCorrect(texture(s_DiffuseAccumulation, inUV).rgb);

  //vec4 emissive = vec4(0.5, 0.5, 0.5, 1.0);

	outColor = vec4(albedo * (diffuse + specular + (emissive.rgb * emissive.a)), 1.0);
}