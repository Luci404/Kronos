cmake_minimum_required(VERSION 3.10)

# glad
project(glad)
set(GLAD_DIRECTORY "${CMAKE_SOURCE_DIR}/Kronos/Vendor/glad")
add_library(glad "${GLAD_DIRECTORY}/include/glad/glad.h" "${GLAD_DIRECTORY}/include/KHR/khrplatform.h" "${GLAD_DIRECTORY}/src/glad.c")
target_include_directories(glad PUBLIC "${GLAD_DIRECTORY}/include")

# glfw
add_subdirectory("${CMAKE_SOURCE_DIR}/Kronos/Vendor/glfw")

# Kronos
project(Kronos)
file(GLOB_RECURSE KRONOS_HEADERS "${CMAKE_SOURCE_DIR}/Kronos/Source/*.h")
file(GLOB_RECURSE KRONOS_SOURCES "${CMAKE_SOURCE_DIR}/Kronos/Source/*.cpp")
add_library(Kronos STATIC "${KRONOS_HEADERS}" "${KRONOS_SOURCES}")
set_property(TARGET Kronos PROPERTY CXX_STANDARD 20)
include_directories(AFTER "${CMAKE_SOURCE_DIR}/Kronos/Source")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
target_link_libraries(Kronos glfw)

target_link_libraries(Kronos glad)
set_target_properties(glad PROPERTIES FOLDER "Dependencies/glad")

get_property(TARGET_SOURCE_FILES TARGET Kronos PROPERTY SOURCES)
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/Kronos/Source" FILES ${TARGET_SOURCE_FILES})

# Kronos Editor
project(KronosEditor)
file(GLOB_RECURSE KRONOS_HEADERS "${CMAKE_SOURCE_DIR}/KronosEditor/Source/*.h")
file(GLOB_RECURSE KRONOS_SOURCES "${CMAKE_SOURCE_DIR}/KronosEditor/Source/*.cpp")
include_directories(AFTER "${CMAKE_SOURCE_DIR}/KronosEditor/Source")
add_executable(KronosEditor "${KRONOS_HEADERS}" "${KRONOS_SOURCES}")
target_link_libraries(KronosEditor Kronos)
set_property(TARGET KronosEditor PROPERTY CXX_STANDARD 20)

get_property(TARGET_SOURCE_FILES TARGET KronosEditor PROPERTY SOURCES)
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/KronosEditor/Source" FILES ${TARGET_SOURCE_FILES})