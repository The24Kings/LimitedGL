#ifndef _CAMERA_HPP
#define _CAMERA_HPP

#include <glm/glm.hpp>

#include "transformations.hpp"

struct camera {
	transformation transform_data;
	
	float fov; // Degrees
	float near_plane, far_plane;

	camera(float fov, float np, float fp) : fov(fov), near_plane(np), far_plane(fp) {
		create_identity(&transform_data);
	} // camera ctor
};

inline glm::mat4 raw_perpective(float fov, float aspect, float near_plane, float far_plane) {
	return glm::perspective(glm::radians(fov), aspect, near_plane, far_plane);
}

#endif // _CAMERA_HPP