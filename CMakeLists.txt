cmake_minimum_required(VERSION 3.27)
project(raycaster LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(raycaster src/main.cpp
        src/App.cpp
        src/App.h)
target_link_libraries(raycaster PRIVATE sfml-graphics)
target_compile_features(raycaster PRIVATE cxx_std_17)