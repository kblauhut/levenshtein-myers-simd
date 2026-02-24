#include <levenshtein_myers.hpp>
#include <benchmark/benchmark.h>
#include <array>
#include <cstring>
#include <random>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static std::mt19937 make_rng() { return std::mt19937(42); }

static std::string random_string(std::mt19937 &rng, int min_len, int max_len) {
  std::uniform_int_distribution<int> len_dist(min_len, max_len);
  std::uniform_int_distribution<int> char_dist('a', 'z');
  int len = len_dist(rng);
  std::string s;
  s.reserve(len);
  for (int i = 0; i < len; ++i)
    s += static_cast<char>(char_dist(rng));
  return s;
}

static std::string random_string_exact(std::mt19937 &rng, int len) {
  std::uniform_int_distribution<int> char_dist('a', 'z');
  std::string s;
  s.reserve(len);
  for (int i = 0; i < len; ++i)
    s += static_cast<char>(char_dist(rng));
  return s;
}

// ---------------------------------------------------------------------------
// 8x16 (query ≤ 8 chars, 16 database words)
// ---------------------------------------------------------------------------

static void BM_Myers8x16_Random(benchmark::State &state) {
  auto rng = make_rng();
  constexpr int N = 100;
  std::vector<std::string> queries(N);
  std::vector<std::array<std::string, 16>> db(N);
  for (int i = 0; i < N; ++i) {
    queries[i] = random_string(rng, 1, 8);
    for (int k = 0; k < 16; ++k)
      db[i][k] = random_string(rng, 1, 8);
  }
  int idx = 0;
  for (auto _ : state) {
    Myers8x16Input input;
    input.q_wrd = queries[idx].c_str();
    input.q_wrd_len = queries[idx].length();
    for (int i = 0; i < 16; ++i) {
      input.d_wrds[i] = db[idx][i].c_str();
      input.d_wrd_lens[i] = db[idx][i].length();
    }
    auto result = levenshtein_myers_8x16(input);
    benchmark::DoNotOptimize(result);
    idx = (idx + 1) % N;
  }
}
BENCHMARK(BM_Myers8x16_Random);

static void BM_Myers8x16_MaxLength(benchmark::State &state) {
  std::string query(8, 'a');
  std::array<std::string, 16> d_words;
  for (int i = 0; i < 16; ++i) d_words[i] = std::string(8, 'b');
  for (auto _ : state) {
    Myers8x16Input input;
    input.q_wrd = query.c_str();
    input.q_wrd_len = 8;
    for (int i = 0; i < 16; ++i) {
      input.d_wrds[i] = d_words[i].c_str();
      input.d_wrd_lens[i] = 8;
    }
    auto result = levenshtein_myers_8x16(input);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Myers8x16_MaxLength);

static void BM_Myers8x16_Identical(benchmark::State &state) {
  std::string word = "testword";
  std::array<std::string, 16> d_words;
  for (int i = 0; i < 16; ++i) d_words[i] = word;
  for (auto _ : state) {
    Myers8x16Input input;
    input.q_wrd = word.c_str();
    input.q_wrd_len = word.length();
    for (int i = 0; i < 16; ++i) {
      input.d_wrds[i] = d_words[i].c_str();
      input.d_wrd_lens[i] = d_words[i].length();
    }
    auto result = levenshtein_myers_8x16(input);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Myers8x16_Identical);

static void BM_Myers8x16_RealWorld(benchmark::State &state) {
  std::string query = "algo";
  std::array<std::string, 16> d_words = {
    "algo", "alge", "alco", "alga", "aldo", "alfa", "alba", "alb",
    "algo", "alge", "alco", "alga", "aldo", "alfa", "alba", "alb"
  };
  for (auto _ : state) {
    Myers8x16Input input;
    input.q_wrd = query.c_str();
    input.q_wrd_len = query.length();
    for (int i = 0; i < 16; ++i) {
      input.d_wrds[i] = d_words[i].c_str();
      input.d_wrd_lens[i] = d_words[i].length();
    }
    auto result = levenshtein_myers_8x16(input);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Myers8x16_RealWorld);

// ---------------------------------------------------------------------------
// 16x8 (query ≤ 16 chars, 8 database words)
// ---------------------------------------------------------------------------

static void BM_Myers16x8_Random(benchmark::State &state) {
  auto rng = make_rng();
  constexpr int N = 100;
  std::vector<std::string> queries(N);
  std::vector<std::array<std::string, 8>> db(N);
  for (int i = 0; i < N; ++i) {
    queries[i] = random_string(rng, 1, 16);
    for (int k = 0; k < 8; ++k)
      db[i][k] = random_string(rng, 1, 16);
  }
  int idx = 0;
  for (auto _ : state) {
    Myers16x8Input input;
    input.q_wrd = queries[idx].c_str();
    input.q_wrd_len = queries[idx].length();
    for (int i = 0; i < 8; ++i) {
      input.d_wrds[i] = db[idx][i].c_str();
      input.d_wrd_lens[i] = db[idx][i].length();
    }
    auto result = levenshtein_myers_16x8(input);
    benchmark::DoNotOptimize(result);
    idx = (idx + 1) % N;
  }
}
BENCHMARK(BM_Myers16x8_Random);

static void BM_Myers16x8_MaxLength(benchmark::State &state) {
  std::string query(16, 'a');
  std::array<std::string, 8> d_words;
  for (int i = 0; i < 8; ++i) d_words[i] = std::string(16, 'b');
  for (auto _ : state) {
    Myers16x8Input input;
    input.q_wrd = query.c_str();
    input.q_wrd_len = 16;
    for (int i = 0; i < 8; ++i) {
      input.d_wrds[i] = d_words[i].c_str();
      input.d_wrd_lens[i] = 16;
    }
    auto result = levenshtein_myers_16x8(input);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Myers16x8_MaxLength);

static void BM_Myers16x8_Identical(benchmark::State &state) {
  std::string word = "testword";
  std::array<std::string, 8> d_words;
  for (int i = 0; i < 8; ++i) d_words[i] = word;
  for (auto _ : state) {
    Myers16x8Input input;
    input.q_wrd = word.c_str();
    input.q_wrd_len = word.length();
    for (int i = 0; i < 8; ++i) {
      input.d_wrds[i] = d_words[i].c_str();
      input.d_wrd_lens[i] = d_words[i].length();
    }
    auto result = levenshtein_myers_16x8(input);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Myers16x8_Identical);

static void BM_Myers16x8_RealWorld(benchmark::State &state) {
  std::string query = "algorithm";
  std::array<std::string, 8> d_words = {
    "algorithm", "algorithim", "algorythm", "algorthm",
    "alorithm",  "algorithms", "logarithm", "algarithm"
  };
  for (auto _ : state) {
    Myers16x8Input input;
    input.q_wrd = query.c_str();
    input.q_wrd_len = query.length();
    for (int i = 0; i < 8; ++i) {
      input.d_wrds[i] = d_words[i].c_str();
      input.d_wrd_lens[i] = d_words[i].length();
    }
    auto result = levenshtein_myers_16x8(input);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Myers16x8_RealWorld);

// ---------------------------------------------------------------------------
// 32x4 (query ≤ 32 chars, 4 database words)
// ---------------------------------------------------------------------------

static void BM_Myers32x4_Random(benchmark::State &state) {
  auto rng = make_rng();
  constexpr int N = 100;
  std::vector<std::string> queries(N);
  std::vector<std::array<std::string, 4>> db(N);
  for (int i = 0; i < N; ++i) {
    queries[i] = random_string(rng, 1, 32);
    for (int k = 0; k < 4; ++k)
      db[i][k] = random_string(rng, 1, 32);
  }
  int idx = 0;
  for (auto _ : state) {
    Myers32x4Input input;
    input.q_wrd = queries[idx].c_str();
    input.q_wrd_len = queries[idx].length();
    for (int i = 0; i < 4; ++i) {
      input.d_wrds[i] = db[idx][i].c_str();
      input.d_wrd_lens[i] = db[idx][i].length();
    }
    auto result = levenshtein_myers_32x4(input);
    benchmark::DoNotOptimize(result);
    idx = (idx + 1) % N;
  }
}
BENCHMARK(BM_Myers32x4_Random);

static void BM_Myers32x4_MaxLength(benchmark::State &state) {
  std::string query(32, 'a');
  std::array<std::string, 4> d_words;
  for (int i = 0; i < 4; ++i) d_words[i] = std::string(32, 'b');
  for (auto _ : state) {
    Myers32x4Input input;
    input.q_wrd = query.c_str();
    input.q_wrd_len = 32;
    for (int i = 0; i < 4; ++i) {
      input.d_wrds[i] = d_words[i].c_str();
      input.d_wrd_lens[i] = 32;
    }
    auto result = levenshtein_myers_32x4(input);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Myers32x4_MaxLength);

static void BM_Myers32x4_Identical(benchmark::State &state) {
  std::string word = "testwordtestwordtestwordtestword";
  std::array<std::string, 4> d_words;
  for (int i = 0; i < 4; ++i) d_words[i] = word;
  for (auto _ : state) {
    Myers32x4Input input;
    input.q_wrd = word.c_str();
    input.q_wrd_len = word.length();
    for (int i = 0; i < 4; ++i) {
      input.d_wrds[i] = d_words[i].c_str();
      input.d_wrd_lens[i] = d_words[i].length();
    }
    auto result = levenshtein_myers_32x4(input);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Myers32x4_Identical);

static void BM_Myers32x4_RealWorld(benchmark::State &state) {
  std::string query = "internationalization";
  std::array<std::string, 4> d_words = {
    "internationalization", "internationalisation",
    "internatjonalization", "internationalizaton"
  };
  for (auto _ : state) {
    Myers32x4Input input;
    input.q_wrd = query.c_str();
    input.q_wrd_len = query.length();
    for (int i = 0; i < 4; ++i) {
      input.d_wrds[i] = d_words[i].c_str();
      input.d_wrd_lens[i] = d_words[i].length();
    }
    auto result = levenshtein_myers_32x4(input);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Myers32x4_RealWorld);

// ---------------------------------------------------------------------------
// 64x2 (query ≤ 64 chars, 2 database words)
// ---------------------------------------------------------------------------

static void BM_Myers64x2_Random(benchmark::State &state) {
  auto rng = make_rng();
  constexpr int N = 100;
  std::vector<std::string> queries(N);
  std::vector<std::array<std::string, 2>> db(N);
  for (int i = 0; i < N; ++i) {
    queries[i] = random_string(rng, 1, 64);
    for (int k = 0; k < 2; ++k)
      db[i][k] = random_string(rng, 1, 64);
  }
  int idx = 0;
  for (auto _ : state) {
    Myers64x2Input input;
    input.q_wrd = queries[idx].c_str();
    input.q_wrd_len = queries[idx].length();
    for (int i = 0; i < 2; ++i) {
      input.d_wrds[i] = db[idx][i].c_str();
      input.d_wrd_lens[i] = db[idx][i].length();
    }
    auto result = levenshtein_myers_64x2(input);
    benchmark::DoNotOptimize(result);
    idx = (idx + 1) % N;
  }
}
BENCHMARK(BM_Myers64x2_Random);

static void BM_Myers64x2_MaxLength(benchmark::State &state) {
  std::string query(64, 'a');
  std::array<std::string, 2> d_words = {std::string(64, 'b'), std::string(64, 'b')};
  for (auto _ : state) {
    Myers64x2Input input;
    input.q_wrd = query.c_str();
    input.q_wrd_len = 64;
    for (int i = 0; i < 2; ++i) {
      input.d_wrds[i] = d_words[i].c_str();
      input.d_wrd_lens[i] = 64;
    }
    auto result = levenshtein_myers_64x2(input);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Myers64x2_MaxLength);

static void BM_Myers64x2_Identical(benchmark::State &state) {
  std::string word(64, 'x');
  std::array<std::string, 2> d_words = {word, word};
  for (auto _ : state) {
    Myers64x2Input input;
    input.q_wrd = word.c_str();
    input.q_wrd_len = word.length();
    for (int i = 0; i < 2; ++i) {
      input.d_wrds[i] = d_words[i].c_str();
      input.d_wrd_lens[i] = d_words[i].length();
    }
    auto result = levenshtein_myers_64x2(input);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Myers64x2_Identical);

static void BM_Myers64x2_RealWorld(benchmark::State &state) {
  std::string query = "the quick brown fox jumps over the lazy dog near the river";
  std::array<std::string, 2> d_words = {
    "the quick brown fox jumps over the lazy dog near the river",
    "the quikc brown fox jumps ovr the lazy dog near the rivr"
  };
  for (auto _ : state) {
    Myers64x2Input input;
    input.q_wrd = query.c_str();
    input.q_wrd_len = query.length();
    for (int i = 0; i < 2; ++i) {
      input.d_wrds[i] = d_words[i].c_str();
      input.d_wrd_lens[i] = d_words[i].length();
    }
    auto result = levenshtein_myers_64x2(input);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Myers64x2_RealWorld);

// ---------------------------------------------------------------------------
// 32x1, 64x1, 128x1, anyx1 — scalar single-pair variants
// ---------------------------------------------------------------------------

// Helper macro to avoid copy-pasting the four benchmarks per variant
#define BENCH_SCALAR(NAME, FUNC, MAXLEN)                                        \
  static void BM_##NAME##_Random(benchmark::State &state) {                     \
    auto rng = make_rng();                                                       \
    constexpr int N = 100;                                                       \
    std::vector<std::string> queries(N), targets(N);                            \
    for (int i = 0; i < N; ++i) {                                               \
      queries[i] = random_string(rng, 1, MAXLEN);                               \
      targets[i] = random_string(rng, 1, MAXLEN);                               \
    }                                                                            \
    int idx = 0;                                                                 \
    for (auto _ : state) {                                                       \
      auto r = FUNC(queries[idx].c_str(), queries[idx].length(),                \
                    targets[idx].c_str(), targets[idx].length());               \
      benchmark::DoNotOptimize(r);                                               \
      idx = (idx + 1) % N;                                                      \
    }                                                                            \
  }                                                                              \
  BENCHMARK(BM_##NAME##_Random);                                                 \
                                                                                 \
  static void BM_##NAME##_MaxLength(benchmark::State &state) {                  \
    std::string q(MAXLEN, 'a'), d(MAXLEN, 'b');                                 \
    for (auto _ : state) {                                                       \
      auto r = FUNC(q.c_str(), q.length(), d.c_str(), d.length());              \
      benchmark::DoNotOptimize(r);                                               \
    }                                                                            \
  }                                                                              \
  BENCHMARK(BM_##NAME##_MaxLength);                                              \
                                                                                 \
  static void BM_##NAME##_Identical(benchmark::State &state) {                  \
    std::string word(MAXLEN, 'x');                                               \
    for (auto _ : state) {                                                       \
      auto r = FUNC(word.c_str(), word.length(), word.c_str(), word.length());  \
      benchmark::DoNotOptimize(r);                                               \
    }                                                                            \
  }                                                                              \
  BENCHMARK(BM_##NAME##_Identical);

BENCH_SCALAR(Myers32x1,  levenshtein_myers_32x1,  32)
BENCH_SCALAR(Myers64x1,  levenshtein_myers_64x1,  64)
BENCH_SCALAR(Myers128x1, levenshtein_myers_128x1, 128)

// anyx1 — benchmark across several representative lengths
static void BM_MyersAnyx1_Random(benchmark::State &state) {
  auto rng = make_rng();
  constexpr int N = 100;
  std::vector<std::string> queries(N), targets(N);
  for (int i = 0; i < N; ++i) {
    queries[i] = random_string(rng, 1, 256);
    targets[i] = random_string(rng, 1, 256);
  }
  int idx = 0;
  for (auto _ : state) {
    auto r = levenshtein_myers_anyx1(queries[idx].c_str(), queries[idx].length(),
                                     targets[idx].c_str(), targets[idx].length());
    benchmark::DoNotOptimize(r);
    idx = (idx + 1) % N;
  }
}
BENCHMARK(BM_MyersAnyx1_Random);

static void BM_MyersAnyx1_VaryingLen(benchmark::State &state) {
  int len = state.range(0);
  std::string q(len, 'a'), d(len, 'b');
  for (auto _ : state) {
    auto r = levenshtein_myers_anyx1(q.c_str(), len, d.c_str(), len);
    benchmark::DoNotOptimize(r);
  }
  state.SetLabel("len=" + std::to_string(len));
}
BENCHMARK(BM_MyersAnyx1_VaryingLen)->RangeMultiplier(2)->Range(32, 512);

static void BM_MyersAnyx1_Identical(benchmark::State &state) {
  std::string word(256, 'x');
  for (auto _ : state) {
    auto r = levenshtein_myers_anyx1(word.c_str(), word.length(),
                                     word.c_str(), word.length());
    benchmark::DoNotOptimize(r);
  }
}
BENCHMARK(BM_MyersAnyx1_Identical);

// ---------------------------------------------------------------------------
// Fixed-length cross-method comparison
// All strings are exactly `state.range(0)` characters so results are
// directly comparable across methods at the same string length.
// Divide the reported time by the batch size to get ns/comparison.
// ---------------------------------------------------------------------------

static void BM_Myers8x16_FixedLen(benchmark::State &state) {
  int len = state.range(0);
  auto rng = make_rng();
  constexpr int N = 100;
  std::vector<std::string> queries(N);
  std::vector<std::array<std::string, 16>> db(N);
  for (int i = 0; i < N; ++i) {
    queries[i] = random_string_exact(rng, len);
    for (int k = 0; k < 16; ++k) db[i][k] = random_string_exact(rng, len);
  }
  int idx = 0;
  for (auto _ : state) {
    Myers8x16Input input;
    input.q_wrd = queries[idx].c_str();
    input.q_wrd_len = len;
    for (int i = 0; i < 16; ++i) {
      input.d_wrds[i] = db[idx][i].c_str();
      input.d_wrd_lens[i] = len;
    }
    benchmark::DoNotOptimize(levenshtein_myers_8x16(input));
    idx = (idx + 1) % N;
  }
}
BENCHMARK(BM_Myers8x16_FixedLen)->Arg(8);

static void BM_Myers16x8_FixedLen(benchmark::State &state) {
  int len = state.range(0);
  auto rng = make_rng();
  constexpr int N = 100;
  std::vector<std::string> queries(N);
  std::vector<std::array<std::string, 8>> db(N);
  for (int i = 0; i < N; ++i) {
    queries[i] = random_string_exact(rng, len);
    for (int k = 0; k < 8; ++k) db[i][k] = random_string_exact(rng, len);
  }
  int idx = 0;
  for (auto _ : state) {
    Myers16x8Input input;
    input.q_wrd = queries[idx].c_str();
    input.q_wrd_len = len;
    for (int i = 0; i < 8; ++i) {
      input.d_wrds[i] = db[idx][i].c_str();
      input.d_wrd_lens[i] = len;
    }
    benchmark::DoNotOptimize(levenshtein_myers_16x8(input));
    idx = (idx + 1) % N;
  }
}
BENCHMARK(BM_Myers16x8_FixedLen)->Arg(8)->Arg(16);

static void BM_Myers32x4_FixedLen(benchmark::State &state) {
  int len = state.range(0);
  auto rng = make_rng();
  constexpr int N = 100;
  std::vector<std::string> queries(N);
  std::vector<std::array<std::string, 4>> db(N);
  for (int i = 0; i < N; ++i) {
    queries[i] = random_string_exact(rng, len);
    for (int k = 0; k < 4; ++k) db[i][k] = random_string_exact(rng, len);
  }
  int idx = 0;
  for (auto _ : state) {
    Myers32x4Input input;
    input.q_wrd = queries[idx].c_str();
    input.q_wrd_len = len;
    for (int i = 0; i < 4; ++i) {
      input.d_wrds[i] = db[idx][i].c_str();
      input.d_wrd_lens[i] = len;
    }
    benchmark::DoNotOptimize(levenshtein_myers_32x4(input));
    idx = (idx + 1) % N;
  }
}
BENCHMARK(BM_Myers32x4_FixedLen)->Arg(8)->Arg(16)->Arg(32);

static void BM_Myers64x2_FixedLen(benchmark::State &state) {
  int len = state.range(0);
  auto rng = make_rng();
  constexpr int N = 100;
  std::vector<std::string> queries(N);
  std::vector<std::array<std::string, 2>> db(N);
  for (int i = 0; i < N; ++i) {
    queries[i] = random_string_exact(rng, len);
    for (int k = 0; k < 2; ++k) db[i][k] = random_string_exact(rng, len);
  }
  int idx = 0;
  for (auto _ : state) {
    Myers64x2Input input;
    input.q_wrd = queries[idx].c_str();
    input.q_wrd_len = len;
    for (int i = 0; i < 2; ++i) {
      input.d_wrds[i] = db[idx][i].c_str();
      input.d_wrd_lens[i] = len;
    }
    benchmark::DoNotOptimize(levenshtein_myers_64x2(input));
    idx = (idx + 1) % N;
  }
}
BENCHMARK(BM_Myers64x2_FixedLen)->Arg(8)->Arg(16)->Arg(32)->Arg(64);

BENCHMARK_MAIN();
