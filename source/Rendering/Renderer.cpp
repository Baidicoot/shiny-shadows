#include "Renderer.h"

using namespace glm;
using namespace std;

const ArrayAttrib MESH_attribs[3] = {
	ArrayAttrib{ //verts
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		0
	},
	ArrayAttrib{ //uvs
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		0
	},
	ArrayAttrib{ //normals
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		0
	},
};
const GLuint MESH_lengths[3] = {
	sizeof(vec3),
	sizeof(vec2),
	sizeof(vec3)
};

const ArrayAttrib SHADOW_attrib = ArrayAttrib{
	0,
	3,
	GL_FLOAT,
	GL_FALSE,
	0,
	0
};
const GLuint SHADOW_length = sizeof(vec3);

bool Renderer::init(GLuint w, GLuint h, GLuint sw, GLuint sh, GLuint maxVerts, GLuint maxShadowVerts) {
	width = w;
	height = h;
	swidth = sw;
	sheight = sh;
	maxverts = maxVerts;
	maxshadowverts = maxShadowVerts;
	maxshadowsize = maxshadowverts * sizeof(vec3);

	/*Buffers*/

	meshbuffer.init((ArrayAttrib*)MESH_attribs, (GLuint*)MESH_lengths, maxverts);

	shadowbuffer.init((ArrayAttrib*)&SHADOW_attrib, (GLuint*)&SHADOW_length, maxshadowverts);
	
	glGenBuffers(1, &transformedverts);
	glBindBuffer(GL_ARRAY_BUFFER, transformedverts);
	glBufferData(GL_ARRAY_BUFFER, maxshadowverts * sizeof(vec3), NULL, GL_STATIC_DRAW);

	/*G-Buffers*/

	GLuint textures[3];
	int res = genRGBADepthFBO(width, height, fbo, 3, textures);
	if (res != 0) {
		cout << "Framebuffer not complete! Error: " << res << endl;
		return false;
	}

	depthbuffer = textures[0];
	colorbuffer = textures[1];
	normbuffer = textures[2];
	rendertarget = genTexRGBAFiltered(swidth, sheight, TARGET_FILTER);

	/*Programs*/

	string shaderCode = read("source/Rendering/Shaders/raytracer.glsl");
	string vertCode = read("source/Rendering/Shaders/vert.glsl");
	string fragCode = read("source/Rendering/Shaders/frag.glsl");
	string feedbackCode = read("source/Rendering/Shaders/transform.glsl");
	if (shaderCode.empty() || vertCode.empty() || fragCode.empty() || feedbackCode.empty()) {
		cout << "Could not read from shader files!" << endl;
		return false;
	}

	const char* varyings[] = {
		"position"
	};

	transformer = loadTransform(feedbackCode.c_str(), 1, varyings, GL_INTERLEAVED_ATTRIBS);

	raytracer = loadCompute(shaderCode.c_str());
	tracer_uniforms = RaytracerUniforms{
		glGetUniformLocation(raytracer, "dims"),
		glGetUniformLocation(raytracer, "ntris"),
	};

	rasterizer = loadProgram(vertCode.c_str(), fragCode.c_str());
	raster_uniforms = RasterizerUniforms{
		glGetUniformLocation(rasterizer, "mvp"),
		glGetUniformLocation(rasterizer, "cameraPos"),
		glGetUniformLocation(rasterizer, "tex")
	};

	return true;
}

GlobalMesh Renderer::writeObjectToBuffers(RenderMesh mesh) {
	void* dataLocs[3] = {
		&mesh.verts[0][0],
		&mesh.uvs[0][0],
		&mesh.normals[0][0]
	};
	return meshbuffer.append(dataLocs, mesh.verts.size());
}

void Renderer::beginDraws(vec3 Pos) {
	pos = Pos;

	glDisable(GL_RASTERIZER_DISCARD);

	glUseProgram(rasterizer);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, width, height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	meshbuffer.beginDraws();
}

void Renderer::drawObjectToBuffers(GlobalMesh id, mat4 mvp, GLuint tex) {
	glUniform3fv(raster_uniforms.cameraPos, 1, &pos[0]);
	glUniformMatrix4fv(raster_uniforms.mvp, 1, GL_FALSE, &mvp[0][0]);

	bindTexture2D(0, tex);
	glUniform1i(raster_uniforms.tex, 0);

	meshbuffer.draw(GL_TRIANGLES, id);
}

GlobalMesh Renderer::writeShadowMeshToBuffers(vector<vec3> verts) {
	void* dataLocs[1] = {
		&verts[0][0],
	};
	return shadowbuffer.append(dataLocs, verts.size());
}

void Renderer::beginShadowTransforms() {
	glEnable(GL_RASTERIZER_DISCARD);
	glUseProgram(transformer);

	shadowoffset = 0;
}

void Renderer::drawShadowToBuffers(GlobalMesh id, mat4 mvp) {
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, transformedverts);

	glBeginTransformFeedback(GL_TRIANGLES);
	shadowbuffer.draw(GL_TRIANGLES, id);
	glEndTransformFeedback();
	glFlush();

	shadowoffset += id.size;
}

void Renderer::shade() {
	bindTexture2D(0, depthbuffer);
	bindTexture2D(1, colorbuffer);
	bindTexture2D(2, normbuffer);

	bindImageTexture(3, GL_TEXTURE_2D, rendertarget, GL_RGBA32F);

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadowbuffer.vbos[0]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, shadowbuffer.vbos[0]);

	glUseProgram(raytracer);
	glUniform2ui(tracer_uniforms.dims, swidth, sheight);
	glUniform1i(tracer_uniforms.ntris, shadowbuffer.end / 3);

	glDispatchCompute(swidth, sheight, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}