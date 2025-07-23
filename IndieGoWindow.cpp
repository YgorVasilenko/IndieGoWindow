#include <IndieGoWindow.h>
#include <glad/glad.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>

// on-screen auto logging, FPS counter
#include <IndieGoUI.h>
#include <Renderer.h>

using namespace IndieGo::vkI;

#ifndef WIDTH 
#define WIDTH 1920
#endif

#ifndef HEIGHT 
#define HEIGHT 1080
#endif

using namespace IndieGo::UI;
using namespace IndieGo::Win;

// using namespace std;

Keyboard Window::keyboard = {};
Mouse Window::mouse = {};
Gamepad Window::joystick_state[MAX_GAMEPADS] = {};
extern Manager GUI;

void IndieGo::Win::window_focus_callback(GLFWwindow* window, int focused) {}

void IndieGo::Win::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    // don't process mouse clicks, if mouse is over any widget, but screen log.
    if (action) {
        if (Manager::hoveredWidget && Manager::hoveredWidget->name != App::appWindow.name + "_screenLog"){
            Manager::mouse_button(window, button, action, mods);
            return;
        }
        if (Window::mouse[button].pressCallback.second)
            Window::mouse[button].pressCallback.second(
                Window::mouse[button].pressCallback.first
            );
    } else {
        if (Window::mouse[button].releaseCallback.second)
            Window::mouse[button].releaseCallback.second(
                Window::mouse[button].releaseCallback.first
            );
    }
    Window::mouse[button].pressed = action;
}

void IndieGo::Win::framebuffer_size_callback(GLFWwindow* window, int width, int height) {}

void IndieGo::Win::window_size_callback(GLFWwindow* window, int width, int height) {
    App::appWindow.width = width;
    App::appWindow.height = height;

    Manager::screen_size.w = width;
    Manager::screen_size.h = height;
}

void IndieGo::Win::window_close_callback(GLFWwindow* window) {
    std::cout << "[WINDOW::INFO] calling window_close_callback for " << App::appWindow.name << "!" << std::endl;
};

void IndieGo::Win::takeScreenshot(GLFWwindow* window) {
    std::vector<PixelData> data;
    data.resize( App::appWindow.width * App::appWindow.height );
    data.data();
    glReadPixels(0, 0, App::appWindow.width, App::appWindow.height, GL_BGRA, GL_UNSIGNED_BYTE, data.data());
    std::vector< PixelData > rgbadata(App::appWindow.width * App::appWindow.height);

    // vertical flip
    for (int y = 0; y < App::appWindow.height; y++) {
        for (int x = 0; x < App::appWindow.width; x++) {
            rgbadata[ y * App::appWindow.width + x ] = data[ y * App::appWindow.width + ( App::appWindow.width - x - 1 ) ];
        }
    }

    // horizontal flip
    std::reverse(
        rgbadata.begin(),
        rgbadata.end()
    );

    glfwSetClipboardBitmap(
        reinterpret_cast<unsigned char*>(rgbadata.data()), 
        App::appWindow.width, 
        App::appWindow.height
    );
}

void IndieGo::Win::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    bool isPressed = glfwGetKey(window, key) == GLFW_PRESS;

    App::appWindow.keyboard[key].pressed = isPressed;
    App::appWindow.keyboard.lastPressedKey = key;
    App::appWindow.keyboard.pressFlag = true;

    if (isPressed) {
        if (App::appWindow.keyboard[key].pressCallback.second)
            App::appWindow.keyboard[key].pressCallback.second(
                App::appWindow.keyboard[key].pressCallback.first
            );
        
        if (key == GLFW_KEY_PRINT_SCREEN && App::appWindow.isFullscreen()) {
            IndieGo::Win::takeScreenshot(window);
        }
    } else { 
        if (App::appWindow.keyboard[key].releaseCallback.second)
            App::appWindow.keyboard[key].pressCallback.second(
                App::appWindow.keyboard[key].pressCallback.first
            );
    }
    if (Window::keyCallback)
        Window::keyCallback(key);
    
    Manager::key_input(window, key, scancode, action, mods);
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
    App::appWindow.mouse.dX = (App::appWindow.mouse.x - xpos) / App::appWindow.width;
    App::appWindow.mouse.dY = (App::appWindow.mouse.y - ypos) / App::appWindow.height;

    App::appWindow.mouse.prevX = App::appWindow.mouse.x;
    App::appWindow.mouse.prevY = App::appWindow.mouse.y;

    App::appWindow.mouse.x = xpos;
    App::appWindow.mouse.y = ypos;
}

void IndieGo::Win::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (IndieGo::Win::Window::scrollCallback) {
        IndieGo::Win::Window::scrollCallback(nullptr);
    }

    // don't process scroll, if mouse is over any widget, but screen log.
    if (Manager::hoveredWidget && Manager::hoveredWidget->name != App::appWindow.name + "_screenLog") {
        Manager::scroll(window, xoffset, yoffset);
        return;
    }
    App::appWindow.scrollOffset = yoffset;
}

void IndieGo::Win::char_callback(GLFWwindow* window, unsigned int codepoint) {
    Manager::char_input(window, codepoint);
}

void IndieGo::Win::window_iconify_callback(GLFWwindow* window, int iconified) {}

void Window::restore() {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
    glfwSetWindowMonitor(window, nullptr, winPos[0], winPos[1], width, height, 0);

    _fullscreen = false;
    _borderless = false;
}

void Window::goBorderless() {
    if (_borderless)
        return;

    if (_fullscreen) {
        restore();
    }
    GLFWmonitor * monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode * mode = glfwGetVideoMode(monitor);
    glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
    glfwSetWindowPos(window, (mode->width - width) / 2, (mode->height - height) / 2);
    _borderless = true;
}

void Window::goFullscreen() {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    glfwGetWindowPos(window, &winPos[0], &winPos[1]);
    glfwSetWindowMonitor(window, monitor, 0, 0, width, height, 0);

    _fullscreen = true;
    _borderless = false;
}

void Window::onFrameStart() {
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
void Window::printOnScreen(const std::string & line) {
    UI_elements_map & UIMap = Manager::UIMap;
    std::string currLineName = logLineName + std::to_string(screen_log_lines_taken);

    if (screen_log_lines_taken >= screen_log_lines_total) {
        // TODO : check, if screen log widget should be extended
        WIDGET & screenLog = Manager::widgets[screenLogName];
        UIMap.addElement(currLineName, UI_STRING_LABEL, &screenLog);
        UIMap[currLineName].text_align = LEFT;
        screen_log_lines_total++;
        screenLog.updateRowHeight(screen_log_lines_total - 1, 0.04f);
    }

    UIMap[currLineName].label = line;
    screen_log_lines_taken++;
}

void Window::clearScreenLog() {
    if ( screen_log_lines_total == 0 ) return;
    std::string currLineName;
    UI_elements_map & UIMap = Manager::UIMap;
    for (int i = 0; i < screen_log_lines_total; i++){
        currLineName = logLineName + std::to_string(i);
        UIMap[currLineName].label = "";
    }
    screen_log_lines_taken = 0;
}

void Window::onFrameEnd() {
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
        glfwSetWindowShouldClose(window, 1);
    }
}

void IndieGo::Win::Window::toggleVsync() {
    _vsync = !_vsync;
    glfwSwapInterval(_vsync);
}

void IndieGo::Win::Window::create(const int & width_, const int & height_, const std::string & name_, bool fullscreen) {
    width = width_;
    height = height_;
    name = name_;
    _fullscreen = fullscreen;
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
	// glfwMakeContextCurrent(window);
    // std::cout << glfwGetVersionString() << std::endl;
}

void IndieGo::Win::Window::init() {
    if (_fullscreen) {
        goFullscreen();
    }

    Manager::screen_size.w = width;
    Manager::screen_size.h = height;

    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetWindowCloseCallback(window, window_close_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCharCallback(window, char_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetJoystickCallback(joystick_callback);

    // vsync on by default
    glfwSwapInterval(_vsync);

    WIDGET screenLog;
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
    screenLog.name = screenLogName;
    _GUI = &GUI;
    _GUI->addWidget(screenLog);
    _GUI->ignoreWidgets.insert(screenLogName);
    logLineName = name + screen_log_line;

    // initialize frame time here, to have sane duration
    frameStartTime = std::chrono::high_resolution_clock::now();

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
    glfwDestroyWindow(window);
}

int IndieGo::Win::Window::attached_joysticks[GLFW_JOYSTICK_LAST] = { 0 };
int IndieGo::Win::Window::main_joystick = -1;
void (*IndieGo::Win::Window::scrollCallback)(void*) = nullptr;
void (*IndieGo::Win::Window::keyCallback)(unsigned int) = nullptr;
IndieGo::Win::Window IndieGo::App::appWindow = {};

// App items:
#ifdef _WIN32 // it seems there is no cpp cross-platform way to get executable path
#include <windows.h>
#endif

void IndieGo::App::initLocale() {
#ifdef _WIN32
    // get locale
    if (GetUserDefaultLCID() == 0x0419){
        locale = LANG_LOCALE::rus;
    } else {
        locale = LANG_LOCALE::eng;
    }
#endif
};


void IndieGo::App::initHomedirs() {
#ifdef _WIN32
    TCHAR binary_path_[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, binary_path_, MAX_PATH);
    binary_path = std::string(binary_path_);

    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#else
    home_dir = binary_path.parent_path();
#endif
    std::cout << "IndieGo::App homedir is " << home_dir.string() << std::endl;

    // try loading mappings
    std::ifstream mappings_data(fs::path(home_dir).append("gamecontrollerdb.txt"), std::ios::binary);
    if (mappings_data.is_open()) {
        mappings_data.seekg(0, mappings_data.end);
        size_t size = mappings_data.tellg();
        char* mappings = new char[size];
        mappings_data.seekg(0, mappings_data.beg);
        mappings_data.read(mappings, size);
        glfwUpdateGamepadMappings(mappings);
    }

    // project_dir = getenv("PROJECT_DIR");
    project_dir = home_dir.string();
    std::cout << "Using PROJECT_DIR: " << project_dir << '\n' << std::flush;
    scenes_dir = project_dir + "/Scenes/";
    common_dir = scenes_dir + "/common/";
    Manager::project_dir = project_dir;
}

void IndieGo::App::run() {
    while (!glfwWindowShouldClose(appWindow.window)) {
        appWindow.onFrameStart();
        processFrame();

        Manager::drawFrameStart();
        Manager::displayWidgets();
        drawFrame();

        appWindow.onFrameEnd();
        glfwSwapBuffers(appWindow.window);
	    glfwPollEvents();
    }
}

void resizeUI() {
    Manager::resize(
        IndieGo::App::appWindow.width,
        IndieGo::App::appWindow.height
    );
}

void IndieGo::App::init() {
    vkRenderer::resizeCallback = resizeUI;
    appWindow.create(WIDTH, HEIGHT);
    initLocale();
    initHomedirs();

    vkI::window = appWindow.window;
    initRenderingPipelines();
    Manager::init(vkI::window, uiCanvHolder);
    appWindow.init();
    initAppData();
}

void IndieGo::App::drawFrame() {
    vkRenderer::acquireImageFromSwapChain(uiCanvHolder.get());

    // Render UI
    Manager::currFrame = vkRenderer::currFrame;
    Manager::drawFrameEnd();
    drawAppData();
    vkRenderer::presentImageToScreen(uiCanvHolder.get());
}

// TODO :
// - "default" rendering pipeline with imageView for Ui
// - screen_quad + inputs
// - add items to inputs in editors and game