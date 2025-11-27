#ifndef _TRANSFORM_COMPONENT_HPP
#define _TRANSFORM_COMPONENT_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "component_base.hpp"

struct transform_component : public component { // 128 bytes
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	glm::vec3 localFront = glm::identity<glm::vec3>();
	glm::vec3 localRight = glm::identity<glm::vec3>();
	glm::vec3 localUp = glm::identity<glm::vec3>();

	float m_degrees = 0.0f;

	glm::mat4 model = glm::identity<glm::mat4>();

	transform_component() : position(0.0f), rotation(glm::identity<glm::quat>()), scale(1.0f) {}

	void update(float dt) override;

	/**
	* @brief Get the position matrix (local)
	*/
	inline glm::mat4 getPositionMatrix() const;

	void moveForward(float speed, float deltaTime) {
		position += localFront * speed * deltaTime;
	}

	void moveBackward(float speed, float deltaTime) {
		position -= localFront * speed * deltaTime;
	}

	void moveLeft(float speed, float deltaTime) {
		position -= localRight * speed * deltaTime;
	}

	void moveRight(float speed, float deltaTime) {
		position += localRight * speed * deltaTime;
	}

	void moveUp(float speed, float deltaTime) {
		position += glm::vec3(0.0f, 1.0f, 0.0f) * speed * deltaTime;
	}

	void moveDown(float speed, float deltaTime) {
		position -= glm::vec3(0.0f, 1.0f, 0.0f) * speed * deltaTime;
	}
}; // transform_component

#endif // _TRANSFORM_COMPONENT_HPP