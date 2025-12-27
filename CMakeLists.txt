cmake_minimum_required(VERSION 4.0)
project(Iron VERSION 0.1.0 LANGUAGES CXX DESCRIPTION "Open source and easy to use 3D game engine.")
set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(IRON_DIR ${Iron_SOURCE_DIR})
set(IRON_SRC_DIR ${IRON_DIR}/src)
set(IRON_LIB_DIR ${IRON_DIR}/lib)

option(USE_VULKAN "Should we build the Vulkan renderer?" ON)

option(BUILD_EXAMPLES "Should we build the engine examples?" ON)

if(USE_VULKAN)
    find_package(Vulkan REQUIRED)
endif()

find_package(SDL3 QUIET)
find_package(glm QUIET)

if(EXISTS "${IRON_LIB_DIR}/sdl/CMakeLists.txt")
    add_subdirectory("${IRON_LIB_DIR}/sdl" EXCLUDE_FROM_ALL)
elseif(SDL3_FOUND)
    message(STATUS "Found SDL!")
else()
    message(FATAL_ERROR "Could not find SDL. Please either install SDL or run 'git submodule update --init --recursive' to install all submodules.")
endif()

if(EXISTS "${IRON_LIB_DIR}/glm/CMakeLists.txt")
    add_subdirectory("${IRON_LIB_DIR}/glm" EXCLUDE_FROM_ALL)
elseif(glm_FOUND)
    message(STATUS "Found GLM!")
else()
    message(FATAL_ERROR "Could not find GLM. Please either install GLM or run 'git submodule update --init --recursive' to install all submodules.")
endif()

find_package(Boost REQUIRED)

file(GLOB_RECURSE IRON_SOURCE ${IRON_DIR}/src/*.cpp)
file(GLOB_RECURSE IRON_PRIVATE_HEADERS ${IRON_DIR}/src/*.hpp)
set(IRON_INCLUDE ${IRON_DIR}/include/)

add_library(IronEngine SHARED ${IRON_SOURCE} ${IRON_PRIVATE_HEADERS} ${IRON_LIB_DIR}/glad/src/glad.c)

target_include_directories(IronEngine PUBLIC ${IRON_INCLUDE})
target_include_directories(IronEngine PRIVATE 
    ${IRON_PRIVATE_HEADERS}
    ${Boost_INCLUDE_DIRS}
    ${SDL3_INCLUDE_DIRS}
    ${IRON_LIB_DIR}/glad/include
)
target_link_libraries(IronEngine PRIVATE 
    Boost::boost
    SDL3::SDL3
)
if (USE_VULKAN)
    target_include_directories(IronEngine PRIVATE ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(IronEngine PRIVATE Vulkan::Vulkan)
endif()
