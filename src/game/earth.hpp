#ifndef _EARTH_OBJ_HPP
#define _EARTH_OBJ_HPP

#include <GLEW/glew.h>
#include <string>
#include <stdexcept>
#include <cstdio>

#include "loaded_obj.hpp"
#include "transform_component.hpp"

/**
* @brief A loaded object from an OBJ file with a texture
*/
class earth : public loaded_obj {
public:
	transform_component* m_transform;

	/**
	* Create a new cube object
	*
	* @param linked_shader Object shader
	*/
	earth(shader* linked_shader) : loaded_obj("obj/earth.obj", "obj/textures/earth.jpg", linked_shader) {
		m_transform = (transform_component*)addComponent(new transform_component());
	}

	void update(float dt) override {
		// Accumulate rotation over time
		m_transform->m_degrees += dt * 10.0f;

		// Apply initial tilt (-90° on X) then spin around Z
		glm::quat tilt = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1, 0, 0));
		glm::quat spin = glm::angleAxis(glm::radians(m_transform->m_degrees), glm::vec3(0, 1, 0));
		m_transform->rotation = spin * tilt;

		for (auto c : m_components) {
			c->update(dt);
		}
	}


}; // cube

#endif _EARTH_OBJ_HPP