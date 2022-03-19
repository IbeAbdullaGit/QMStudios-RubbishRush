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

