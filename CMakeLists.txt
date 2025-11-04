cmake_minimum_required(VERSION 3.15)
project(levenshtein-myers-simd VERSION 1.0.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# SIMD flags
set(SIMD_FLAGS -march=armv8)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(bench)

enable_testing()
add_subdirectory(test)
