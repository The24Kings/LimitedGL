#ifndef _CROSSHAIR_HPP
#define _CROSSHAIR_HPP

#include "game_data.hpp"
#include "compile_shaders.hpp"

class crosshair : virtual public obj_data {
public:
	int init() override {
		float vertices[]{
			-.025f, .002f,  0.0f,
			.025f, .002f,  0.0f,
			.025f,  -.002f, 0.0f,
			-.025f, .002f,  0.0f,
			.025f, -.002f, 0.0f,
			-.025f, -.002f, 0.0f,
			
			-.002f, .04f,  0.0f,
			.002f, .04f,  0.0f,
			.002f,  -.04f, 0.0f,
			-.002f, .04f,  0.0f,
			.002f, -.04f, 0.0f,
			-.002f, -.04f, 0.0f,
		};

		glGenVertexArrays(1, &vao); // Generate the vertex array object
		glBindVertexArray(vao); // Bind the vertex array object

		glGenBuffers(1, &v_buf); // Generate the vertex buffer object
		glBindBuffer(GL_ARRAY_BUFFER, v_buf); // Bind the vertex buffer object
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Load the vertex buffer object

		// Create the shader program
		shader_program* program = create_shader_program("shaders/crosshair_vertex_shader.glsl", 0, 0, 0, "shaders/crosshair_fragment_shader.glsl");

		if (program == nullptr) { // Check if the program was created successfully
			return 1;
		}

		this->program = program->handle;

		// Set the uniform locations
		v_attr = glGetAttribLocation(this->program, "in_vertex");
	}

	void draw(glm::mat4 vp) override {
		glUseProgram(this->program);

		// Set the vertex attribute pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Draw the object
		glDrawArrays(GL_TRIANGLES, 0, 12);
	}
};

#endif // _CROSSHAIR_HPP