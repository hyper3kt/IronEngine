cmake_minimum_required(VERSION 4.0)
project(Iron VERSION 0.1.0 LANGUAGES CXX DESCRIPTION "Open source and easy to use 3D game engine.")
set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

set(IRON_DIR ${Iron_SOURCE_DIR})
set(IRON_SRC_DIR ${IRON_DIR}/src)
set(IRON_EXAMPLES_DIR ${IRON_DIR}/examples)

option(USE_VULKAN "Should we build the vulkan renderer?" ON)

option(BUILD_EXAMPLES "Should we build the engine examples?" ON)

option(USE_SYSTEM_SDL3 "Should we use the SDL3 on the user's system?" OFF)
option(USE_SYSTEM_GLM "Should we use the GLM on the user's system?" OFF)

if(USE_VULKAN)
	find_package(Vulkan REQUIRED)
endif()

if(USE_SYSTEM_SDL3)
	find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
else()
	if(EXISTS "${IRON_DIR}/lib/sdl/CMakeLists.txt")
		add_subdirectory(lib/sdl EXCLUDE_FROM_ALL)
	else()
		message(FATAL_ERROR "Could not find SDL Git submodule. You must run 'git submodule update --init --recursive' to install the submodules, then rerun CMake.")
	endif()
endif()

if(USE_SYSTEM_GLM)
	find_package(glm REQUIRED)
else()
	if(EXISTS "${IRON_DIR}/lib/glm/CMakeLists.txt")
		add_subdirectory(lib/glm EXCLUDE_FROM_ALL)
	else()
		message(FATAL_ERROR "Could not find GLM Git submodule. You must run 'git submodule update --init --recursive'
		to install the submodules, then rerun CMake.")
	endif()
endif()

find_package(Boost REQUIRED COMPONENTS log)

file(GLOB_RECURSE IRON_SOURCES src/*.cpp)
add_executable(IronExampleMain ${IRON_SOURCES})

target_include_directories(IronExampleMain PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(IronExampleMain PRIVATE Boost::boost Boost::log)