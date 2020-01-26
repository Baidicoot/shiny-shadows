using namespace glm;
using namespace std;

void print(VertexCS vert) {
	cout << "Pos: " << vert.pos.x << " " << vert.pos.y << " " << vert.pos.z << " ID: " << vert.id.x << endl;
}

mat4x4 genV(vec3 pos, vec3 facing, vec3 up) {
	return lookAt(pos, facing, up);
}

mat4x4 genP(float fovy, float aspect) {
	return perspective(radians(fovy), aspect, 0.125f, 256.0f);
}

vector<vec3> genVerticies(vector<vec3> vertexes, vector<ivec3> faces) {
	vector<vec3> verticies;

	for (int i = 0; i < faces.size(); i++) {
		verticies.push_back(vertexes[faces[i][0]]);
		verticies.push_back(vertexes[faces[i][1]]);
		verticies.push_back(vertexes[faces[i][2]]);
	}

	return verticies;
}

bool Renderer::init(GLuint w, GLuint h, GLuint maxVerts, GLuint maxDraws) {
	width = w;
	height = h;
	maxverts = maxVerts;
	frame = 0;

	/* VAOs and VBOs */

	ArrayAttrib attribs[3] = {
		ArrayAttrib{
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		},
		ArrayAttrib{
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		},
		ArrayAttrib{
			2,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		},
	};
	GLuint vbos[3];
	genVAOArrBuf(vao, 3, attribs, vbos);

	verts = vbos[0];
	uvs = vbos[1];
	normals = vbos[2];

	glBindBuffer(GL_ARRAY_BUFFER, verts);
	glBufferData(GL_ARRAY_BUFFER, maxverts * sizeof(vec3), NULL, GL_DYNAMIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, uvs);
	glBufferData(GL_ARRAY_BUFFER, maxverts * sizeof(vec2), NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, normals);
	glBufferData(GL_ARRAY_BUFFER, maxverts * sizeof(vec3), NULL, GL_DYNAMIC_DRAW);

	/* SSBOs for transformed verticies */

	glGenBuffers(1, &transformedverts);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, transformedverts);
	glBufferData(GL_SHADER_STORAGE_BUFFER, maxverts * sizeof(VertexCS), NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &materials);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, materials);
	glBufferData(GL_SHADER_STORAGE_BUFFER, maxDraws * sizeof(Material), NULL, GL_DYNAMIC_DRAW);

	/* FBO and Textures */

	GLuint textures[3];
	int res = genRGBADepthFBO(width, height, fbo, 3, textures);
	if (res != 0) {
		cout << "Framebuffer not complete! Error: " << res << endl;
		return false;
	}

	depthbuffer = textures[0];
	uvbuffer = textures[1];
	normbuffer = textures[2];
	rendertarget = genTexRGBA(width, height);

	/* Programs */

	string shaderCode = read("source/Rendering/Shaders/raytracer.glsl");
	string vertCode = read("source/Rendering/Shaders/vert.glsl");
	string fragCode = read("source/Rendering/Shaders/frag.glsl");
	if (shaderCode.empty() || vertCode.empty() || fragCode.empty()) {
		cout << "Could not read from shader files!" << endl;
		return false;
	}

	raytracer = loadCompute(shaderCode.c_str());
	tracer_uniforms = RaytracerUniforms{
		glGetUniformLocation(raytracer, "nobjs"),
		glGetUniformLocation(raytracer, "time"),
		glGetUniformLocation(raytracer, "frame"),
		glGetUniformLocation(raytracer, "dims"),
		glGetUniformLocation(raytracer, "pos"),
	};

	rasterizer = loadProgram(vertCode.c_str(), fragCode.c_str());
	raster_uniforms = RasterizerUniforms{
		glGetUniformLocation(rasterizer, "mvp"),
		glGetUniformLocation(rasterizer, "drawID"),
		glGetUniformLocation(rasterizer, "offset"),
	};

	return true;
};

void Renderer::begin(glm::vec3 Pos, glm::vec3 facing, glm::vec3 up, float fovy, float aspect) {
	pos = Pos;
	v = genV(pos, facing, up);
	p = genP(fovy, aspect);
	vp = p * v;
	currdraw = 1;
	nverts = 0;
	frame = 0;

	glUseProgram(rasterizer);
	glUniformMatrix4fv(raster_uniforms.mvp, 1, GL_FALSE, &vp[0][0]);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, width, height);

	glDisable(GL_BLEND);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, transformedverts);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::drawObjectToBuffers(RenderMesh mesh) {
	glUniform1i(raster_uniforms.drawID, currdraw);
	glUniform1i(raster_uniforms.offset, nverts);

	glBindBuffer(GL_ARRAY_BUFFER, verts);
	glBufferSubData(GL_ARRAY_BUFFER, 0, mesh.verts.size() * sizeof(vec3), &mesh.verts[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, uvs);
	glBufferSubData(GL_ARRAY_BUFFER, 0, mesh.uvs.size() * sizeof(vec2), &mesh.uvs[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, normals);
	glBufferSubData(GL_ARRAY_BUFFER, 0, mesh.normals.size() * sizeof(vec3), &mesh.normals[0][0]);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh.verts.size());

	nverts += mesh.verts.size();
	currdraw += 1;
}

void Renderer::shade(vector<Material> mats, GLuint textureArray) {
	frame += 1;

	glUseProgram(raytracer);
	glUniform1i(tracer_uniforms.nobjs, nverts);
	glUniform1f(tracer_uniforms.time, glfwGetTime());
	glUniform1i(tracer_uniforms.frame, frame);
	glUniform2iv(tracer_uniforms.dims, 1, &ivec2(width, height)[0]);
	glUniform3fv(tracer_uniforms.pos, 1, &pos[0]);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, materials);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, mats.size() * sizeof(Material), &mats[0]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, materials);

	bindTexture2D(0, depthbuffer);
	bindTexture2D(1, uvbuffer);
	bindTexture2D(2, normbuffer);
	bindImageTexture(3, GL_TEXTURE_2D, rendertarget, GL_RGBA32F);
	bindTexture(4, GL_TEXTURE_2D_ARRAY, textureArray);

	glDispatchCompute(width, height, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}