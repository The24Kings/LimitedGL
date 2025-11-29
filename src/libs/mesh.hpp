#ifndef _MESH_HPP
#define _MESH_HPP

#include <glm/glm.hpp>
#include <GLEW/glew.h>
#include <cstdint>
#include <vector>

#include "material.hpp"
#include "vertex.hpp"

struct mesh {
	GLuint vao, vbo, ibo;
	std::vector<vertex> m_vertices;
	std::vector<uint32_t> m_indices;

	mesh() : m_vertices(std::vector<vertex>()), m_indices(std::vector<uint32_t>()), vao(-1), vbo(-1), ibo(-1) {}

	~mesh() {
		if (isUploaded()) {
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(1, &vbo);
			glDeleteBuffers(1, &ibo);
		}

		m_vertices.clear();
		m_indices.clear();
	}

	void draw(material* mat) {
		upload(mat);

		if (mat->m_tex) { // Not all materials have textures
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mat->m_tex->m_handle);
		}

		// Rebind the buffers
		glBindVertexArray(vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		glDrawElements(GL_TRIANGLES, (GLsizei)m_indices.size(), GL_UNSIGNED_INT, NULL);
	}

	/**
	* @brief Load a mesh from raw vertex data (positions only)
	* 
	* @param raw_vertices Pointer to raw vertex data (float array of x, y, z positions)
	* @param indecies Number of vertices (not number of floats)
	*/
	void load_mesh(float* raw_vertices, size_t indecies) {
		for (size_t index = 0; index < indecies; ++index) {
			vertex vert = {};

			vert.m_pos = glm::vec3(
				raw_vertices[index * 3 + 0],
				raw_vertices[index * 3 + 1],
				raw_vertices[index * 3 + 2]
			);

			// Default color white
			vert.m_color = glm::vec3(1.0f, 1.0f, 1.0f);

			// Default texcoord zero
			vert.m_texCoord = glm::vec2(0.0f, 0.0f);

			// Default normal up
			vert.m_normal = glm::vec3(0.0f, 1.0f, 0.0f);

			m_vertices.push_back(vert);
		}
	}

private:
	inline bool isUploaded() const {
		return vao != -1;
	}

	void upload(material* mat) {
		if (isUploaded()) {
			return;
		}

		// Generate the vertex array object
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Generate the vertex buffer object
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(vertex), m_vertices.data(), GL_STATIC_DRAW);

		// Generate the element buffer object
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(uint32_t), m_indices.data(), GL_STATIC_DRAW);

		// Set vertex attribute pointers
		for (auto& [name, loc] : mat->m_attributes) {
			if (name == vertexAttr(vertex_attr::VERTEX)) {
				glEnableVertexAttribArray(loc);
				glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, m_pos));
			}
			else if (name == vertexAttr(vertex_attr::COLOR)) {
				glEnableVertexAttribArray(loc);
				glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, m_color));
			}
			else if (name == vertexAttr(vertex_attr::TEXCOORD)) {
				glEnableVertexAttribArray(loc);
				glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, m_texCoord));
			}
			else if (name == vertexAttr(vertex_attr::NORMAL)) {
				glEnableVertexAttribArray(loc);
				glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, m_normal));
			}
			else {
				throw std::invalid_argument("Unknown vertex attribute: " + std::string(name));
			}
		}
	}
}; // mesh

#endif // _MESH_HPP