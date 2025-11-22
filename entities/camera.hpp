#ifndef _CAMERA_HPP
#define _CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "transformations.hpp"

struct frustum {
	float fov; // Degrees
	float near_plane, far_plane;

	frustum(float fov, float np, float fp) : fov(glm::radians(fov)), near_plane(np), far_plane(fp) {}
}; // frustum

struct camera {
	frustum* camera_frustum;

	glm::vec3 cameraPos;
	glm::vec3 cameraTarget;
	glm::quat cameraRotation;

	glm::vec3 cameraDirection;
	glm::vec3 cameraRight;
	glm::vec3 cameraUp;

	camera(glm::vec3 pos, glm::vec3 target, frustum* frustum) : 
		cameraPos(pos), 
		cameraTarget(target), 
		camera_frustum(frustum), 
		cameraDirection(glm::vec3(0, 0, 0)), 
		cameraRotation(glm::identity<glm::quat>()) 
	{
		update();
	}

	void update() {
		glm::quat x_rotation = glm::angleAxis(cameraTarget.x, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat y_rotation = glm::angleAxis(cameraTarget.y, glm::vec3(1.0f, 0.0f, 0.0f));

		cameraRotation = x_rotation * y_rotation;
		cameraDirection = glm::rotate(cameraRotation, glm::vec3(0.0f, 0.0f, -1.0f));

		cameraRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraDirection));
		cameraUp = glm::cross(cameraDirection, cameraRight);
	}
}; // camera

#endif // _CAMERA_HPP