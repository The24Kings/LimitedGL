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

EXTERN int SCRN_WIDTH INIT(1280);
EXTERN int SCRN_HEIGHT INIT(720);

EXTERN bool shutdown INIT(false);

EXTERN double deltaTime INIT(1.0);

/* Game Strutures */

struct vertex { // 32 bytes: texCoords is 24 bytes offset into the struct
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	vertex() : pos(glm::vec3(0.0f, 0.0f, 0.0f)), color(glm::vec3(1.0f, 1.0f, 1.0f)), texCoord(glm::vec2(0.0f, 0.0f)) {}

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
	texture* texture;
	float ambient, diffuse, specular, shininess;

	material() : texture(nullptr), ambient(0.0f), diffuse(0.0f), specular(0.0f), shininess(0.0f) { }
}; // material

struct obj_mesh {
	material mat;
	std::vector<vertex> vertices;
	std::vector<uint32_t> indices;

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

bool load_obj(const char* baseDir, const char* filename, obj_mesh* mesh);
bool load_texture(const char* filename, texture* tex);

struct model_data {
	// Model data for drawing
	glm::vec3 pos;
	glm::quat rot;
	glm::vec3 scale;
}; // model_data

// Object Data
class obj_data {
public:
	GLuint program;
	GLuint model_uniform, view_uniform, projection_uniform;
	GLuint vao; // vertex array object
	GLuint v_attr, t_attr, c_attr; // vertex attribute, texture attribute, color attribute
	GLuint v_buf, c_buf, e_buf; // vertex buffer, color buffer, element buffer

	//std::vector<model_data> models; TODO: Change this to probably use uniforms
	obj_mesh* mesh; //TODO: Add support for UVs for multiple textures

	virtual int init() { return 0; }
	virtual void draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {}
	virtual void deinit() {}
	//virtual void add(glm::vec3 pos, glm::quat rot = glm::quat(1, 0, 0, 0), glm::vec3 scale = glm::vec3(1)) { models.push_back({pos, rot, scale, pos, rot, scale}); }

	virtual void move(double dt) {}
	virtual void animate(double dt) {}

	virtual ~obj_data() { deinit(); }

	obj_data() : program(-1), model_uniform(-1), view_uniform(-1), projection_uniform(-1), v_attr(-1), t_attr(-1), c_attr(-1), v_buf(-1), c_buf(-1), e_buf(-1), vao(-1) {
		//models = std::vector<model_data>();
		mesh = new obj_mesh();
	}
}; // obj_data

#endif // _GAME_DATA_HPP