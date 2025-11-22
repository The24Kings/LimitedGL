#ifndef _CAMERA_HPP
#define _CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "transformations.hpp"

struct frustum {
	float near_plane, far_plane;
	float fovDegrees;


	frustum(float fov, float np, float fp) : fovDegrees(fov), near_plane(np), far_plane(fp) {}

	void cameraZoom(float yoffset) {
		fovDegrees -= yoffset;

		if (fovDegrees < 1.0f)
			fovDegrees = 1.0f;
		if (fovDegrees > 45.0f)
			fovDegrees = 45.0f;
	}
}; // frustum

struct camera {
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraRight;
	glm::vec3 cameraUp;

	float pitch;
	float yaw;

	float movementSpeed = 2.5f;
	float mouseSensitivity = 0.05f;

	camera(glm::vec3 pos = glm::vec3(0, 0, 0), float pitch = 0.0, float yaw = -90.0) : cameraPos(pos), pitch(pitch), yaw(yaw) {
		update();
	}

	inline glm::mat4 getViewMatrix() const {
		return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}

	void cameraMoveForward(float deltaTime) {
		cameraPos += cameraFront * movementSpeed * deltaTime;
	}

	void cameraMoveBackward(float deltaTime) {
		cameraPos -= cameraFront * movementSpeed * deltaTime;
	}

	void cameraMoveLeft(float deltaTime) {
		cameraPos -= cameraRight * movementSpeed * deltaTime;
	}

	void cameraMoveRight(float deltaTime) {
		cameraPos += cameraRight * movementSpeed * deltaTime;
	}

	void cameraMoveUp(float deltaTime) {
		cameraPos += glm::vec3(0, 1, 0) * movementSpeed * deltaTime;
	}

	void cameraMoveDown(float deltaTime) {
		cameraPos -= glm::vec3(0, 1, 0) * movementSpeed * deltaTime;
	}

	void cameraMouseMovement(float xpos, float ypos, float center_x, float center_y, float deltaTime, bool constrainPitch = true) {
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

		update();
	}

	void update() {
		glm::vec3 front = glm::vec3(0, 0, 0);

		front.x = cosf(glm::radians(yaw)) * cosf(glm::radians(-pitch));
		front.y = sinf(glm::radians(-pitch));
		front.z = sinf(glm::radians(yaw)) * cosf(glm::radians(-pitch));
		cameraFront = glm::normalize(front);

		cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0, 1, 0)));
		cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
	}
}; // camera

#endif // _CAMERA_HPP