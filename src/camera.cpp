#include "camera.hpp"

inline glm::mat4 camera::getCameraRotation() const {
	return glm::mat4_cast(glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f)));
}

inline glm::mat4 camera::getViewMatrix() const {
	return getCameraRotation() * m_transform->getPositionMatrix();
}

inline glm::mat4 camera::getWorldMatrix() const {
	return m_transform->getPositionMatrix() * getCameraRotation();
}

void camera::pointCamera(float xpos, float ypos, float center_x, float center_y, float deltaTime, bool constrainPitch) {
	// Move the camera based on mouse movement
	float dx = xpos - center_x;
	float dy = ypos - center_y;

	xpos *= dx * mouseSensitivity * deltaTime;
	ypos *= dy * mouseSensitivity * deltaTime;
	yaw += xpos;
	pitch += ypos;

	if (constrainPitch) {
		if (pitch > 89.9f)
			pitch = 89.9f;
		if (pitch < -89.9f)
			pitch = -89.9f;
	}
}

void camera::update(float dt) {
	for (auto c : m_components) {
		c->update(dt);
	}

	const glm::mat4 transposed = glm::transpose(getCameraRotation());
	m_transform->localFront = -transposed[2];
	m_transform->localRight = transposed[0];
	m_transform->localUp = transposed[1];
}