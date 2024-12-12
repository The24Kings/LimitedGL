#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GLEW/glew.h>
#include <glfw/glfw3.h>
#include <inttypes.h>
#include <stdio.h>
#include <vector>
#include <thread>
#include <chrono>

#define _USE_MATH_DEFINES
#include<math.h>

#include "compile_shaders.hpp"
#include "game_data.hpp"

#include "base_objects.hpp"

float width = 1280;
float height = 720;

double fps = 0.0;
double target_fps = 60.0;
double deltaTime = 0.0;

static void resize_callback(GLFWwindow* window, int width, int height);

int main(void) {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "LimitedGL Engine", NULL, NULL);

    if (!window) {
        glfwTerminate();

        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwMakeContextCurrent(window);
    glewInit();

    /* Callbacks */
    glfwSetFramebufferSizeCallback(window, resize_callback);

    /* Objects */
	std::vector<obj_data*> objects;

	loaded_obj obj = loaded_obj("objects/cube.obj", "objects/", 1, "objects/textures/brick.jpg");
	obj.add(glm::vec3(0.0f, 0.0f, 10.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	objects.push_back(&obj);

	/* Initialize objects */
	for (obj_data* obj : objects) {
        if (obj->init()) {
			puts(RED("Failed to init objects").c_str());

			return 1;
		}
	}

    /* Loop until the user closes the window */
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
		auto start = std::chrono::high_resolution_clock::now();

        /* Poll for and process events */
        glfwPollEvents();
        glClearColor(0, 0, 0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Get the view and projection matrices
        glm::vec3 global_up(0, 1, 0);

        glm::vec3 look_at_point = glm::vec3(0.0f, 0.0f, 0.0f) + glm::vec3(cosf(0) * sinf(M_PI), sinf(0), cosf(0) * cosf(M_PI));
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), look_at_point, global_up);
        glm::mat4 projection = glm::perspective(45.0f, width / height, 0.1f, 1000.0f);
        glm::mat4 vp = projection * view;

		for (obj_data* obj : objects) {
			obj->draw(vp); //FIXME: Nothing is drawn/ the color is black
		}

		/* Swap front and back buffers */
        glfwSwapBuffers(window);

		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		fps = 1000.0 / (double)elapsed.count();
		deltaTime = (target_fps / fps) > 1 ? 1 : (target_fps / fps); // Cap the delta time to 1

		// Use the delta time to limit the frame rate
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000.0 / target_fps - fps))); // TODO: Look into to make sure this is accurate
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