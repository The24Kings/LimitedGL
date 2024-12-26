#ifndef _TRANSFORMATIONS_HPP
#define _TRANSFORMATIONS_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <unordered_set>

#include "game_data.hpp"

struct transformation; // Forward declaration

struct transformation { // 128 bytes
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	transformation* parent; // Parent transformation for hierarchical transformations (shenanigans)
	std::unordered_set<transformation*> children; // Children transformations for hierarchical transformations

	transformation() : position(0.0f, 0.0f, 0.0f), rotation(1.0f, 0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), parent(nullptr), children() {}
}; // transformation

void set_parent(transformation* new_parent, transformation* child_transform);
void create_identity(transformation* result);

glm::mat4 get_transform_matrix(transformation* source);
glm::mat4 get_camera_transform_matrix(transformation* source);

glm::vec3 get_forward(glm::mat4 source);
glm::vec3 get_right(glm::mat4 source);
glm::vec3 get_up(glm::mat4 source);

#endif // _TRANSFORMATIONS_HPP