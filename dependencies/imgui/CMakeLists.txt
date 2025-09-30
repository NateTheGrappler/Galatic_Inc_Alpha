cmake_minimum_required(VERSION 3.8)
project(imgui)

# Compile all .cpp files in the current directory
file(GLOB IMGUI_SOURCES "*.cpp")

# Create the actual library
add_library(imgui ${IMGUI_SOURCES})

# Include directories
target_include_directories(imgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# If using ImGui-SFML, add the static definition
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/imgui-SFML.cpp")
    target_compile_definitions(imgui PUBLIC SFML_STATIC)
endif()