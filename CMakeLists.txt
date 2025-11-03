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
		add_subdirectory("${IRON_DIR}/lib/sdl" EXCLUDE_FROM_ALL)
	else()
		message(FATAL_ERROR "Could not find SDL Git submodule. You must run 'git submodule update --init --recursive' to install the submodules, then rerun CMake.")
	endif()
endif()

if(USE_SYSTEM_GLM)
	find_package(glm REQUIRED)
else()
	if(EXISTS "${IRON_DIR}/lib/glm/CMakeLists.txt")
		add_subdirectory("${IRON_DIR}/lib/glm" EXCLUDE_FROM_ALL)
	else()
		message(FATAL_ERROR "Could not find GLM Git submodule. You must run 'git submodule update --init --recursive'
		to install the submodules, then rerun CMake.")
	endif()
endif()

find_package(Boost REQUIRED COMPONENTS log unordered variant filesystem)

file(GLOB_RECURSE IRON_SOURCE ${IRON_DIR}/src/*.cpp)
file(GLOB_RECURSE IRON_HEADERS ${IRON_DIR}/src/*.hpp)
set(IRON_INCLUDE ${IRON_DIR}/include/)

add_library(IronEngine STATIC ${IRON_SOURCE} ${IRON_HEADERS})

target_include_directories(IronEngine PUBLIC ${IRON_INCLUDE})
target_include_directories(IronEngine PRIVATE  ${IRON_HEADERS} ${Boost_INCLUDE_DIRS} ${SDL3_INCLUDE_DIRS})
target_link_libraries(IronEngine PRIVATE 
	Boost::boost 
	Boost::log 
	Boost::unordered 
	Boost::variant 
	Boost::filesystem 
	SDL3::SDL3
)