cmake_minimum_required(VERSION 4.0)
project(Iron VERSION 0.1.0 LANGUAGES CXX DESCRIPTION "Open source and easy to use 3D game engine.")
set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

set(IRON_DIR ${Iron_SOURCE_DIR})
set(IRON_SRC_DIR ${IRON_DIR}/src)
set(IRON_EXAMPLES_DIR ${IRON_DIR}/examples)

option(USE_VULKAN "Should we build the vulkan renderer?" ON)

option(BUILD_EXAMPLES "Should we build the engine examples?" ON)

if(USE_VULKAN)
	find_package(Vulkan REQUIRED)
endif()

if(EXISTS "${IRON_DIR}/lib/sdl/CMakeLists.txt")
	add_subdirectory("${IRON_DIR}/lib/sdl" EXCLUDE_FROM_ALL)
else()
	message(FATAL_ERROR "Could not find SDL Git submodule. You must run 'git submodule update --init --recursive' to install the submodules, then rerun CMake.")
endif()

if(EXISTS "${IRON_DIR}/lib/glm/CMakeLists.txt")
	add_subdirectory("${IRON_DIR}/lib/glm" EXCLUDE_FROM_ALL)
else()
	message(FATAL_ERROR "Could not find GLM Git submodule. You must run 'git submodule update --init --recursive'
	to install the submodules, then rerun CMake.")
endif()

find_package(Boost REQUIRED COMPONENTS log unordered variant filesystem)

file(GLOB IRON_SOURCE ${IRON_DIR}/src/*.cpp)
file(GLOB IRON_PRIVATE_HEADERS ${IRON_DIR}/src/*.hpp)
set(IRON_INCLUDE ${IRON_DIR}/include/)

add_library(IronEngine STATIC ${IRON_SOURCE} ${IRON_PRIVATE_HEADERS})

target_include_directories(IronEngine PUBLIC ${IRON_INCLUDE})
target_include_directories(IronEngine PRIVATE  ${IRON_PRIVATE_HEADERS} ${Boost_INCLUDE_DIRS} ${SDL3_INCLUDE_DIRS})
target_link_libraries(IronEngine PRIVATE 
	Boost::boost 
	Boost::log 
	Boost::unordered 
	Boost::variant 
	Boost::filesystem 
	SDL3::SDL3
)