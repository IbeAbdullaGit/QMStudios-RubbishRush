#version 430

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec3 inViewDir;

//layout (location =2) in vec3 inDiffuse;
//layout(location = 2) in vec3 inNormal;

layout (location = 0) out vec3 outColor;

uniform layout(binding = 0) sampler2D s_Image;
uniform layout(binding = 1) sampler2D s_Depth;
uniform layout(binding = 2) sampler2D s_Normals;

uniform float rimLightPower = 1.0;
//rim color
uniform vec4 _color;

#define MarchLength 255.
#define MaxDepth 100.
#include "../../fragments/frame_uniforms.glsl"

uniform vec3 iResolution;
uniform float iTime =0.1;

float getDist(vec3 pos) {
    return length(pos) - 1.;
}
vec3 getNormal(vec3 pos) {
    float d = getDist(pos);
    vec2 e = vec2(.001, 0);
    
    vec3 n = d - vec3(
        getDist(pos-e.xyy),
        getDist(pos-e.yxy),
        getDist(pos-e.yyx));
    
    return normalize(n);
}

float rayMarch(vec3 origin, vec3 dir) {
    vec3 pos = origin;
    float depth = 0.;
    for(float i = 0.; i <= MarchLength; i++) {
        float dis = getDist(pos);
        depth += dis;
        if(dis <= .01 || depth > MaxDepth) {
            break;
        }
        pos = pos + dis * dir;
    }
    return depth;
}

void main() {
    
    vec4 fragColor;

     // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = inUV/iResolution.xy;
    uv = uv * 2. -1.;
    uv.x *= iResolution.x/iResolution.y;
    
    vec3 camPos = inViewDir;

    vec3 rayDir = vec3(uv, -1.);
    float dis = rayMarch(camPos, normalize(rayDir));
    vec3 lightPos = vec3(0, 5, 13);
    if(dis < MaxDepth) {
        vec3 pos = camPos + rayDir * dis;
        vec3 viewDir = normalize(camPos - pos);
        vec3 lightDir = normalize(lightPos);
        vec3 normal = getNormal(pos);
        float diffuseF = clamp(dot(lightDir, normal), 0., 1.0);
        float rimF = pow(1. - clamp(dot(viewDir, normal), 0., 1.), clamp(sin(iTime * 10.) * 10., 1., 10.));
        fragColor = vec4(diffuseF) +  smoothstep(vec4(1.000,0.000,0.882,0.), vec4(1.000,1.000,1.000,1), vec4(rimF));
        return;
    }

    vec3 color = texture(s_Image, inUV).rgb;

    outColor = fragColor.rgb;
}
