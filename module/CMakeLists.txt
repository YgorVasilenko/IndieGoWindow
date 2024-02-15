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
set(RENDERER_SRC ../IndieGoWindow.cpp glad.c)

if (DEFINED INDIEGO_UI)
    set(RENDERER_INCLUDES ${RENDERER_INCLUDES} ${INDIEGO_UI} ${PROTOBUF_SRC})
    set(RENDERER_SRC ${RENDERER_SRC} ${INDIEGO_UI}/backends/Nuklear/renderer.cpp ${INDIEGO_UI}/IndieGoUI.cpp ${INDIEGO_UI}/IndieGoUI.pb.cc ${INDIEGO_UI}/ui_editor/editor_data.pb.cc)
endif()

message(STATUS "RENDERER_INCLUDES: ${RENDERER_INCLUDES}")
message(STATUS "RENDERER_SRC: ${RENDERER_SRC}")

add_library(
    IndieGoWindow STATIC
    ${RENDERER_SRC}
)

target_include_directories(
    IndieGoWindow
    PUBLIC ${RENDERER_INCLUDES}
)

target_compile_definitions(
    IndieGoWindow PUBLIC 
        # NO_SERIALIZATION=1
        WIDTH=1920
        HEIGHT=1080
)

set(CMAKE_CXX_FLAGS_DEBUG " /std:c++17 /DEBUG /Zi /Od ")