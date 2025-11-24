#ifndef _COMPLILE_SHADERS_HPP
#define _COMPLILE_SHADERS_HPP

#include <GLEW/glew.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

#include "scolor.hpp"

constexpr auto BUFFER_SIZE = 1024;

/**
* @brief Read a shader from a file
 *
 * @param buffer The buffer to read the shader into
 * @param filename The name of the file to read the shader from
 *
 * @return True if the shader was read successfully, false if not
*/
static bool read_shader(std::string* buffer, const char* filename) { //TODO: change this to some kind of compiler to read #include statements in the shader
	std::ifstream input_file(filename);

	if (!input_file.is_open()) {
		printf(RED("Unable to open file!").c_str());
		return false;
	}

	std::string content, line;
	while (std::getline(input_file, line)) {
		content += line + "\n";
	}
	content.pop_back();

	input_file.close();

	printf(YELLOW("Read %zu bytes from %s\n").c_str(), (content.size() * sizeof(std::string::value_type)), filename);
	puts(content.c_str());

	return true;
} // read_shader

class shader_source {
	friend class shader;

	GLenum m_handle;
	GLuint m_type;
	const char* m_filepath;
	char m_error[BUFFER_SIZE];

	GLint m_isCompiled;

	shader_source(GLuint type, const char* filepath) : m_handle(-1), m_type(type), m_filepath(filepath), m_isCompiled(-1) {
		memset(m_error, 0, BUFFER_SIZE);
	}

	/**
	* @brief Create a shader source
	 *
	 * @param type The type of shader
	 * @param source The shader source
	 *
	 * @return The shader source
	*/
	shader_source* compile() {
		std::string source;

		if (!this->m_filepath)
			return nullptr;

		if (!read_shader(&source, this->m_filepath)) { // Read the shader source from the file
			printf(RED("Failed to read shader source from %s\n").c_str(), this->m_filepath);
			return nullptr;
		}

		// Create the shader
		unsigned int handle = glCreateShader(this->m_type);
		const char* shader_data = source.c_str();

		glShaderSource(handle, 1, &shader_data, NULL);
		glCompileShader(handle);
		glGetShaderInfoLog(handle, BUFFER_SIZE, NULL, this->m_error);

		// Check if the shader compiled successfully
		glGetShaderiv(handle, GL_COMPILE_STATUS, &this->m_isCompiled);

		if (!this->m_isCompiled) {
			// Clear up GPU memory
			printf(RED("Compile Failed: %s\n").c_str(), this->m_error);

			glDeleteShader(handle);

			return nullptr;
		}

		puts(GREEN("Compile Success\n").c_str());

		// Set the shader properties
		this->m_handle = handle;
		this->m_type = m_type;
		this->m_filepath = m_filepath;

		return this;
	}
}; // shader_source

class shader {
public:
	GLuint m_handle;
	GLint m_isLinked;

	std::vector<shader_source> m_shaders;

	shader(const char* v_file, const char* tcs_file, const char* tes_file, const char* g_file, const char* f_file) : m_handle(-1) {
		this->compile(v_file, tcs_file, tes_file, g_file, f_file);
	}

	void use() const {
		glUseProgram(m_handle);
	}

private:
	void compile(const char* v_file, const char* tcs_file, const char* tes_file, const char* g_file, const char* f_file) {
		// Compile the shaders
		if (!v_file || !f_file) { throw std::runtime_error("Vertex and Fragment shaders are required."); }

		this->m_handle = glCreateProgram();

		// Compile shaders
		shader_source* vertex_shader = shader_source(GL_VERTEX_SHADER, v_file).compile();
		shader_source* tess_control_shader = shader_source(GL_TESS_CONTROL_SHADER, tcs_file).compile();
		shader_source* tess_eval_shader = shader_source(GL_TESS_EVALUATION_SHADER, tes_file).compile();
		shader_source* geometry_shader = shader_source(GL_GEOMETRY_SHADER, g_file).compile();
		shader_source* fragment_shader = shader_source(GL_FRAGMENT_SHADER, f_file).compile();

		// Attach the shaders to the program
		if (vertex_shader) { glAttachShader(this->m_handle, vertex_shader->m_handle); }
		if (tess_control_shader) { glAttachShader(this->m_handle, tess_control_shader->m_handle); }
		if (tess_eval_shader) { glAttachShader(this->m_handle, tess_eval_shader->m_handle); }
		if (geometry_shader) { glAttachShader(this->m_handle, geometry_shader->m_handle); }
		if (fragment_shader) { glAttachShader(this->m_handle, fragment_shader->m_handle); }

		glLinkProgram(this->m_handle);
		glGetProgramiv(this->m_handle, GL_LINK_STATUS, &this->m_isLinked);

		// Mark the shaders for deletion as they are no longer needed after linking
		if (vertex_shader) { glDeleteShader(vertex_shader->m_handle); }
		if (tess_control_shader) { glDeleteShader(tess_control_shader->m_handle); }
		if (tess_eval_shader) { glDeleteShader(tess_eval_shader->m_handle); }
		if (geometry_shader) { glDeleteShader(geometry_shader->m_handle); }
		if (fragment_shader) { glDeleteShader(fragment_shader->m_handle); }

		if (!this->m_isLinked) {
			puts(RED("Link Failed").c_str());

			glDeleteProgram(this->m_handle);
			this->m_handle = -1;

			return;
		}

		puts(GREEN("Link Success").c_str());
	}
}; // shader

/*TODO
	Compile shaders and store them in a hash table, hash on the filename and the type of shader
	Create a shader program and attach the shaders to it using the hash table to prevent recompilation
	Store the shader program in a hash table with the handle and the number of sources
*/

#endif // _COMPLILE_SHADERS_HPP
