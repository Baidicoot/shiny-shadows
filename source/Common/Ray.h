#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;
};

struct Sphere {
	glm::vec3 center;
	float radius;
};

Sphere fromVector(glm::vec4 s);

struct Intersection {
	bool exists;
	float t;
};

Intersection intersectSphere(Ray r, Sphere s);
Intersection castRay(Ray r, std::vector<Sphere>);