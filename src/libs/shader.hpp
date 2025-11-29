#ifndef _COMPLILE_SHADERS_HPP
#define _COMPLILE_SHADERS_HPP

#include <GLEW/glew.h>
#include <stdexcept>
#include <vector>
#include <string>
#include <cstdio>

#include "scolor.hpp"
#include "shader_source.hpp"

class shader {
public:
	GLuint m_handle;
	GLint m_isLinked;

	std::vector<shader_source> m_shaders;

	/**
	* @brief Buider style shader compiler
	*/
	shader() : m_handle(glCreateProgram()), m_isLinked(GL_FALSE) {
		printf(BLUE("Constructing shader\n").c_str());
		if (!this->m_handle) { throw std::runtime_error("Failed to create shader handle"); }
	}

	~shader() {
		glDeleteProgram(this->m_handle);
	}

	shader(shader&) = delete; // No copy constructor
	shader& operator=(const shader&) = delete; // No copy assignment

	/**
	* @brief Attach a shader source to the shader program
	*/
	void add(GLuint type, const char* filepath);

	/**
	* @brief Link the shader to the GPU
	*/
	void link();

	/**
	* @brief
	*/
	void use() const;
}; // shader

#endif // _COMPLILE_SHADERS_HPP
