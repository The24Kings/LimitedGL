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
	glm::vec2 cameraTarget;

	glm::vec3 cameraDirection;
	glm::vec3 cameraRight;
	glm::vec3 cameraUp;


	camera(glm::vec3 pos, glm::vec2 target, frustum* frustum) : cameraPos(pos), cameraTarget(target), camera_frustum(frustum), cameraDirection(glm::vec3(0, 0, 0)) {
		update();
	}

	void update() {
		cameraDirection.x = cosf(glm::radians(cameraTarget.y)) * cosf(glm::radians(cameraTarget.x));
		cameraDirection.y = sinf(glm::radians(cameraTarget.x));
		cameraDirection.z = sinf(glm::radians(cameraTarget.y)) * cosf(glm::radians(cameraTarget.x));
		cameraDirection = glm::normalize(cameraDirection);

		cameraRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraDirection));
		cameraUp = glm::cross(cameraDirection, cameraRight);
	}
}; // camera

//inline glm::mat4 raw_perpective(float fov, float aspect, float near_plane, float far_plane) {
//	return glm::perspective(glm::radians(fov), aspect, near_plane, far_plane);
//}

#endif // _CAMERA_HPP