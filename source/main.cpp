#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

using namespace std;
using namespace glm;

const int width = 1366;
const int height = 768;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Rendering/Renderer.h"
#include "Object/Object.h"
#include "Common/Read.h"
#include "Common/Image.h"

GLFWwindow* window;

int main() {

	stbi_set_flip_vertically_on_load(true);

	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int count;
	window = glfwCreateWindow(1366, 768, "Scene", glfwGetPrimaryMonitor(), NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.");
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		return false;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	stbi_set_flip_vertically_on_load(true);

	string raster_v = read("source/Shaders/p2vert.glsl");
	string raster_f = read("source/Shaders/p2frag.glsl");
	GLuint blitter = loadProgram(raster_v.c_str(), raster_f.c_str());

	GLint tex = glGetUniformLocation(blitter, "tex");
	GLint shadow_tex = glGetUniformLocation(blitter, "shadows");
	GLint color_tex = glGetUniformLocation(blitter, "colors");

	int w, h, n;
	void* data = stbi_load("sky.png", &w, &h, &n, 0);
	GLuint sky = loadTex(data, w, h, GL_RGBA, GL_RGBA);

	data = stbi_load("art.jpg", &w, &h, &n, 0);
	GLuint art = loadTex(data, w, h, GL_RGB, GL_RGB);

	Scene view = loadScene(read("plane.obj"));
	RenderMesh plane = view.genRenderMesh(view.indexByName("Plane"));
	GlobalMeshBuffer<2> viewMesh;
	ArrayAttrib attribs[2] = {
		ArrayAttrib {
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		},
		ArrayAttrib {
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		},
	};
	GLuint lengths[2] = {
		sizeof(vec3),
		sizeof(vec2)
	};
	viewMesh.init(attribs, lengths, 1000);
	void* locs[2] = {
		&plane.verts[0][0],
		&plane.uvs[0][0]
	};
	GlobalMesh plane_ID = viewMesh.append(locs, plane.verts.size());

	Scene scene = loadScene(read("scene.obj"));
	RenderMesh monkey = scene.genRenderMesh(scene.indexByName("monkey"));
	RenderMesh shadow = scene.genRenderMesh(scene.indexByName("shadow"));
	RenderMesh floor = scene.genRenderMesh(scene.indexByName("floor"));

	Renderer camera;
	if (!camera.init(width, height, width / 3, height / 3, 3000, 200)) {
		return -1;
	}

	GlobalMesh obj = camera.writeObjectToBuffers(monkey);
	GlobalMesh floor_mesh = camera.writeObjectToBuffers(floor);

	GlobalMesh obj_shadow = camera.writeShadowMeshToBuffers(shadow.verts);

	float prev, last;
	last = prev = glfwGetTime();

	int nframes = 0;
	do {
		float radius = 4 + sin(last);
		vec3 cameraPos = vec3(sin(last / 3) * radius, 1.5, cos(last / 3) * radius);
		mat4 mvp = perspective(radians(45.0f), float(width)/height, 0.1f, 100.0f);
		mvp = mvp * lookAt(cameraPos, vec3(0), vec3(0, 1, 0));

		//camera.beginShadowTransforms();
		//camera.drawShadowToBuffers(obj_shadow, identity<mat4>());

		camera.beginDraws(cameraPos);
		camera.drawObjectToBuffers(obj, mvp, sky);
		camera.drawObjectToBuffers(floor_mesh, mvp, art);
		camera.shade();

		glUseProgram(blitter);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 1366, 768);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		bindTexture2D(1, camera.rendertarget);
		bindTexture2D(0, camera.depthbuffer);
		bindTexture2D(2, camera.colorbuffer);
		glUniform1i(tex, 0);
		glUniform1i(shadow_tex, 1);
		glUniform1i(color_tex, 2);

		viewMesh.beginDraws();
		viewMesh.draw(GL_TRIANGLES, plane_ID);

		float curr = glfwGetTime();
		if (curr - prev >= 1) {
			prev = curr;
			float deltaT = curr - last;
			cout << nframes << endl;
			nframes = 0;
		}
		last = curr;
		nframes++;
		
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while(glfwWindowShouldClose(window) == 0 && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS);
	
	glfwTerminate();

	return 0;
}