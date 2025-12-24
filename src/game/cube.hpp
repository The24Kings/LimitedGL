#ifndef _CUBE_OBJ_HPP
#define _CUBE_OBJ_HPP

#include <GLEW/glew.h>
#include <string>
#include <stdexcept>
#include <cstdio>

#include "loaded_obj.hpp"
#include "transform_component.hpp"

/**
* @brief A loaded object from an OBJ file with a texture
*/
class cube : public loaded_obj {
public:
	transform_component* m_transform;

	/**
	* Create a new cube object
	*
	* @param linked_shader Object shader
	*/
	cube(shader* linked_shader) : loaded_obj("obj/cube.obj", "obj/textures/brick.jpg", linked_shader) {
		m_transform = (transform_component*)addComponent(new transform_component());
	}

	void update(float dt) override {
		for (auto c : m_components) {
			c->update(dt);
		}

		m_transform->rotation = glm::angleAxis(glm::radians(m_transform->m_degrees + (dt * 10)), glm::vec3(0, 1, 0));
	}
}; // cube

#endif _CUBE_OBJ_HPP