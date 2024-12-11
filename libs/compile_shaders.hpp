#pragma once
#ifndef COMPLILE_SHADERS_HPP
#define COMPLILE_SHADERS_HPP

#include <GLEW/glew.h>
#include <stdio.h>

#include "scolor.hpp"

#define BUFFER_SIZE 1024

struct shader_source {
	GLenum handle;
	GLuint type;
	const char* source;
	char error[BUFFER_SIZE];
};

struct shader_program {
	GLuint handle;
	shader_source* sources;
	size_t source_count;
};

/*
	Compile shaders and store them in a hash table, hash on the filename and the type of shader
	Create a shader program and attach the shaders to it using the hash table to prevent recompilation
	Store the shader program in a hash table with the handle and the number of sources
*/

static void read_shader(char* buffer, const char* filename) { //TODO: change this to some kind of compiler to read #include statements in the shader
	// Read in the shader source from a file
	FILE* file;
	size_t readlen;

	fopen_s(&file, filename, "r");

	if (file == NULL) {
		fprintf(stderr, RED("Failed to open file %s\n").c_str(), filename);
		return;
	}

	readlen = fread(buffer, sizeof(char), BUFFER_SIZE, file);

	if (readlen == 0) {
		fprintf(stderr, RED("Failed to read file %s\n").c_str(), filename);
		return;
	}

	if (readlen == BUFFER_SIZE) {
		fprintf(stderr, RED("Buffer too small for file %s\n").c_str(), filename);
		return;
	}

	// Null terminate the buffer
	buffer[readlen] = 0;

	// Print the shader source
	printf("\nRead in shader source from %s (%zu bytes):\n", filename, readlen);
	puts(buffer);

	fclose(file);
}

static shader_source* create_shader_source(GLuint type, const char* source) {
	shader_source* shader = (shader_source*)malloc(sizeof(shader_source)); // Allocate memory for the shader
	char* buffer = (char*)malloc(BUFFER_SIZE); // Allocate memory for the shader source

	if (shader == NULL) {
		fprintf(stderr, RED("Failed to allocate memory for shader_source\n").c_str());
		return NULL;
	}

	if (buffer == NULL) {
		fprintf(stderr, RED("Failed to allocate memory for buffer\n").c_str());
		return NULL;
	}

	read_shader(buffer, source); // Read the shader source from the file

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

	shader->handle = handle;
	shader->type = type;
	shader->source = source;

	return shader;
}

static shader_program* create_shader_program(shader_source* sources, size_t source_count) {
	shader_program* program = (shader_program*)malloc(sizeof(shader_program)); // Allocate memory for the shader program

	if (program == NULL) {
		fprintf(stderr, RED("Failed to allocate memory for shader_program\n").c_str());
		return NULL;
	}

	unsigned int program_handle = glCreateProgram(); // Create the shader program

	// Attach the shaders to the program
	for (size_t i = 0; i < source_count; i++) {
		shader_source* source = &sources[i];
		glAttachShader(program_handle, source->handle);
	}

	glLinkProgram(program_handle);

	// Validate the program
	GLint isLinked = 0;
	glGetProgramiv(program_handle, GL_LINK_STATUS, &isLinked);

	if (isLinked) { puts(GREEN("Link Success").c_str()); }
	else { puts(RED("Link Failed").c_str()); return nullptr; } // Return null if the program failed to link

	program->handle = program_handle;
	program->sources = sources;
	program->source_count = source_count;

	return program;
}

#endif // COMPLILE_SHADERS_HPP
