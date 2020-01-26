#include "Helper.h"

using namespace std;
using namespace glm;

GLuint genQuadVBO() {
	GLuint quadID;
	glGenVertexArrays(1, &quadID);
	glBindVertexArray(quadID);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

	return buffer;
}

int genVAOArrBuf(GLuint& vaoID, int nBuf, ArrayAttrib* attribs, GLuint* buffers) {
	GLint max_bufs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_bufs);

	if (nBuf > max_bufs) {
		cout << "Too many vertex buffers!" << endl;
		return -1;
	}

	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	glGenBuffers(nBuf, buffers);

	for (int i = 0; i < nBuf; i++) {
		glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(attribs[i].index, attribs[i].size, attribs[i].type, attribs[i].normalised, attribs[i].stride, (void*)attribs[i].offset);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

GLuint genFBO() {
	GLuint fbo = 0;
	glGenFramebuffers(1, &fbo);

	return fbo;
}

GLuint genTexRGBA(GLuint width, GLuint height) {
	return genTexRGBAFiltered(width, height, GL_LINEAR, GL_LINEAR);
}

GLuint genTexRGBAFiltered(GLuint width, GLuint height, GLenum filterMag, GLenum filterMin) {
	GLuint tex;
	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMag);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

void bindTexture(GLuint unit, GLenum type, GLuint texture) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(type, texture);
}

void bindTexture2D(GLuint unit, GLuint texture) {
	bindTexture(unit, GL_TEXTURE_2D, texture);
}

void bindImageTexture(GLuint unit, GLenum type, GLuint texture, GLenum format) {
	glBindImageTexture(unit, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, format);
	bindTexture(unit, type, texture);
}

GLuint loadCompute(const char* source) {
	GLuint shaderID = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(shaderID, 1, &source, NULL);
	glCompileShader(shaderID);

	int logLen;
	GLint result = GL_FALSE;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLen);

	if (logLen > 0) {
		vector<char> errmsg(logLen + 1);
		glGetShaderInfoLog(shaderID, logLen, NULL, &errmsg[0]);
		printf("%s\n", &errmsg[0]);
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, shaderID);
	glLinkProgram(program);

	return program;
}

GLuint loadTransform(const char* vert, int nvaryings, const char** varyings, GLenum format) {
	GLuint vertID = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertID, 1, &vert, NULL);
	glCompileShader(vertID);

	int logLen;
	GLint result = GL_FALSE;

	glGetShaderiv(vertID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertID, GL_INFO_LOG_LENGTH, &logLen);

	if (logLen > 0) {
		vector<char> errmsg(logLen + 1);
		glGetShaderInfoLog(vertID, logLen, NULL, &errmsg[0]);
		printf("%s\n", &errmsg[0]);
	}

	GLuint prgID = glCreateProgram();
	glAttachShader(prgID, vertID);

	glTransformFeedbackVaryings(prgID, nvaryings, varyings, format);

	glLinkProgram(prgID);

	glGetProgramiv(prgID, GL_LINK_STATUS, &result);
	glGetProgramiv(prgID, GL_INFO_LOG_LENGTH, &logLen);
	if (logLen > 0) {
		std::vector<char> ProgramErrorMessage(logLen + 1);
		glGetProgramInfoLog(prgID, logLen, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(prgID, vertID);
	glDeleteShader(vertID);

	return prgID;
}

GLuint loadProgram(const char* vert, const char* frag) {
	GLuint vertID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragID = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertID, 1, &vert, NULL);
	glCompileShader(vertID);

	int logLen;
	GLint result = GL_FALSE;

	glGetShaderiv(vertID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertID, GL_INFO_LOG_LENGTH, &logLen);

	if (logLen > 0) {
		vector<char> errmsg(logLen + 1);
		glGetShaderInfoLog(vertID, logLen, NULL, &errmsg[0]);
		printf("%s\n", &errmsg[0]);
	}

	glShaderSource(fragID, 1, &frag, NULL);
	glCompileShader(fragID);

	glGetShaderiv(fragID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragID, GL_INFO_LOG_LENGTH, &logLen);

	if (logLen > 0) {
		vector<char> errmsg(logLen + 1);
		glGetShaderInfoLog(fragID, logLen, NULL, &errmsg[0]);
		printf("%s\n", &errmsg[0]);
	}

	GLuint prgID = glCreateProgram();
	glAttachShader(prgID, vertID);
	glAttachShader(prgID, fragID);
	glLinkProgram(prgID);

	glGetProgramiv(prgID, GL_LINK_STATUS, &result);
	glGetProgramiv(prgID, GL_INFO_LOG_LENGTH, &logLen);
	if (logLen > 0) {
		std::vector<char> ProgramErrorMessage(logLen + 1);
		glGetProgramInfoLog(prgID, logLen, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(prgID, vertID);
	glDetachShader(prgID, fragID);

	glDeleteShader(vertID);
	glDeleteShader(fragID);

	return prgID;
}

int genRGBADepthFBO(GLuint width, GLuint height, GLuint& fboID, int ntex, GLuint* textures) {
	glGenFramebuffers(1, &fboID);
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);

	GLuint depthbuffer;
	glGenRenderbuffers(1, &depthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

	if (ntex > 16) {
		cout << "Too many color attachments used!" << endl;
		return -1;
	}

	vector<GLenum> attachments;

	for (int i = 0; i < ntex; i++) {
		textures[i] = genTexRGBA(width, height);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, textures[i], 0);

		attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
	}

	glDrawBuffers(ntex, &attachments[0]);

	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		cout << "Framebuffer not complete! status: " << fboStatus << endl;
		return fboStatus;
	}

	return 0;
}

GLuint loadTex(void* data, int width, int height, GLenum internalformat, GLenum format) {
	glActiveTexture(GL_TEXTURE0);

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		internalformat,
		width,
		height,
		0,
		format,
		GL_UNSIGNED_BYTE,
		data
	);

	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	
	return tex;
}

GLuint loadSpriteSheet(void* data, int width, int height, int depth, GLenum internalformat, GLenum format) {
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
	glTexImage3D(
		GL_TEXTURE_2D_ARRAY,
		0,
		internalformat,
		width,
		height,
		depth,
		0,
		format,
		GL_UNSIGNED_BYTE,
		data
	);

	glTexParameteri(GL_TEXTURE_2D_ARRAY,
		GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,
		GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return tex;
}