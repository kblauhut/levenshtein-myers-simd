#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <levenshtein_myers.hpp>

TEST(LevenshteinMyers64x2Test, IdenticalStrings) {
  auto input = Myers64x2Input{.q_wrd = "hello",
                              .q_wrd_len = 5,
                              .d_wrds = {"hello", "hello"},
                              .d_wrd_lens = {5, 5}};
  std::array<uint64_t, 2> result = levenshtein_myers_64x2(input);
  std::array<uint64_t, 2> expected = {0, 0};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers64x2Test, SingleCharDifference) {
  auto input = Myers64x2Input{.q_wrd = "hello",
                              .q_wrd_len = 5,
                              .d_wrds = {"hella", "helko"},
                              .d_wrd_lens = {5, 5}};
  std::array<uint64_t, 2> result = levenshtein_myers_64x2(input);
  std::array<uint64_t, 2> expected = {1, 1};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers64x2Test, InsertionOperations) {
  auto input = Myers64x2Input{
      .q_wrd = "cat",
      .q_wrd_len = 3,
      .d_wrds = {"cart", "coat"},
      .d_wrd_lens = {4, 4}};
  std::array<uint64_t, 2> result = levenshtein_myers_64x2(input);
  std::array<uint64_t, 2> expected = {1, 1};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers64x2Test, DeletionOperations) {
  auto input = Myers64x2Input{.q_wrd = "hello",
                              .q_wrd_len = 5,
                              .d_wrds = {"hell", "hllo"},
                              .d_wrd_lens = {4, 4}};
  std::array<uint64_t, 2> result = levenshtein_myers_64x2(input);
  std::array<uint64_t, 2> expected = {1, 1};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers64x2Test, EmptyStrings) {
  auto input = Myers64x2Input{.q_wrd = "",
                              .q_wrd_len = 0,
                              .d_wrds = {"", "abc"},
                              .d_wrd_lens = {0, 3}};
  std::array<uint64_t, 2> result = levenshtein_myers_64x2(input);
  std::array<uint64_t, 2> expected = {0, 3};
  EXPECT_THAT(result, ::testing::ContainerEq(expected));
}

TEST(LevenshteinMyers64x2Test, SingleCharStrings) {
  auto input =
      Myers64x2Input{.q_wrd = "a",
                     .q_wrd_len = 1,
                     .d_wrds = {"a", "z"},
                     .d_wrd_lens = {1, 1}};
  std::array<uint64_t, 2> result = levenshtein_myers_64x2(input);
  std::array<uint64_t, 2> expected = {0, 1};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers64x2Test, MaxLengthStrings) {
  auto input = Myers64x2Input{
      .q_wrd = "abcdefghijklmnop",
      .q_wrd_len = 16,
      .d_wrds = {"abcdefghijklmnop", "abcdefghijklmnop"},
      .d_wrd_lens = {16, 16}};
  std::array<uint64_t, 2> result = levenshtein_myers_64x2(input);
  std::array<uint64_t, 2> expected = {0, 0};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers64x2Test, MaxLengthWithDifferences) {
  auto input = Myers64x2Input{
      .q_wrd = "abcdefghijklmnop",
      .q_wrd_len = 16,
      .d_wrds = {"xbcdefghijklmnop", "abcdefghijklmnox"},
      .d_wrd_lens = {16, 16}};
  std::array<uint64_t, 2> result = levenshtein_myers_64x2(input);
  std::array<uint64_t, 2> expected = {1, 1};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers64x2Test, CompletelyDifferentStrings) {
  auto input = Myers64x2Input{
      .q_wrd = "abc",
      .q_wrd_len = 3,
      .d_wrds = {"xyz", "def"},
      .d_wrd_lens = {3, 3}};
  std::array<uint64_t, 2> result = levenshtein_myers_64x2(input);
  std::array<uint64_t, 2> expected = {3, 3};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers64x2Test, TranspositionLikePatterns) {
  auto input = Myers64x2Input{.q_wrd = "abcd",
                              .q_wrd_len = 4,
                              .d_wrds = {"abdc", "bacd"},
                              .d_wrd_lens = {4, 4}};
  std::array<uint64_t, 2> result = levenshtein_myers_64x2(input);
  std::array<uint64_t, 2> expected = {2, 2};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers64x2Test, MixedLengths) {
  auto input = Myers64x2Input{
      .q_wrd = "test",
      .q_wrd_len = 4,
      .d_wrds = {"t", "tests"},
      .d_wrd_lens = {1, 5}};
  std::array<uint64_t, 2> result = levenshtein_myers_64x2(input);
  std::array<uint64_t, 2> expected = {3, 1};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers64x2Test, PrefixSuffixDifferences) {
  auto input =
      Myers64x2Input{.q_wrd = "middle",
                     .q_wrd_len = 6,
                     .d_wrds = {"xmiddle", "middlex"},
                     .d_wrd_lens = {7, 7}};
  std::array<uint64_t, 2> result = levenshtein_myers_64x2(input);
  std::array<uint64_t, 2> expected = {1, 1};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers64x2Test, RepeatedCharacters) {
  auto input = Myers64x2Input{
      .q_wrd = "aaa",
      .q_wrd_len = 3,
      .d_wrds = {"aa", "aaaaaa"},
      .d_wrd_lens = {2, 6}};
  std::array<uint64_t, 2> result = levenshtein_myers_64x2(input);
  std::array<uint64_t, 2> expected = {1, 3};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers64x2Test, AllDifferentTargets) {
  auto input = Myers64x2Input{.q_wrd = "alpha",
                              .q_wrd_len = 5,
                              .d_wrds = {"alpha", "beta"},
                              .d_wrd_lens = {5, 4}};
  std::array<uint64_t, 2> result = levenshtein_myers_64x2(input);
  std::array<uint64_t, 2> expected = {0, 4};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyers64x2Test, LengthLimited) {
  auto input = Myers64x2Input{.q_wrd = "aaaa",
                              .q_wrd_len = 4,
                              .d_wrds = {"bbbbbbbbbbbbbbbb", "bbbbbbbbbbbbbbbb"},
                              .d_wrd_lens = {1, 8}};
  std::array<uint64_t, 2> result = levenshtein_myers_64x2(input);
  std::array<uint64_t, 2> expected = {4, 8};
  EXPECT_EQ(result, expected);
}
