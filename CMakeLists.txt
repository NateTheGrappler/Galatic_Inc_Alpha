cmake_minimum_required(VERSION 3.8)

project(Galatic_Inc_Alpha)
set(CMAKE_CXX_STANDARD 20)

# Windows-specific settings
if(WIN32)
    if (POLICY CMP0141)
        cmake_policy(SET CMP0141 NEW)
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
    endif()

    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# SFML setup - manual configuration
set(SFML_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/SFML")
include_directories(${SFML_ROOT}/include)

if(WIN32)
    link_directories(${SFML_ROOT}/lib)
    add_definitions(-DSFML_STATIC)
else()
    # On Linux, try both lib and lib64 directories
    link_directories(${SFML_ROOT}/lib ${SFML_ROOT}/lib64)
endif()

# Add ImGui subdirectory
add_subdirectory(dependencies/imgui)

# Source files
file(GLOB_RECURSE MY_SOURCES CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(Galatic_Inc_Alpha ${MY_SOURCES})

target_include_directories(Galatic_Inc_Alpha PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

target_compile_definitions(Galatic_Inc_Alpha PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/")

# Platform-specific linking
if(WIN32)
    target_link_libraries(Galatic_Inc_Alpha
        PRIVATE
        # SFML Release libraries
        $<$<CONFIG:Release>:sfml-graphics-s>
        $<$<CONFIG:Release>:sfml-window-s>
        $<$<CONFIG:Release>:sfml-system-s>
        $<$<CONFIG:Release>:sfml-audio-s>
        
        # SFML Debug libraries  
        $<$<CONFIG:Debug>:sfml-graphics-s-d>
        $<$<CONFIG:Debug>:sfml-window-s-d>
        $<$<CONFIG:Debug>:sfml-system-s-d>
        $<$<CONFIG:Debug>:sfml-audio-s-d>
        
        # ImGui
        imgui
        
        # SFML static dependencies
        opengl32
        winmm
        gdi32
        freetype
        openal32
        flac
        vorbisenc
        vorbisfile
        vorbis
        ogg
    )
else()
    # Linux linking
    target_link_libraries(Galatic_Inc_Alpha
        PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
        imgui
        GL
        pthread
        openal
    )
endif()

set_property(TARGET Galatic_Inc_Alpha PROPERTY CXX_STANDARD 20)