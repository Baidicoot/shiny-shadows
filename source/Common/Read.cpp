#include "Read.h"
using namespace std;

string read(const char* path) {
	std::ifstream t(path);
	std::string str((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
	return str;
}

vector<char> getBytes(const char* path) {
	std::ifstream file(path);
	if (file) {
		file.seekg(0, std::ios::end);
		std::streampos length = file.tellg();
		file.seekg(0, std::ios::beg);

		std::vector<char> buffer(length);
		file.read(&buffer[0], length);

		return buffer;
	}
	vector<char> nothing;
	return nothing;
}