#ifndef _BASE_OBJECTS_HPP
#define _BASE_OBJECTS_HPP

#include <glm/glm.hpp>
#include <stdarg.h>

#include "compile_shaders.hpp"
#include "game_data.hpp"

class loaded_obj : virtual public obj_data {
public:
	std::vector<std::string> texture_files;
	const char* object_file;
	const char* baseDir;

	/**
	* Create a new loaded_obj object
	*	
	* @param of The object file
	* @param bD The base directory of the texture file
	* @param tf The texture file
	*/
	loaded_obj(const char* of, const char* bD, size_t tex_count, ...) : object_file(of), baseDir(bD) {
		va_list args;
		va_start(args, tex_count);

		// Push the texture files into the vector
		for (size_t i = 0; i < tex_count; i++) {
			texture_files.push_back(va_arg(args, const char*));
		}

		va_end(args);
	}

	int init() override {
		// Load obj file
		load_obj(baseDir, object_file, mesh);

		glGenBuffers(1, &v_buf);
		glBindBuffer(GL_ARRAY_BUFFER, v_buf);
		glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(vertex), mesh.vertices.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &e_buf);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, e_buf);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &models_buf);

		// Load and Bind textures
		for (size_t i = 0; i < texture_files.size(); i++) {
			texture tex = {};

			load_texture(texture_files[i].c_str(), tex);

			this->mesh.mat.texture[i] = &tex;
			this->mesh.mat.texture_count++;

			// Bind the texture
			glActiveTexture(tex.texture_handle);
			glBindTexture(GL_TEXTURE_2D, tex.texture_handle);
		}

		// TODO: Change this to implement the hash table for one compiled shader program if they share the same shaders

		// Make Program
		shader_program* program = create_shader_program("shaders/loaded_obj_vertex_shader.glsl", 0, 0, 0, "shaders/loaded_obj_fragment_shader.glsl");

		if (program == nullptr) { // Check if the program was created successfully
			return 1;
		}

		this->program = program->handle;

		// Set the uniform locations
		mvp_uniform = glGetUniformLocation(program->handle, "vp");
		v_attr = glGetAttribLocation(program->handle, "in_vertex");
		t_attr = glGetAttribLocation(program->handle, "in_texCoord");

		return 0;
	} // init

	void draw(glm::mat4 vp) override {
		glUseProgram(this->program);

		// Apply the model data
		std::vector<glm::mat4> models_buffer;
		models_buffer.reserve(this->models.size());

		for (model_data& model : this->models) {
			glm::mat4 new_model = glm::mat4(1.0f);

			new_model = glm::translate(new_model, model.pos);
			new_model = new_model * glm::mat4_cast(model.rot);
			new_model = glm::scale(new_model, model.scale);

			models_buffer.push_back(new_model);
		}

		// Bind the model buffer data
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->models_buf);
		glBufferData(GL_SHADER_STORAGE_BUFFER, models_buffer.size() * sizeof(glm::mat4), models_buffer.data(), GL_STATIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->models_buf);

		glEnableVertexAttribArray(v_attr);
		glBindBuffer(GL_ARRAY_BUFFER, v_buf);
		glVertexAttribPointer(v_attr, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, pos));

		glEnableVertexAttribArray(t_attr);
		glVertexAttribPointer(t_attr, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texCoord));

		//FIXME: Doesn't do the thing
		// Texture
		for (size_t i = 0; i < mesh.mat.texture_count; i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, mesh.mat.texture[i]->texture_handle);
		}

		int size;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, e_buf);
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

		glUniformMatrix4fv(mvp_uniform, 1, 0, glm::value_ptr(vp));

		glDrawElementsInstanced(GL_TRIANGLES, size / sizeof(GLuint), GL_UNSIGNED_INT, 0, models_buffer.size());
	} // draw

	void deinit() override {
		for (size_t i = 0; i < mesh.mat.texture_count; i++) {
			if (mesh.mat.texture[i]->texture_handle != -1) {
				glDeleteTextures(1, &mesh.mat.texture[i]->texture_handle);
			}
		}
	} // deinit
}; // loaded_obj

#endif // _BASE_OBJECTS_HPP