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
}; // transformation struct

static void set_parent(transformation* new_parent, transformation* child_transform);
static void	create_identity(transformation* result);

static glm::mat4 get_transform_matrix(transformation* source);
static glm::mat4 get_camera_transform_matrix(transformation* source);

static glm::vec3 get_forward(glm::mat4 source);
static glm::vec3 get_right(glm::mat4 source);
static glm::vec3 get_up(glm::mat4 source);

#endif // _TRANSFORMATIONS_HPP