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

	camera(glm::vec3 pos = glm::vec3(0.0f), float pitch = 0.0f, float yaw = -90.0f) : cameraPos(pos), pitch(pitch), yaw(yaw) {
		update();
	}
	
	/**
	* @brief Get the camera view matrix
	*/
	inline glm::mat4 getCameraViewMatrix() const {
		return getCameraRotation() * getCameraPositionMatrix();
	}

	/**
	* @brief Get the camera world matrix
	*/
	inline glm::mat4 getCameraWorldMatrix() const {
		return getCameraPositionMatrix() * getCameraRotation();
	}

	/**
	* @brief Get the camera position matrix (local)
	*/
	inline glm::mat4 getCameraPositionMatrix() const {
		return glm::translate(glm::mat4(1.0f), -cameraPos); // Note the negative sign for moving the world opposite to the camera position
	}

	inline glm::mat4 getCameraRotation() const {
		return glm::mat4_cast(glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f)));
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
		cameraPos += glm::vec3(0.0f, 1.0f, 0.0f) * movementSpeed * deltaTime;
	}

	void cameraMoveDown(float deltaTime) {
		cameraPos -= glm::vec3(0.0f, 1.0f, 0.0f) * movementSpeed * deltaTime;
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
		const glm::mat4 transposed = glm::transpose(getCameraRotation());
		cameraFront = -transposed[2];
		cameraRight = transposed[0];
		cameraUp = transposed[1];
	}
}; // camera

#endif // _CAMERA_HPP