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
	bool load() { //TODO: change this to some kind of compiler to read #include statements in the shader
		std::ifstream inputFileStream(this->m_source, std::ios::ate | std::ios::binary);
		std::stringstream bufferStream;

		if (!inputFileStream.is_open()) {
			fprintf(stderr, RED("Failed to open file %s\n").c_str(), this->m_source);
			return false; 
		}

		std::streamsize filesize = inputFileStream.tellg();
		if (filesize <= 0) {
			fprintf(stderr, RED("File %s is empty or unreadable\n").c_str(), this->m_source);
			return false;
		}

		if (filesize > (1024 * 1024)) {
			fprintf(stderr, RED("File %s is too large (Larger than 1MB)\n").c_str(), this->m_source);
			return false;
		}

		// Create buffer
		char* buffer = new char[filesize + 1];
		inputFileStream.seekg(0);
		inputFileStream.read(buffer, filesize);

		std::streamsize bytesRead = inputFileStream.gcount();
		buffer[bytesRead] = '\0';

		inputFileStream.close();

		printf(YELLOW("Read %lld bytes from %s\n").c_str(), static_cast<long long>(bytesRead), this->m_source);
		puts(buffer);

		glShaderSource(this->m_handle, 1, &buffer, NULL);

		free(buffer);

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
		if (!this->m_handle) { printf(RED("Failed to create shader handle").c_str()); }
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

		// Validate the program
		glGetProgramiv(this->m_handle, GL_LINK_STATUS, &this->m_isLinked);

		if (!this->m_isLinked) {
			puts(RED("Link Failed").c_str());

			glDeleteProgram(this->m_handle);
			this->m_handle = -1;

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
