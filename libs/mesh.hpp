#ifndef _MESH_HPP
#define _MESH_HPP

#include <glm/glm.hpp>
#include <GLEW/glew.h>

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

		// Set the vertex attribute pointers
		glEnableVertexAttribArray(mat->v_attr);
		glVertexAttribPointer(mat->v_attr, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, m_pos));

		// Set the color attribute pointers
		glEnableVertexAttribArray(mat->c_attr);
		glVertexAttribPointer(mat->c_attr, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, m_color));

		// Set the texture attribute pointers
		glEnableVertexAttribArray(mat->t_attr);
		glVertexAttribPointer(mat->t_attr, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, m_texCoord));

		// Set the normal attribute pointers
		glEnableVertexAttribArray(mat->n_attr);
		glVertexAttribPointer(mat->n_attr, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, m_normal));
	}

	void draw(material* mat) {
		upload(mat);

		// Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mat->m_tex->m_handle);

		// Rebind the buffers
		glBindVertexArray(vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		glDrawElements(GL_TRIANGLES, (GLsizei)m_indices.size(), GL_UNSIGNED_INT, NULL);
	}
}; // mesh

#endif // _MESH_HPP