#ifndef _TRANSFORM_COMPONENT_HPP
#define _TRANSFORM_COMPONENT_HPP

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "component_base.hpp"

struct transform_component : public component { // 128 bytes
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	glm::vec3 localFront = glm::vec3(0.0f);
	glm::vec3 localRight = glm::vec3(0.0f);
	glm::vec3 localUp = glm::vec3(0.0f);

	float m_degrees = 0.0f;

	transform_component() : position(0.0f), rotation(glm::identity<glm::quat>()), scale(1.0f) {}

	void update(float dt) override;

	/**
	* @brief Get the position matrix (local)
	*/
	inline glm::mat4 getPositionMatrix() const {
		return glm::translate(glm::mat4(1.0f), -position); // Note the negative sign for moving the world opposite to the camera position
	}

	inline glm::mat4 getRotationMatrix() const {
		return glm::mat4_cast(rotation);
	}

	inline glm::mat4 getScaleMatrix() const {
		return glm::scale(glm::mat4(1.0f), scale);
	}

	inline glm::mat4 getModelMatrix() const {
		return glm::translate(glm::mat4(1.0f), position) * getRotationMatrix() * getScaleMatrix();
	}

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