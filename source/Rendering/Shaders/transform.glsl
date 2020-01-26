#version 430 core

layout(location = 0) in vec3 pos_IN;
out vec3 position;

void main()
{
    position = pos_IN;
}