#ifndef _MATERIAL_HPP
#define _MATERIAL_HPP

#include <GLEW/glew.h>
#include <unordered_map>
#include <string_view>

#include "shader.hpp"
#include "uniform.hpp"
#include "texture.hpp"

struct material {
	shader* m_shader;
	texture* m_tex;

	std::unordered_map<std::string_view, GLuint> m_attributes;
	std::unordered_map<std::string_view, uniform_data> uniforms;

	material(shader* linked_shader, texture* linked_texture) : m_shader(linked_shader), m_tex(linked_texture) {}

	/**
	* @brief Set material attribute location
	*/
	void set_attribute(std::string_view name);

	/**
	* @brief Set material uniform of given type
	*/
	template<typename T>
	void set_uniform(std::string_view name, const T& data) {
		auto it = this->uniforms.find(name);

		if (it == this->uniforms.end()) {
			GLuint loc = glGetUniformLocation(this->m_shader->m_handle, name.data());
			it = this->uniforms.emplace(name, uniform_data(loc, data)).first;
		}

		// Overwrite value
		it->second.value = data;
	}

	/**
	* @brief Use the shader and set all uniforms
	*/
	void use();
}; // material

#endif // _MATERIAL_HPP