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

int SCRN_WIDTH = 1920;
int SCRN_HEIGHT = 1080;

/* Engine Data */

bool shutdown = false;

/* Game Data */

std::vector<obj_data*> objects;

frustum main_frustum = frustum(45.0, 0.1, 100.0);
camera main_camera = camera(glm::vec3(0.0, 0, 5.0), glm::vec2(0, 0), &main_frustum);
float camera_speed = 4.5f;
player main_player = player(&main_camera);

/* Frame Data */

double deltaTime = 0.0;
double lastFrame = 0.0;

/* Call Backs */

static void resize_callback(GLFWwindow* window, int width, int height);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void glfw_error_callback(int error, const char* description);
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void GLAPIENTRY
MessageCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
) {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message
    );
}

int main(void) {
    /* Initialize GLFW */
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(SCRN_WIDTH, SCRN_HEIGHT, "LimitedGL Engine", NULL, NULL);

    if (!window) {
        glfwTerminate();

        return 1;
    }

    /* Initialize GLFW */
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwMakeContextCurrent(window);
    glewInit();

    /* Callbacks */
    glfwSetFramebufferSizeCallback(window, resize_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glDebugMessageCallback(MessageCallback, 0);

    /* Objects */
	loaded_obj obj = loaded_obj("objects/cube.obj", "objects/textures/brick.jpg");
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
	// Movement Thread

    /* Loop until the user closes the window */
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEBUG_OUTPUT);

    while (!glfwWindowShouldClose(window)) {
		auto start = glfwGetTime();

        /* Poll for and process events */
        glfwPollEvents();

		/* Handle minimized window */
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            continue;
        }

		/* Render Main */
        glClearColor(0, 0, 0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Temporary Player Movement */
		// TODO: Move towards the direction the camera is facing
		// FIXME: There is some shenanigans going on with the camera movement, some axis are inverted?
        if (main_player.keys.w) { main_camera.cameraPos.z -= camera_speed * deltaTime; }
        if (main_player.keys.s) { main_camera.cameraPos.z += camera_speed * deltaTime; }
        if (main_player.keys.a) { main_camera.cameraPos.x -= camera_speed * deltaTime; }
        if (main_player.keys.d) { main_camera.cameraPos.x += camera_speed * deltaTime; }
		if (main_player.keys.space) { main_camera.cameraPos.y += camera_speed * deltaTime; }
		if (main_player.keys.shift) { main_camera.cameraPos.y -= camera_speed * deltaTime; }

        /* Get the view and projection matrices */
		glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(main_camera.cameraPos, main_camera.cameraPos + main_camera.cameraDirection, main_camera.cameraUp);
		glm::mat4 projection = glm::perspective(main_camera.camera_frustum->fov, SCRN_WIDTH / (float)SCRN_HEIGHT, main_camera.camera_frustum->near_plane, main_camera.camera_frustum->far_plane);
		
        glm::mat4 mvp = projection * view * model; // Apply the transformations from the player

		for (obj_data* obj : objects) {
			obj->draw(mvp);
		}

		/* Swap front and back buffers */
        glfwSwapBuffers(window);

		/* Calculate the delaTime */
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
	} // Game Loop

	/* Join the threads */
	//movement_thread.join();

	/* Deinitialize objects */
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
} // main

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

/**
* @brief Resize the window
*/
static void resize_callback(GLFWwindow* window, int width, int height) {
    ::SCRN_HEIGHT = height;
    ::SCRN_WIDTH = width;

    glViewport(0, 0, width, height);
} // resize_callback

/**
* @brief Callback to handle key presses
*/
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        shutdown = true;
        glfwSetWindowShouldClose(window, true);
    }

	// Player Movement
    main_player.keys.w = glfwGetKey(window, GLFW_KEY_W);
    main_player.keys.s = glfwGetKey(window, GLFW_KEY_S);
    main_player.keys.a = glfwGetKey(window, GLFW_KEY_A);
    main_player.keys.d = glfwGetKey(window, GLFW_KEY_D);
	main_player.keys.shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT);
	main_player.keys.space = glfwGetKey(window, GLFW_KEY_SPACE);
} // key_callback

/**
* @brief Callback to handle mouse scroll events
*/
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera_speed += (float)yoffset;
	camera_speed = camera_speed < 0.0f ? 0.0f : camera_speed;
} // scroll_callback

static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	// Clamp the mouse position to the window bounds
    double center_x = SCRN_WIDTH / 2.0f;
    double center_y = SCRN_HEIGHT / 2.0f;
    double dx = xpos - center_x;
    double dy = ypos - center_y;

    glfwSetCursorPos(window, center_x, center_y);

    float sensitivity = 0.1f;
    double xoffset = dx * sensitivity;
    double yoffset = dy * sensitivity;
	// FIXME: The axes need to be inverted here for some reason, probably something to do with trigonometry in camera update
    main_camera.cameraTarget.x += -yoffset;
    main_camera.cameraTarget.y += xoffset;

	main_camera.update();
} // mouse_callback