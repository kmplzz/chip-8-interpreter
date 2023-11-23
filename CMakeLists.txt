cmake_minimum_required(VERSION 3.27)
project(chip8_interpreter)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(chip8_interpreter main.cpp
        src/Chip8.cpp
        src/Chip8.h
        src/Renderer.cpp
        src/Renderer.h)
target_link_libraries(chip8_interpreter PRIVATE sfml-graphics)
target_compile_features(chip8_interpreter PRIVATE cxx_std_17)

install(TARGETS chip8_interpreter)