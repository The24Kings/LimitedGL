#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GLEW/glew.h>
#include <glfw/glfw3.h>
#include <inttypes.h>
#include <stdio.h>

#include "compile_shaders.hpp"

float width = 1280;
float height = 720;

static void resize_callback(GLFWwindow* window, int width, int height);

int main(void) {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwMakeContextCurrent(window);
    glewInit();

    /* Callbacks */
    glfwSetFramebufferSizeCallback(window, resize_callback);

	float triangle[] { // Triangle Vertices TEST
        0, 0, 0, 
        0, 1, 0, 
        1, 1, 0 
    };

	// Create Objects for OpenGL
	uint32_t vao; // Vertex Array Object Handle
    glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	uint32_t vbo; // Vertex Buffer Object Handle
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Make Shader
	shader_source* vertex_shader = create_shader_source(GL_VERTEX_SHADER, "shaders/vertex_shader.glsl");
	shader_source* fragment_shader = create_shader_source(GL_FRAGMENT_SHADER, "shaders/fragment_shader.glsl");

	shader_source sources[] = { *vertex_shader, *fragment_shader };

	// Make Program
	shader_program* program = create_shader_program(sources, 2);

    /* Loop until the user closes the window */
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        /* Poll for and process events */
        glfwPollEvents();
        glClearColor(0, 0, 0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

		// Draw Triangle
		glUseProgram(program->handle);

        glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		/* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

static void resize_callback(GLFWwindow* window, int width, int height) {
    ::height = height;
    ::width = width;

    glViewport(0, 0, width, height);
}