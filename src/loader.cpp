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
 * @param mesh The mesh object to load the obj data into
 *
 * @return bool True if the obj file was loaded successfully; throw runtime_error on failure
 */
void load_obj(std::string baseDir, std::string filename, obj_mesh* mesh) {
	tinyobj::attrib_t v_attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(&v_attrib, &shapes, &materials, &err, filename.c_str(), baseDir.c_str())) {
		throw std::runtime_error(err);
	}

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			vertex vert = {};

			vert.pos = {
				v_attrib.vertices[3 * index.vertex_index + 0],
				v_attrib.vertices[3 * index.vertex_index + 1],
				v_attrib.vertices[3 * index.vertex_index + 2]
			};

			vert.color = { 1.0f, 1.0f, 1.0f };

			vert.texCoord = {
				v_attrib.texcoords[2 * index.texcoord_index + 0],
				v_attrib.texcoords[2 * index.texcoord_index + 1]
			};

			mesh->vertices.push_back(vert);
			mesh->indices.push_back(mesh->indices.size());
		}
	}

	printf(GREEN("\nLoaded obj: %s\n").c_str(), filename);
	printf("# of vertices  = %d\n", (int)(v_attrib.vertices.size()) / 3);
	printf("# of normals   = %d\n", (int)(v_attrib.normals.size()) / 3);
	printf("# of texcoords = %d\n", (int)(v_attrib.texcoords.size()) / 2);
	printf("# of materials = %d\n", (int)materials.size());
	printf("# of shapes    = %d\n", (int)shapes.size());
} // load_obj

/**
 * Load a texture into the GPU
 *
 * @param filename The name of the texture file
 * @param tex The texture object
 */
bool load_texture(const char* filename, texture* tex) {
	stbi_set_flip_vertically_on_load(true); // Flip the texture vertically on load

	tex->filename = filename;
	tex->image_data = stbi_load(filename, &tex->width, &tex->height, 0, STBI_rgb_alpha);

	if (!tex->image_data) {
		printf(RED("Failed to load texture '%s'\n").c_str(), filename);

		return false;
	}

	printf(GREEN("Loaded texture: '%s' - %d by %d\n").c_str(), filename, tex->width, tex->height);

	// Bind texture to GPU
	glGenTextures(1, &tex->texture_handle);
	glBindTexture(GL_TEXTURE_2D, tex->texture_handle);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->image_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_WRAP_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_WRAP_BORDER);

	stbi_image_free(tex->image_data);

	return true;
} // load_texture