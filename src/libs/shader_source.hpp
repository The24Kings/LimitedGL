#ifndef _SHADER_SOURCE_HPP
#define _SHADER_SOURCE_HPP

#include <GLEW/glew.h>
#include <cstring>

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

	shader_source(shader_source&) = delete; // No copy constructor
	shader_source& operator=(const shader_source&) = delete; // No copy assignment

private:
	/**
	* @brief Load shader source from filepath
	*
	* @returns bool Returns true if loading was successful, false on failure
	*/
	bool load();

	/**
	* @brief Create a shader source
	*
	* @return The shader source, nullptr on fail
	*/
	void compile(GLuint shader_handle);
}; // shader_source

#endif // _SHADER_SOURCE_HPP