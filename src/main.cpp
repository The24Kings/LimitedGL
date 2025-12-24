#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <GLEW/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glfw/glfw3.h>
#include <cstdio>
#include <thread>
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
#include "loaded_obj.hpp"
//#include "crosshair.hpp"

/* Window Data */

int SCRN_WIDTH = 1920;
int SCRN_HEIGHT = 1080;

/* Game Data */

std::vector<object*> objects;

frustum main_frustum = frustum(65.0f, 0.1f, 100.0f);
camera main_camera = camera();
player main_player = player();

bool enable_mouse = false;

/* Frame Data */

double deltaTime = 0.0;
double lastFrame = 0.0;

/* Call Backs */

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
static void resize_callback(GLFWwindow* window, int width, int height);
static void glfw_error_callback(int error, const char* description);
static void keyboard_input(GLFWwindow* window);
static void scroll_input(GLFWwindow* window);
static void mouse_input(GLFWwindow* window);

int main(void) {
    /* Initialize GLFW */
    if (!glfwInit())
        return 1;

    glfwSetErrorCallback(glfw_error_callback);

    /* Create a windowed mode window and its OpenGL context */
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    GLFWwindow* window = glfwCreateWindow(SCRN_WIDTH, SCRN_HEIGHT, "LimitedGL Engine", NULL, NULL);

    if (!window) {
        glfwTerminate();

        return 1;
    }

    /* Initialize GLFW */
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // V-Sync
    glewInit();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    if (!enable_mouse)
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
    else
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // Imgui States
    ImVec4 background = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

    /* Callbacks */
    glfwSetFramebufferSizeCallback(window, resize_callback);
	glDebugMessageCallback(MessageCallback, 0);

    /* Objects */
    main_camera.m_transform->position = glm::vec3(0.0f, 0.0f, 30.0f);
    objects.push_back(&main_camera);

    shader* loaded_obj_shader = new shader();
    loaded_obj_shader->add(GL_VERTEX_SHADER, "shaders/loaded_obj_vertex_shader.glsl");
    loaded_obj_shader->add(GL_FRAGMENT_SHADER, "shaders/loaded_obj_fragment_shader.glsl");
    loaded_obj_shader->link();

    shader* crosshair_shader = new shader();
    crosshair_shader->add(GL_VERTEX_SHADER, "shaders/crosshair_vertex_shader.glsl");
    crosshair_shader->add(GL_FRAGMENT_SHADER, "shaders/crosshair_fragment_shader.glsl");
    crosshair_shader->link();

    loaded_obj obj = loaded_obj("objects/earth.obj", "objects/textures/earth albedo.jpg", loaded_obj_shader);
    objects.push_back(&obj);

    //crosshair cross = crosshair(crosshair_shader);
    //objects.push_back(&cross);

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

    /* Threads */
    std::thread keyboard_input_thread(keyboard_input, window);
    std::thread mouse_movement_thread(mouse_input, window);
    std::thread scroll_thread(scroll_input, window);

    /* Main Loop */
    glm::mat4 view = glm::identity<glm::mat4>();
    glm::mat4 projection = glm::identity<glm::mat4>();
    glm::mat4 vp = glm::identity<glm::mat4>();

    while (!glfwWindowShouldClose(window)) {
		auto start = glfwGetTime();

        /* Poll for and process events */
        glfwPollEvents();

		/* Handle minimized window */
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

		/* Render Main */
        glClearColor(background.x * background.w, background.y * background.w, background.z * background.w, background.w);
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

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::ColorEdit3("", (float*)&background); // Edit 3 floats representing a color

            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // Imgui Render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

		/* Swap front and back buffers */
        glfwSwapBuffers(window);

		/* Calculate the delaTime */
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
	} // Game Loop

	/* Deinitialize objects */
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    /* Terminate Threads */
    keyboard_input_thread.join();
    mouse_movement_thread.join();
    scroll_thread.join();

    return 0;
} // main

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

/**
* @brief Resize the window
*/
static void resize_callback(GLFWwindow* window, int width, int height) {
    ::SCRN_HEIGHT = height;
    ::SCRN_WIDTH = width;

    glViewport(0, 0, width, height);
} // resize_callback

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

/**
* @brief Callback to handle key presses
*/
static void keyboard_input(GLFWwindow* window) {
    ImGuiIO& io = ImGui::GetIO();

    while (!glfwWindowShouldClose(window)) {
        if (io.WantCaptureKeyboard) {
            continue;
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            glfwSetWindowShouldClose(window, true);
        }

        if (ImGui::IsKeyPressed(ImGuiKey_F1)) {
            if (enable_mouse) // Disable
                ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
            else // Enable
                ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;

            enable_mouse = !enable_mouse;
        }

        // Player Movement
        main_player.keys.w = ImGui::IsKeyDown(ImGuiKey_W);
        main_player.keys.a = ImGui::IsKeyDown(ImGuiKey_A);
        main_player.keys.s = ImGui::IsKeyDown(ImGuiKey_S);
        main_player.keys.d = ImGui::IsKeyDown(ImGuiKey_D);
        main_player.keys.shift = ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift);
        main_player.keys.space = ImGui::IsKeyDown(ImGuiKey_Space);
    }
} // keyboard_input

/**
* @brief Callback to handle mouse movement
*/
static void mouse_input(GLFWwindow* window) { // FIXME: Isn't functioning right now, unable to clamp mouse to center and movement is really fast
    ImGuiIO& io = ImGui::GetIO();

    while (!glfwWindowShouldClose(window)) {
        if (io.WantCaptureMouse) {
            continue;
        }

        if (enable_mouse) {
            continue;
        }

        // Re-center the mouse
        float center_x = SCRN_WIDTH / 2.0f;
        float center_y = SCRN_HEIGHT / 2.0f;

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        glfwSetCursorPos(window, center_x, center_y);

        main_camera.pointCamera(io.MouseDelta.x, io.MouseDelta.y, center_x, center_y, deltaTime);
    }
} // mouse_input

/**
* @brief Callback to handle mouse scroll events
*/
static void scroll_input(GLFWwindow* window) {
    ImGuiIO& io = ImGui::GetIO();

    while (!glfwWindowShouldClose(window)) {
        if (ImGui::GetIO().WantCaptureMouse) {
            continue;
        }

        main_frustum.cameraZoom(io.MouseWheel);
    }
} // scroll_input