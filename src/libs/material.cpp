#include <glm/glm.hpp>
#include <GLEW/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <string_view>
#include <type_traits>
#include <stdexcept>

#include "material.hpp"

void material::set_attribute(std::string_view name) {
	this->m_attributes[name] = glGetAttribLocation(this->m_shader->m_handle, name.data());
}

void material::use() {
	this->m_shader->use();

	for (auto& [name, loc] : m_attributes) {
		glEnableVertexAttribArray(loc);
	}

	for (auto& [name, u] : this->uniforms) {
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