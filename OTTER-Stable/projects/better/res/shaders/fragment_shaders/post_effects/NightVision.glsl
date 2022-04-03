#version 430
layout(location = 0) in vec2 inUV;
layout(location = 0) out vec4 outColor;
uniform float iTime;

uniform layout(binding = 0) sampler2D s_Image;
uniform layout(binding = 1) sampler2D noiseTex;
uniform layout(binding = 2) sampler2D maskTex;
uniform float luminanceThreshold = 0.2; // 0.2
uniform float colorAmplification = 4.0; // 4.0
uniform float effectCoverage = 1.0; // 0.5

//https://www.geeks3d.com/20091009/shader-library-night-vision-post-processing-filter-glsl/

void main ()
{
  vec4 finalColor;
  // Set effectCoverage to 1.0 for normal use.  
  if (inUV.x < effectCoverage) 
  {
    vec2 uv;           
    uv.x = 0.4*sin(iTime*50.0);                                 
    uv.y = 0.4*cos(iTime*50.0);                                 
    float m = texture2D(maskTex, inUV.st).r;
    vec3 n = texture2D(noiseTex, 
                 (inUV.st*3.5) + uv).rgb;
    vec3 c = texture2D(s_Image, inUV.st 
                               + (n.xy*0.005)).rgb;
  
    float lum = dot(vec3(0.30, 0.59, 0.11), c);
    if (lum < luminanceThreshold)
      c *= colorAmplification; 
  
    vec3 visionColor = vec3(0.1, 0.95, 0.2);
    finalColor.rgb = (c + (n*0.2)) * visionColor * m;
   }
   else
   {
    finalColor = texture2D(s_Image, 
                  inUV);
   }
  outColor.rgb = finalColor.rgb;
  outColor.a = 1.0;
}			