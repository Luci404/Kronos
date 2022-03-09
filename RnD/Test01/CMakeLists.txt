cmake_minimum_required(VERSION 3.10)


# glad
project(glad)
set(GLAD_DIRECTORY "${CMAKE_SOURCE_DIR}/Kronos/Vendor/glad")
add_library(glad "${GLAD_DIRECTORY}/include/glad/glad.h" "${GLAD_DIRECTORY}/include/KHR/khrplatform.h" "${GLAD_DIRECTORY}/src/glad.c")
target_include_directories(glad PUBLIC "${GLAD_DIRECTORY}/include")

# glfw
add_subdirectory("${CMAKE_SOURCE_DIR}/Kronos/Vendor/glfw")

# vulkan
add_library(vulkan INTERFACE)
set(VULKAN_PATH "C:/VulkanSDK/1.2.170.0")       
target_include_directories(vulkan INTERFACE "${VULKAN_PATH}/Include")
target_link_libraries(vulkan INTERFACE "${VULKAN_PATH}/Lib/vulkan-1.lib")

# vma
add_library(vma INTERFACE)
target_include_directories(vma INTERFACE "${CMAKE_SOURCE_DIR}/Kronos/Vendor/vma/include")
target_link_libraries(vma INTERFACE vulkan)

# imgui
project(imgui)
set(IMGUI_DIRECTORY "${CMAKE_SOURCE_DIR}/../../Kronos/Vendor/imgui")
file(GLOB IMGUI_HEADERS "${IMGUI_DIRECTORY}/*.h")
file(GLOB IMGUI_SOURCES "${IMGUI_DIRECTORY}/*.cpp")
add_library(imgui "${IMGUI_HEADERS}" "${IMGUI_SOURCES}" "${IMGUI_DIRECTORY}/backends/imgui_impl_win32.cpp" "${IMGUI_DIRECTORY}/backends/imgui_impl_vulkan.cpp")
target_include_directories(imgui PUBLIC "${IMGUI_DIRECTORY}" "${IMGUI_DIRECTORY}/examples")
target_link_libraries(imgui vulkan)

# Kronos
project(Kronos)
file(GLOB_RECURSE KRONOS_HEADERS "${CMAKE_SOURCE_DIR}/Kronos/Source/*.h")
file(GLOB_RECURSE KRONOS_SOURCES "${CMAKE_SOURCE_DIR}/Kronos/Source/*.cpp")
add_library(Kronos STATIC "${KRONOS_HEADERS}" "${KRONOS_SOURCES}")
set_property(TARGET Kronos PROPERTY CXX_STANDARD 20)
set_target_properties(Kronos PROPERTIES LINKER_LANGUAGE CXX)
include_directories(AFTER "${CMAKE_SOURCE_DIR}/Kronos/Source")

target_link_libraries(Kronos PUBLIC "opengl32.lib")
target_link_libraries(Kronos PUBLIC "glu32.lib")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
target_link_libraries(Kronos PUBLIC glfw)
target_link_libraries(Kronos PUBLIC vma)
target_link_libraries(Kronos PUBLIC glad)
set_target_properties(glad PROPERTIES FOLDER "Dependencies/glad")

add_subdirectory("Kronos/Vendor/glm")
include_directories("${CMAKE_SOURCE_DIR}/Kronos/Vendor/glm")   
                       
target_link_libraries(Kronos PRIVATE vulkan)

target_link_libraries(Kronos PUBLIC imgui) 
set_target_properties(imgui PROPERTIES FOLDER "Dependencies/imgui")

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