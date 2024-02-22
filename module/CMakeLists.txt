cmake_minimum_required(VERSION 3.10)

project(IndieGoWindow VERSION 1.0)

# Assume env vars set
if(NOT DEFINED INDIEGO_UI)
    file(TO_CMAKE_PATH $ENV{INDIEGO_UI_HOME} INDIEGO_UI)
endif()

set(PROTOBUF_SRC $ENV{PROTOBUF_HOME}/src)

if (NOT DEFINED INDIEGO_UI)
    message(FATAL_ERROR "INDIEGO_UI path is not defined! Please, provide with -DINDIEGO_UI")
endif()

set(RENDERER_INCLUDES .. ../os)
set(RENDERER_SRC ../IndieGoWindow.cpp glad.cpp)

if (DEFINED INDIEGO_UI)
    set(RENDERER_INCLUDES ${RENDERER_INCLUDES} ${INDIEGO_UI} ${PROTOBUF_SRC})
    set(
        RENDERER_SRC 
        ${RENDERER_SRC} 
        ${INDIEGO_UI}/backends/Nuklear/renderer.cpp 
        ${INDIEGO_UI}/IndieGoUI.cpp 
        ${INDIEGO_UI}/IndieGoUI.pb.cc 
        ${INDIEGO_UI}/ui_editor/editor_data.pb.cc
    )
endif()

message(STATUS "CMAKE_HOST_SYSTEM_NAME: ${CMAKE_HOST_SYSTEM_NAME}")

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES Windows)
    set(CMAKE_CXX_FLAGS_DEBUG " /std:c++17 /DEBUG /Zi /Od ")
else()
    # on mac os glfw staticalli linked to window
    set(GLFW_LIB_NAME libglfw3.a)
    set(GLFW_VERSION_MAJOR 3) 
    set(GLFW_VERSION_MINOR 4)
    set(GLFW_VERSION 3.4)
    set(GLFW_SOURCE_DIR $ENV{GLFW_PATH})
    set(GLAD_GL "${GLFW_SOURCE_DIR}/deps")

    # glfw requirements
    find_package(Threads REQUIRED)
    find_package(OpenGL REQUIRED)

    add_subdirectory($ENV{GLFW_PATH} $ENV{GLFW_PATH}/build)
endif()

message(STATUS "RENDERER_INCLUDES: ${RENDERER_INCLUDES}")
message(STATUS "RENDERER_SRC: ${RENDERER_SRC}")

add_library(
    IndieGoWindow STATIC
    ${RENDERER_SRC}
)

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES Darwin)
    target_compile_options(
        IndieGoWindow PUBLIC 
            -framework OpenGL -std=c++17 -O0 -g
    )

    target_link_libraries(
        IndieGoWindow
        PUBLIC glfw
    )
endif()

target_include_directories(
    IndieGoWindow
    PUBLIC ${RENDERER_INCLUDES}
)

target_compile_definitions(
    IndieGoWindow PUBLIC 
        NDEBUG=1
)