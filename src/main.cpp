#include <glm/glm.hpp>
#include <GLEW/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glfw/glfw3.h>
#include <cstdio>
#include <vector>
#include <string>
#define _USE_MATH_DEFINES
#include<math.h>

#include "scolor.hpp"

#include "camera.hpp"
#include "player.hpp"
#include "shader.hpp"
#include "object.hpp"

#include "render_3d_component.hpp"
#include "earth.hpp"
#include "cube.hpp"
//#include "crosshair.hpp"

/* Window Data */

int SCRN_WIDTH = 1920;
int SCRN_HEIGHT = 1080;

/* Game Data */

std::vector<object*> objects;

frustum main_frustum = frustum(65.0f, 0.1f, 100.0f);
camera main_camera = camera();
player main_player = player();

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
);

int main(void) {
    /* Initialize GLFW */
    if (!glfwInit())
        return 1;

    glfwSetErrorCallback(glfw_error_callback);

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
    main_camera.m_transform->position = glm::vec3(0.0f, 0.0f, 10.0f);
    objects.push_back(&main_camera);

    shader* object_shader = new shader();
    object_shader->add(GL_VERTEX_SHADER, "src/shaders/loaded_obj_vertex_shader.glsl");
    object_shader->add(GL_FRAGMENT_SHADER, "src/shaders/loaded_obj_fragment_shader.glsl");
    object_shader->link();

    /*shader* crosshair_shader = new shader();
    crosshair_shader->add(GL_VERTEX_SHADER, "src/shaders/crosshair_vertex_shader.glsl");
    crosshair_shader->add(GL_FRAGMENT_SHADER, "src/shaders/crosshair_fragment_shader.glsl");
    crosshair_shader->link();

    crosshair cross = crosshair(crosshair_shader);
    objects.push_back(&cross);*/

    earth planet = earth(object_shader);
    //objects.push_back(&planet);

    cube bricks = cube(object_shader);
    objects.push_back(&bricks);

	/* Initialize objects */
	for (object* obj : objects) {
        if (!obj->init()) {
			puts(RED("Failed to init objects").c_str());

			return 1;
		}
	}

    /* Loop until the user closes the window */
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEBUG_OUTPUT);

    glm::mat4 view = glm::identity<glm::mat4>();
    glm::mat4 projection = glm::identity<glm::mat4>();
    glm::mat4 vp = glm::identity<glm::mat4>();

    while (!glfwWindowShouldClose(window)) {
		auto start = glfwGetTime();

        /* Poll for and process events */
        glfwPollEvents();

		/* Handle minimized window */
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) { continue; }

		/* Render Main */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Player Movement
        if (main_player.keys.w) { main_camera.m_transform->moveForward(main_player.movementSpeed, deltaTime); }
        if (main_player.keys.s) { main_camera.m_transform->moveBackward(main_player.movementSpeed, deltaTime); }
        if (main_player.keys.a) { main_camera.m_transform->moveLeft(main_player.movementSpeed, deltaTime); }
        if (main_player.keys.d) { main_camera.m_transform->moveRight(main_player.movementSpeed, deltaTime); }
        if (main_player.keys.space) { main_camera.m_transform->moveUp(main_player.movementSpeed, deltaTime); }
        if (main_player.keys.shift) { main_camera.m_transform->moveDown(main_player.movementSpeed, deltaTime); }

        /* Get the view and projection matrices */
		view = main_camera.getViewMatrix();
        projection = glm::perspective(glm::radians(main_frustum.fovDegrees), (float)SCRN_WIDTH / (float)SCRN_HEIGHT, main_frustum.near_plane, main_frustum.far_plane);
        vp = projection * view;

		for (object* obj : objects) {
			// Update render components static variables
			render_3d_component::vp = vp;
            render_3d_component::lightPos = glm::vec3(2.0f, 25.0f, 25.0f);
            render_3d_component::cameraPos = main_camera.m_transform->position;

            obj->update(deltaTime);
		}

		/* Swap front and back buffers */
        glfwSwapBuffers(window);

		/* Calculate the delaTime */
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
	} // Game Loop

	/* Deinitialize objects */
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
} // main

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static GLchar* getErrorString(GLenum errorCode) {
    switch (errorCode) {
        case GL_INVALID_VALUE:                  return (GLchar*)"Invalid Value";
        case GL_INVALID_ENUM:                   return (GLchar*)"Invalid Enum";
        case GL_INVALID_OPERATION:              return (GLchar*)"Invalid Operation";
        case GL_STACK_OVERFLOW:                 return (GLchar*)"Stack Overflow";
        case GL_STACK_UNDERFLOW:                return (GLchar*)"Stack Underflow";
        case GL_OUT_OF_MEMORY:                  return (GLchar*)"Out of Memory";
        case GL_INVALID_FRAMEBUFFER_OPERATION:  return (GLchar*)"Invalid Frame Buffer";
        default:                                return (GLchar*)"Unknown";
    }
}

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
    if (source == GL_NO_ERROR) return;

    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        getErrorString(source),
        type, severity, message
    );
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
	main_frustum.cameraZoom(yoffset);
} // scroll_callback

/**
* @brief Callback to handle mouse movement
*/
static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	// Re-center the mouse
	float center_x = SCRN_WIDTH / 2.0f;
	float center_y = SCRN_HEIGHT / 2.0f;

    glfwSetCursorPos(window, center_x, center_y);

	main_camera.pointCamera((float)xpos, (float)ypos, center_x, center_y, deltaTime);
} // mouse_callback