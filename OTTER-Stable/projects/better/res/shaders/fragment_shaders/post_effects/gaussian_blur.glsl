#version 430

layout(location = 0) in vec2 inUV;
layout(location = 0) out vec3 outColor;
uniform layout(binding = 0) sampler2D s_Image;

uniform float resolution;
uniform float radius;
uniform vec2 direction;

void main() {
	float blur = radius/resolution;
	float hstep = direction.x;
	float vstep = direction.y;

	//9x9 kernel
	vec4 sum = texture2D( s_Image , vec2( inUV.x - 4.0*blur*hstep , inUV.y - 4.0*blur*vstep )) * 0.0162162162;
	sum += texture2D( s_Image , vec2( inUV.x - 3.0*blur*hstep , inUV.y - 3.0*blur*vstep )) * 0.0540540541;
	sum += texture2D( s_Image , vec2( inUV.x - 2.0*blur*hstep , inUV.y - 2.0*blur*vstep )) * 0.1216216216;
	sum += texture2D( s_Image , vec2( inUV.x - 1.0*blur*hstep , inUV.y - 1.0*blur*vstep )) * 0.1945945946;
	sum += texture2D( s_Image , vec2( inUV.x , inUV.y )) * 0.2270270270;
	sum += texture2D( s_Image , vec2( inUV.x + 1.0*blur*hstep , inUV.y + 1.0*blur*vstep )) * 0.1945945946;
	sum += texture2D( s_Image , vec2( inUV.x + 2.0*blur*hstep , inUV.y + 2.0*blur*vstep )) * 0.1216216216;
	sum += texture2D( s_Image , vec2( inUV.x + 3.0*blur*hstep , inUV.y + 3.0*blur*vstep )) * 0.0540540541;
	sum += texture2D( s_Image , vec2( inUV.x + 4.0*blur*hstep , inUV.y + 4.0*blur*vstep )) * 0.0162162162;

	outColor = sum.rgb;
}