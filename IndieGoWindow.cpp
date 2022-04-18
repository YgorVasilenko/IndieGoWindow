#include <IndieGoWindow.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// TODO : integrate IndieGoUI, 
// on-screen auto logging, FPS counter
// 2 types of logging 
// - persistent -> usual log, could be flushed to file
// - per-frame call -> data collected in single line 
#include <IndieGoUI.h>

using namespace IndieGo::UI;
using namespace IndieGo::Win;

void IndieGo::Win::window_focus_callback(GLFWwindow* window, int focused) {

}

void IndieGo::Win::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Window::screens[window]->mouse[button].pressed = action;
}

void IndieGo::Win::framebuffer_size_callback(GLFWwindow* window, int width, int height) {

}

void IndieGo::Win::window_size_callback(GLFWwindow* window, int width, int height) {

}

void IndieGo::Win::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Window::screens[window]->keyboard[key].pressed = action;
}

void IndieGo::Win::joystick_callback(int jid, int _event) {

}

void IndieGo::Win::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    Window::screens[window]->mouse.dX = (Window::screens[window]->mouse.x - xpos) / Window::screens[window]->width;
    Window::screens[window]->mouse.dY = (Window::screens[window]->mouse.y - ypos) / Window::screens[window]->height;

    Window::screens[window]->mouse.x = xpos;
    Window::screens[window]->mouse.y = ypos;
}

void IndieGo::Win::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

}

void IndieGo::Win::char_callback(GLFWwindow* window, unsigned int codepoint) {

}

void IndieGo::Win::window_iconify_callback(GLFWwindow* window, int iconified) {
	
}

void Window::onFrameStart(){
    frameStartTime = glfwGetTime();
}

Manager GUI;

// logging
std::string screen_log_line = "_screen_log_line_";
std::string system_log_line = "_system_log_line_";

void Window::printInLog(const std::string & line) {
    std::string currLineName = sysLogLineName + std::to_string(system_log_lines_total);
    WIDGET & systemLog = GUI.widgets[name][systemLogName];
    UI_elements_map & UIMap = GUI.UIMaps[name];
    UIMap.addElement(currLineName, UI_STRING_LABEL, &systemLog, system_log_lines_total);
    UIMap[currLineName].text_align = LEFT;
    UIMap[currLineName].label = line;
    system_log_lines_total++;
}

void Window::printOnScreen(const std::string & line) {
    UI_elements_map & UIMap = GUI.UIMaps[name];
    std::string currLineName = logLineName + std::to_string(screen_log_lines_taken);

    if (screen_log_lines_taken >= screen_log_lines_total) {
        // TODO : check, if screen log widget should be extended
        WIDGET & screenLog = GUI.widgets[name][screenLogName];
        UIMap.addElement(currLineName, UI_STRING_LABEL, &screenLog, screen_log_lines_total);
        UIMap[currLineName].text_align = LEFT;
        screen_log_lines_total++;
    }

    UIMap[currLineName].label = line;
    screen_log_lines_taken++;
}

void Window::clearScreenLog() {
    if ( screen_log_lines_total == 0 ) return;
    std::string currLineName;
    UI_elements_map & UIMap = GUI.UIMaps[name];
    for (int i = 0; i < screen_log_lines_total; i++){
        currLineName = logLineName + std::to_string(i);
        UIMap[currLineName].label = "";
    }
    screen_log_lines_taken = 0;
}

void Window::onFrameEnd(){
    clearScreenLog();
    mouse.dX = 0;
    mouse.dY = 0;

    framesCounter++;
    duration = glfwGetTime() - frameStartTime;
    timeCounter += duration;
    if (timeCounter > 1.f){
        fps = framesCounter;
        framesCounter = 0;
        // use last measured frametime 
        frametime = duration;
        timeCounter = 0.0;
    }
}

#include <iostream>

IndieGo::Win::Window::Window(const int & width_, const int & height_, const std::string & name_, Window * parent){
    width = width_;
    height = height_;
    name = name_;
    GLFWwindow * screen = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    screens[ screen ] = this;
	glfwMakeContextCurrent(screen);
    if (!gladInitialized) {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }
        gladInitialized = true;

        // GUI gets initialized with first created window
        GUI.init(screen);
    }

    glfwSetCursorPosCallback(screen, cursor_position_callback);
    glfwSetMouseButtonCallback(screen, mouse_button_callback);
    glfwSetScrollCallback(screen, scroll_callback);
    glfwSetCharCallback(screen, char_callback);
    glfwSetKeyCallback(screen, key_callback);

    // initialize UIMap for this window
    // WIDGETS configured in *some* place of program,
    // then COPIED to UIMap. 
    WIDGET systemLog, screenLog;
    systemLogName = name + "_systemLog";
    screenLogName = name + "_screenLog";


    // Screen log initializarion
    screenLog.screen_region.x = 0;
	screenLog.screen_region.y = 0;
	screenLog.screen_region.w = width;
	screenLog.screen_region.h = height / 2;
	screenLog.custom_style = true;
	screenLog.style.elements[COLOR_WINDOW].a = 0;
	screenLog.border = false;
	screenLog.title = false;
	screenLog.minimizable = false;
	screenLog.scalable = false;
	screenLog.movable = false;


    // System log widget initialization
    systemLog.screen_region.x = 0;
	systemLog.screen_region.y = height - height / 4;
	systemLog.screen_region.w = width / 2;
	systemLog.screen_region.h = height / 4;
	systemLog.custom_style = true;
	systemLog.style.elements[COLOR_WINDOW].a = 75;

    systemLog.name = systemLogName;
    screenLog.name = screenLogName;
    GUI.addWidget(name, systemLog);
    GUI.addWidget(name, screenLog);
    logLineName = name + screen_log_line;
    sysLogLineName = name + system_log_line;
}

bool IndieGo::Win::Window::gladInitialized = false;
std::map< GLFWwindow*, IndieGo::Win::Window* > IndieGo::Win::Window::screens = {};