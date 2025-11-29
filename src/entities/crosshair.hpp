#ifndef _CROSSHAIR_HPP
#define _CROSSHAIR_HPP

#include "object.hpp"
#include "shader.hpp"
#include "render_2d_component.hpp"

class crosshair : public object {
public:
	render_2d_component* m_render;
	bool uploaded = false;

	crosshair(shader* linked_shader) {
		m_render = (render_2d_component*)addComponent(new render_2d_component(linked_shader, nullptr));
		m_render->m_mat->m_tex = nullptr; // No texture
	}

	bool init() override {
		// Define crosshair vertices
		float vertices[] {
			-.025f, .002f,  0.0f,
			.025f, .002f,  0.0f,
			.025f,  -.002f, 0.0f,
			-.025f, .002f,  0.0f,
			.025f, -.002f, 0.0f,
			-.025f, -.002f, 0.0f,
			
			-.002f, .04f,  0.0f,
			.002f, .04f,  0.0f,
			.002f,  -.04f, 0.0f,
			-.002f, .04f,  0.0f,
			.002f, -.04f, 0.0f,
			-.002f, -.04f, 0.0f,
		};

		m_render->m_mesh->load_mesh(vertices, 12);

		// Set attribute locations
		m_render->m_mat->set_attribute("in_vertex", 0);

		return true;
	}

	void update(float dt) override {
		m_render->m_mat->use();

		if (!uploaded) {
			// Generate the vertex array object
			glGenVertexArrays(1, &m_render->m_mesh->vao);
			glBindVertexArray(m_render->m_mesh->vao);

			glGenBuffers(1, &m_render->m_mesh->vbo);
			glBindBuffer(GL_ARRAY_BUFFER, m_render->m_mesh->vbo);
			glBufferData(GL_ARRAY_BUFFER, m_render->m_mesh->m_vertices.size() * sizeof(vertex), m_render->m_mesh->m_vertices.data(), GL_STATIC_DRAW);

			for (auto& [name, loc] : m_render->m_mat->m_attributes) {
				if (name == vertexAttr(vertex_attr::VERTEX)) {
					glEnableVertexAttribArray(loc);
					glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, m_pos));
				}
			}

			uploaded = true;
		}

		glBindVertexArray(m_render->m_mesh->vbo);

		glDrawArrays(GL_TRIANGLES, 0, m_render->m_mesh->m_vertices.size());
	}
};

#endif // _CROSSHAIR_HPP