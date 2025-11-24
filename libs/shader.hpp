#ifndef _COMPLILE_SHADERS_HPP
#define _COMPLILE_SHADERS_HPP

#include <GLEW/glew.h>
#include <stdio.h>

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
static bool read_shader(char** buffer, const char* filename) { //TODO: change this to some kind of compiler to read #include statements in the shader
	FILE* file = nullptr;
	uint64_t file_size = 0;
	size_t readlen = 0;
	char* temp_buffer = nullptr;

	fopen_s(&file, filename, "r");

	if (!file) {
		fprintf(stderr, RED("Failed to open file %s\n").c_str(), filename);
		return false;
	}

	// Find the size of the file
	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	rewind(file);

	// Check if file size is over 1MB
	if (file_size > (uint64_t)(1024 * 1024)) {
		fprintf(stderr, RED("File %s is too large (Larger than 1MB)\n").c_str(), filename);
		return false;
	}

	temp_buffer = (char*)malloc(file_size);

	if (!temp_buffer) {
		fprintf(stderr, RED("Failed to allocate memory for buffer\n").c_str());
		return false;
	}

	// Read the file into the buffer
	readlen = fread(temp_buffer, 1, file_size, file);

	// Check if the file was read successfully
	if (!readlen) {
		fprintf(stderr, RED("Failed to read file %s\n").c_str(), filename);
		return false;
	}

	temp_buffer[readlen] = '\0'; // Null terminate the buffer

	printf(YELLOW("Read %zu bytes from %s\n").c_str(), readlen, filename);
	puts(temp_buffer);

	// Free up memory and set the buffer
	*buffer = temp_buffer;
	temp_buffer = NULL;

	fclose(file);

	return true;
} // read_shader

class shader_source {
	GLenum m_handle;
	GLuint m_type;
	const char* m_source;
	char m_error[BUFFER_SIZE];

	GLint m_isCompiled;

	// Allow shader access to private class
	friend class shader;

	shader_source() {
		m_handle = 0;
		m_type = 0;
		m_source = nullptr;

		m_isCompiled = 0;

		memset(m_error, 0, BUFFER_SIZE);
	}

	shader_source(GLuint type, const char* source) : m_type(type), m_source(source), m_isCompiled(-1) {
		m_handle = 0;
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
		uint64_t file_size = 0;
		char* buffer = nullptr;

		if (!this->m_source)
			return nullptr;

		if (!read_shader(&buffer, this->m_source)) { // Read the shader source from the file
			printf(RED("Failed to read shader source from %s\n").c_str(), this->m_source);
			return nullptr;
		}

		// Create the shader
		unsigned int handle = glCreateShader(this->m_type);
		glShaderSource(handle, 1, &buffer, NULL);
		glCompileShader(handle);
		glGetShaderInfoLog(handle, BUFFER_SIZE, NULL, this->m_error);

		free(buffer);

		// Check if the shader compiled successfully
		GLint isCompiled = 0;
		glGetShaderiv(handle, GL_COMPILE_STATUS, &isCompiled);

		if (!isCompiled) {
			// Clear up GPU memory
			printf(RED("Compile Failed: %s\n").c_str(), this->m_error);

			glDeleteShader(handle);

			return nullptr;
		}

		puts(GREEN("Compile Success\n").c_str());

		// Set the shader properties
		this->m_handle = handle;
		this->m_type = m_type;
		this->m_source = m_source;

		return this;
	}
}; // shader_source

class shader {
public:
	GLuint m_handle;
	GLint m_isLinked;

	std::vector<shader_source> m_shaders;

	shader(const char* v_file, const char* tcs_file, const char* tes_file, const char* g_file, const char* f_file) : m_handle(-1), m_shaders({}) {
		this->compile(v_file, tcs_file, tes_file, g_file, f_file);
	}

	void use() const {
		glUseProgram(m_handle);
	}

private:
	void compile(const char* v_file, const char* tcs_file, const char* tes_file, const char* g_file, const char* f_file) {
		// Compile the shaders
		if (!v_file || !f_file) {
			fprintf(stderr, RED("Vertex and Fragment shaders are required\n").c_str());
			return;
		}

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

		// Link the program
		glLinkProgram(this->m_handle);

		// Validate the program
		glGetProgramiv(this->m_handle, GL_LINK_STATUS, &this->m_isLinked);

		// Delete the shaders as they are no longer needed after linking
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
