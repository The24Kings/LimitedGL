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

		this->m_transform->rotation = glm::rotate(glm::identity<glm::quat>(), glm::radians(-90.0f), glm::vec3(1, 0, 0));
	}

	void update(float dt) override {
		for (auto c : m_components) {
			c->update(dt);
		}

		m_transform->rotation = glm::angleAxis(glm::radians(m_transform->m_degrees + (dt * 10)), glm::vec3(0, 0, 1));
	}


}; // cube

#endif _EARTH_OBJ_HPP