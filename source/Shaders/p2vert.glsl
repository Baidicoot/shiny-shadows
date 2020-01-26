#version 430

layout(location = 0) in vec3 pos_IN;
layout(location = 1) in vec2 uv_IN;

out vec2 uv;

void main() {
	gl_Position = vec4(pos_IN, 1);
	uv = uv_IN;
}