#include <gtest/gtest.h>
#include <levenshtein_myers.hpp>
#include <random>
#include <string>

static uint32_t levenshtein_reference(const char *a, int len_a, const char *b,
                                      int len_b) {
  std::vector<uint32_t> prev(len_b + 1), curr(len_b + 1);

  for (int j = 0; j <= len_b; j++)
    prev[j] = j;

  for (int i = 0; i < len_a; i++) {
    curr[0] = i + 1;
    for (int j = 0; j < len_b; j++) {
      uint32_t cost = (a[i] == b[j]) ? 0 : 1;
      curr[j + 1] = std::min({
          prev[j + 1] + 1, // deletion
          curr[j] + 1,     // insertion
          prev[j] + cost   // substitution
      });
    }
    std::swap(prev, curr);
  }
  return prev[len_b];
}

static std::string rand_string(std::mt19937 &rng, int max_len = 16) {
  std::uniform_int_distribution<int> len_dist(0, max_len);
  std::uniform_int_distribution<int> char_dist('a', 'z');

  int len = len_dist(rng);
  std::string s;
  s.reserve(len);

  for (int i = 0; i < len; i++)
    s.push_back(char_dist(rng));

  return s;
}

TEST(LevenshteinMyers64x2Fuzz, RandomizedCompareAgainstReference) {
  std::mt19937 rng(1337);

  for (int iter = 0; iter < 2000000; ++iter) {
    auto q = rand_string(rng, 64);
    auto d0 = rand_string(rng, 64);
    auto d1 = rand_string(rng, 64);

    Myers64x2Input input{
        .q_wrd = q.c_str(),
        .q_wrd_len = (int)q.size(),
        .d_wrds = {d0.c_str(), d1.c_str()},
        .d_wrd_lens = {(uint64_t)d0.size(), (uint64_t)d1.size()}};

    auto simd = levenshtein_myers_64x2(input);
    uint64_t ref0 =
        levenshtein_reference(q.c_str(), q.size(), d0.c_str(), d0.size());
    uint64_t ref1 =
        levenshtein_reference(q.c_str(), q.size(), d1.c_str(), d1.size());

    EXPECT_EQ(simd[0], ref0) << "Mismatch q=" << q << " a=" << d0;
    EXPECT_EQ(simd[1], ref1) << "Mismatch q=" << q << " b=" << d1;
  }
}

TEST(LevenshteinMyers32x4Fuzz, RandomizedCompareAgainstReference) {
  std::mt19937 rng(1337);

  for (int iter = 0; iter < 2000000; ++iter) {
    auto q = rand_string(rng, 32);
    auto d0 = rand_string(rng, 32);
    auto d1 = rand_string(rng, 32);
    auto d2 = rand_string(rng, 32);
    auto d3 = rand_string(rng, 32);

    Myers32x4Input input{
        .q_wrd = q.c_str(),
        .q_wrd_len = (int)q.size(),
        .d_wrds = {d0.c_str(), d1.c_str(), d2.c_str(), d3.c_str()},
        .d_wrd_lens = {(uint32_t)d0.size(), (uint32_t)d1.size(),
                       (uint32_t)d2.size(), (uint32_t)d3.size()}};

    auto simd = levenshtein_myers_32x4(input);

    uint32_t ref[4] = {
        levenshtein_reference(q.c_str(), q.size(), d0.c_str(), d0.size()),
        levenshtein_reference(q.c_str(), q.size(), d1.c_str(), d1.size()),
        levenshtein_reference(q.c_str(), q.size(), d2.c_str(), d2.size()),
        levenshtein_reference(q.c_str(), q.size(), d3.c_str(), d3.size())};

    for (int i = 0; i < 4; i++) {
      EXPECT_EQ(simd[i], ref[i])
          << "Mismatch (idx " << i << ") q=" << q
          << " d=" << (i == 0 ? d0 : (i == 1 ? d1 : (i == 2 ? d2 : d3)));
    }
  }
}

TEST(LevenshteinMyers16x8Fuzz, RandomizedCompareAgainstReference) {
  std::mt19937 rng(1337);

  for (int iter = 0; iter < 2000000; ++iter) {
    auto q = rand_string(rng, 16);
    auto d0 = rand_string(rng, 16);
    auto d1 = rand_string(rng, 16);
    auto d2 = rand_string(rng, 16);
    auto d3 = rand_string(rng, 16);
    auto d4 = rand_string(rng, 16);
    auto d5 = rand_string(rng, 16);
    auto d6 = rand_string(rng, 16);
    auto d7 = rand_string(rng, 16);

    Myers16x8Input input{
        .q_wrd = q.c_str(),
        .q_wrd_len = (int)q.size(),
        .d_wrds = {d0.c_str(), d1.c_str(), d2.c_str(), d3.c_str(), d4.c_str(),
                   d5.c_str(), d6.c_str(), d7.c_str()},
        .d_wrd_lens = {(uint16_t)d0.size(), (uint16_t)d1.size(),
                       (uint16_t)d2.size(), (uint16_t)d3.size(),
                       (uint16_t)d4.size(), (uint16_t)d5.size(),
                       (uint16_t)d6.size(), (uint16_t)d7.size()}};

    auto simd = levenshtein_myers_16x8(input);

    uint32_t ref[8] = {
        levenshtein_reference(q.c_str(), q.size(), d0.c_str(), d0.size()),
        levenshtein_reference(q.c_str(), q.size(), d1.c_str(), d1.size()),
        levenshtein_reference(q.c_str(), q.size(), d2.c_str(), d2.size()),
        levenshtein_reference(q.c_str(), q.size(), d3.c_str(), d3.size()),
        levenshtein_reference(q.c_str(), q.size(), d4.c_str(), d4.size()),
        levenshtein_reference(q.c_str(), q.size(), d5.c_str(), d5.size()),
        levenshtein_reference(q.c_str(), q.size(), d6.c_str(), d6.size()),
        levenshtein_reference(q.c_str(), q.size(), d7.c_str(), d7.size())};

    for (int i = 0; i < 8; i++) {
      const std::string *d = (i == 0   ? &d0
                              : i == 1 ? &d1
                              : i == 2 ? &d2
                              : i == 3 ? &d3
                              : i == 4 ? &d4
                              : i == 5 ? &d5
                              : i == 6 ? &d6
                                       : &d7);

      EXPECT_EQ(simd[i], ref[i])
          << "Mismatch (idx " << i << ") q=" << q << " d=" << *d;
    }
  }
}

TEST(LevenshteinMyers8x16Fuzz, RandomizedCompareAgainstReference) {
  std::mt19937 rng(1337);

  for (int iter = 0; iter < 2000000; ++iter) {
    auto q = rand_string(rng, 8);
    auto d0 = rand_string(rng, 8);
    auto d1 = rand_string(rng, 8);
    auto d2 = rand_string(rng, 8);
    auto d3 = rand_string(rng, 8);
    auto d4 = rand_string(rng, 8);
    auto d5 = rand_string(rng, 8);
    auto d6 = rand_string(rng, 8);
    auto d7 = rand_string(rng, 8);
    auto d8 = rand_string(rng, 8);
    auto d9 = rand_string(rng, 8);
    auto d10 = rand_string(rng, 8);
    auto d11 = rand_string(rng, 8);
    auto d12 = rand_string(rng, 8);
    auto d13 = rand_string(rng, 8);
    auto d14 = rand_string(rng, 8);
    auto d15 = rand_string(rng, 8);

    Myers8x16Input input{
        .q_wrd = q.c_str(),
        .q_wrd_len = (int)q.size(),
        .d_wrds = {d0.c_str(), d1.c_str(), d2.c_str(), d3.c_str(), d4.c_str(),
                   d5.c_str(), d6.c_str(), d7.c_str(), d8.c_str(), d9.c_str(),
                   d10.c_str(), d11.c_str(), d12.c_str(), d13.c_str(),
                   d14.c_str(), d15.c_str()},
        .d_wrd_lens = {
            (uint8_t)d0.size(), (uint8_t)d1.size(), (uint8_t)d2.size(),
            (uint8_t)d3.size(), (uint8_t)d4.size(), (uint8_t)d5.size(),
            (uint8_t)d6.size(), (uint8_t)d7.size(), (uint8_t)d8.size(),
            (uint8_t)d9.size(), (uint8_t)d10.size(), (uint8_t)d11.size(),
            (uint8_t)d12.size(), (uint8_t)d13.size(), (uint8_t)d14.size(),
            (uint8_t)d15.size()}};

    auto simd = levenshtein_myers_8x16(input);

    uint32_t ref[16] = {
        levenshtein_reference(q.c_str(), q.size(), d0.c_str(), d0.size()),
        levenshtein_reference(q.c_str(), q.size(), d1.c_str(), d1.size()),
        levenshtein_reference(q.c_str(), q.size(), d2.c_str(), d2.size()),
        levenshtein_reference(q.c_str(), q.size(), d3.c_str(), d3.size()),
        levenshtein_reference(q.c_str(), q.size(), d4.c_str(), d4.size()),
        levenshtein_reference(q.c_str(), q.size(), d5.c_str(), d5.size()),
        levenshtein_reference(q.c_str(), q.size(), d6.c_str(), d6.size()),
        levenshtein_reference(q.c_str(), q.size(), d7.c_str(), d7.size()),
        levenshtein_reference(q.c_str(), q.size(), d8.c_str(), d8.size()),
        levenshtein_reference(q.c_str(), q.size(), d9.c_str(), d9.size()),
        levenshtein_reference(q.c_str(), q.size(), d10.c_str(), d10.size()),
        levenshtein_reference(q.c_str(), q.size(), d11.c_str(), d11.size()),
        levenshtein_reference(q.c_str(), q.size(), d12.c_str(), d12.size()),
        levenshtein_reference(q.c_str(), q.size(), d13.c_str(), d13.size()),
        levenshtein_reference(q.c_str(), q.size(), d14.c_str(), d14.size()),
        levenshtein_reference(q.c_str(), q.size(), d15.c_str(), d15.size()),
    };

    for (int i = 0; i < 16; i++) {
      const std::string *d = (i == 0    ? &d0
                              : i == 1  ? &d1
                              : i == 2  ? &d2
                              : i == 3  ? &d3
                              : i == 4  ? &d4
                              : i == 5  ? &d5
                              : i == 6  ? &d6
                              : i == 7  ? &d7
                              : i == 8  ? &d8
                              : i == 9  ? &d9
                              : i == 10 ? &d10
                              : i == 11 ? &d11
                              : i == 12 ? &d12
                              : i == 13 ? &d13
                              : i == 14 ? &d14
                                        : &d15);

      EXPECT_EQ(simd[i], ref[i])
          << "Mismatch (idx " << i << ") q=" << q << " d=" << *d;
    }
  }
}
