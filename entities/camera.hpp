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
		fovDegrees -= yoffset;

		if (fovDegrees < 1.0f)
			fovDegrees = 1.0f;
		if (fovDegrees > 65.0f)
			fovDegrees = 65.0f;
	}
}; // frustum

struct camera : public object {
	float pitch;
	float yaw;

	float movementSpeed = 2.5f;
	float mouseSensitivity = 0.05f;

	transform_component* m_transform;

	camera(float pitch = 0.0f, float yaw = 0.0f) : pitch(pitch), yaw(yaw) {
		m_transform = (transform_component*)addComponent(new transform_component());
	}

	void update(float dt) override;

	/**
	* @brief Get the camera rotation matrix
	*/
	inline glm::mat4 getCameraRotation() const;

	/**
	* @brief Get the camera view matrix
	*/
	inline glm::mat4 getViewMatrix() const;

	/**
	* @brief Get the camera world matrix
	*/
	inline glm::mat4 getWorldMatrix() const;

	/**
	* @brief Point the camera using the mouse position
	*/
	void pointCamera(float xpos, float ypos, float center_x, float center_y, float deltaTime, bool constrainPitch = true);
}; // camera

#endif // _CAMERA_HPP