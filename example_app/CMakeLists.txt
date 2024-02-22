cmake_minimum_required(VERSION 3.10)

project(WindowDemo VERSION 1.0)

# Assume env vars set
if(NOT DEFINED INDIEGO_UI)
    file(TO_CMAKE_PATH $ENV{INDIEGO_UI_HOME} INDIEGO_UI)
endif()

if (NOT DEFINED INDIEGO_UI)
    message(FATAL_ERROR "INDIEGO_UI path is not defined! Please, provide with -DINDIEGO_UI")
endif()

set(RENDERER_INCLUDES .. ../os)
set(RENDERER_SRC ../IndieGoWindow.cpp src/app.cpp src/glad.cpp)

if (DEFINED INDIEGO_UI)
    set(RENDERER_INCLUDES ${RENDERER_INCLUDES} ${INDIEGO_UI})
    set(RENDERER_SRC ${RENDERER_SRC} ${INDIEGO_UI}/backends/Nuklear/renderer.cpp ${INDIEGO_UI}/IndieGoUI.cpp)
endif()

message(STATUS "RENDERER_INCLUDES: ${RENDERER_INCLUDES}")
message(STATUS "RENDERER_SRC: ${RENDERER_SRC}")

add_executable(
    WindowDemo
    ${RENDERER_SRC}
)

target_include_directories(
    WindowDemo
    PUBLIC ${RENDERER_INCLUDES}
)

# link glfw library
target_link_libraries(
    WindowDemo
    PUBLIC "../lib/glfw3"
)

target_compile_definitions(
    WindowDemo PUBLIC 
        NO_SERIALIZATION=1
        WIDTH=1920
        HEIGHT=1080
)

set(CMAKE_CXX_FLAGS_DEBUG " /std:c++17 ")