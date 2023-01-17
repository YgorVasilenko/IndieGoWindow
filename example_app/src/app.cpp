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

#define GLFW_GAMEPAD_BUTTON_A               0
#define GLFW_GAMEPAD_BUTTON_B               1
#define GLFW_GAMEPAD_BUTTON_X               2
#define GLFW_GAMEPAD_BUTTON_Y               3
#define GLFW_GAMEPAD_BUTTON_LEFT_BUMPER     4
#define GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER    5
#define GLFW_GAMEPAD_BUTTON_BACK            6
#define GLFW_GAMEPAD_BUTTON_START           7
#define GLFW_GAMEPAD_BUTTON_GUIDE           8
#define GLFW_GAMEPAD_BUTTON_LEFT_THUMB      9
#define GLFW_GAMEPAD_BUTTON_RIGHT_THUMB     10
#define GLFW_GAMEPAD_BUTTON_DPAD_UP         11
#define GLFW_GAMEPAD_BUTTON_DPAD_RIGHT      12
#define GLFW_GAMEPAD_BUTTON_DPAD_DOWN       13
#define GLFW_GAMEPAD_BUTTON_DPAD_LEFT       14
#define GLFW_GAMEPAD_BUTTON_LAST            GLFW_GAMEPAD_BUTTON_DPAD_LEFT

#define BUTTON_CROSS       GLFW_GAMEPAD_BUTTON_A
#define BUTTON_CIRCLE      GLFW_GAMEPAD_BUTTON_B
#define BUTTON_SQUARE      GLFW_GAMEPAD_BUTTON_X
#define BUTTON_TRIANGLE    GLFW_GAMEPAD_BUTTON_Y

#define AXIS_LEFT_X        0
#define AXIS_LEFT_Y        1
#define AXIS_RIGHT_X       2
#define AXIS_RIGHT_Y       3
#define AXIS_LEFT_TRIGGER  4
#define AXIS_RIGHT_TRIGGER 5
#define AXIS_LAST          AXIS_RIGHT_TRIGGER

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
        /*mainWin.printOnScreen("cursor pos X:" + std::to_string(mainWin.mouse.x));
        mainWin.printOnScreen("cursor pos Y:" + std::to_string(mainWin.mouse.y));*/
        // mainWin.printOnScreen("Keyboard buttons pressed: " + checkVal);
        /*if (mainWin.scrollOffset != (double)0)
            lastScrollOffset = mainWin.scrollOffset;*/
        // mainWin.printOnScreen("Last scroll offset: " + std::to_string(lastScrollOffset));
        // mainWin.printOnScreen("My home folder: " + mainWin.home_dir.string());
        
        /*std::string systemLocale = mainWin.locale == IndieGo::LANG_LOCALE::rus ? "russian" : "english";
        mainWin.printOnScreen("Default system locale: " + systemLocale);*/
        // mainWin.printOnScreen("main joystick: " + std::to_string(Window::main_joystick));

        if (Window::main_joystick != -1) {
            mainWin.printOnScreen("cross: " + std::to_string(mainWin.joystick_state[Window::main_joystick][BUTTON_CROSS].pressed));
            mainWin.printOnScreen("left x axis:" + std::to_string(mainWin.joystick_state[Window::main_joystick].sticks_input[Gamepad::AXES::lx]));
            mainWin.printOnScreen("left y axis:" + std::to_string(mainWin.joystick_state[Window::main_joystick].sticks_input[Gamepad::AXES::ly]));

            mainWin.printOnScreen("right x axis:" + std::to_string(mainWin.joystick_state[Window::main_joystick].sticks_input[Gamepad::AXES::rx]));
            mainWin.printOnScreen("right y axis:" + std::to_string(mainWin.joystick_state[Window::main_joystick].sticks_input[Gamepad::AXES::ry]));
        }

        GUI.drawFrameStart(mainWin.name);
        GUI.displayWidgets(mainWin.name);
        GUI.drawFrameEnd(mainWin.name);

        // obligatory call
        mainWin.onFrameEnd();

        glfwSwapBuffers(mainWin.getScreen());
	    glfwPollEvents();
    }
    return 0;
}
