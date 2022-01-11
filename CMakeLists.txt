cmake_minimum_required(VERSION 3.10)

# spdlog
add_subdirectory("${CMAKE_SOURCE_DIR}/Kronos/Vendor/spdlog")

# glad
project(glad)
set(GLAD_DIRECTORY "${CMAKE_SOURCE_DIR}/Kronos/Vendor/glad")
add_library(glad "${GLAD_DIRECTORY}/include/glad/glad.h" "${GLAD_DIRECTORY}/include/KHR/khrplatform.h" "${GLAD_DIRECTORY}/src/glad.c")
target_include_directories(glad PUBLIC "${GLAD_DIRECTORY}/include")

# glfw
add_subdirectory("${CMAKE_SOURCE_DIR}/Kronos/Vendor/glfw")

# imgui
project(imgui)
set(IMGUI_DIRECTORY "${CMAKE_SOURCE_DIR}/Kronos/Vendor/imgui")
file(GLOB IMGUI_HEADERS "${IMGUI_DIRECTORY}/*.h")
file(GLOB IMGUI_SOURCES "${IMGUI_DIRECTORY}/*.cpp")
add_library(imgui "${IMGUI_HEADERS}" "${IMGUI_SOURCES}" "${IMGUI_DIRECTORY}/backends/imgui_impl_glfw.cpp" "${IMGUI_DIRECTORY}/backends/imgui_impl_opengl3.cpp")
target_include_directories(imgui PUBLIC "${IMGUI_DIRECTORY}" "${IMGUI_DIRECTORY}/examples")
target_link_libraries(imgui glfw)

# Kronos
project(Kronos)
file(GLOB_RECURSE KRONOS_HEADERS "${CMAKE_SOURCE_DIR}/Kronos/Source/*.h")
file(GLOB_RECURSE KRONOS_SOURCES "${CMAKE_SOURCE_DIR}/Kronos/Source/*.cpp")
include_directories(AFTER "${CMAKE_SOURCE_DIR}/Kronos/Source")
add_library(Kronos SHARED "${KRONOS_HEADERS}" "${KRONOS_SOURCES}")
set_property(TARGET Kronos PROPERTY CXX_STANDARD 17)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES "${KRONOS_HEADERS}" "${KRONOS_SOURCES}")


target_link_libraries(Kronos spdlog)
set_target_properties(spdlog PROPERTIES FOLDER "Dependencies/spdlog")

target_link_libraries(Kronos glad)
set_target_properties(glad PROPERTIES FOLDER "Dependencies/glad")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
target_link_libraries(Kronos glfw)
set_target_properties(glfw PROPERTIES FOLDER "Dependencies/glfw")

target_link_libraries(Kronos imgui)
set_target_properties(imgui PROPERTIES FOLDER "Dependencies/imgui")