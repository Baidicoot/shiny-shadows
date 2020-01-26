#include "GlobalVAO.h"

using namespace std;
using namespace glm;

template <size_t SIZE>
void GlobalMeshBuffer<SIZE>::init(ArrayAttrib* attribs, GLuint* lens, GLuint maxsize) {
	genVAOArrBuf(vao, SIZE, attribs, vbos);

	for (int i = 0; i < SIZE; i++) {
		lengths[i] = lens[i];
		glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
		//cout << "Data allocated (buffer " << i << "): " << lengths[i] * maxsize << " bytes" << endl;
		glBufferData(GL_ARRAY_BUFFER, lengths[i] * maxsize, NULL, GL_STATIC_DRAW);
	}

	max = maxsize;
	end = 0;
}

template <size_t SIZE>
GlobalMesh GlobalMeshBuffer<SIZE>::append(void** data, GLuint length) {
	for (int i = 0; i < SIZE; i++) {
		glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
		//cout << "Data stored (buffer " << i << "): " << lengths[i] * length << " bytes" << endl;
		glBufferSubData(GL_ARRAY_BUFFER, end * lengths[i], lengths[i] * length, data[i]);
	}

	GlobalMesh o = GlobalMesh{
		end,
		length
	};
	end += length;
	cout << end << " of " << max << endl;
	return o;
}

template <size_t SIZE>
GlobalMesh GlobalMeshBuffer<SIZE>::all() {
	return GlobalMesh{
		0,
		end
	};
}

template <size_t SIZE>
void GlobalMeshBuffer<SIZE>::beginDraws() {
	glBindVertexArray(vao);
}

template <size_t SIZE>
void GlobalMeshBuffer<SIZE>::draw(GLenum mode, GlobalMesh m) {
	glDrawArrays(mode, m.offset, m.size);
}

template <size_t SIZE>
void GlobalMeshBuffer<SIZE>::drawInstanced(GLenum mode, GlobalMesh m, GLuint instances) {
	glDrawArraysInstanced(mode, m.offset, m.size, instances);
}