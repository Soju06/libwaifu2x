cmake_minimum_required(VERSION 3.9)
cmake_policy(SET CMP0091 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(waifu2x-test DESCRIPTION "waifu2x test")

set (CMAKE_CXX_STANDARD 23)

get_filename_component(CMAKE_BINARY_DIR ./build ABSOLUTE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Threads)
find_package(OpenMP)
find_package(Vulkan REQUIRED)

add_subdirectory(../../src ./library/libwaifu2x)

file(GLOB SOURCE_CODES "*.h" "*.cpp")
add_executable(waifu2x-test ${SOURCE_CODES})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(waifu2x-test PRIVATE ../../src)
target_link_libraries(waifu2x-test libwaifu2x ncnn)

file(COPY ${WAIFU2X_MODEL_DIR} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})