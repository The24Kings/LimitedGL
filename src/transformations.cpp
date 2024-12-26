#include "transformations.hpp"

/**
* @brief Set the parent of a transformation
* 
* @param new_parent The new parent transformation
* @param child_transform The child transformation
*/
void set_parent(transformation* new_parent, transformation* child_transform) {
	if (child_transform->parent != NULL) {
		child_transform->parent->children.erase(child_transform);
	}

	child_transform->parent = new_parent;

	if (new_parent != NULL) {
		new_parent->children.insert(child_transform);
	}
}

/**
* @brief Create an identity transformation
* 
* @param result The transformation to set to the identity
*/
void create_identity(transformation* result) {
	result->position = glm::vec3(0.0f, 0.0f, 0.0f);
	result->rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	result->scale = glm::vec3(1.0f, 1.0f, 1.0f);

	result->parent = NULL;
	result->children = std::unordered_set<transformation*>();
}

/**
* @brief Get the transformation matrix of a transformation
* 
* @param source The transformation to get the matrix of
* 
* @return The transformation matrix
*/
glm::mat4 get_transform_matrix(transformation* source) {
	glm::mat4 result = glm::mat4(1.0f);

	if (source->parent != NULL) {
		result = get_transform_matrix(source->parent); // Recursively get the parent's transformation matrix (shenanigans) :3
	}

	result = glm::translate(result, source->position);
	result = result * glm::mat4_cast(source->rotation);
	result = glm::scale(result, source->scale);

	return result;
}

/**
* @brief Get the forward vector of a transformation matrix
* 
* @param source The transformation matrix to get the forward vector of
* 
* @return The local forward vector
*/
glm::vec3 get_forward(glm::mat4 source) {
	return glm::vec3(source[0][2], source[1][2], source[2][2]);
}

/**
* @brief Get the right vector of a transformation matrix
* 
* @param source The transformation matrix to get the right vector of
* 
* @return The local right vector
*/
glm::vec3 get_right(glm::mat4 source) {
	return glm::vec3(source[0][0], source[1][0], source[2][0]);
}

/**
* @brief Get the up vector of a transformation matrix
* 
* @param source The transformation matrix to get the up vector of
* 
* @return The local up vector
*/
glm::vec3 get_up(glm::mat4 source) {
	return glm::vec3(source[0][1], source[1][1], source[2][1]);
}