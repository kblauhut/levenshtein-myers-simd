# Levenshtein Myers SIMD

SIMD-accelerated Levenshtein distance using the Myers bit-parallel algorithm, targeting ARM NEON (Apple Silicon).

## Algorithm

All methods implement [Myers' bit-parallel algorithm](https://dl.acm.org/doi/10.1145/316542.316550).

The naming convention `NxM` encodes two parameters:
- **N** — bit width of the bitvector used for the query, which equals the maximum supported query length in characters.
- **M** — number of database strings compared simultaneously in one call.

## Methods

### SIMD batch variants

These pack M independent Myers computations into a single 128-bit ARM NEON register, processing M database strings against one query in parallel. The trade-off is that a wider bitvector (longer strings) leaves fewer NEON lanes available.

| Function | Max query length | DB strings per call | NEON element type |
|---|---|---|---|
| `levenshtein_myers_8x16`  |  8 chars | 16 | `uint8x16_t`  |
| `levenshtein_myers_16x8`  | 16 chars |  8 | `uint16x8_t`  |
| `levenshtein_myers_32x4`  | 32 chars |  4 | `uint32x4_t`  |
| `levenshtein_myers_64x2`  | 64 chars |  2 | `uint64x2_t`  |

Each function takes a single query and a batch of database strings, and returns an array of distances — one per database string.

```cpp
// Example: compare one query against 8 database strings (max 16 chars each)
Myers16x8Input input;
input.q_wrd     = "algorithm";
input.q_wrd_len = 9;
input.d_wrds[0]     = "algorithms";  input.d_wrd_lens[0] = 10;
input.d_wrds[1]     = "logarithm";   input.d_wrd_lens[1] = 9;
// ... fill remaining 6 slots ...

std::array<uint16_t, 8> distances = levenshtein_myers_16x8(input);
```

### Scalar single-pair variants

These operate on one pair of strings at a time using a plain integer bitvector. They are useful when batching is not practical or when the query is longer than 16 characters but SIMD batching is not worth the overhead.

| Function | Max query length | Bitvector type |
|---|---|---|
| `levenshtein_myers_32x1`  | 32 chars | `uint32_t` |
| `levenshtein_myers_64x1`  | 64 chars | `uint64_t` |

```cpp
uint32_t dist = levenshtein_myers_32x1("hello", 5, "helo", 4);
```

### Arbitrary-length variant

`levenshtein_myers_anyx1` supports strings of any length by dynamically allocating a multi-word bitvector on the heap. It is significantly slower than the fixed-width variants due to heap allocation and byte-level bitwise loops, but has no length restriction.

```cpp
uint32_t dist = levenshtein_myers_anyx1(long_query, q_len, long_target, t_len);
```

## Benchmarks

Measured on a MacBook Pro M1 Max. All strings are random lowercase a–z of exactly the given length.

### SIMD batch variants — ns per comparison

Each call processes a batch of strings simultaneously. The table shows **ns per individual comparison** (call time ÷ batch size). Empty cells mean the method does not support that string length.

| String length | `8x16` (batch 16) | `16x8` (batch 8) | `32x4` (batch 4) | `64x2` (batch 2) |
|---|---|---|---|---|
|  8 chars |  3.3 |  5.3 |  9.0 | 17.2 |
| 16 chars |    — | 11.6 | 22.0 | 43.6 |
| 32 chars |    — |    — | 51.5 |  103 |
| 64 chars |    — |    — |    — |  224 |

At any given string length, the method with the most lanes is always fastest per comparison. The trade-off is that wider NEON elements (needed for longer strings) leave fewer lanes available, which is why `8x16` is the fastest option for short strings.

### Scalar single-pair variants

| Method | Max length | Random (ns) | Max-length (ns) |
|---|---|---|---|
| `32x1` | 32 |  71 | 136 |
| `64x1` | 64 | 147 | 329 |

### Arbitrary-length variant (`anyx1`)

| String length | Time (ns) |
|---|---|
|  32 |     1,716 |
|  64 |     4,514 |
| 128 |    12,840 |
| 256 |    41,183 |
| 512 |   102,208 |

## Build

```sh
cmake -DCMAKE_BUILD_TYPE=Release -B build
cmake --build build
```

## Test

```sh
ctest --test-dir build
ctest --test-dir build -V   # verbose
```

## Run benchmarks

```sh
cmake --build build --target levenshtein_bench
./build/bench/levenshtein_bench
```
