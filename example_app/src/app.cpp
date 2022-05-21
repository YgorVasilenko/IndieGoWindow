#include <IndieGoWindow.h>
#include <IndieGoUI.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Plain simple code for window creation
// Created window will hold a widget
#define WIDTH 1280
#define HEIGHT 800

GLFWwindow * screen;

using namespace IndieGo::Win;
using namespace IndieGo::UI;

extern Manager GUI;
#include <iostream>

std::vector<std::string> exampleLogText  = {
    "Hello! This is general IndieGoWindow log.",
    "Any entity, that uses IndieGo::Win::Window,",
    "may print there it's events, and this data will",
    "stay on screen until application is running!"
};

int main(){
  	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Window mainWin(WIDTH, HEIGHT);
    glViewport(0, 0, WIDTH, HEIGHT);


    // Printing in screen log widget
    for (auto line : exampleLogText) {
        mainWin.printInLog(line);
    }

    std::string checkVal;
    double lastScrollOffset = 0;
    while (!glfwWindowShouldClose(mainWin.getScreen())) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // obligatory call
        mainWin.onFrameStart();

        checkVal.clear();
        for (auto keyval : mainWin.keyboard.keys){
            checkVal.push_back(keyval.first);
            checkVal += ": " + std::to_string(keyval.second.pressed) + " ";
        }

        // Display input data on screen
        mainWin.printOnScreen(mainWin.name);
        mainWin.printOnScreen("cursor pos X:" + std::to_string(mainWin.mouse.x));
        mainWin.printOnScreen("cursor pos Y:" + std::to_string(mainWin.mouse.y));
        mainWin.printOnScreen("Keyboard buttons pressed: " + checkVal);
        if (mainWin.scrollOffset != (double)0)
            lastScrollOffset = mainWin.scrollOffset;
        mainWin.printOnScreen("Last scroll offset: " + std::to_string(lastScrollOffset));
        mainWin.printOnScreen("My home folder: " + mainWin.home_dir.string());

        GUI.drawFrameStart();
        GUI.displayWidgets(mainWin.name);
        GUI.drawFrameEnd();

        // obligatory call
        mainWin.onFrameEnd();

        glfwSwapBuffers(mainWin.getScreen());
	    glfwPollEvents();
    }
    return 0;
}
