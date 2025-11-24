#include <glm/glm.hpp>
#include <GLEW/glew.h>
#include <glfw/glfw3.h>
#include <vector>
#include <stdexcept>

#include "scolor.hpp"
#include "stb_image.h"
#include "tiny_obj_loader.h"
#include "object.hpp"

/**
 * Load an obj file into a mesh object
 *
 * @param baseDir The base directory of the obj file
 * @param filename The name of the obj file
 * @param mesh The mesh object to load the obj data into
 *
 * @return bool True if the obj file was loaded successfully; throw runtime_error on failure
 */
bool load_obj(const char* baseDir, const char* filename, mesh* mesh) {
	tinyobj::attrib_t v_attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(&v_attrib, &shapes, &materials, &err, filename, baseDir)) {
		return false;
	}

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			vertex vert = {};

			// Position
			vert.m_pos = {
				v_attrib.vertices[3 * index.vertex_index + 0],
				v_attrib.vertices[3 * index.vertex_index + 1],
				v_attrib.vertices[3 * index.vertex_index + 2]
			};

			// Color (default to white)
			vert.m_color = { 1.0f, 1.0f, 1.0f };

			// Texture Coordinates
			vert.m_texCoord = {
				v_attrib.texcoords[2 * index.texcoord_index + 0],
				v_attrib.texcoords[2 * index.texcoord_index + 1]
			};

			// Normals
			if (index.normal_index >= 0) {
				vert.m_normal = {
					v_attrib.normals[3 * index.normal_index + 0],
					v_attrib.normals[3 * index.normal_index + 1],
					v_attrib.normals[3 * index.normal_index + 2]
				};
			}
			else {
				vert.m_normal = glm::vec3(0.0f, 0.0f, 0.0f); // No normal data
			}

			mesh->m_vertices.push_back(vert);
			mesh->m_indices.push_back(mesh->m_indices.size());
		}
	}

	printf(GREEN("\nSuccessfully Loaded obj: %s\n").c_str(), filename);
	printf("# of vertices  = %d\n", (int)(v_attrib.vertices.size()) / 3);
	printf("# of normals   = %d\n", (int)(v_attrib.normals.size()) / 3);
	printf("# of texcoords = %d\n", (int)(v_attrib.texcoords.size()) / 2);
	printf("# of materials = %d\n", (int)materials.size());
	printf("# of shapes    = %d\n", (int)shapes.size());

	return true;
} // load_obj

/**
 * Load a texture into the GPU
 *
 * @param filename The name of the texture file
 * @param tex The texture object
 */
bool load_texture(const char* filename, texture* tex) {
	stbi_set_flip_vertically_on_load(true); // Flip the texture vertically on load

	tex->m_filename = filename;
	tex->m_image_data = stbi_load(filename, &tex->m_width, &tex->m_height, 0, STBI_rgb_alpha);

	if (!tex->m_image_data) {
		printf(RED("Failed to load texture '%s'\n").c_str(), tex->m_filename);
		return false;
	}

	printf(BLUE("Loaded texture: '%s' - %d by %d\n").c_str(), tex->m_filename, tex->m_width, tex->m_height);

	// Bind texture to GPU
	glGenTextures(1, &tex->m_handle);
	glBindTexture(GL_TEXTURE_2D, tex->m_handle);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->m_width, tex->m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->m_image_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_WRAP_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_WRAP_BORDER);

	stbi_image_free(tex->m_image_data);

	printf(GREEN("Successfully Generated Texture: %d\n").c_str(), tex->m_handle);

	return true;
} // load_texture