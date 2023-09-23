#include <IndieGoWindow.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>

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
    Window & screen = *Window::screens[window];
    // don't process mouse clicks, if mouse is over any widget, but screen log.
    if (action) {
        if (GUI.hoveredWidgets[screen.name] && GUI.hoveredWidgets[screen.name]->name != screen.name + "_screenLog"){
            return;
        }
        if (screen.mouse[button].pressCallback.second)
            screen.mouse[button].pressCallback.second(
                screen.mouse[button].pressCallback.first
            );
    } else {
        if (screen.mouse[button].releaseCallback.second)
            screen.mouse[button].releaseCallback.second(
                screen.mouse[button].releaseCallback.first
            );
    }
    screen.mouse[button].pressed = action;
}

void IndieGo::Win::framebuffer_size_callback(GLFWwindow* window, int width, int height) {

}

void IndieGo::Win::window_size_callback(GLFWwindow* window, int width, int height) {
    Window & screen = *Window::screens[window];
    screen.width = width;
    screen.height = height;

    GUI.screen_size.w = width;
    GUI.screen_size.h = height;
}

#include <iostream>
void IndieGo::Win::window_close_callback(GLFWwindow* window) {
    Window & screen = *Window::screens[window];
    std::cout << "[WINDOW::INFO] calling window_close_callback for " << screen.name << "!" << std::endl;
};

void IndieGo::Win::takeScreenshot(GLFWwindow* window) {
    std::vector<PixelData> data;
    Window & screen = *Window::screens[window];

    data.resize( screen.width * screen.height );
    data.data();
    glReadPixels(0, 0, screen.width, screen.height, GL_BGRA, GL_UNSIGNED_BYTE, data.data());
    std::vector< PixelData > rgbadata(screen.width * screen.height);

    // vertical flip
    for (int y = 0; y < screen.height; y++) {
        for (int x = 0; x < screen.width; x++) {
            rgbadata[ y * screen.width + x ] = data[ y * screen.width + ( screen.width - x - 1 ) ];
        }
    }

    // horizontal flip
    std::reverse(
        rgbadata.begin(),
        rgbadata.end()
    );

    glfwSetClipboardBitmap(
        reinterpret_cast<unsigned char*>(rgbadata.data()), 
        screen.width, 
        screen.height
    );
}

void IndieGo::Win::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    bool isPressed = glfwGetKey(window, key) == GLFW_PRESS;
    Window & screen = *Window::screens[window];

    screen.keyboard[key].pressed = isPressed;
    screen.keyboard.lastPressedKey = key;
    screen.keyboard.pressFlag = true;

    if (isPressed) {
        if (screen.keyboard[key].pressCallback.second)
            screen.keyboard[key].pressCallback.second(
                screen.keyboard[key].pressCallback.first
            );
        
        if (key == GLFW_KEY_PRINT_SCREEN && Window::screens[window]->isFullscreen()) {
            IndieGo::Win::takeScreenshot(window);
        }
    } else { 
        if (screen.keyboard[key].releaseCallback.second)
            screen.keyboard[key].pressCallback.second(
                screen.keyboard[key].pressCallback.first
            );
    }
    GUI.key_input(&screen.name, key, isPressed);
}

void IndieGo::Win::joystick_callback(int jid, int _event) {
    if (_event == GLFW_CONNECTED) {
		// The joystick was connected
		Window::attached_joysticks[jid] = 1;
		if (Window::main_joystick == -1)
			Window::main_joystick = jid;
	} else if (_event == GLFW_DISCONNECTED) {
		// The joystick was disconnected
		Window::attached_joysticks[jid] = -1;
		if (Window::main_joystick == jid) {
            Window::main_joystick = -1;

			// Go through all joystick ports, get attached
			for (int i = 0; i < GLFW_JOYSTICK_LAST; i++) {
				if (Window::attached_joysticks[i] != -1) {
                    Window::main_joystick = Window::attached_joysticks[i];
                    break;
                };
			}
		}
	}
}

void IndieGo::Win::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    Window & screen = *Window::screens[window];
    screen.mouse.dX = (screen.mouse.x - xpos) / screen.width;
    screen.mouse.dY = (screen.mouse.y - ypos) / screen.height;

    screen.mouse.prevX = screen.mouse.x;
    screen.mouse.prevY = screen.mouse.y;

    screen.mouse.x = xpos;
    screen.mouse.y = ypos;
}

void IndieGo::Win::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    GUI.scroll(&Window::screens[window]->name, xoffset, yoffset);
    Window & screen = *Window::screens[window];
    // don't process scroll, if mouse is over any widget, but screen log.
    if (GUI.hoveredWidgets[screen.name] && GUI.hoveredWidgets[screen.name]->name != screen.name + "_screenLog") {
        return;
    }
    screen.scrollOffset = yoffset;
}

void IndieGo::Win::char_callback(GLFWwindow* window, unsigned int codepoint) {
    GUI.char_input(&Window::screens[window]->name, codepoint);
}

void IndieGo::Win::window_iconify_callback(GLFWwindow* window, int iconified) {
	
}

void Window::restore() {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    for (auto windows : screens) {
        if (windows.second == this) {
            glfwSetWindowMonitor(windows.first, nullptr, winPos[0], winPos[1], width, height, 0);
            break;
        }
    }
    _fullscreen = false;
}

void Window::goFullscreen() {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    for (auto windows : screens) {
        if (windows.second == this) {
            glfwGetWindowPos(windows.first, &winPos[0], &winPos[1]);
            glfwSetWindowMonitor(windows.first, monitor, 0, 0, width, height, 0);
            break;
        }
    }
    _fullscreen = true;
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

    // joysticks input fetch
    GLFWgamepadstate state;
    for (int i = 0; i < GLFW_JOYSTICK_LAST; i++) {
        if (attached_joysticks[i] != -1) {
            glfwGetGamepadState(i, &state);
            // fetch joystick buttons
            for (int j = 0; j < 15; j++) {
                joystick_state[i][j].pressed = state.buttons[j];
            }

            // fetch axes
            for (int j = 0; j < 5; j++) {
                joystick_state[i].sticks_input[j] = state.axes[j];
            }
        }
    }
}

// logging
std::string screen_log_line = "_screen_log_line_";
std::string system_log_line = "_system_log_line_";

void Window::printInLog(const std::string & line) {
#if !defined RELEASE_BUILD || defined EDITOR
    std::string currLineName = sysLogLineName + std::to_string(system_log_lines_total);
    WIDGET & systemLog = GUI.widgets[name][systemLogName];
    UI_elements_map & UIMap = GUI.UIMaps[name];
    UIMap.addElement(currLineName, UI_STRING_LABEL, &systemLog);
    UIMap[currLineName].text_align = LEFT;
    UIMap[currLineName].label = line;
    system_log_lines_total++;
    systemLog.updateRowHeight(system_log_lines_total - 1, 0.04f);
#endif
}

void Window::printOnScreen(const std::string & line) {
#if !defined RELEASE_BUILD || defined EDITOR
    UI_elements_map & UIMap = GUI.UIMaps[name];
    std::string currLineName = logLineName + std::to_string(screen_log_lines_taken);

    if (screen_log_lines_taken >= screen_log_lines_total) {
        // TODO : check, if screen log widget should be extended
        WIDGET & screenLog = GUI.widgets[name][screenLogName];
        UIMap.addElement(currLineName, UI_STRING_LABEL, &screenLog);
        UIMap[currLineName].text_align = LEFT;
        screen_log_lines_total++;
        screenLog.updateRowHeight(screen_log_lines_total - 1, 0.04f);
    }

    UIMap[currLineName].label = line;
    screen_log_lines_taken++;
#endif
}

void Window::clearScreenLog() {
#if !defined RELEASE_BUILD || defined EDITOR
    if ( screen_log_lines_total == 0 ) return;
    std::string currLineName;
    UI_elements_map & UIMap = GUI.UIMaps[name];
    for (int i = 0; i < screen_log_lines_total; i++){
        currLineName = logLineName + std::to_string(i);
        UIMap[currLineName].label = "";
    }
    screen_log_lines_taken = 0;
#endif
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

    // this resetted at the end of frame, because callbacks evoked before onFrameStart()
    keyboard.pressFlag = false;
    
    if (shouldClose) {
        GLFWwindow* m_screen = NULL;
        for (auto screen : screens) {
            if (screen.second->name == name) {
                m_screen = screen.first;
                break;
            }
        }
        glfwSetWindowShouldClose(m_screen, 1);
    }
}

void IndieGo::Win::Window::toggleVsync() {
    _vsync = !_vsync;
    glfwSwapInterval(_vsync);
}

// #include <iostream>

#ifdef _WIN32 // it seems there is no cpp cross-platform way to get executable path
#include <windows.h>
// #include <tchar.h>
#endif

#include <filesystem>
namespace fs = std::filesystem;

IndieGo::Win::Window::Window(const int & width_, const int & height_, const std::string & name_, Window * parent, bool fullscreen){
    width = width_;
    height = height_;
    name = name_;
    _fullscreen = fullscreen;

    GLFWwindow* screen = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    // screen->monitor;
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
        std::cout << glfwGetVersionString() << std::endl;  
    } else {
        GUI.addWindow(name, screen);
    }

    if (fullscreen) {
        goFullscreen();
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
    glfwSetJoystickCallback(joystick_callback);

    // vsync on by default
    glfwSwapInterval(_vsync);

#if !defined RELEASE_BUILD || defined EDITOR
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
    screenLog.screen_region.y = 0.1f;
    screenLog.screen_region.w = 0.5f;
    screenLog.screen_region.h = 0.5f;
    screenLog.custom_style = true;
    screenLog.style.elements[UI_COLOR_WINDOW].a = 0;
    screenLog.border = false;
    screenLog.title = false;
    screenLog.minimizable = false;
    screenLog.scalable = false;
    screenLog.movable = false;
    screenLog.has_scrollbar = false;
    screenLog.style.elements[UI_COLOR_TEXT].r = 255;
    screenLog.style.elements[UI_COLOR_TEXT].g = 255;
    screenLog.style.elements[UI_COLOR_TEXT].b = 255;
    screenLog.forceNoFocus = true;

    // System log widget initialization
    systemLog.screen_region.x = 0.f;
    systemLog.screen_region.y = 0.75f;
    systemLog.screen_region.w = 0.5f;
    systemLog.screen_region.h = 0.5f;
    systemLog.custom_style = true;
    systemLog.style.elements[UI_COLOR_WINDOW].a = 75;
    systemLog.style.elements[UI_COLOR_TEXT].r = 255;
    systemLog.style.elements[UI_COLOR_TEXT].g = 255;
    systemLog.style.elements[UI_COLOR_TEXT].b = 255;

    systemLog.name = systemLogName;
    screenLog.name = screenLogName;
    GUI.addWidget(systemLog, name);
    GUI.addWidget(screenLog, name);
    logLineName = name + screen_log_line;
    sysLogLineName = name + system_log_line;
#endif

    // initialize frame time here, to have sane duration
    frameStartTime = std::chrono::high_resolution_clock::now();

#ifdef _WIN32
    // get locale
    if (GetUserDefaultLCID() == 0x0419){
        locale = LANG_LOCALE::rus;
    } else {
        locale = LANG_LOCALE::eng;
    }
    
#endif
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

    // try loading mappings
    std::ifstream mappings_data(fs::path(home_dir).append("gamecontrollerdb.txt"), std::ios::binary);
    if (mappings_data.is_open()) {
        mappings_data.seekg(0, mappings_data.end);
        std::size_t size = mappings_data.tellg();
        char* mappings = new char[size];
        mappings_data.seekg(0, mappings_data.beg);
        mappings_data.read(mappings, size);
        glfwUpdateGamepadMappings(mappings);
    }

    // check if joystick is plugged
    for (int i = 0; i < GLFW_JOYSTICK_LAST; i++) {
        attached_joysticks[i] = -1;
        if (glfwJoystickPresent(i) && glfwJoystickIsGamepad(i)) {
            attached_joysticks[i] = 0;
            if (main_joystick == -1)
                main_joystick = i;
        }
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
std::unordered_map< GLFWwindow*, IndieGo::Win::Window* > IndieGo::Win::Window::screens = {};
GLFWwindow * IndieGo::Win::Window::mainScreen = NULL;
int IndieGo::Win::Window::attached_joysticks[GLFW_JOYSTICK_LAST] = { 0 };
int IndieGo::Win::Window::main_joystick = -1;