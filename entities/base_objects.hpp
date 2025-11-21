#ifndef _BASE_OBJECTS_HPP
#define _BASE_OBJECTS_HPP

#include <glm/glm.hpp>
#include <stdarg.h>

#include "compile_shaders.hpp"
#include "utils.hpp"
#include "game_data.hpp"

class loaded_obj : virtual public obj_data {
public:
	const char* texture_file;
	const char* object_file;
	const char* objBaseDir;

	/**
	* Create a new loaded_obj object
	*	
	* @param of The object file
	* @param bD The base directory of the texture file
	* @param tf The texture file
	*/
	loaded_obj(const char* of, const char* obD, const char* tf) : object_file(of), objBaseDir(obD), texture_file(tf) { } // loaded_obj ctor

	int init() override {
		// Load obj file
		if (!load_obj(objBaseDir, object_file, mesh)) {
			printf(RED("Failed to load obj file: %s\n").c_str(), object_file);

			return 1;
		}

		// Generate the vertex buffer object
		glGenBuffers(1, &v_buf);
		glBindBuffer(GL_ARRAY_BUFFER, v_buf);
		glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(vertex), mesh->vertices.data(), GL_STATIC_DRAW);

		// Generate the indices buffer object
		glGenBuffers(1, &e_buf);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, e_buf);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(uint32_t), mesh->indices.data(), GL_STATIC_DRAW);

		//glGenBuffers(1, &models_buf);

		// Load and Bind textures
		texture* tex = new texture();

		if (!load_texture(texture_file, tex)) {
			printf(RED("Failed to load texture: %s\n").c_str(), texture_file);

			return 1;
		}

		this->mesh->mat.texture = tex;

		printf(BLUE("Texture Handle: %d\n").c_str(), this->mesh->mat.texture->texture_handle);

		// TODO: Change this to implement the hash table for one compiled shader program if they share the same shaders

		shader_program* shader_program = create_shader_program("shaders/loaded_obj_vertex_shader.glsl", nullptr, nullptr, nullptr, "shaders/loaded_obj_fragment_shader.glsl");

		if (shader_program == nullptr) { // Check if the program was created successfully
			printf(RED("Failed to create shader program\n").c_str());

			return 1;
		}

		program = shader_program->handle;

		// Set the uniform locations
		mvp_uniform = glGetUniformLocation(program, "vp");
		v_attr = glGetAttribLocation(program, "in_vertex");
		t_attr = glGetAttribLocation(program, "in_texCoord");

		return 0;
	} // loaded_obj::init

	void draw(glm::mat4 vp) override {
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
		glBindTexture(GL_TEXTURE_2D, mesh->mat.texture->texture_handle);
		
		// Set the model view projection matrix
		glUniformMatrix4fv(mvp_uniform, 1, 0, glm::value_ptr(vp));

		// Rebind the buffers
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, e_buf);
		glBindBuffer(GL_ARRAY_BUFFER, v_buf);

		// Set the vertex attribute pointers
		glEnableVertexAttribArray(v_attr);
		glVertexAttribPointer(v_attr, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, pos));

		// Set the texture attribute pointers
		glEnableVertexAttribArray(t_attr);
		glVertexAttribPointer(t_attr, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texCoord));

		// Handle Errors
		GLenum error = glGetError();

		if (error != GL_NO_ERROR) {
			printf(RED("OpenGL Error: %d\n").c_str(), error);

			return;
		}

		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, NULL);
	} // loaded_obj::draw

	void deinit() override {
		if (mesh->mat.texture->texture_handle != -1) {
			glDeleteTextures(1, &mesh->mat.texture->texture_handle);
		}
	} // loaded_obj::deinit
}; // loaded_obj

#endif // _BASE_OBJECTS_HPP
