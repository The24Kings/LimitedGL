#ifndef _CAMERA_HPP
#define _CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "transformations.hpp"

struct frustum {
	float fov; // Degrees
	float near_plane, far_plane;

	frustum(float fov, float np, float fp) : fov(fov), near_plane(np), far_plane(fp) {}
}; // frustum

struct camera {
	float pitch;
	float yaw;
	glm::quat camera_angle;
	glm::mat4 view;

	frustum* camera_frustum;

	camera(float pitch, float yaw, frustum* frustum) : pitch(pitch), yaw(yaw), camera_frustum(frustum) {
		camera_angle = glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0)) * glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0));
		view = glm::inverse(glm::mat4_cast(camera_angle));
	}
}; // camera

inline glm::mat4 raw_perpective(float fov, float aspect, float near_plane, float far_plane) {
	return glm::perspective(glm::radians(fov), aspect, near_plane, far_plane);
}

#endif // _CAMERA_HPP