#ifndef _VERTEX_HPP
#define _VERTEX_HPP

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <GLEW/glew.h>

struct vertex {
	glm::vec3 m_pos;
	glm::vec3 m_color;
	glm::vec2 m_texCoord;
	glm::vec3 m_normal;

	vertex() : m_pos(glm::vec3(0.0f, 0.0f, 0.0f)), m_color(glm::vec3(1.0f, 1.0f, 1.0f)), m_texCoord(glm::vec2(0.0f, 0.0f)), m_normal(glm::vec3(1.0f, 1.0f, 1.0f)) {}

	bool operator==(const vertex& other) const {
		return m_pos == other.m_pos && m_color == other.m_color && m_texCoord == other.m_texCoord && m_normal == other.m_normal;
	}
}; // vertex

template<> struct std::hash<vertex> {
	size_t operator()(vertex const& v) const {
		return ((
			(std::hash<glm::vec3>()(v.m_pos) ^
				(std::hash<glm::vec3>()(v.m_color) << 1)) >> 1) ^
			(std::hash<glm::vec2>()(v.m_texCoord) << 1) >> 1) ^
			(std::hash<glm::vec3>()(v.m_normal) << 1);
	}
}; // vertex hash

#endif // _VERTEX_HPP