#version 450

layout(location = 0) in vec2 inUV;

float GooFunc(vec2 uv,float zoom,float distortion, float gooeyness,float wibble)
{
    float s = sin(iTime*0.1);
    float s2 = 0.5+sin(iTime*1.8);
    vec2 d = uv*(distortion+s*.3);
    d.x += iTime*0.25+sin(d.x+d.y + iTime*0.3)*wibble;
    d.y += iTime*0.25+sin(d.x + iTime*0.3)*wibble;
    float v1=length(0.5-fract(d.xy))+gooeyness;
    d = (1.0-zoom)*0.5+(uv*zoom);					// try removing this :)
    float v2=length(0.5-fract(d.xy));
    v1 *= 1.0-v2*v1;
    v1 = v1*v1*v1;
    v1 *= 1.9+s2*0.2;
    return v1;
}

void mainImage(out vec4 k, vec2 p)
{
    vec3 uv = GetNormal(inUV);

	float distortion = 4.0;						// increase or decrease to suit your taste.
    float zoom = 0.7;							// zoom value
    float gooeyness = 0.95;						// smaller = more gooey bits
    float wibble = 0.5;							// tweak the wibble!
    float goo = GooFunc(uv, zoom, distortion, gooeyness,wibble);
    
    const vec4 col1 = vec4(0.0,.1,.1,1.0);
    const vec4 col2 = vec4(0.5,0.9,0.3,1.0);
    float saturation = 2.4;
    k = mix(col2,col1,goo)*saturation;

    float avg = max(max(k.r,k.g),k.b);		//float avg = k.g;	//(k.r+k.g+k.b)/3.0;
    float alpha=1.0;
    if (avg<=0.4)
    {
        // darken & alpha edge of goo...
        avg = clamp(avg,0.0,1.0);
        k*=avg+0.2;						// 0.0 = black edges
        alpha = clamp((avg*avg)*5.5,0.0,1.0);
    }

    // blend goo + background based on the Alpha
    k = mix(k,alpha);
}