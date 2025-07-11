cmake_minimum_required(VERSION 3.10)

project(WindowDemo VERSION 1.0)
set(CMAKE_MODULE_PATH $ENV{VK_RENDERER_HOME})
include(system)

file(GLOB RENDERER_SRC ${VK_RENDERER_HOME}/renderer/*.cpp)
file(GLOB EDITOR_SRC ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

set(
    INCLUDES 
    ${OS_HOME}
    ${DEPS_HOME}
    ${SHADERS_HOME}
    ${VK_RENDERER_HOME}/renderer/
    ${UI_HOME}/
    ${UI_HOME}/backend/
    ${PROTOBUF_HOME}/src
    ${VULKAN_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/../
    # ${STB_HOME}
)

set(
    SRC
    ${EDITOR_SRC}
    ${RENDERER_SRC}
    ${OS_HOME}/glad/glad.cpp
    ${UI_HOME}/IndieGoUI.cpp
    ${UI_HOME}/backend/renderer.cpp
    ${SHADERS_HOME}/Shader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../IndieGoWindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../default_pipeline.cpp
)

message("SRC: ${SRC}")
message("INCLUDES: ${INCLUDES}")

add_executable(
    WindowDemo MACOSX_BUNDLE
    ${SRC}
)

target_include_directories(
    WindowDemo
    PUBLIC 
    ${INCLUDES}
)


# link glfw and opencv libraries
target_link_libraries(
    WindowDemo
    PUBLIC
        ${VULKAN_LIBS}
        ${PROTOBUF_LIB}
)

target_compile_definitions(
    WindowDemo PUBLIC
    WIDTH=1920
    HEIGHT=1080
    UI_FLT_VAL_SCALE=100
    NO_SERIALIZATION=1
    NDEBUG=1
)

target_compile_options(
    WindowDemo PUBLIC 
        ${COMPILE_FLAGS}
)