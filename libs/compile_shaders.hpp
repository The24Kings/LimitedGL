#ifndef _COMPLILE_SHADERS_HPP
#define _COMPLILE_SHADERS_HPP

#include <GLEW/glew.h>
#include <stdio.h>

#include "scolor.hpp"

constexpr auto BUFFER_SIZE = 1024;

struct shader_source {
	GLenum handle;
	GLuint type;
	const char* source;
	char error[BUFFER_SIZE];
}; // shader_source

struct shader_program {
	GLuint handle;
	shader_source* sources[5]; // 5 is the max number of shaders in a program; vertex, tessellation control, tessellation evaluation, geometry, fragment

	shader_program() {
		handle = 0;

		for (size_t i = 0; i < 5; i++) {
			sources[i] = 0;
		}
	}
}; // shader_program

/*
	Compile shaders and store them in a hash table, hash on the filename and the type of shader
	Create a shader program and attach the shaders to it using the hash table to prevent recompilation
	Store the shader program in a hash table with the handle and the number of sources
*/

/**
* @brief Read a shader from a file
 *
 * @param buffer The buffer to read the shader into
 * @param filename The name of the file to read the shader from
 *
 * @return True if the shader was read successfully, false if not
*/
static bool read_shader(char** buffer, const char* filename) { //TODO: change this to some kind of compiler to read #include statements in the shader
	// Read in the shader source from a file
	FILE* file = NULL;
	uint64_t file_size = 0;
	size_t readlen = 0;
	char* temp_buffer = NULL;

	fopen_s(&file, filename, "r");

	if (file == NULL) {
		fprintf(stderr, RED("Failed to open file %s\n").c_str(), filename);
		return false;
	}

	// Allocate memory for the buffer
	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	rewind(file);

	// Check if file size is over 1MB
	if (file_size > (uint64_t)(1024 * 1024)) {
		fprintf(stderr, RED("File %s is too large (Larger than 1MB)\n").c_str(), filename);
		return false;
	}

	temp_buffer = (char*)malloc(file_size);

	if (temp_buffer == NULL) {
		fprintf(stderr, RED("Failed to allocate memory for buffer\n").c_str());
		return false;
	}

	// Read the file into the buffer
	readlen = fread(temp_buffer, 1, file_size, file);

	// Check if the file was read successfully
	if (readlen == 0) {
		fprintf(stderr, RED("Failed to read file %s\n").c_str(), filename);
		return false;
	}

	temp_buffer[readlen] = '\0'; // Null terminate the buffer

	printf("Read %zu bytes from %s\n", readlen, filename);
	puts(temp_buffer);

	*buffer = temp_buffer; // Set the buffer

	fclose(file);

	return true;
} // read_shader

/**
* @brief Create a shader source
 *
 * @param type The type of shader
 * @param source The shader source
 *
 * @return The shader source
*/
static shader_source* create_shader_source(GLuint type, const char* source) {
	shader_source* shader = new shader_source(); // Allocate memory for the shader
	uint64_t file_size = 0;
	char* buffer = NULL;

	if (source == NULL) {
		return nullptr;
	}

	if (shader == NULL) {
		fprintf(stderr, RED("Failed to allocate memory for shader_source\n").c_str());
		return nullptr;
	}

	if (!read_shader(&buffer, source)) { // Read the shader source from the file
		printf(RED("Failed to read shader source from %s\n").c_str(), source);
		return nullptr;
	}

	// Create the shader
	unsigned int handle = glCreateShader(type);
	glShaderSource(handle, 1, &buffer, NULL);
	glCompileShader(handle);
	glGetShaderInfoLog(handle, BUFFER_SIZE, NULL, shader->error);

	// Check if the shader compiled successfully
	GLint success = 0;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

	if (success) { puts(GREEN("Compile Success").c_str()); }
	else { puts(RED("Compile Failed").c_str()); return nullptr; } // Return null if the shader failed to compile

	// Set the shader properties
	shader->handle = handle;
	shader->type = type;
	shader->source = source;

	return shader;
} // create_shader_source

/**
* @brief Create a shader program
 *
 * @param v_file The vertex shader file
 * @param tcs_file The tessellation control shader file
 * @param tes_file The tessellation evaluation shader file
 * @param g_file The geometry shader file
 * @param f_file The fragment shader file
 *
 * @return The shader program
*/
static shader_program* create_shader_program(const char* v_file, const char* tcs_file, const char* tes_file, const char* g_file, const char* f_file) {
	// Compile the shaders
	if (!v_file || !f_file) {
		fprintf(stderr, RED("Vertex and Fragment shaders are required\n").c_str());
		return nullptr;
	}

	shader_source* vertex_shader = create_shader_source(GL_VERTEX_SHADER, v_file);
	shader_source* tess_control_shader = create_shader_source(GL_TESS_CONTROL_SHADER, tcs_file);
	shader_source* tess_eval_shader = create_shader_source(GL_TESS_EVALUATION_SHADER, tes_file);
	shader_source* geometry_shader = create_shader_source(GL_GEOMETRY_SHADER, g_file);
	shader_source* fragment_shader = create_shader_source(GL_FRAGMENT_SHADER, f_file);

	// Create the shader program
    shader_program* program = new shader_program(); // Allocate memory for the shader program
    GLuint program_handle = glCreateProgram(); // Create the shader program

    // Attach the shaders to the program
	if (vertex_shader) { glAttachShader(program_handle, vertex_shader->handle); }
	if (tess_control_shader) { glAttachShader(program_handle, tess_control_shader->handle); }
	if (tess_eval_shader) { glAttachShader(program_handle, tess_eval_shader->handle); }
	if (geometry_shader) { glAttachShader(program_handle, geometry_shader->handle); }
	if (fragment_shader) { glAttachShader(program_handle, fragment_shader->handle); }

	// Link the program
	glLinkProgram(program_handle); 

    // Validate the program
    GLint isLinked = 0;
    glGetProgramiv(program_handle, GL_LINK_STATUS, &isLinked);

    if (isLinked) { puts(GREEN("Link Success").c_str()); }
    else { puts(RED("Link Failed").c_str()); return nullptr; } // Return null if the program failed to link

    program->handle = program_handle;

    return program;
} // create_shader_program

#endif // _COMPLILE_SHADERS_HPP
