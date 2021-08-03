#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Game.h"
#include "ResourceManager.h"

#include <iostream>
#include <time.h>
#include <thread>
#include <chrono>

// GLFW function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);


// The Width of the screen
const unsigned int SCREEN_WIDTH = 800;
// The height of the screen
const unsigned int SCREEN_HEIGHT = 840;

Game Tetris(SCREEN_WIDTH, SCREEN_HEIGHT);

int main(int argc, char* argv[])
{
    srand(time(NULL));
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tetris", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // OpenGL configuration
    // --------------------
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // initialize game
    // ---------------
    Tetris.Init();

    // deltaTime variables
    // -------------------
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    int speed = 20;
    int speedCounter = 0;
    int tickCounter = 0;
    bool wasPiecePlaced = false;
    bool movePieceDown = false;

    while (!glfwWindowShouldClose(window))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Game tick
        speedCounter++;
        movePieceDown = (speedCounter == speed);

        // calculate delta time
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();

        // force current piece down for speed counter
        // -----------------
        if (movePieceDown)
        {
            Tetris.ForcePieceDown();
            speedCounter = 0;
            tickCounter++;
        }

        // every 30 ticks speed will decrease - making game harder (stops at 2)
        if (tickCounter == 30)
        {
            speed--;
            if (speed < 2)
            {
                speed = 2;
            }
            tickCounter = 0;
        }

        // reset speed if game is reset
        int gSpeed = Tetris.GetSpeed();
        if (gSpeed - speed > 1 || gSpeed == 21)
        {
            speed = 20;
        }

        // process the user input
        // -----------------
        Tetris.ProcessInput();

        // update game state
        // -----------------
        Tetris.Update(deltaTime, 21 - speed);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        Tetris.Render();

        glfwSwapBuffers(window);
    }

    // delete all resources as loaded using the resource manager
    // ---------------------------------------------------------
    ResourceManager::Clear();

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // when a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            Tetris.Keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            Tetris.Keys[key] = false;
            Tetris.KeysProcessed[key] = false;
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}