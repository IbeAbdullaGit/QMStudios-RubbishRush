#version 450

layout(location = 0) in vec4 fragColor;

out vec4 frag_color;

void main() { 
	frag_color = fragColor;
}

