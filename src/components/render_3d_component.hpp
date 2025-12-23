#ifndef _RENDER_3D_COMPONENT_HPP
#define _RENDER_3D_COMPONENT_HPP

#include <glm/glm.hpp>

#include "component_base.hpp"
#include "shader.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "texture.hpp"

class render_3d_component : public component {
public:
	material* m_mat;
	mesh* m_mesh;

	inline static glm::mat4 vp;
	inline static glm::vec3 lightPos;
	inline static glm::vec3 cameraPos;

	render_3d_component(shader* linked_shader, texture* linked_texture) {
		this->m_mat = new material(linked_shader, linked_texture);
		this->m_mesh = new mesh();
	}

	~render_3d_component() {
		delete m_mat;
		delete m_mesh;
	}

	void update(float dt) override {
		m_mat->set_uniform("vp", vp);

		m_mat->set_uniform("ambient_strength", 0.2f);
		m_mat->set_uniform("specular_strength", 0.5f);
		m_mat->set_uniform("light_pos", lightPos); //glm::vec3(2.0f, 25.0f, 25.0f)
		m_mat->set_uniform("view_pos", cameraPos);

		render();
	}

	void render() {
		m_mat->use();
		m_mesh->draw(m_mat);
	}
}; // render_component

#endif // _RENDER_3D_COMPONENT_HPP