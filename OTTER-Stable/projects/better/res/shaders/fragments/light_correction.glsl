// We need the flags from the frame uniforms
#pragma once
#include "frame_uniforms.glsl"

// Function for applying color correction
vec3 LightCorrect(vec3 lightAccumulation) {
    // If our color correction flag is set, we perform the color lookup
    if (IsFlagSet(FLAG_ENABLE_LIGHTS)) {
        return lightAccumulation;
    }
    // Otherwise just return the input
    else {
        return vec3(0.0);
    }
}
vec3 SpecularCorrect(vec3 specular){
    if (IsFlagSet(FLAG_ENABLE_SPECULAR))
    {
        return specular;
    }
    else
        return vec3(0.0);
}
vec3 AmbientCorrect(vec3 specular){
    if (IsFlagSet(FLAG_ENABLE_AMBIENT))
    {
        return specular;
    }
    else
        return vec3(0.0);
}
