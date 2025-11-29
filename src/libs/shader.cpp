#include <GLEW/glew.h>
#include <string>
#include <cstdio>

#include "scolor.hpp"
#include "shader_source.hpp"
#include "shader.hpp"

void shader::add(GLuint type, const char* filepath) {
	auto _ = shader_source(this->m_handle, type, filepath);
} // add

void shader::link() {
	// Link the program
	glLinkProgram(this->m_handle);
	glGetProgramiv(this->m_handle, GL_LINK_STATUS, &this->m_isLinked);

	if (!this->m_isLinked) {
		fprintf(stderr, RED("Link Failed\n").c_str());
		return;
	}

	printf(GREEN("Link Success\n").c_str());
} // link

void shader::use() const {
	glUseProgram(m_handle);
} // use