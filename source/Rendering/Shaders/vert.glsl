#version 430

layout(location = 0) in vec3 pos_IN;
layout(location = 1) in vec2 uv_IN;
layout(location = 2) in vec3 normal_IN;

out vec4 worldpos;
out vec2 uv;
out vec4 normal;
out vec4 apparentpos;

uniform mat4 mvp;
//uniform mat4 m;

void main() {
	gl_Position = apparentpos = mvp * vec4(pos_IN, 1.0);
	worldpos = vec4(pos_IN, 1);
	normal = vec4(normal_IN, 0);
	uv = uv_IN;
}