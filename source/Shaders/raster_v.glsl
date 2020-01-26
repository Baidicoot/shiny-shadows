#version 430 core

layout(location = 0) in vec3 vertexpos;
layout(location = 1) in vec3 normal_IN;

//uniform mat4 mvp;

out vec3 normal;

void main() {
	gl_Position = vec4(vertexpos, 1);
	normal = normal_IN;
}