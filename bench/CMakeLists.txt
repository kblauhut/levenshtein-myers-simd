include(FetchContent)
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.9.1
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(benchmark)

add_executable(levenshtein_bench levenshtein_benchmark.cpp)

target_link_libraries(levenshtein_bench levenshtein-myers-simd benchmark::benchmark)
