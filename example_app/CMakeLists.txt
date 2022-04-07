cmake_minimum_required(VERSION 3.10)

project(Window_SM VERSION 1.0)

set(RENDERER_INCLUDES .. ../os)
set(RENDERER_SRC ../IndieGoWindow.cpp src/app.cpp src/glad.c)

if (DEFINED INDIEGO_UI)
    set(RENDERER_INCLUDES ${RENDERER_INCLUDES} ${INDIEGO_UI})
    set(RENDERER_SRC ${RENDERER_SRC} ${INDIEGO_UI}/backends/Nuklear/renderer.cpp)
endif()

message(STATUS "RENDERER_INCLUDES: ${RENDERER_INCLUDES}")
message(STATUS "RENDERER_SRC: ${RENDERER_SRC}")

add_executable(
    Window_SM
    ${RENDERER_SRC}
)

target_include_directories(
    Window_SM
    PUBLIC ${RENDERER_INCLUDES}
)

# link glfw library
target_link_libraries(
    Window_SM
    PUBLIC "../lib/glfw3"
)
