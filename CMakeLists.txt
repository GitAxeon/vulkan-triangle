cmake_minimum_required(VERSION 3.24)
project(vulkan-triangle)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin-int)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin-int)

file(
    GLOB source-files
    "src/*.cpp"
    "src/*.hpp"
)

find_package(Vulkan REQUIRED FATAL_ERROR)
add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)

add_executable(vulkan-triangle ${source-files})

target_include_directories(
    vulkan-triangle PUBLIC 
    ../vendor/SDL/include/ 
    C:/VulkanSDK/1.3.216.0/Include/
)

target_link_libraries(
    vulkan-triangle PRIVATE
    SDL3::SDL3
    Vulkan::Vulkan
)