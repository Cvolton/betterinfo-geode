cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(BetterInfo VERSION 1.0.0)

# Use GLOB_RECURSE instead of GLOB
# to recursively add all source files 
# under src/ 
file(GLOB_RECURSE SOURCES
	src/*.cpp
)

# Set up the mod binary
add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (PROJECT_IS_TOP_LEVEL)
    target_compile_definitions(${PROJECT_NAME} PRIVATE BI_EXPORTING)
endif()

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} $ENV{GEODE_SDK}/build)

# Set up dependencies, resources, link Geode
setup_geode_mod(${PROJECT_NAME} EXTERNALS geode.node-ids:1.14.0 cvolton.misc_bugfixes:1.1.3)
