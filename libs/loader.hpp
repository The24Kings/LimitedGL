#ifndef _LOADER_HPP
#define _LOADER_HPP

#include <glm/glm.hpp>
#include <GLEW/glew.h>
#include <glfw/glfw3.h>
#include <vector>
#include <stdexcept>

#include "stb_image.h"
#include "tiny_obj_loader.h"
#include "game_data.hpp"

int load_obj(const char* baseDir, const char* filename, std::vector<vertex> vertices, std::vector<uint32_t> indices, glm::vec2& texScale) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	// Correct the base directory
	std::string baseDirStr = baseDir;

	if (baseDirStr.empty()) {
		baseDir = ".";
	}

#ifdef _WIN32
	baseDirStr += "\\";
#else
	baseDirStr += "/";
#endif

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, baseDirStr.c_str())) {
		throw std::runtime_error(warn + err);

		return 1;
	}

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			vertex vert;

			vert.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vert.color = {
				1.0f,
				1.0f,
				1.0f
			};

			vert.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0] * texScale.x,
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1] * texScale.y // Flip the texture (because stb_image.h flips it)
			};

			vertices.push_back(vert);
			indices.push_back(indices.size());
		}
	}

	printf("# of vertices  = %d\n", (int)(attrib.vertices.size()) / 3);
	printf("# of normals   = %d\n", (int)(attrib.normals.size()) / 3);
	printf("# of texcoords = %d\n", (int)(attrib.texcoords.size()) / 2);
	printf("# of materials = %d\n", (int)materials.size());
	printf("# of shapes    = %d\n", (int)shapes.size());

	// Append `default` material
	materials.push_back(tinyobj::material_t());

	// Test for diffuse textures (Will remove later)
	for (size_t i = 0; i < materials.size(); i++) {
		printf("material[%d].diffuse_texname = %s\n", int(i),
			materials[i].diffuse_texname.c_str());
	}

	return 0;
}

// This loads straight into the GPU
unsigned load_texture(const char* filename) {
	unsigned texture = NULL;
	int texWidth, texHeight;

	// Load the image
	unsigned char* image_data = stbi_load(filename, &texWidth, &texHeight, 0, STBI_rgb_alpha);

	if (!image_data) {
		printf("Failed to load texture %s\n", filename);

		return texture;
	}

	printf("Loaded texture %d by %d\n", texWidth, texHeight);

	// Bind the texture to the GPU
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	free(image_data);

	return texture;
}

#endif // _LOADER_HPP