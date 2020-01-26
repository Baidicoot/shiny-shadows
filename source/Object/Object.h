#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <string>

#include "Object.h"

struct RenderMesh {
	std::vector<glm::vec3> verts;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
};

struct VertexData {
	std::vector<glm::vec3> vertexes;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
};

struct Mesh {
	std::vector<glm::ivec3> faces;
	std::vector<glm::ivec3> uvmaps;
	std::vector<glm::ivec3> normalmaps;
};

struct Scene {
	std::vector<Mesh> objects;
	std::vector<std::string> objectNames;
	VertexData vertexes;
	int indexByName(const char* name);
	RenderMesh genRenderMesh(int id);
};

Scene loadScene(std::string str);
void genDataArrays(VertexData verts, Mesh a, std::vector<glm::vec3>& vertexes, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals);