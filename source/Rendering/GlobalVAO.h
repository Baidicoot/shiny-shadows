#pragma once

#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../OpenGL/Helper.h"
#include "GlobalVAO.h"

struct GlobalMesh {
	GLuint offset;
	GLuint size;
};

template <size_t SIZE>
struct GlobalMeshBuffer {
	GLuint vao;
	GLuint vbos[SIZE];
	GLuint lengths[SIZE];

	GLuint end;
	GLuint max;

	void init(ArrayAttrib* attrib, GLuint* lengths, GLuint maxsize);
	GlobalMesh append(void** data, GLuint length);
	GlobalMesh all();
	void beginDraws();
	void draw(GLenum mode, GlobalMesh m);
	void drawInstanced(GLenum mode, GlobalMesh m, GLuint instances);
};

#include "GlobalVAO.cpp"