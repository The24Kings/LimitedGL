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

#include "utils.hpp"
#include "transformations.hpp"
#include "camera.hpp"
#include "player.hpp"
#include "compile_shaders.hpp"
#include "game_data.hpp"

#include "base_objects.hpp"
#include "crosshair.hpp"

/* Window Data */

int width = 1920;
int height = 1080;

/* Engine Data */

bool shutdown = false;

/* Game Data */

std::vector<obj_data*> objects;
camera main_camera = camera(60.0f, 1.0f, 1000.0f);
player main_player = player(&main_camera);

/* Frame Data */

double target_framerate = 60;
double framerate = 0;
double deltaTime = 1.0;
uint64_t target_frame_time = 16000000; // In nanoseconds (60 FPS)
uint64_t elapsed_time = target_frame_time;

/* Call Backs */

static void resize_callback(GLFWwindow* window, int width, int height);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void object_movement();

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
	main_player.transform_data.position = glm::vec3(0.0f, 0.0f, -5.0f);

    /* Callbacks */
    glfwSetFramebufferSizeCallback(window, resize_callback);
	glfwSetKeyCallback(window, key_callback);

    /* Objects */
	loaded_obj obj = loaded_obj("objects/cube.obj", "objects/", "objects/textures/brick.jpg");
    objects.push_back(&obj);

    crosshair cross = crosshair();
    objects.push_back(&cross);

	/* Initialize objects */
	for (obj_data* obj : objects) {
        if (obj->init()) {
			puts(RED("Failed to init objects").c_str());

			return 1;
		}
	}

    /* Create Threads */
    std::thread movement_thread(object_movement);

    /* Loop until the user closes the window */
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
		auto start = std::chrono::high_resolution_clock::now();

        /* Poll for and process events */
        glfwPollEvents();
        glClearColor(0, 0, 0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        /* Temporary Player Movement */
        if (main_player.keys.up) {
            main_player.transform_data.position.y += 1;
        }
        if (main_player.keys.down) {
            main_player.transform_data.position.y -= 1;
        }

        /* Get the view and projection matrices */
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = raw_perpective(main_camera.fov, width / (float)height, main_camera.near_plane, main_camera.far_plane);
		glm::mat4 position = get_transform_matrix(&main_player.transform_data); //FIXME: I am upsidedown??
		glm::mat4 rotation = glm::mat4_cast(main_player.transform_data.rotation);
		
		glm::mat4 vp = view * projection * rotation * position; // Apply the transformations from the player

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

		/* Pause the thread until the desired fps is achieved */
        if (elapsed_time < target_frame_time) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(target_frame_time - elapsed_time));
        }
	} // Game Loop

	/* Join the threads */
	movement_thread.join();

	/* Deinitialize objects */
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

	// Player Movement
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        main_player.keys.up = 1;
    }
	if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
		main_player.keys.up = 0;
	}
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        main_player.keys.down = 1;
    }
	if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
		main_player.keys.down = 0;
	}
} // key_callback

/**
* @brief Move the objects
*/
static void object_movement() {
    while (!shutdown) {
		auto start = std::chrono::high_resolution_clock::now();

        for (obj_data* obj : objects) {
            obj->move(deltaTime);
        }

        /* Pause the thread until the desired fps is achieved */
		auto end = std::chrono::high_resolution_clock::now();

		elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

		std::this_thread::sleep_for(std::chrono::nanoseconds(16000000 - elapsed_time));
	} // while (!shutdown)
} // object_movement