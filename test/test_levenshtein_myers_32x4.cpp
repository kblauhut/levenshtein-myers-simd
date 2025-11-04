#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <levenshtein_myers.hpp>

TEST(LevenshteinMyers32x4Test, IdenticalStrings) {
  auto input = Myers32x4Input{.q_wrd = "hello",
                              .q_wrd_len = 5,
                              .d_wrds = {"hello", "hello", "hello", "hello"},
                              .d_wrd_lens = {5, 5, 5, 5}};
  std::array<uint32_t, 4> result = levenshtein_myers_32x4(input);
  std::array<uint32_t, 4> expected = {0, 0, 0, 0};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers32x4Test, SingleCharDifference) {
  auto input = Myers32x4Input{.q_wrd = "hello",
                              .q_wrd_len = 5,
                              .d_wrds = {"hella", "helko", "hxllo", "aello"},
                              .d_wrd_lens = {5, 5, 5, 5}};
  std::array<uint32_t, 4> result = levenshtein_myers_32x4(input);
  std::array<uint32_t, 4> expected = {1, 1, 1, 1};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers32x4Test, InsertionOperations) {
  auto input = Myers32x4Input{
      .q_wrd = "cat",
      .q_wrd_len = 3,
      .d_wrds = {"cart", "chat", "coat", "scat"},
      .d_wrd_lens = {4, 4, 4, 4}};
  std::array<uint32_t, 4> result = levenshtein_myers_32x4(input);
  std::array<uint32_t, 4> expected = {1, 1, 1, 1};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers32x4Test, DeletionOperations) {
  auto input = Myers32x4Input{.q_wrd = "hello",
                              .q_wrd_len = 5,
                              .d_wrds = {"hell", "hllo", "ello", "helo"},
                              .d_wrd_lens = {4, 4, 4, 4}};
  std::array<uint32_t, 4> result = levenshtein_myers_32x4(input);
  std::array<uint32_t, 4> expected = {1, 1, 1, 1};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers32x4Test, EmptyStrings) {
  auto input = Myers32x4Input{.q_wrd = "",
                              .q_wrd_len = 0,
                              .d_wrds = {"", "a", "ab", "abc"},
                              .d_wrd_lens = {0, 1, 2, 3}};
  std::array<uint32_t, 4> result = levenshtein_myers_32x4(input);
  std::array<uint32_t, 4> expected = {0, 1, 2, 3};
  EXPECT_THAT(result, ::testing::ContainerEq(expected));
}

TEST(LevenshteinMyers32x4Test, SingleCharStrings) {
  auto input =
      Myers32x4Input{.q_wrd = "a",
                     .q_wrd_len = 1,
                     .d_wrds = {"a", "b", "c", "z"},
                     .d_wrd_lens = {1, 1, 1, 1}};
  std::array<uint32_t, 4> result = levenshtein_myers_32x4(input);
  std::array<uint32_t, 4> expected = {0, 1, 1, 1};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers32x4Test, MaxLengthStrings) {
  auto input = Myers32x4Input{
      .q_wrd = "abcdefghijklmnop",
      .q_wrd_len = 16,
      .d_wrds = {"abcdefghijklmnop", "abcdefghijklmnop",
                 "abcdefghijklmnop", "abcdefghijklmnop"},
      .d_wrd_lens = {16, 16, 16, 16}};
  std::array<uint32_t, 4> result = levenshtein_myers_32x4(input);
  std::array<uint32_t, 4> expected = {0, 0, 0, 0};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers32x4Test, MaxLengthWithDifferences) {
  auto input = Myers32x4Input{
      .q_wrd = "abcdefghijklmnop",
      .q_wrd_len = 16,
      .d_wrds = {"xbcdefghijklmnop", "abcdefghijklmnox",
                 "abcxefghijklmnop", "xbcdefghijklmnox"},
      .d_wrd_lens = {16, 16, 16, 16}};
  std::array<uint32_t, 4> result = levenshtein_myers_32x4(input);
  std::array<uint32_t, 4> expected = {1, 1, 1, 2};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers32x4Test, CompletelyDifferentStrings) {
  auto input = Myers32x4Input{
      .q_wrd = "abc",
      .q_wrd_len = 3,
      .d_wrds = {"xyz", "def", "ghi", "jkl"},
      .d_wrd_lens = {3, 3, 3, 3}};
  std::array<uint32_t, 4> result = levenshtein_myers_32x4(input);
  std::array<uint32_t, 4> expected = {3, 3, 3, 3};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers32x4Test, TranspositionLikePatterns) {
  auto input = Myers32x4Input{.q_wrd = "abcd",
                              .q_wrd_len = 4,
                              .d_wrds = {"abdc", "bacd", "acbd", "dcba"},
                              .d_wrd_lens = {4, 4, 4, 4}};
  std::array<uint32_t, 4> result = levenshtein_myers_32x4(input);
  std::array<uint32_t, 4> expected = {2, 2, 2, 4}; // transpositions count as 2
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers32x4Test, MixedLengths) {
  auto input = Myers32x4Input{
      .q_wrd = "test",
      .q_wrd_len = 4,
      .d_wrds = {"t", "te", "tes", "test"},
      .d_wrd_lens = {1, 2, 3, 4}};
  std::array<uint32_t, 4> result = levenshtein_myers_32x4(input);
  std::array<uint32_t, 4> expected = {3, 2, 1, 0};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers32x4Test, PrefixSuffixDifferences) {
  auto input =
      Myers32x4Input{.q_wrd = "middle",
                     .q_wrd_len = 6,
                     .d_wrds = {"xmiddle", "middlex", "xxmiddle", "middlexx"},
                     .d_wrd_lens = {7, 7, 8, 8}};
  std::array<uint32_t, 4> result = levenshtein_myers_32x4(input);
  std::array<uint32_t, 4> expected = {1, 1, 2, 2};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers32x4Test, RepeatedCharacters) {
  auto input = Myers32x4Input{
      .q_wrd = "aaa",
      .q_wrd_len = 3,
      .d_wrds = {"aa", "aaaa", "aaaaaa", "bbb"},
      .d_wrd_lens = {2, 4, 6, 3}};
  std::array<uint32_t, 4> result = levenshtein_myers_32x4(input);
  std::array<uint32_t, 4> expected = {1, 1, 3, 3};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers32x4Test, AllDifferentTargets) {
  auto input = Myers32x4Input{.q_wrd = "alpha",
                              .q_wrd_len = 5,
                              .d_wrds = {"alpha", "beta", "gamma", "delta"},
                              .d_wrd_lens = {5, 4, 5, 5}};
  std::array<uint32_t, 4> result = levenshtein_myers_32x4(input);
  std::array<uint32_t, 4> expected = {0, 4, 4, 4};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers32x4Test, LengthLimited) {
  auto input = Myers32x4Input{.q_wrd = "aaaa",
                              .q_wrd_len = 4,
                              .d_wrds = {"bbbbbbbbbbbbbbbb", "bbbbbbbbbbbbbbbb",
                                         "bbbbbbbbbbbbbbbb", "bbbbbbbbbbbbbbbb"},
                              .d_wrd_lens = {1, 2, 4, 8}};
  std::array<uint32_t, 4> result = levenshtein_myers_32x4(input);
  std::array<uint32_t, 4> expected = {4, 4, 4, 8};
  EXPECT_EQ(result, expected);
}
