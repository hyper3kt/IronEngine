cmake_minimum_required(VERSION 4.0)
project(Iron VERSION 0.1.0 LANGUAGES CXX DESCRIPTION "Open source and easy to use 3D game engine.")
set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

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
    message(STATUS "Found SDL3 system installation")
else()
    message(FATAL_ERROR "Could not find SDL Git submodule. You must run 'git submodule update --init --recursive' to install the submodules, then rerun CMake.")
endif()

if(EXISTS "${IRON_LIB_DIR}/glm/CMakeLists.txt")
    add_subdirectory("${IRON_LIB_DIR}/glm" EXCLUDE_FROM_ALL)
elseif(glm_FOUND)
    message(STATUS "Found GLM system installation")
else()
    message(FATAL_ERROR "Could not find GLM Git submodule. You must run 'git submodule update --init --recursive' to install the submodules, then rerun CMake.")
endif()

find_package(Boost REQUIRED)

file(GLOB IRON_SOURCE ${IRON_DIR}/src/*.cpp)
file(GLOB IRON_PRIVATE_HEADERS ${IRON_DIR}/src/*.hpp)
set(IRON_INCLUDE ${IRON_DIR}/include/)

add_library(IronEngine STATIC ${IRON_SOURCE} ${IRON_PRIVATE_HEADERS})

target_include_directories(IronEngine PUBLIC ${IRON_INCLUDE})
target_include_directories(IronEngine PRIVATE 
    ${IRON_PRIVATE_HEADERS}
    ${Boost_INCLUDE_DIRS}
    ${SDL3_INCLUDE_DIRS}
)
target_link_libraries(IronEngine PRIVATE 
    Boost::boost
    SDL3::SDL3
)
if (USE_VULKAN)
    target_include_directories(IronEngine PRIVATE ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(IronEngine PRIVATE Vulkan::Vulkan)
else()
	target_include_directories(IronEngine PRIVATE ${IRON_LIB_DIR}/glad/include)
	target_sources(IronEngine PRIVATE ${IRON_LIB_DIR}/glad/src)
endif()
