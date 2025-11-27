#include <glm/glm.hpp>
#include <string>
#include <tiny_obj_loader.h>

#include "scolor.hpp"
#include "vertex.hpp"

#include "object.hpp"

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
				vert.m_normal = glm::vec3(0.0f, 0.0f, 0.0f); //TODO: Calculate normals
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