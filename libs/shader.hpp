#ifndef _COMPLILE_SHADERS_HPP
#define _COMPLILE_SHADERS_HPP

#include <glm/glm.hpp>
#include <GLEW/glew.h>
#include <variant>
#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include <iostream>

#include "scolor.hpp"

constexpr auto BUFFER_SIZE = 1024;

class shader_source {
public:
	GLuint m_handle;
	GLuint m_type;
	const char* m_source;
	char m_error[BUFFER_SIZE];

	GLint m_isCompiled;
	
	//TODO: Add the shader vertex attributes here rather than in the material
	//TODO: change this to some kind of compiler to read #include statements in the shader (i.e. https://github.com/HorseTrain/antworld/blob/main/antworld/assets/shaders/stl/shared_shader_data.h)

	// Allow shader access to private class
	friend class shader;

	/**
	* @brief Construct and compile the provided shader
	*/
	shader_source(GLuint shader_handle, GLuint type, const char* source) : m_handle(-1), m_type(type), m_source(source), m_isCompiled(GL_FALSE) {
		memset(m_error, 0, BUFFER_SIZE);

		this->compile(shader_handle);
	}

	~shader_source() {
		if (this->m_handle != -1) { glDeleteShader(this->m_handle); }
	}

private:
	/**
	* @brief Load shader source from filepath
	* 
	* @returns bool Returns true if loading was successful, false on failure
	*/
	bool load() { 
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

	/**
	* @brief Create a shader source
	*
	* @return The shader source, nullptr on fail
	*/
	void compile(GLuint shader_handle) {
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
}; // compile

class shader {
public:
	GLuint m_handle;
	GLint m_isLinked;

	std::vector<shader_source> m_shaders;

	/**
	* @brief Buider style shader compiler
	*/
	shader() : m_handle(glCreateProgram()), m_isLinked(GL_FALSE) {
		if (!this->m_handle) { throw std::runtime_error("Failed to create shader handle"); }
	}

	~shader() {
		glDeleteProgram(this->m_handle);
	}

	/**
	* @brief Attach a shader source to the shader program
	*/
	void add(GLuint type, const char* filepath) {
		auto _ = shader_source(this->m_handle, type, filepath);
	} // add

	/**
	* @brief Link the shader to the GPU
	*/
	void link() {
		// Link the program
		glLinkProgram(this->m_handle);
		glGetProgramiv(this->m_handle, GL_LINK_STATUS, &this->m_isLinked);

		if (!this->m_isLinked) {
			puts(RED("Link Failed").c_str());
			return;
		}

		puts(GREEN("Link Success").c_str());
	} // link

	/**
	* @brief
	*/
	void use() const {
		glUseProgram(m_handle);
	} // use
}; // shader

#endif // _COMPLILE_SHADERS_HPP
