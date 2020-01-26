#include "Object.h"
using namespace std;
using namespace glm;

vec3 getNormal(vec3 v0, vec3 v1, vec3 v2) {
	vec3 e0 = v1 - v0;
	vec3 e1 = v2 - v0;

	return cross(e0, e1);
}

VertexData loadVertexData(string str, bool* parsed) {
	vector<vec3> vertexes;
	vector<vec3> normals;
	vector<vec2> uvs;

	stringstream source_stream(str);
	string token;
	string head;
	while (getline(source_stream, token)) {
		stringstream line(token);
		getline(line, head, ' '); //discard trailing ' '
		if (head == "v") {
			string f, s, t;
			
			getline(line, f, ' ');
			getline(line, s, ' ');
			getline(line, t, ' ');

			float x, y, z;

			try {
				x = stof(f);
				y = stof(s);
				z = stof(t);
			}
			catch (...) {
				printf("Could not parse .obj");
				*parsed = false;
				return VertexData{};
			}

			vertexes.push_back(vec3(x, y, z));
		}
		else if (head == "vt") {
			string f, s;

			getline(line, f, ' ');
			getline(line, s, ' ');

			float x, y;

			try {
				x = stof(f);
				y = stof(s);
			}
			catch (...) {
				printf("Could not parse .obj");
				*parsed = false;
				return VertexData{};
			}

			uvs.push_back(vec2(x, y));
		}
		else if (head == "vn") {
			string f, s, t;

			getline(line, f, ' ');
			getline(line, s, ' ');
			getline(line, t, ' ');

			float x, y, z;

			try {
				x = stof(f);
				y = stof(s);
				z = stof(t);
			}
			catch (...) {
				printf("Could not parse .obj");
				*parsed = false;
				return VertexData{};
			}

			normals.push_back(vec3(x, y, z));
		}
	}

	*parsed = true;
	return VertexData{ vertexes, uvs, normals };
}

Mesh loadObject(string str, bool* parsed) {
	vector<ivec3> vertexes;
	vector<ivec3> uvs;
	vector<ivec3> normals;

	stringstream source_stream(str);
	string token;
	string head;
	while (getline(source_stream, token)) {
		stringstream line(token);
		getline(line, head, ' '); //discard trailing ' '
		if (head == "f") {
			vector<int> vert;
			vector<int> uv;
			vector<int> norm;

			for (int i = 0; i < 3; i++) {
				string tok;
				getline(line, tok, ' ');

				stringstream corner(tok);
				string v, u, n;
				getline(corner, v, '/');
				getline(corner, u, '/');
				getline(corner, n, ' ');

				try {
					vert.push_back(stoi(v) - 1);
					uv.push_back(stoi(u) - 1);
					norm.push_back(stoi(n) - 1);
				}
				catch (...) {
					printf("Could not parse .obj");
					*parsed = false;
					return Mesh{};
				}
			}

			try {
				vertexes.push_back(ivec3(vert[0], vert[1], vert[2]));
				uvs.push_back(ivec3(uv[0], uv[1], uv[2]));
				normals.push_back(ivec3(norm[0], norm[1], norm[2]));
			}
			catch (...) {
				printf("Could not parse .obj");
				*parsed = false;
				return Mesh{};
			}
		}
	}

	*parsed = true;
	return Mesh{ vertexes, uvs, normals };
}

Scene loadScene(string str) {
	bool parsed = true;
	VertexData data = loadVertexData(str, &parsed);

	vector<Mesh> meshes;
	vector<string> names;
	vector<string> mesh_sources;

	string token;
	string delim("\no ");
	int pos = str.find(delim);
	bool write = false;
	while (pos != string::npos) {
		stringstream stream(str);
		string name;
		getline(stream, name);

		token = str.substr(0, pos);
		str.erase(0, pos + delim.length());
		pos = str.find(delim);
		if (write) {
			names.push_back(name);
			cout << name << endl;
			mesh_sources.push_back(token);
		}
		else {
			write = true;
		}
	}
	mesh_sources.push_back(str);
	stringstream stream(str);
	string name;
	getline(stream, name);
	names.push_back(name);
	cout << name << endl;

	Mesh m;
	for (int i = 0; i < mesh_sources.size(); i++) {
		m = loadObject(mesh_sources[i], &parsed);
		if (parsed) {
			meshes.push_back(m);
		}
	}

	return Scene{ meshes, names, data };
}

int Scene::indexByName(const char* name) {
	int i;
	for (i = 0; i < objectNames.size(); i++) {
		if (strcmp(name, objectNames[i].c_str()) == 0) {
			return i;
		}
	}
	cout << "could not find object: " << name << endl;
	return -1;
}

RenderMesh Scene::genRenderMesh(int id) {
	RenderMesh tmp = RenderMesh{
		vector<vec3>(),
		vector<vec2>(),
		vector<vec3>()
	};
	genDataArrays(vertexes, objects[id], tmp.verts, tmp.uvs, tmp.normals);
	return tmp;
}

void genDataArrays(VertexData verts, Mesh a, vector<vec3>& vertexArray, vector<vec2>& uvArray, vector<vec3>& normalArray) {
	for (int i = 0; i < a.faces.size(); i++) {
		vertexArray.push_back(verts.vertexes[a.faces[i][0]]);
		vertexArray.push_back(verts.vertexes[a.faces[i][1]]);
		vertexArray.push_back(verts.vertexes[a.faces[i][2]]);
	}
	for (int i = 0; i < a.uvmaps.size(); i++) {
		uvArray.push_back(verts.uvs[a.uvmaps[i][0]]);
		uvArray.push_back(verts.uvs[a.uvmaps[i][1]]);
		uvArray.push_back(verts.uvs[a.uvmaps[i][2]]);
	}
	for (int i = 0; i < a.normalmaps.size(); i++) {
		normalArray.push_back(verts.normals[a.normalmaps[i][0]]);
		normalArray.push_back(verts.normals[a.normalmaps[i][1]]);
		normalArray.push_back(verts.normals[a.normalmaps[i][2]]);
	}
}