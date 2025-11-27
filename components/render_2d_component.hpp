#ifndef _RENDER_2D_COMPONENT_HPP
#define _RENDER_2D_COMPONENT_HPP

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "component_base.hpp"
#include "shader.hpp"
#include "material.hpp"
#include "mesh.hpp"

class render_2d_component : public component {
public:
	material* m_mat;
	mesh* m_mesh;

	render_2d_component(shader* linked_shader, texture* linked_texture) {
		this->m_mat = new material(linked_shader, linked_texture);
		this->m_mesh = new mesh();
	}

	~render_2d_component() {
		delete m_mat;
		delete m_mesh;
	}

	void update(float dt) override {
		render();
	}

	void render() {
		m_mat->use();
		m_mesh->draw(m_mat);
	}
}; // render_component

#endif // _RENDER_2D_COMPONENT_HPP