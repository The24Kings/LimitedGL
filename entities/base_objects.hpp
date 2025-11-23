#ifndef _BASE_OBJECTS_HPP
#define _BASE_OBJECTS_HPP

#include <glm/glm.hpp>
#include <stdarg.h>

#include "compile_shaders.hpp"
#include "utils.hpp"
#include "game_data.hpp"

/**
* @brief A loaded object from an OBJ file with a texture
*/
class loaded_obj : virtual public obj_data {
public:
	std::string texture_file;
	std::string object_file;
	std::string objBaseDir;

	/**
	* Create a new loaded_obj object
	*
	* @param of The object file
	* @param tf The texture file
	*/
	loaded_obj(std::string of, std::string tf) : object_file(of), texture_file(tf), objBaseDir(object_file.substr(0, object_file.find('/'))) {
		mat = new material();
		mesh = new obj_mesh();
	}

	int init() override {
		try {
			load_obj(objBaseDir, object_file, mesh);
		} catch (std::runtime_error &e) {
			printf(RED("Failed to load obj file: %s\n").c_str(), e.what());
			return 1;
		}

		// Generate the vertex array object
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Generate the vertex buffer object
		glGenBuffers(1, &v_buf);
		glBindBuffer(GL_ARRAY_BUFFER, v_buf);
		glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(vertex), mesh->vertices.data(), GL_STATIC_DRAW);

		// Generate the element buffer object
		glGenBuffers(1, &e_buf);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, e_buf);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(uint32_t), mesh->indices.data(), GL_STATIC_DRAW);

		// Load and Bind textures
		if (!load_texture(texture_file.c_str(), tex)) {
			printf(RED("Failed to load texture: %s\n").c_str(), texture_file);

			return 1;
		}

		printf(BLUE("Texture Handle: %d\n").c_str(), tex->texture_handle);

		// TODO: Change this to implement the hash table for one compiled shader program if they share the same shaders

		shader_program* shader_program = create_shader_program("shaders/loaded_obj_vertex_shader.glsl", nullptr, nullptr, nullptr, "shaders/loaded_obj_fragment_shader.glsl");

		if (shader_program == nullptr) { // Check if the program was created successfully
			printf(RED("Failed to create shader program\n").c_str());

			return 1;
		}

		program = shader_program->handle;

		// Set the shader variable information
		model_uniform = glGetUniformLocation(program, "model");
		view_uniform = glGetUniformLocation(program, "view");
		projection_uniform = glGetUniformLocation(program, "projection");

		v_attr = glGetAttribLocation(program, "in_vertex");
		c_attr = glGetAttribLocation(program, "in_color");
		t_attr = glGetAttribLocation(program, "in_texCoord");

		// TODO: Add support for normals and lighting
		// vec3 normalvector attribute
		// vec3 lightdir uniform
		// vec3 lightcolor uniform

		return 0;
	} // loaded_obj::init

	void draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection) override {
		if (!mesh || mesh->vertices.empty() || mesh->indices.empty()) {
			printf(RED("Mesh data is not properly initialized.").c_str());

			return;
		}

		glUseProgram(program);

		//TODO: Change to use uniform buffer objects
		//// Apply the model data
		//std::vector<glm::mat4> models_buffer;
		//models_buffer.reserve(this->models.size());
		//
		////TODO: Apply the transformations to the models (using the transformation struct)
		//for (model_data& model : this->models) {
		//	glm::mat4 new_model = glm::mat4(1.0f);
		//
		//	new_model = glm::translate(new_model, model.pos);
		//	new_model = new_model * glm::mat4_cast(model.rot);
		//	new_model = glm::scale(new_model, model.scale);
		//
		//	models_buffer.push_back(glm::mat4(1));
		//}
		//
		//// Bind the model buffer data
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->models_buf);
		//glBufferData(GL_SHADER_STORAGE_BUFFER, models_buffer.size() * sizeof(glm::mat4), models_buffer.data(), GL_STATIC_DRAW);

		// Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex->texture_handle);
		
		// Set the model view projection matrix
		glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));

		// Rebind the buffers
		glBindBuffer(GL_ARRAY_BUFFER, v_buf);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, e_buf);

		// Set the vertex attribute pointers
		glEnableVertexAttribArray(v_attr);
		glVertexAttribPointer(v_attr, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, pos));

		// Set the color attribute pointers
		glEnableVertexAttribArray(c_attr);
		glVertexAttribPointer(c_attr, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, color));

		// Set the texture attribute pointers
		glEnableVertexAttribArray(t_attr);
		glVertexAttribPointer(t_attr, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texCoord));

		glDrawElements(GL_TRIANGLES, (GLsizei)mesh->indices.size(), GL_UNSIGNED_INT, NULL);
	} // loaded_obj::draw

	void deinit() override {
		if (tex->texture_handle != -1) {
			glDeleteTextures(1, &tex->texture_handle);
		}

		if (vao != -1) { glDeleteVertexArrays(1, &vao); }
		if (v_buf != -1) { glDeleteBuffers(1, &v_buf); }
		if (e_buf != -1) { glDeleteBuffers(1, &e_buf); }
	} // loaded_obj::deinit
}; // loaded_obj

#endif // _BASE_OBJECTS_HPP
