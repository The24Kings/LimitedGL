#ifndef _COMPLILE_SHADERS_HPP
#define _COMPLILE_SHADERS_HPP

#include <glm/glm.hpp>
#include <GLEW/glew.h>
#include <stdio.h>
#include <variant>

#include "scolor.hpp"

constexpr auto BUFFER_SIZE = 1024;

class shader_source {
public:
	GLuint m_handle;
	GLuint m_type;
	const char* m_source;
	char* m_buffer;
	char m_error[BUFFER_SIZE];

	GLint m_isCompiled;

	// Allow shader access to private class
	friend class shader;

	/**
	* @brief Construct and compile the provided shader
	*/
	shader_source(GLuint shader_handle, GLuint type, const char* source) : m_handle(-1), m_type(type), m_source(source), m_isCompiled(GL_FALSE) {
		m_buffer = nullptr;
		memset(m_error, 0, BUFFER_SIZE);

		this->compile(shader_handle);
	}

	~shader_source() {
		if (this->m_isCompiled) { glDeleteShader(this->m_handle); }
	}

private:
	/**
	* @brief Load shader source from filepath
	* 
	* @returns bool Returns true if loading was successful, false on failure
	*/
	bool load() { //TODO: change this to some kind of compiler to read #include statements in the shader
		FILE* file = nullptr;
		uint64_t file_size = 0;
		size_t readlen = 0;

		fopen_s(&file, this->m_source, "r");

		if (!file) {
			fprintf(stderr, RED("Failed to open file %s\n").c_str(), this->m_source);
			return false;
		}

		// Find the size of the file
		fseek(file, 0, SEEK_END);
		file_size = ftell(file);
		rewind(file);

		// Check if file size is over 1MB
		if (file_size > (uint64_t)(1024 * 1024)) {
			fprintf(stderr, RED("File %s is too large (Larger than 1MB)\n").c_str(), this->m_source);
			return false;
		}

		this->m_buffer = (char*)malloc(file_size);

		if (!this->m_buffer) {
			fprintf(stderr, RED("Failed to allocate memory for buffer\n").c_str());
			return false;
		}

		// Read the file into the buffer
		readlen = fread(this->m_buffer, 1, file_size, file);

		// Check if the file was read successfully
		if (!readlen) {
			fprintf(stderr, RED("Failed to read file %s\n").c_str(), this->m_source);
			return false;
		}

		this->m_buffer[readlen] = '\0'; // Null terminate the buffer

		printf(YELLOW("Read %zu bytes from %s\n").c_str(), readlen, this->m_source);
		puts(this->m_buffer);

		fclose(file);

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

		if (!load()) {
			printf(RED("Failed to read shader source from %s\n").c_str(), this->m_source);
			return;
		}

		// Create the shader
		this->m_handle = glCreateShader(this->m_type);

		glShaderSource(this->m_handle, 1, &this->m_buffer, NULL);
		glCompileShader(this->m_handle);
		glGetShaderInfoLog(this->m_handle, BUFFER_SIZE, NULL, this->m_error);

		// Check if the shader compiled successfully
		glGetShaderiv(this->m_handle, GL_COMPILE_STATUS, &this->m_isCompiled);

		if (!this->m_isCompiled) {
			printf(RED("Compile Failed: %s\n").c_str(), this->m_error);
			return;
		}

		puts(GREEN("Compile Success\n").c_str());

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
