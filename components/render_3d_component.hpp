#ifndef _RENDER_3D_COMPONENT_HPP
#define _RENDER_3D_COMPONENT_HPP

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "component_base.hpp"
#include "shader.hpp"
#include "material.hpp"
#include "mesh.hpp"

class render_3d_component : public component {
public:
	material* m_mat;
	mesh* m_mesh;

	glm::mat4 model = glm::identity<glm::mat4>();
	inline static glm::mat4 vp;

	render_3d_component(shader* linked_shader, texture* linked_texture) {
		this->m_mat = new material(linked_shader, linked_texture);
		this->m_mesh = new mesh();
	}

	~render_3d_component() {
		delete m_mat;
		delete m_mesh;
	}

	void update(float dt) override {
		m_mat->set_uniform("model", model);
		m_mat->set_uniform("vp", vp);

		m_mat->set_uniform("ambient_strength", 0.2f);
		m_mat->set_uniform("light_pos", glm::vec3(2.0f, 5.0f, 5.0f));

		render();
	}

	void render() {
		m_mat->use();
		m_mesh->draw(m_mat);
	}
}; // render_component

#endif // _RENDER_3D_COMPONENT_HPP