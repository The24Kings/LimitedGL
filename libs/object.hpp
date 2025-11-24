#ifndef _GAME_DATA_HPP
#define _GAME_DATA_HPP

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GLEW/glew.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <type_traits>

#include "shader.hpp"

struct vertex {
	glm::vec3 m_pos;
	glm::vec3 m_color;
	glm::vec2 m_texCoord;
	glm::vec3 m_normal;

	vertex() : m_pos(glm::vec3(0.0f, 0.0f, 0.0f)), m_color(glm::vec3(1.0f, 1.0f, 1.0f)), m_texCoord(glm::vec2(0.0f, 0.0f)), m_normal(glm::vec3(1.0f, 1.0f, 1.0f)) {}

	bool operator==(const vertex& other) const {
		return m_pos == other.m_pos && m_color == other.m_color && m_texCoord == other.m_texCoord && m_normal == other.m_normal;
	}
}; // vertex

template<> struct std::hash<vertex> {
	size_t operator()(vertex const& v) const {
		return ((
			(std::hash<glm::vec3>()(v.m_pos) ^ 
			(std::hash<glm::vec3>()(v.m_color) << 1)) >> 1) ^
			(std::hash<glm::vec2>()(v.m_texCoord) << 1) >> 1) ^ 
			(std::hash<glm::vec3>()(v.m_normal) << 1);
	}
}; // vertex hash

struct texture {
	const char* m_filename;
	unsigned char* m_image_data; // (Using STB Image) Texture buffer: size = width * height * 4 (RGBA)
	GLuint m_handle;
	GLint m_width, m_height;

	texture() : m_filename(nullptr), m_image_data(nullptr), m_handle(-1), m_width(0), m_height(0) {}
}; // texture

enum class uniform_type {
	t_int,
	t_float,
	t_vec3,
	t_mat3,
	t_mat4,
	t_none,
}; // uniform_type

struct uniform_data {
	GLuint m_location;
	uniform_type m_type;
	char m_buffer[64]; // Size of mat4f

	uniform_data() : m_location(-1), m_type(uniform_type::t_none) {
		memset(m_buffer, 0, sizeof(m_buffer));
	};

	uniform_data(uniform_type type, GLuint location) : m_location(location), m_type(type) {
		memset(m_buffer, 0, sizeof(m_buffer));
	}
}; // uniform_data

struct material {
	shader* m_shader;
	texture* m_tex;
	GLuint v_attr, t_attr, c_attr, n_attr; // vertex attribute, texture attribute, color attribute, normals attribute

	std::unordered_map<std::string, uniform_data> uniforms;

	material(shader* shader) : m_shader(shader), m_tex(nullptr) {
		v_attr = glGetAttribLocation(shader->m_handle, "in_vertex");
		c_attr = glGetAttribLocation(shader->m_handle, "in_color");
		t_attr = glGetAttribLocation(shader->m_handle, "in_texCoord");
		n_attr = glGetAttribLocation(shader->m_handle, "in_normal");
	}

	~material() {
		uniforms.clear();
	}
	
	/**
	* @brief Set material uniform of given type
	*/
	template<typename T>
	bool set_uniform(std::string name, T data) {
		const uniform_type type = get_type<T>();

		// Check if exists, create empty if it doesn't
		if (uniforms.find(name) == uniforms.end()) {
			uniforms[name] = uniform_data(type, glGetUniformLocation(m_shader->m_handle, name.c_str()));
		}

		// Get uniform from list
		uniform_data* uniform = &uniforms[name];

		// Check if the type matches data
		if (type != uniform->m_type) {
			return false;
		}

		// Cast buffer into T type
		*(T*)uniform->m_buffer = data;

		return true;
	}

	/**
	* @brief Translate T type into uniform_d_type
	*/
	template<typename T>
	uniform_type get_type() {
		if constexpr (std::is_same<T, float>())
			return uniform_type::t_float;
		if constexpr (std::is_same<T, glm::mat4>())
			return uniform_type::t_mat4;
		if constexpr (std::is_same<T, int>())
			return uniform_type::t_int;
		if constexpr (std::is_same<T, glm::mat3>())
			return uniform_type::t_mat3;
		if constexpr (std::is_same<T, glm::vec3>())
			return uniform_type::t_vec3;
		return uniform_type::t_none;
	}

	/**
	* @brief Apply the uniform to the shader
	*/
	void use() {
		m_shader->use();

		for (auto &u : uniforms) {

			auto* uniform_data = &u.second;

			switch (uniform_data->m_type) {
				case uniform_type::t_float:
					glUniform1f(uniform_data->m_location, *(GLfloat*)uniform_data->m_buffer);
					break;
				case uniform_type::t_int:
					glUniform1i(uniform_data->m_location, *(GLint*)uniform_data->m_buffer);
					break;
				case uniform_type::t_vec3:
					glUniform3fv(uniform_data->m_location, 1, (float*)uniform_data->m_buffer);
					break;
				case uniform_type::t_mat3:
					glUniformMatrix3fv(uniform_data->m_location, 1, GL_FALSE, (float*)uniform_data->m_buffer);
					break;
				case uniform_type::t_mat4:
					glUniformMatrix4fv(uniform_data->m_location, 1, GL_FALSE, (float*)uniform_data->m_buffer);
					break;
				default:
					break;
			};
		}
	}
}; // material

struct mesh {
	GLuint vao, vbo, ibo;
	std::vector<vertex> m_vertices;
	std::vector<uint32_t> m_indices;

	mesh() : m_vertices(std::vector<vertex>()), m_indices(std::vector<uint32_t>()), vao(-1), vbo(-1), ibo(-1) {}

	~mesh() {
		if (isUploaded()) {
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(1, &vbo);
			glDeleteBuffers(1, &ibo);
		}

		m_vertices.clear();
		m_indices.clear();
	}

	bool isUploaded() const {
		return vao != -1;
	}

	void upload(material* mat) {
		if (isUploaded()) {
			return;
		}

		// Generate the vertex array object
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Generate the vertex buffer object
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(vertex), m_vertices.data(), GL_STATIC_DRAW);

		// Generate the element buffer object
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(uint32_t), m_indices.data(), GL_STATIC_DRAW);

		// Set the vertex attribute pointers
		glEnableVertexAttribArray(mat->v_attr);
		glVertexAttribPointer(mat->v_attr, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, m_pos));

		// Set the color attribute pointers
		glEnableVertexAttribArray(mat->c_attr);
		glVertexAttribPointer(mat->c_attr, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, m_color));

		// Set the texture attribute pointers
		glEnableVertexAttribArray(mat->t_attr);
		glVertexAttribPointer(mat->t_attr, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, m_texCoord));

		// Set the normal attribute pointers
		glEnableVertexAttribArray(mat->n_attr);
		glVertexAttribPointer(mat->n_attr, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, m_normal));
	}

	void draw(material* mat) {
		upload(mat);

		// Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mat->m_tex->m_handle);

		// Rebind the buffers
		glBindVertexArray(vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		glDrawElements(GL_TRIANGLES, (GLsizei)m_indices.size(), GL_UNSIGNED_INT, NULL);
	}
}; // mesh

struct object; // Forward decl

struct component {
	object* m_object;

	virtual void update(float dt) {}

	component() : m_object(nullptr) {}
};

struct transform_component; // Forward decl

struct transform_component : public component { // 128 bytes
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	//transform_component* parent; // Parent transformation for hierarchical transformations (shenanigans)
	//std::unordered_set<transform_component*> children; // Children transformations for hierarchical transformations

	transform_component() : position(0.0f, 0.0f, 0.0f), rotation(1.0f, 0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f) {}
}; // transformation

class render_component : public component {
public:
	material* m_mat;
	mesh* m_mesh;

	render_component(shader* shader) {
		this->m_mat = new material(shader);
		this->m_mesh = new mesh();
	}

	~render_component() {
		delete m_mat;
		delete m_mesh;
	}

	void update(float dt) override {
		render();
	}

	void render() {
		m_mat->use();
		m_mesh->draw(m_mat);
	}
}; // render_component

struct object {
	std::vector<component*> m_components;

	object(object&) = delete; // No copy constructor

	virtual bool init() { return 0; }
	virtual void deinit() {}
	
	void update(float dt) {
		for (auto c : m_components) {
			c->update(dt);
		}
	}

	component* addComponent(component* component) {
		component->m_object = this;

		m_components.push_back(component);

		return component;
	}

	object() {}

	~object() { 
		deinit();

		for (auto c : m_components) {
			delete c;
		}
	}
}; // obj_data

bool load_obj(const char* baseDir, const char* filename, mesh* mesh);
bool load_texture(const char* filename, texture* tex);

#endif // _GAME_DATA_HPP