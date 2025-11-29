#ifndef _UNIFORM_HPP
#define _UNIFORM_HPP

#include <glm/glm.hpp>
#include <GLEW/glew.h>
#include <variant>

using uniform_value = std::variant<
	int,
	float,
	glm::vec3,
	glm::mat3,
	glm::mat4
>;

struct uniform_data {
	GLuint location;
	uniform_value value;

	uniform_data() : location(-1) {}
	uniform_data(GLuint loc, uniform_value v) : location(loc), value(v) {}
};

#endif // _UNIFORM_HPP