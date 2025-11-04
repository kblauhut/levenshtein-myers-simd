# Levenshtein Myers SIMD

cmake -DCMAKE_BUILD_TYPE=Release -B build

cmake --build build

ctest --test-dir build
ctest --test-dir build -V
