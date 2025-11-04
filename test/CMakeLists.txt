# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable GoogleTest discovery
include(GoogleTest)

# Create test executable
add_executable(levenshtein_tests
    test_levenshtein_myers_16x8.cpp
)

target_link_libraries(levenshtein_tests
    PRIVATE
        levenshtein-myers-simd
        GTest::gtest_main
        GTest::gmock
)

# Discover tests
gtest_discover_tests(levenshtein_tests)
