#include <levenshtein_myers.hpp>
#include <benchmark/benchmark.h>
#include <array>
#include <cstring>
#include <random>

// Helper to generate random strings
class TestData {
public:
  std::vector<std::string> query_words;
  std::vector<std::array<std::string, 8>> database_words;

  TestData(int num_samples = 100) {
    std::mt19937 rng(42); // Fixed seed for reproducibility
    std::uniform_int_distribution<int> len_dist(1, 16);
    std::uniform_int_distribution<int> char_dist('a', 'z');

    for (int i = 0; i < num_samples; ++i) {
      // Generate query word (can be any reasonable length)
      int q_len = len_dist(rng);
      std::string q_word;
      for (int j = 0; j < q_len; ++j) {
        q_word += static_cast<char>(char_dist(rng));
      }
      query_words.push_back(q_word);

      // Generate 8 database words (max 16 chars)
      std::array<std::string, 8> d_batch;
      for (int k = 0; k < 8; ++k) {
        int d_len = len_dist(rng);
        std::string d_word;
        for (int j = 0; j < d_len; ++j) {
          d_word += static_cast<char>(char_dist(rng));
        }
        d_batch[k] = d_word;
      }
      database_words.push_back(d_batch);
    }
  }
};

// Benchmark with random data
static void BM_Myers16x8_Random(benchmark::State& state) {
  TestData data;
  int idx = 0;

  for (auto _ : state) {
    // Setup input
    Myers16x8Input input;
    input.q_wrd = data.query_words[idx].c_str();
    input.q_wrd_len = data.query_words[idx].length();

    for (int i = 0; i < 8; ++i) {
      input.d_wrds[i] = data.database_words[idx][i].c_str();
      input.d_wrd_lens[i] = data.database_words[idx][i].length();
    }

    auto result = levenshtein_myers_16x8(input);
    benchmark::DoNotOptimize(result);

    idx = (idx + 1) % data.query_words.size();
  }
}
BENCHMARK(BM_Myers16x8_Random);

// Benchmark with varying query lengths
static void BM_Myers16x8_VaryingQueryLen(benchmark::State& state) {
  int q_len = state.range(0);

  // Fixed database words
  std::array<std::string, 8> d_words = {
    "hello", "world", "test", "benchmark",
    "myers", "algorithm", "fast", "simd"
  };

  // Generate query of specified length
  std::string query(q_len, 'a');

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

  state.SetLabel("q_len=" + std::to_string(q_len));
}
BENCHMARK(BM_Myers16x8_VaryingQueryLen)->DenseRange(1, 16, 3);

// Benchmark worst case: all max length strings
static void BM_Myers16x8_MaxLength(benchmark::State& state) {
  std::string query(16, 'a');
  std::array<std::string, 8> d_words;
  for (int i = 0; i < 8; ++i) {
    d_words[i] = std::string(16, 'b');
  }

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

// Benchmark best case: identical strings
static void BM_Myers16x8_Identical(benchmark::State& state) {
  std::string word = "testword";
  std::array<std::string, 8> d_words;
  for (int i = 0; i < 8; ++i) {
    d_words[i] = word;
  }

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

// Benchmark with real-world-like data
static void BM_Myers16x8_RealWorld(benchmark::State& state) {
  // Simulate typos and similar words
  std::string query = "algorithm";
  std::array<std::string, 8> d_words = {
    "algorithm",   // exact match
    "algorithim",  // common typo
    "algorythm",   // another typo
    "algorthm",    // missing letter
    "alorithm",    // transposition
    "algorithms",  // plural
    "logarithm",   // similar word
    "algarithm"    // different typo
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

BENCHMARK_MAIN();
