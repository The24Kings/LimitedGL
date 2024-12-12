#ifndef _GAME_DATA_HPP
#define _GAME_DATA_HPP

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

struct vertex { // 32 bytes: texCoords is 24 bytes offset into the struct
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	bool operator==(const vertex& other) const {
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
};

// ((vec3 ^ vec3 << 1) >> 1) ^ (vec2 << 1)
template<> struct std::hash<vertex> { // Hash function for vertex struct
	size_t operator()(vertex const& v) const {
		return ((std::hash<glm::vec3>()(v.pos) ^ (std::hash<glm::vec3>()(v.color) << 1)) >> 1) ^ (std::hash<glm::vec2>()(v.texCoord) << 1);
	}
};

int load_obj(const char* baseDir, const char* filename, std::vector<vertex> vertices, std::vector<uint32_t> indices, glm::vec2& texScale);
unsigned load_texture(const char* filename);

#endif // _GAME_DATA_HPP