#ifndef _PLAYER_HPP
#define _PLAYER_HPP

struct key_status {
	bool w, a, s, d;
	bool up, down, left, right;
	bool space, shift;

	key_status() : w(false), a(false), s(false), d(false), up(false), down(false), left(false), right(false), space(false), shift(false) {}
}; // key_status

struct player {
	key_status keys;

	float movementSpeed = 2.5f;

	player() : keys() { } 
}; // player

#endif // _PLAYER_HPP