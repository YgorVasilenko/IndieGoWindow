cmake_minimum_required(VERSION 3.10)

project(WindowDemo VERSION 1.0)

# Assume env vars set
if(NOT DEFINED INDIEGO_UI)
    file(TO_CMAKE_PATH $ENV{INDIEGO_UI_HOME} INDIEGO_UI)
endif()

if (NOT DEFINED INDIEGO_UI)
    message(FATAL_ERROR "INDIEGO_UI path is not defined! Please, provide with -DINDIEGO_UI")
endif()

set(DEMO_INCLUDES .. ../os)
set(DEMO_SRC src/app.cpp)

set(CMAKE_MODULE_PATH $ENV{CMAKE_MODULE_PATH})
include(system)

message(STATUS "DEMO_INCLUDES: ${DEMO_INCLUDES}")
message(STATUS "DEMO_SRC: ${DEMO_SRC}")

add_executable(
    WindowDemo WIN32 MACOSX_BUNDLE
    ${DEMO_SRC}
)

target_include_directories(
    WindowDemo
    PUBLIC ${DEMO_INCLUDES} ${INDIEGO_UI}
)

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES Darwin)
    set_source_files_properties(
        src/glfw.icns PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
    )

    set_target_properties(
        WindowDemo PROPERTIES
            MACOSX_BUNDLE_BUNDLE_NAME "WindowDemo"
            MACOSX_BUNDLE_SHORT_VERSION_STRING ${GLFW_VERSION}
            MACOSX_BUNDLE_LONG_VERSION_STRING ${GLFW_VERSION}
            MACOSX_BUNDLE_ICON_FILE src/glfw.icns
            MACOSX_BUNDLE_INFO_PLIST "${GLFW_SOURCE_DIR}/CMake/Info.plist.in"
    )

    target_compile_options(
        WindowDemo PUBLIC 
            -framework OpenGL -std=c++17 -O0 -g
    )

    # link library
    target_link_libraries(
        WindowDemo
        PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../module/build/libIndieGoWindow.a
        PUBLIC glfw
        PUBLIC OpenGL::GL
        PUBLIC ${PROTOBUF_LIBS}
    )
else()
    target_link_libraries(
        WindowDemo
        PUBLIC "../lib/glfw3"

        # serialization
        PUBLIC ${PROTOBUF_LIB}/libprotobufd.lib
    )    
endif()

target_compile_definitions(
    WindowDemo PUBLIC 
        NO_SERIALIZATION=1
        WIDTH=1920
        HEIGHT=1080
        NDEBUG=1
)
