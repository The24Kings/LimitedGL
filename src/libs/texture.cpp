#include <stb_image.h>
#include <cstdio>

#include "scolor.hpp"
#include "texture.hpp"

bool load_texture(const char* filename, texture* tex) {
	stbi_set_flip_vertically_on_load(true); // Flip the texture vertically on load

	tex->m_filename = filename;
	tex->m_image_data = stbi_load(filename, &tex->m_width, &tex->m_height, 0, STBI_rgb_alpha);

	if (!tex->m_image_data) {
		printf(RED("Failed to load texture '%s'\n").c_str(), tex->m_filename);
		return false;
	}

	printf(BLUE("Loaded texture: '%s' - %d by %d\n").c_str(), tex->m_filename, tex->m_width, tex->m_height);

	//TODO: Generate MIPMAPS

	// Bind texture to GPU
	glGenTextures(1, &tex->m_handle);
	glBindTexture(GL_TEXTURE_2D, tex->m_handle);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->m_width, tex->m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->m_image_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_WRAP_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_WRAP_BORDER);

	stbi_image_free(tex->m_image_data);

	printf(GREEN("Successfully Generated Texture: %d\n").c_str(), tex->m_handle);

	return true;
} // load_texture