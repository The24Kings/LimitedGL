#ifndef _GAME_DATA_HPP
#define _GAME_DATA_HPP

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GLEW/glew.h>
#include <vector>

/* Global shenanigans (so extern is used when defined outside main) */
#ifdef MAIN
#define EXTERN
#define INIT(x) = x
#else
#define EXTERN extern
#define INIT(x)
#endif

/* Globals */

EXTERN float width INIT(1280);
EXTERN float height INIT(720);

EXTERN bool shutdown INIT(false);

EXTERN double target_framerate INIT(60);
EXTERN double framerate INIT(0.0);
EXTERN double deltaTime INIT(1.0);
EXTERN uint64_t target_frame_time INIT(16000000); // In nanoseconds
EXTERN uint64_t elapsed_time INIT(target_frame_time);

/* Game Strutures */

struct vertex { // 32 bytes: texCoords is 24 bytes offset into the struct
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	vertex() : pos(0.0f), color(1.0f), texCoord(0.0f) {}

	bool operator==(const vertex& other) const {
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
}; // vertex

struct texture {
	const char* filename;
	unsigned char* image_data; // (Using STB Image) Texture buffer: size = width * height * 4 (RGBA)
	GLuint texture_handle;
	GLint width, height;

	texture() : filename(nullptr), image_data(nullptr), texture_handle(-1), width(0), height(0) {}
}; // texture

struct material {
	texture* texture[32];
	size_t texture_count; // Important to know how many textures are in the array (I never have any luck with sizeof)
	float ambient, diffuse, specular, shininess;

	material() : texture_count(0), ambient(0.0f), diffuse(0.0f), specular(0.0f), shininess(0.0f) {
		memset(texture, 0, 32);
	}
}; // material

struct obj_mesh {
	std::vector<vertex> vertices;
	std::vector<uint32_t> indices;
	material mat;

	obj_mesh() : mat() {
		vertices = std::vector<vertex>();
		indices = std::vector<uint32_t>();
	}
}; // obj_mesh

// ((vec3 ^ vec3 << 1) >> 1) ^ (vec2 << 1)
template<> struct std::hash<vertex> { // Hash function for vertex struct
	size_t operator()(vertex const& v) const {
		return ((std::hash<glm::vec3>()(v.pos) ^ (std::hash<glm::vec3>()(v.color) << 1)) >> 1) ^ (std::hash<glm::vec2>()(v.texCoord) << 1);
	}
};

int load_obj(const char* baseDir, const char* filename, obj_mesh &mesh);
void load_texture(const char* filename, texture* tex);

struct model_data {
	// Model data for drawing
	glm::vec3 pos;
	glm::quat rot;
	glm::vec3 scale;

	// Start position, rotation, and scale for animations
	glm::vec3 start_pos;
	glm::quat start_rot;
	glm::vec3 start_scale;
}; // model_data

// Object Data
class obj_data {
public:
	GLuint program, mvp_uniform, models_buf, v_attr, t_attr, c_attr, v_buf, c_buf, e_buf; // model view projection uniform, models buffer, vertex attribute, texture attribute, color attribute, vertex buffer, color buffer, element buffer, texture
	std::vector<model_data> models;
	obj_mesh mesh;

	virtual int init() { return 0; }
	virtual void draw(glm::mat4 vp) {}
	virtual void deinit() {}
	virtual void add(glm::vec3 pos, glm::quat rot, glm::vec3 scale) { models.push_back({ pos, rot, scale, pos, rot, scale }); }

	virtual void move(double dt) {}
	virtual void animate(double dt) {}
}; // obj_data

#endif // _GAME_DATA_HPP