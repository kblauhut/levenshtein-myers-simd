# Levenshtein Myers SIMD

SIMD-accelerated Levenshtein distance using the Myers bit-parallel algorithm, targeting ARM NEON (Apple Silicon).

## Algorithm

All methods implement [Myers' bit-parallel algorithm](https://dl.acm.org/doi/10.1145/316542.316550). The core idea is to represent the DP recurrence as bitwise operations on bitvectors, so an entire column of the DP matrix is computed with a handful of integer instructions. This makes it significantly faster than the classical O(mn) DP approach.

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

Measured on a MacBook Pro M1 Max. Benchmark strings use lowercase a–z.

### SIMD batch variants

The batch variants are best compared by their **per-comparison throughput** (total call time divided by batch size), since each call processes multiple strings. The random benchmark uses mixed-length strings uniformly sampled up to the method's maximum; the max-length benchmark uses strings of exactly the maximum length.

| Method | Max len | Batch | Random call (ns) | ns/comparison | Max-len call (ns) | ns/comparison |
|---|---|---|---|---|---|---|
| `8x16`  |  8 | 16 |  53 |  3.3 |  52 |  3.3 |
| `16x8`  | 16 |  8 |  83 | 10.4 |  91 | 11.4 |
| `32x4`  | 32 |  4 | 160 | 40.0 | 208 | 52.0 |
| `64x2`  | 64 |  2 | 282 | 141  | 478 | 239  |

`8x16` achieves the best throughput because the 8-bit bitvector fits 16 lanes in a 128-bit NEON register. As the bitvector widens to accommodate longer strings, fewer lanes fit and throughput falls. Use the narrowest variant that covers your string lengths.

### Scalar single-pair variants

| Method | Max len | Random (ns) | Max-len (ns) |
|---|---|---|---|
| `32x1` | 32 |  71 | 137 |
| `64x1` | 64 | 147 | 330 |

The scalar variants are slower than the SIMD batch methods on a per-comparison basis but avoid the overhead of assembling a batch.

### Arbitrary-length variant (`anyx1`)

Runtime scales roughly quadratically with string length due to the O(mn/w) nature of Myers' algorithm with multi-word bitvectors (where w = 8 for the byte-level implementation here).

| String length | Time (ns) |
|---|---|
|  32 |     1,722 |
|  64 |     4,455 |
| 128 |    12,874 |
| 256 |    41,475 |
| 512 |   103,104 |

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
