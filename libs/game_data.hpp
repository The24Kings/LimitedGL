#ifndef _GAME_DATA_HPP
#define _GAME_DATA_HPP

#include <glm/glm.hpp>

struct vertex { // 32 bytes: texCoords is 24 bytes offset into the struct
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	bool operator==(const vertex& other) const {
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
};

#endif // _GAME_DATA_HPP