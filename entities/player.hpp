#ifndef _PLAYER_HPP
#define _PLAYER_HPP

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <cmath>

#include "game_data.hpp"
#include "camera.hpp"
#include "transformations.hpp"

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

//static void move_player(player* source, float deltaTime) {
//	// Get inputs
//	int forward_backward_axis = source->keys.w - source->keys.s;
//	int left_right_axis = source->keys.d - source->keys.a;
//	int up_down_axis = source->keys.up - source->keys.down;
//
//	// Get the look at angle of the player
//	glm::quat look_at = camera_rotation(source);
//	glm::mat4 transform = glm::mat4_cast(look_at);
//
//	// Move the player
//	glm::vec3 dir_forward = get_forward(glm::mat4_cast(look_at));
//	glm::vec3 dir_right = get_right(glm::mat4_cast(look_at));
//
//	float gravity = 0.01f * deltaTime;
//	float speed = 0.01f;
//
//	// Motion
//	float& x_vel = source->velocity.x;
//	float& y_vel = source->velocity.y;
//	float& z_vel = source->velocity.z;
//
//	// Apply gravity
//	y_vel -= gravity;
//
//	// Update movement velocity
//	source->velocity += (dir_forward * (float)forward_backward_axis * deltaTime * speed) + (dir_right * (float)left_right_axis * deltaTime * speed);
//
//	// Apply friction
//	//std::lerp(x_vel, 0.0f, 10.0f / deltaTime);
//	//std::lerp(y_vel, 0.0f, 10.0f / deltaTime);
//
//	// Update the player position
//	source->transform_data.position += source->velocity; // TODO: Add collision detection
//
//} // player::move_player

#endif // _PLAYER_HPP