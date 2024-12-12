#include <glm/glm.hpp>
#include <GLEW/glew.h>
#include <glfw/glfw3.h>
#include <vector>
#include <stdexcept>

#include "scolor.hpp"
#include "stb_image.h"
#include "tiny_obj_loader.h"
#include "game_data.hpp"

/**
 * Load an obj file into the GPU
 *
 * @param baseDir The base directory of the obj file
 * @param filename The name of the obj file
 * @param vertices The vertices of the obj file
 * @param indices The indices of the obj file
 * @param texScale The scale of the texture
 *
 * @return 0 if successful, 1 if not
 */
int load_obj(const char* baseDir, const char* filename, obj_mesh &mesh) {
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

	// Load the obj file
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, baseDirStr.c_str())) {
		throw std::runtime_error(err);

		return 1;
	}

	// Load the vertices and indices
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			vertex vert = {};

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
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1] // Flip the texture (because stb_image.h flips it)
			};

			mesh.vertices.push_back(vert);
			mesh.indices.push_back(mesh.indices.size());
		}
	}

	// Print the obj file
	printf(GREEN("\nLoaded obj: %s\n").c_str(), filename);
	printf("# of vertices  = %d\n", (int)(attrib.vertices.size()) / 3);
	printf("# of normals   = %d\n", (int)(attrib.normals.size()) / 3);
	printf("# of texcoords = %d\n", (int)(attrib.texcoords.size()) / 2);
	printf("# of materials = %d\n", (int)materials.size());
	printf("# of shapes    = %d\n", (int)shapes.size());

	return 0;
}

/**
 * Load a texture into the GPU
 *
 * @param filename The name of the texture file
 * @param tex The texture object
 */
void load_texture(const char* filename, texture &tex) {
	// Set the filename
	tex.filename = filename;

	// Load the image
	tex.image_data = stbi_load(filename, &tex.width, &tex.height, 0, STBI_rgb_alpha);

	if (!tex.image_data) {
		printf(RED("Failed to load texture '%s'\n").c_str(), filename);

		return;
	}

	printf(GREEN("Loaded texture: '%s' - %d by %d\n").c_str(), filename, tex.width, tex.height);

	// Bind the texture to the GPU
	glGenTextures(1, &tex.texture_handle);
	glBindTexture(GL_TEXTURE_2D, tex.texture_handle);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.image_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_WRAP_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_WRAP_BORDER);

	free(tex.image_data); // Free the image data (we don't need it anymore)
}