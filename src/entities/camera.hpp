#ifndef _CAMERA_HPP
#define _CAMERA_HPP

#include <glm/glm.hpp>

#include "object.hpp"
#include "transform_component.hpp"

struct frustum {
	float near_plane, far_plane;
	float fovDegrees;

	frustum(float fov, float np, float fp) : fovDegrees(fov), near_plane(np), far_plane(fp) {}

	void cameraZoom(float yoffset) {
		float scroll_sensitivity = 20000.0f;
		fovDegrees -= yoffset / scroll_sensitivity;

		if (fovDegrees < 1.0f)
			fovDegrees = 1.0f;
		if (fovDegrees > 65.0f)
			fovDegrees = 65.0f;
	}
}; // frustum

struct camera : public object {
	float pitch;
	float yaw;

	float mouseSensitivity = 0.000005f;

	transform_component* m_transform;

	camera(float pitch = 0.0f, float yaw = 0.0f) : pitch(pitch), yaw(yaw) {
		m_transform = (transform_component*)addComponent(new transform_component());
	}

	void update(float dt) override {
		for (auto c : m_components) {
			c->update(dt);
		}

		const glm::mat4 transposed = glm::transpose(getCameraRotation());
		m_transform->localFront = -transposed[2];
		m_transform->localRight = transposed[0];
		m_transform->localUp = transposed[1];
	}

	/**
	* @brief Get the camera rotation matrix
	*/
	inline glm::mat4 getCameraRotation() const {
		return glm::mat4_cast(glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f)));
	}

	/**
	* @brief Get the camera view matrix
	*/
	inline glm::mat4 getViewMatrix() const {
		return getCameraRotation() * m_transform->getPositionMatrix();
	}

	/**
	* @brief Get the camera world matrix
	*/
	inline glm::mat4 getWorldMatrix() const {
		return m_transform->getPositionMatrix() * getCameraRotation();
	}

	/**
	* @brief Point the camera using the mouse position
	*/
	void pointCamera(float xpos, float ypos, float center_x, float center_y, float deltaTime, bool constrainPitch = true) {
		// Move the camera based on mouse movement
		float dx = (xpos * mouseSensitivity) -center_x;
		float dy = (ypos * mouseSensitivity) -center_y;

		xpos *= dx * deltaTime;
		ypos *= dy * deltaTime;
		yaw += xpos;
		pitch += ypos;

		if (constrainPitch) {
			if (pitch > 89.9f)
				pitch = 89.9f;
			if (pitch < -89.9f)
				pitch = -89.9f;
		}
	}
}; // camera

#endif // _CAMERA_HPP