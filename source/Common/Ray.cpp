#include <glm/glm.hpp>
#include <vector>

#include "Ray.h"

using namespace glm;
using namespace std;

Sphere fromVector(vec4 s) {
	return Sphere{ vec3(s.x, s.y, s.z), s.w };
}

Intersection intersectSphere(Ray r, Sphere s) {
	float t = dot(s.center - r.origin, r.direction);
	if (t < 0) return Intersection{ false, 0 };
	float y = distance(r.origin + t * r.direction, s.center);
	if (y >= s.radius) return Intersection{ false, 0 };
	float x = sqrt(s.radius*s.radius - y*y);
	return Intersection{ true, min(t - x, t + x) };
}

Intersection castRay(Ray r, vector<Sphere> s) {
	Intersection minIntersection = Intersection{ false, 0 };

	for (int i = 0; i < s.size(); i++) {
		Intersection intersection = intersectSphere(r, s[i]);
		if (intersection.exists) {
			if (intersection.t < minIntersection.t || !minIntersection.exists) {
				minIntersection = intersection;
			}
		}
	}

	return minIntersection;
}