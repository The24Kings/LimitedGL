#ifndef _TEXTURE_HPP
#define _TEXTURE_HPP

#include <glm/glm.hpp>
#include <GLEW/glew.h>

struct texture {
	const char* m_filename;
	unsigned char* m_image_data; // (Using STB Image) Texture buffer: size = width * height * 4 (RGBA)
	GLuint m_handle;
	GLint m_width, m_height;

	texture() : m_filename(nullptr), m_image_data(nullptr), m_handle(-1), m_width(0), m_height(0) {}
}; // texture

bool load_texture(const char* filename, texture* tex);

#endif // _TEXTURE_HPP