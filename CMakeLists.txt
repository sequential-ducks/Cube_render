cmake_minimum_required(VERSION 3.20...4.0)

# Exit if in-source build is attempted
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed! Please use a separate build directory.")
endif()

set(CMAKE_CXX_STANDARD 17)
# Ensure C++17 is strictly enforced
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
# Vendor-specific extensions are off
set(CMAKE_CXX_EXTENSIONS OFF) 

# Define source directories
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(CORE_DIR ${SRC_DIR}/core)
set(GRAPHICS_DIR ${SRC_DIR}/graphics)

# Collect all source files
file(GLOB_RECURSE SOURCES
    "${SRC_DIR}/*.cpp"
    "${CORE_DIR}/*.cpp"
    "${GRAPHICS_DIR}/*.cpp"
)

# Define project with C++ as language and source files
project(hello_3d LANGUAGES CXX)
# Collect all header files
include_directories(${CMAKE_SOURCE_DIR}/include)
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

# Set the source of the vcpkg package manager
set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/external/vcpkg/installed/x64-linux/")
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()

# Look for OpenGL on the host system
find_package(OpenGL REQUIRED)
# Check that OpenGL is included
if(OpenGL_FOUND)
    message(STATUS "OpenGL found, linking..")
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
else()
    # Stop compilation without OpenGL
    message(FATAL_ERROR "OpenGL not found. Please install the required OpenGL libraries.")
endif()

# Glad exposes openGL functionality
find_package(glad CONFIG REQUIRED)
if (glad_FOUND)
    message(STATUS "glad found at ${glad_DIR}")
    target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)
else()
    message(FATAL_ERROR "glad not found. Please install it via vcpkg.")
endif()

# Simple and fast multimedia library
find_package(SFML COMPONENTS Graphics Window System CONFIG REQUIRED)
if(SFML_FOUND)
    message(STATUS "SFML3 found at ${SFML_DIR}")
    target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics)
else()
    message(FATAL_ERROR "SFML3 not found. Install it with vcpkg.")
endif()

include(FetchContent)
# OpenGL Mathematics header library
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863 #release 1.0.1
)
FetchContent_MakeAvailable(GLM)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

# Include a module for checking link-time optimization
include(CheckIPOSupported)
# If link-time interprocedural optimization is supported
check_ipo_supported(RESULT ipo_supported LANGUAGES CXX)
# Set interprocedural optimization on if supported, otherwise default 
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ${ipo_supported})

# Enable debugging options for compiling in debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build: enabling debug symbols and warnings and testing options")
    target_compile_options(${PROJECT_NAME} PRIVATE -g -O0 -Wall -Wextra -Wpedantic)
    include(CTest)
    enable_testing()
endif()

# Display end message
message("Build successful!")
