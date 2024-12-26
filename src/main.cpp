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

#include "transformations.hpp"
#include "camera.hpp"
#include "player.hpp"
#include "compile_shaders.hpp"
#include "game_data.hpp"

#include "base_objects.hpp"
#include "entities/crosshair.hpp"

/* Window Data */

int width = 1280;
int height = 720;

/* Engine Data */

bool shutdown = false;

/* Frame Data */

double target_framerate = 60;
double framerate = 0;
double deltaTime = 1.0;
uint64_t target_frame_time = 16000000; // In nanoseconds (60 FPS)
uint64_t elapsed_time = target_frame_time;

/* Call Backs */

static void resize_callback(GLFWwindow* window, int width, int height);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main(void) {
    GLFWwindow* window;

    /* Initialize GLFW */
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

    /* Player Data */
	camera main_camera = camera(60.0f, 1.0f, 1000.0f);
	player main_player = player(main_camera);

	main_player.transform_data.position = glm::vec3(0.0f, 0.0f, 10.0f); // Set the player's position

    /* Callbacks */
    glfwSetFramebufferSizeCallback(window, resize_callback);
	glfwSetKeyCallback(window, key_callback);

    /* Objects */
	std::vector<obj_data*> objects;

	loaded_obj obj = loaded_obj("objects/cube.obj", "objects/", 1, "objects/textures/brick.jpg");
	obj.add(glm::vec3(0.0f, 0.0f, 10.0f));
    objects.push_back(&obj);

	//crosshair cross = crosshair();
	//objects.push_back(&cross);

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

        /* Get the view and projection matrices */
        glm::vec3 global_up(0, 1, 0);

		// Get the view matrix
		glm::mat4 view = view_matrix(&main_camera, width / (float)height);
		glm::mat4 projection = raw_perpective(main_camera.fov, width / (float)height, main_camera.near_plane, main_camera.far_plane);

		glm::mat4 vp = projection * view;

		for (obj_data* obj : objects) {
			obj->draw(vp);
		}

		/* Swap front and back buffers */
        glfwSwapBuffers(window);

		/* Calculate the frame rate */
		auto end = std::chrono::high_resolution_clock::now();
		
		elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        framerate = 1000000000.0 / (double)elapsed_time; // Calculate the frame rate (in nanoseconds)
		deltaTime = (target_framerate / framerate) > 1 ? 1 : (target_framerate / framerate); // Cap the delta time to 1

		//printf("Framerate: %f\t DeltaTime: %f\tElapsed Time : % " PRIu64 "\n", framerate, (float)deltaTime, elapsed_time);

		/* Use Delta Time to sleep accoringly */
        if (elapsed_time < target_frame_time) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(target_frame_time - elapsed_time));
        }
	} // Game Loop

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
} // main

/**
* @brief Resize the window
 *
 * @param window The window to resize
 * @param width The new width
 * @param height The new height
*/
static void resize_callback(GLFWwindow* window, int width, int height) {
    ::height = height;
    ::width = width;

    glViewport(0, 0, width, height);
} // resize_callback

/**
* @brief Handle key presses
 *
 * @param window The window
 * @param key The key pressed
 * @param scancode The scancode
 * @param action The action
 * @param mods The mods
*/
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		shutdown = true;
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	// Move the camera
	int cam_up_down_axis = -GLFW_KEY_UP + GLFW_KEY_DOWN;
	int cam_left_right_axis = GLFW_KEY_LEFT - GLFW_KEY_RIGHT;

	printf("Camera Moving: %d\n", cam_up_down_axis);
	printf("Camera Moving: %d\n", cam_left_right_axis);

    // Move the player
	int forward_backward_axis = -GLFW_KEY_W + GLFW_KEY_S;
	int left_right_axis = GLFW_KEY_D - GLFW_KEY_A;
	int up_down_axis = GLFW_KEY_SPACE - GLFW_KEY_LEFT_SHIFT;

	//printf("Moving: %d\n", forward_backward_axis); 
	//printf("Moving: %d\n", left_right_axis);
	//printf("Moving: %d\n", up_down_axis);

} // key_callback