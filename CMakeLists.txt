cmake_minimum_required(VERSION 3.10)

# ImGui
project(imgui)
set(IMGUI_DIRECTORY "${CMAKE_SOURCE_DIR}/Kronos/Vendor/imgui")
file(GLOB IMGUI_HEADERS "${IMGUI_DIRECTORY}/*.h")
file(GLOB IMGUI_SOURCES "${IMGUI_DIRECTORY}/*.cpp")
add_library(imgui "${IMGUI_HEADERS}" "${IMGUI_SOURCES}")
target_include_directories(imgui PUBLIC "${IMGUI_DIRECTORY}" "${IMGUI_DIRECTORY}/examples")

# Kronos
project(Kronos)
file(GLOB_RECURSE KRONOS_HEADERS "${CMAKE_SOURCE_DIR}/Kronos/Source/*.h")
file(GLOB_RECURSE KRONOS_SOURCES "${CMAKE_SOURCE_DIR}/Kronos/Source/*.cpp")
include_directories(AFTER "${CMAKE_SOURCE_DIR}/Kronos/Source")
add_executable(Kronos ${KRONOS_HEADERS} ${KRONOS_SOURCES})
target_include_directories(Kronos PUBLIC "${CMAKE_SOURCE_DIR}/Kronos/Vendor")
target_link_libraries(Kronos PUBLIC imgui)