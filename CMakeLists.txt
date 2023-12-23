cmake_minimum_required(VERSION 3.27)
project(raycaster LANGUAGES CXX)
include(CheckIPOSupported)
check_ipo_supported()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -O3 -march=native")
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)
add_subdirectory(glm)

add_executable(raycaster src/main.cpp
        src/App.cpp
        src/App.h
        src/Render.h
        src/Config.h
        src/Wall.h
        src/Floor.h
        src/Math.h)
target_link_libraries(raycaster PRIVATE sfml-graphics)
target_link_libraries(raycaster PRIVATE glm::glm)
target_compile_features(raycaster PRIVATE cxx_std_20)
set_property(TARGET raycaster PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)