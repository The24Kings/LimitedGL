#ifndef _PLAYER_HPP
#define _PLAYER_HPP

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <cmath>

#include "object.hpp"
#include "camera.hpp"

struct key_status {
	bool w, a, s, d;
	bool up, down, left, right;
	bool space, shift;

	key_status() : w(false), a(false), s(false), d(false), up(false), down(false), left(false), right(false), space(false), shift(false) {}
}; // key_status

struct player {
	key_status keys;

	glm::vec3 velocity;

	player() : keys(), velocity(0.0f, 0.0f, 0.0f) { } 
}; // player

#endif // _PLAYER_HPP