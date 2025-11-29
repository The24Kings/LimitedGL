#ifndef _BASE_OBJECTS_HPP
#define _BASE_OBJECTS_HPP

#include <GLEW/glew.h>
#include <string>
#include <stdexcept>
#include <cstdio>

#include "scolor.hpp"
#include "object.hpp"
#include "vertex.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "render_3d_component.hpp"
#include "transform_component.hpp"

/**
* @brief A loaded object from an OBJ file with a texture
*/
class loaded_obj : public object {
public:
	std::string texture_file;
	std::string object_file;
	std::string objBaseDir;

	render_3d_component* m_render;
	transform_component* m_transform;

	/**
	* Create a new loaded_obj object
	*
	* @param of The object file
	* @param tf The texture file
	* @param linked_shader Object shader
	*/
	loaded_obj(std::string of, std::string tf, shader* linked_shader) : object_file(of), texture_file(tf), objBaseDir(object_file.substr(0, object_file.find('/'))) {
		if (!linked_shader->m_isLinked) { throw std::invalid_argument("You must link the shader before using it"); }

		m_render = (render_3d_component*)addComponent(new render_3d_component(linked_shader, new texture()));
		m_transform = (transform_component*)addComponent(new transform_component());
	}

	void update(float dt) override {
		for (auto c : m_components) {
			c->update(dt);
		}

		m_transform->rotation = glm::angleAxis(glm::radians(m_transform->m_degrees + (dt * 10)), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	bool init() override {
		// Load OBJ file
		if (!load_obj(objBaseDir.c_str(), object_file.c_str(), m_render->m_mesh)) {
			printf(RED("Failed to load obj file\n").c_str());
			return false;
		}

		// Load and Bind textures
		if (!load_texture(texture_file.c_str(), m_render->m_mat->m_tex)) {
			printf(RED("Failed to load texture: %s\n").c_str(), texture_file);
			return false;
		}

		// Set attribute locations
		GLuint s_handle = m_render->m_mat->m_shader->m_handle;

		m_render->m_mat->set_attribute(vertexAttr(vertex_attr::VERTEX), glGetAttribLocation(s_handle, vertexAttr(vertex_attr::VERTEX)));
		m_render->m_mat->set_attribute(vertexAttr(vertex_attr::COLOR), glGetAttribLocation(s_handle, vertexAttr(vertex_attr::COLOR)));
		m_render->m_mat->set_attribute(vertexAttr(vertex_attr::TEXCOORD), glGetAttribLocation(s_handle, vertexAttr(vertex_attr::TEXCOORD)));
		m_render->m_mat->set_attribute(vertexAttr(vertex_attr::NORMAL), glGetAttribLocation(s_handle, vertexAttr(vertex_attr::NORMAL)));

		return true;
	} // init

	void deinit() override {
		// Cleanup texture
		if (m_render->m_mat->m_tex) {
			delete m_render->m_mat->m_tex;
		}
	} // deinit
}; // loaded_obj

#endif // _BASE_OBJECTS_HPP
