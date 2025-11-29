#ifndef _MATERIAL_HPP
#define _MATERIAL_HPP

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLEW/glew.h>
#include <unordered_map>
#include <string>

#include "shader.hpp"
#include "uniform.hpp"
#include "texture.hpp"

struct material {
	shader* m_shader;
	texture* m_tex;

	std::unordered_map<std::string_view, GLuint> m_attributes;
	std::unordered_map<std::string_view, uniform_data> uniforms;

	material(shader* linked_shader, texture* linked_texture) : m_shader(linked_shader), m_tex(linked_texture) {}

	~material() {
		uniforms.clear();
	}

	/**
	* @brief Set material attribute location
	*/
	void set_attribute(std::string_view name, GLuint location) {
		m_attributes[name] = location;
	}

	/**
	* @brief Set material uniform of given type
	*/
	template<typename T>
	void set_uniform(std::string_view name, const T &data) {
		auto it = uniforms.find(name);

		if (it == uniforms.end()) {
			GLuint loc = glGetUniformLocation(m_shader->m_handle, name.data());
			it = uniforms.emplace(name, uniform_data(loc, data)).first;
		}

		// Overwrite value
		it->second.value = data;
	}

	/**
	* @brief Use the shader and set all uniforms
	*/
	void use() {
		m_shader->use();

		for (auto & [name, loc] : m_attributes) {
			glEnableVertexAttribArray(loc);
		}

		for (auto& [name, u] : uniforms) {
			std::visit([&](auto&& v) {
				using T = std::decay_t<decltype(v)>;

				if constexpr (std::is_same_v<T, int>)
					glUniform1i(u.location, v);
				else if constexpr (std::is_same_v<T, float>)
					glUniform1f(u.location, v);
				else if constexpr (std::is_same_v<T, glm::vec3>)
					glUniform3fv(u.location, 1, glm::value_ptr(v));
				else if constexpr (std::is_same_v<T, glm::mat3>)
					glUniformMatrix3fv(u.location, 1, GL_FALSE, glm::value_ptr(v));
				else if constexpr (std::is_same_v<T, glm::mat4>)
					glUniformMatrix4fv(u.location, 1, GL_FALSE, glm::value_ptr(v));
				else
					throw std::invalid_argument("Unsupported uniform type");

				}, u.value);
		}
	}
}; // material

#endif // _MATERIAL_HPP