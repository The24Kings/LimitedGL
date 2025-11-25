#ifndef _MATERIAL_HPP
#define _MATERIAL_HPP

#include <glm/glm.hpp>
#include <GLEW/glew.h>
#include <unordered_map>
#include <string>
#include <type_traits>

#include "shader.hpp"
#include "texture.hpp"

struct material {
	shader* m_shader;
	texture* m_tex;
	GLuint v_attr, t_attr, c_attr, n_attr; // vertex attribute, texture attribute, color attribute, normals attribute

	std::unordered_map<std::string, uniform_data> uniforms;

	material(shader* linked_shader) : m_shader(linked_shader), m_tex(nullptr) {
		v_attr = glGetAttribLocation(linked_shader->m_handle, "in_vertex");
		c_attr = glGetAttribLocation(linked_shader->m_handle, "in_color");
		t_attr = glGetAttribLocation(linked_shader->m_handle, "in_texCoord");
		n_attr = glGetAttribLocation(linked_shader->m_handle, "in_normal");
	}

	~material() {
		uniforms.clear();
	}

	/**
	* @brief Set material uniform of given type
	*/
	template<typename T>
	bool set_uniform(std::string name, T data) {
		const uniform_type type = get_type<T>();

		// Check if exists, create empty if it doesn't
		if (uniforms.find(name) == uniforms.end()) {
			uniforms[name] = uniform_data(type, glGetUniformLocation(m_shader->m_handle, name.c_str()));
		}

		// Get uniform from list
		uniform_data* uniform = &uniforms[name];

		// Check if the type matches data
		if (type != uniform->m_type) {
			return false;
		}

		// Cast buffer into T type
		*(T*)uniform->m_buffer = data;

		return true;
	}

	/**
	* @brief Translate T type into uniform_d_type
	*/
	template<typename T>
	uniform_type get_type() {
		if constexpr (std::is_same<T, float>())
			return uniform_type::t_float;
		if constexpr (std::is_same<T, glm::mat4>())
			return uniform_type::t_mat4;
		if constexpr (std::is_same<T, int>())
			return uniform_type::t_int;
		if constexpr (std::is_same<T, glm::mat3>())
			return uniform_type::t_mat3;
		if constexpr (std::is_same<T, glm::vec3>())
			return uniform_type::t_vec3;
		return uniform_type::t_none;
	}

	/**
	* @brief Apply the uniform to the shader
	*/
	void use() {
		m_shader->use();

		for (auto& u : uniforms) {

			auto* uniform_data = &u.second;

			switch (uniform_data->m_type) {
			case uniform_type::t_float:
				glUniform1f(uniform_data->m_location, *(GLfloat*)uniform_data->m_buffer);
				break;
			case uniform_type::t_int:
				glUniform1i(uniform_data->m_location, *(GLint*)uniform_data->m_buffer);
				break;
			case uniform_type::t_vec3:
				glUniform3fv(uniform_data->m_location, 1, (float*)uniform_data->m_buffer);
				break;
			case uniform_type::t_mat3:
				glUniformMatrix3fv(uniform_data->m_location, 1, GL_FALSE, (float*)uniform_data->m_buffer);
				break;
			case uniform_type::t_mat4:
				glUniformMatrix4fv(uniform_data->m_location, 1, GL_FALSE, (float*)uniform_data->m_buffer);
				break;
			default:
				break;
			};
		}
	}
}; // material

#endif // _MATERIAL_HPP