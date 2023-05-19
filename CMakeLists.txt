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

add_executable(vulkan-triangle ${source-files})

target_include_directories (
    vulkan-triangle PUBLIC
    "../Vendor/SDL/include/"
)

target_link_libraries
(
    vulkan-triangle PRIVATE
    SDL3
)