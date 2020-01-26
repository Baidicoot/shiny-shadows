#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <sstream>
#include <string>

#include "../Object/Object.h"
#include "../OpenGL/Helper.h"
#include "../Common/Read.h"
#include "Renderer.h"
#include "GlobalVAO.h"

#define TARGET_FILTER GL_LINEAR, GL_LINEAR

struct RaytracerUniforms {
	GLint dims;
	GLint ntris;
};

struct RasterizerUniforms {
	GLint mvp;
	GLint cameraPos;
	GLint tex;
};

struct VertexCS { //for some reason packed...
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::ivec2 id;
};

struct Material {
	GLfloat smoothness;
	glm::vec3 emissivity;
};

struct Light {
	bool directional;
	std::vector<glm::vec3> pos;
	std::vector<glm::vec3> col;
};

struct Renderer { //TODO:: DECLUTTER AND ADD MSAA
	GLuint width;
	GLuint height;
	GLuint swidth;
	GLuint sheight;

	glm::vec3 pos;

	GLuint maxverts;
	GLuint maxshadowverts;
	GLuint maxshadowsize;
	GLuint shadowoffset;

	GLuint raytracer;
	RaytracerUniforms tracer_uniforms;

	GLuint rasterizer;
	RasterizerUniforms raster_uniforms;

	GLuint transformer;

	GlobalMeshBuffer<3> meshbuffer;
	GlobalMeshBuffer<1> shadowbuffer;
	GLuint transformedverts;

	/*
	GLuint vao;		// input
	GLuint verts;	// input
	GLuint uvs;		// input
	GLuint normals;	// input
	*/

	GLuint fbo;
	GLuint depthbuffer;	// Assigned as GL_COLOR_ATTACHMENT0 to fbo
	GLuint colorbuffer;	// Assigned as GL_COLOR_ATTACHMENT1 to fbo
	GLuint normbuffer;  // Assigned as GL_COLOR_ATTACHMENT2 to fbo
	GLuint rendertarget;

	bool init(GLuint w, GLuint h, GLuint sw, GLuint sh, GLuint maxVerts, GLuint maxShadowVerts);

	GlobalMesh writeObjectToBuffers(RenderMesh mesh);
	GlobalMesh writeShadowMeshToBuffers(std::vector<glm::vec3> verts);

	void beginDraws(glm::vec3 pos);
	void beginShadowTransforms();
	void drawObjectToBuffers(GlobalMesh id, glm::mat4 mvp, GLuint tex);
	void drawShadowToBuffers(GlobalMesh id, glm::mat4 mvp);

	void shade();

	/*
	bool init(GLuint w, GLuint h, GLuint maxVerts, GLuint maxDraws);

	void begin(glm::vec3 cameraPos, glm::vec3 cameraFacing, glm::vec3 upVector, float fovy, float aspect);
	void drawObjectToBuffers(RenderMesh mesh);
	void shade(std::vector<Material> materials, GLuint textureArray);
	*/

	//void begin(glm::vec3 pos, glm::vec3 facing, glm::vec3 up, float fovy, float aspect);
	//void renderObject(std::vector<glm::vec3> verticies, std::vector<glm::vec2> uvs, std::vector<glm::vec3> normals);
};