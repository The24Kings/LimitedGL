#include <GLEW/glew.h>
#include <iterator>
#include <fstream>
#include <string>
#include <cstdio>

#include "scolor.hpp"
#include "shader_source.hpp"

bool shader_source::load() {
	std::ifstream input_file(this->m_source, std::ios::binary);
	if (!input_file.is_open()) {
		fprintf(stderr, RED("Failed to open file %s\n").c_str(), this->m_source);
		return false;
	}

	// Read file into std::string
	std::string s_data((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
	input_file.close();

	if (s_data.empty()) {
		fprintf(stderr, RED("File %s is empty or unreadable: %lld bytes\n").c_str(), this->m_source, s_data.size());
		return false;
	}

	if (s_data.size() > (1024 * 1024)) {
		fprintf(stderr, RED("File %s is too large (Larger than 1MB): %lld bytes\n").c_str(), this->m_source, s_data.size());
		return false;
	}

	printf(YELLOW("Read %zu bytes from %s\n").c_str(), s_data.size(), this->m_source);

	// Pass to OpenGL
	const char* buffer = s_data.c_str();
	glShaderSource(this->m_handle, 1, &buffer, NULL);

	return true;
} // load

void shader_source::compile(GLuint shader_handle) {
	if (!this->m_source)
		return;

	this->m_handle = glCreateShader(this->m_type);

	if (!load()) {
		fprintf(stderr, RED("Failed to read shader source from %s\n").c_str(), this->m_source);
		return;
	}

	// Compile
	glCompileShader(this->m_handle);
	glGetShaderInfoLog(this->m_handle, BUFFER_SIZE, NULL, this->m_error);
	glGetShaderiv(this->m_handle, GL_COMPILE_STATUS, &this->m_isCompiled);

	if (!this->m_isCompiled) {
		fprintf(stderr, RED("Compile Failed: %s\n").c_str(), this->m_error);
		return;
	}

	fprintf(stdout, GREEN("Compile Success\n").c_str());

	glAttachShader(shader_handle, this->m_handle);
}