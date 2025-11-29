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

	mesh(const mesh&) = delete; // No copy constructor
	mesh& operator=(const mesh&) = delete; // No copy assignment

	void draw(material* mat);

	/**
	* @brief Load a mesh from raw vertex data (positions only)
	* 
	* @param raw_vertices Pointer to raw vertex data (float array of x, y, z positions)
	* @param indecies Number of vertices (not number of floats)
	*/
	void load_mesh(float* raw_vertices, size_t indecies);

private:
	inline bool isUploaded() const;

	void upload(material* mat);
}; // mesh

#endif // _MESH_HPP