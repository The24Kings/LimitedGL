#ifndef _TRANSFORM_COMPONENT_HPP
#define _TRANSFORM_COMPONENT_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "component_base.hpp"


struct transform_component : public component { // 128 bytes
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	//transform_component* parent; // Parent transformation for hierarchical transformations (shenanigans)
	//std::unordered_set<transform_component*> children; // Children transformations for hierarchical transformations

	transform_component() : position(0.0f, 0.0f, 0.0f), rotation(1.0f, 0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f) {}
}; // transform_component

#endif // _TRANSFORM_COMPONENT_HPP