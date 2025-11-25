#ifndef _RENDER_COMPONENT_HPP
#define _RENDER_COMPONENT_HPP

#include "component_base.hpp"
#include "shader.hpp"
#include "material.hpp"
#include "mesh.hpp"

class render_component : public component {
public:
	material* m_mat;
	mesh* m_mesh;

	render_component(shader* linked_shader) {
		this->m_mat = new material(linked_shader);
		this->m_mesh = new mesh();
	}

	~render_component() {
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

#endif // _RENDER_COMPONENT_HPP