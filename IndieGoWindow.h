#ifndef INDIEGO_WINDOW_H_
#define INDIEGO_WINDOW_H_
#include <map>
#include <vector>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;
//
// 1. App should be able to maintain several windows
// 2. For each window following data should be maintained between frames:
//   - keyboard state 
//   - mouse buttons 
//   - cursor position
//

struct GLFWwindow;


namespace IndieGo {
    namespace UI {
        struct WIDGET;
    }
	namespace Win {
    
        // GLFW-dependent callbacks. TODO - add more
        extern void window_iconify_callback(GLFWwindow* window, int focused);
        extern void window_focus_callback(GLFWwindow* window, int focused);
        extern void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        extern void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        extern void window_size_callback(GLFWwindow* window, int width, int height);
        extern void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        extern void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
        extern void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
        extern void char_callback(GLFWwindow* window, unsigned int codepoint);
        extern void joystick_callback(int jid, int _event);
        
        struct ButtonState {
            bool pressed = false;
            // checks, if button become pressed at this exact frame
            bool checkPress() {
               	if (!pressed)
		            checkPressFlag = false;

                if (pressed && !checkPressFlag) {
                    checkPressFlag = true;
                    return true;
                }
                return false;
            }

            private:
                bool checkPressFlag = false;
        };

        struct Keyboard {
            std::map<int, ButtonState> keys;
            ButtonState & operator[](int keycode){
                return keys[keycode];
            }
        };

        struct Joystick {
            // TODO - joystick input
        };

        struct Mouse {
            double x = 0.0, dX = 0.0, prevX = 0.0, 
                y = 0.0, dY = 0.0, prevY = 0.0;
            std::map<int, ButtonState> keys;
            ButtonState & operator[](int keycode){
                return keys[keycode];
            }
        };

        struct Window {
            Keyboard keyboard;
            Mouse mouse;

            // Window is close to OS, so let it keep this data
            fs::path home_dir;

            double scrollOffset = 0;
            
            // system will send signals to app's GLFWwindow*
            // this will be global app's windows list
            static std::map< GLFWwindow*, Window* > screens;
            static bool gladInitialized;

            // time measurments
	        unsigned int fps = 0;
            double duration = 0.0;
            double frametime = 0.0;

            std::string name;
            int width, height;
            Window(const int & width_ = 1280, const int & height_ = 800, const std::string & name_ = "IndieGo Window", Window * parent = nullptr);
            GLFWwindow* getScreen() { 
                for (auto screen : screens){
                    if (screen.second == this)
                        return screen.first;
                }
                return nullptr; 
            }
            virtual void Update() {};
            virtual void Render() {};

            void onFrameStart();
            void onFrameEnd();

            // adds line to screen log per frame. 
            // Each frame screen log is cleared, and particular line will be 
            // displayed, only if passed to printOnScreen() during frame processing
            virtual void printOnScreen(const std::string & line);

            // adds line to system log. System log is persistent - all lines will 
            // stay, unless explicitly deleted by Window user. 
            // System log could be flushed to file.
            virtual void printInLog(const std::string & line);
            virtual void flushSystemLog(std::string & logPath) {
                // TODO
            };

        private:

            // logging
            std::string systemLogName, screenLogName, logLineName, sysLogLineName;

            // lines in screen log added during whole runtime
            unsigned int screen_log_lines_total = 0;

            // after each frame all screen log lines are set to ""
            // in next frame each call of printOnScreen() checks, 
            // if there are free log lines - they become taken for than frame.
            // if there are no free log lines, new line gets added to screen log.
            unsigned int screen_log_lines_taken = 0;

            unsigned int system_log_lines_total = 0;

            void clearScreenLog();

            // time measurement
            double frameStartTime = 0.0;
            double timeCounter = 0.0;
            unsigned int framesCounter = 0;
        };
    }
}
#endif