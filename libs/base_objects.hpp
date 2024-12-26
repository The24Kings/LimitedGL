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
	} // loaded_obj ctor

	int init() override {
		// Load obj file
		load_obj(baseDir, object_file, mesh);

		// Generate the vertex array object
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Generate the vertex buffer object
		glGenBuffers(1, &v_buf);
		glBindBuffer(GL_ARRAY_BUFFER, v_buf);
		glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(vertex), mesh.vertices.data(), GL_STATIC_DRAW);

		// Generate the indices buffer object
		glGenBuffers(1, &e_buf);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, e_buf);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);

		// Set the vertex attribute pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, pos));
		glEnableVertexAttribArray(0);

		// Set the texture attribute pointers
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texCoord));
		glEnableVertexAttribArray(1);

		//glGenBuffers(1, &models_buf);

		// Load and Bind textures
		for (size_t i = 0; i < texture_files.size(); i++) {
			texture* tex = new texture();

			load_texture(texture_files[i].c_str(), tex);

			this->mesh.mat.texture[i] = tex;
			this->mesh.mat.texture_count++;

			printf(BLUE("Bound texture: %d\tHandle: %d\n").c_str(), i, this->mesh.mat.texture[i]->texture_handle);
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
	} // loaded_obj::init

	void draw(glm::mat4 vp) override {
		glUseProgram(this->program);

		// Apply the model data
		std::vector<glm::mat4> models_buffer;
		models_buffer.reserve(this->models.size());

		//TODO: Apply the transformations to the models (using the transformation struct)
		for (model_data& model : this->models) {
			glm::mat4 new_model = glm::mat4(1.0f);

			new_model = glm::translate(new_model, model.pos);
			new_model = new_model * glm::mat4_cast(model.rot);
			new_model = glm::scale(new_model, model.scale);

			models_buffer.push_back(glm::mat4(1));
		}

		// Bind the model buffer data
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->models_buf);
		glBufferData(GL_SHADER_STORAGE_BUFFER, models_buffer.size() * sizeof(glm::mat4), models_buffer.data(), GL_STATIC_DRAW);

		// Texture
		for (size_t i = 0; i < this->mesh.mat.texture_count; i++) {
			//printf("Activating Texture: %d\tHandle: %d\n", i, this->mesh.mat.texture[i]->texture_handle);

			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, this->mesh.mat.texture[i]->texture_handle);
		}

		glUniformMatrix4fv(mvp_uniform, 1, 0, glm::value_ptr(vp)); // Set the model view projection matrix

		glBindVertexArray(vao); // Bind the vertex array object
		glBindBuffer(e_buf, GL_ELEMENT_ARRAY_BUFFER); // Bind the element buffer object

		glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, nullptr);

		//int size;
		//glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		//glDrawElementsInstanced(GL_TRIANGLES, size / sizeof(GLuint), GL_UNSIGNED_INT, 0, models_buffer.size());
	} // loaded_obj::draw

	void deinit() override {
		for (size_t i = 0; i < mesh.mat.texture_count; i++) {
			if (mesh.mat.texture[i]->texture_handle != -1) {
				glDeleteTextures(1, &mesh.mat.texture[i]->texture_handle);
			}
		}
	} // loaded_obj::deinit
}; // loaded_obj

#endif // _BASE_OBJECTS_HPP
