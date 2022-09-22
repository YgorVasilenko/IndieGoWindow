#include <IndieGoWindow.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// on-screen auto logging, FPS counter
// 2 types of logging 
// - persistent -> usual log, could be flushed to file
// - per-frame call -> data collected in single line 
#include <IndieGoUI.h>

using namespace IndieGo::UI;
using namespace IndieGo::Win;

Manager GUI;

void IndieGo::Win::window_focus_callback(GLFWwindow* window, int focused) {

}

void IndieGo::Win::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    // don't process mouse clicks, if mouse is over any widget, but screen log.
    if (action && GUI.hoveredWidgets[Window::screens[window]->name] && GUI.hoveredWidgets[Window::screens[window]->name]->name != Window::screens[window]->name + "_screenLog"){
        return;
    }
    Window::screens[window]->mouse[button].pressed = action;
}

void IndieGo::Win::framebuffer_size_callback(GLFWwindow* window, int width, int height) {

}

void IndieGo::Win::window_size_callback(GLFWwindow* window, int width, int height) {
    Window::screens[window]->width = width;
    Window::screens[window]->height = height;

    GUI.screen_size.w = width;
    GUI.screen_size.h = height;
}

#include <iostream>
void IndieGo::Win::window_close_callback(GLFWwindow* window) {
    std::cout << "[WINDOW::INFO] calling window_close_callback for " << Window::screens[window]->name << "!" << std::endl;
};

void IndieGo::Win::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Window::screens[window]->keyboard[key].pressed = action;
    GUI.key_input(&Window::screens[window]->name, key, glfwGetKey(window, key) == GLFW_PRESS);
}

void IndieGo::Win::joystick_callback(int jid, int _event) {

}

void IndieGo::Win::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    Window::screens[window]->mouse.dX = (Window::screens[window]->mouse.x - xpos) / Window::screens[window]->width;
    Window::screens[window]->mouse.dY = (Window::screens[window]->mouse.y - ypos) / Window::screens[window]->height;

    Window::screens[window]->mouse.prevX = Window::screens[window]->mouse.x;
    Window::screens[window]->mouse.prevY = Window::screens[window]->mouse.y;

    Window::screens[window]->mouse.x = xpos;
    Window::screens[window]->mouse.y = ypos;
}

void IndieGo::Win::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    GUI.scroll(&Window::screens[window]->name, xoffset, yoffset);
    // don't process scroll, if mouse is over any widget, but screen log.
    if (GUI.hoveredWidgets[Window::screens[window]->name] && GUI.hoveredWidgets[Window::screens[window]->name]->name != Window::screens[window]->name + "_screenLog") {
        return;
    }
    Window::screens[window]->scrollOffset = yoffset;
}

void IndieGo::Win::char_callback(GLFWwindow* window, unsigned int codepoint) {
    GUI.char_input(&Window::screens[window]->name, codepoint);
}

void IndieGo::Win::window_iconify_callback(GLFWwindow* window, int iconified) {
	
}

void Window::onFrameStart() {

#ifdef INDIEGO_ENGINE_DEV
    for (auto windows : screens) {
        if (windows.second == this) {
            // make context current here to be able to use multiple windows in one app
            // TODO : get rid of iteration over map?
            glfwMakeContextCurrent(windows.first);
            break;
        }
    }
#endif

    // make shure, that log widget is NOT in focus
    if (GUI.getWidget(name + "_screenLog", name).focused){
        // TODO : set focus on previously selected widget
        // GUI.getWidget(name + "_systemLog", name).setFocus = true;
        if (GUI.prevFocusedWidgets[name])
            GUI.prevFocusedWidgets[name]->setFocus = true;
        else 
            GUI.getWidget(name + "_systemLog", name).setFocus = true;
    }
}

// logging
std::string screen_log_line = "_screen_log_line_";
std::string system_log_line = "_system_log_line_";

void Window::printInLog(const std::string & line) {
    std::string currLineName = sysLogLineName + std::to_string(system_log_lines_total);
    WIDGET & systemLog = GUI.widgets[name][systemLogName];
    UI_elements_map & UIMap = GUI.UIMaps[name];
    UIMap.addElement(currLineName, UI_STRING_LABEL, &systemLog);
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
        UIMap.addElement(currLineName, UI_STRING_LABEL, &screenLog);
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
    auto currentTime = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - frameStartTime).count();
    frameStartTime = currentTime;
    timeCounter += duration;
    if (timeCounter > 1000000.f){
        fps = framesCounter;
        framesCounter = 0;
        // use last measured frametime 
        frametime = duration;
        timeCounter = 0.0;
    }
    scrollOffset = 0;
}

// #include <iostream>

#ifdef _WIN32 // it seems there is no cpp cross-platform way to get executable path
#include <windows.h>
// #include <tchar.h>
#endif

#include <filesystem>
namespace fs = std::filesystem;

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
        mainScreen = screen;
        // GUI gets initialized with first created window
        GUI.init(name, screen);
    } else {
        GUI.addWindow(name, screen);
    }

    GUI.screen_size.w = width;
    GUI.screen_size.h = height;

    glfwSetCursorPosCallback(screen, cursor_position_callback);
    glfwSetMouseButtonCallback(screen, mouse_button_callback);
    glfwSetWindowSizeCallback(screen, window_size_callback);
    glfwSetWindowCloseCallback(screen, window_close_callback);
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

    // TODO : fix issue with screen log getting focused
    // so far just make screen log appear as quarter of screen
    screenLog.screen_region.w = 0.5f;
    screenLog.screen_region.h = 0.5f;
    screenLog.custom_style = true;
    screenLog.style.elements[UI_COLOR_WINDOW].a = 0;
    screenLog.border = false;
    screenLog.title = false;
    screenLog.minimizable = false;
    screenLog.scalable = false;
    screenLog.movable = false;


    // System log widget initialization
    systemLog.screen_region.x = 0.f;
    systemLog.screen_region.y = 0.75f;
    systemLog.screen_region.w = 0.5f;
    systemLog.screen_region.h = 0.5f;
    systemLog.custom_style = true;
    systemLog.style.elements[UI_COLOR_WINDOW].a = 75;

    systemLog.name = systemLogName;
    screenLog.name = screenLogName;
    GUI.addWidget(systemLog, name);
    GUI.addWidget(screenLog, name);
    logLineName = name + screen_log_line;
    sysLogLineName = name + system_log_line;

    // initialize frame time here, to have sane duration
    frameStartTime = std::chrono::high_resolution_clock::now();

    if (!parent) {
#ifdef _WIN32
        TCHAR binary_path_[MAX_PATH] = { 0 };
        GetModuleFileName(NULL, binary_path_, MAX_PATH);
        binary_path = std::string(binary_path_);

        SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#else
        home_dir = fs::current_path();
#endif
        home_dir = binary_path.parent_path();
        std::cout << "Hello! My homedir is " << home_dir.string() << std::endl;
    } else {
        home_dir = parent->home_dir;

        // switch context back to parent
        glfwMakeContextCurrent(parent->getScreen());
    }
}

IndieGo::Win::Window::~Window() {
    std::cout << "[WINDOW::INFO] destructor for " << name << " is called!" << std::endl;
    GUI.removeWindow(name, getScreen());
    screens.erase(
        screens.find(getScreen())
    );
    glfwDestroyWindow(getScreen());
}

bool IndieGo::Win::Window::gladInitialized = false;
std::map< GLFWwindow*, IndieGo::Win::Window* > IndieGo::Win::Window::screens = {};
GLFWwindow * IndieGo::Win::Window::mainScreen = NULL;