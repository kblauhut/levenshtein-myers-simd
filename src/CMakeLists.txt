# Create the library
add_library(levenshtein-myers-simd
    levenshtein_myers_16x8.cpp
    levenshtein_myers_32x4.cpp
    levenshtein_myers_64x2.cpp
)

# Include directories
target_include_directories(levenshtein-myers-simd
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Apply SIMD flags
target_compile_options(levenshtein-myers-simd PRIVATE ${SIMD_FLAGS})
