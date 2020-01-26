#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "Helper.h"

const GLfloat vertex_buffer_data[] = {
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
};

struct ArrayAttrib {
	GLuint index;
	GLint size;
	GLenum type;
	GLboolean normalised;
	GLsizei stride;
	GLuint offset;
};

int genVAOArrBuf(GLuint& vaoID, int nBuf, ArrayAttrib* attribs, GLuint* buffers);

GLuint genQuadVBO();

GLuint genTexRGBA(GLuint width, GLuint height);
GLuint genTexRGBAFiltered(GLuint width, GLuint height, GLenum filterMag, GLenum filterMin);
void bindTexture2D(GLuint unit, GLuint texture);
void bindTexture(GLuint unit, GLenum type, GLuint texture);
void bindImageTexture(GLuint unit, GLenum type, GLuint texture, GLenum format);

GLuint loadCompute(const char* source);
GLuint loadTransform(const char* vert, int nvaryings, const char** varyings, GLenum format);
GLuint loadProgram(const char* vert, const char* frag);

int genRGBADepthFBO(GLuint width, GLuint height, GLuint& fboID, int ntex, GLuint* textures);

GLuint loadTex(void* data, int width, int height, GLenum internalformat, GLenum format);

GLuint loadSpriteSheet(void* data, int width, int height, int depth, GLenum internalformat, GLenum format);
