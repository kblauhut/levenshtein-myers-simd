#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <levenshtein_myers.hpp>

TEST(LevenshteinMyersTest, IdenticalStrings) {
  auto input = Myers16x8Input{.q_wrd = "hello",
                              .q_wrd_len = 5,
                              .d_wrds = {"hello", "hello", "hello", "hello",
                                         "hello", "hello", "hello", "hello"},
                              .d_wrd_lens = {5, 5, 5, 5, 5, 5, 5, 5}};
  std::array<uint16_t, 8> result = levenshtein_myers_16x8(input);
  std::array<uint16_t, 8> expected = {0, 0, 0, 0, 0, 0, 0, 0};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyersTest, SingleCharDifference) {
  auto input = Myers16x8Input{.q_wrd = "hello",
                              .q_wrd_len = 5,
                              .d_wrds = {"hella", "helko", "hxllo", "aello",
                                         "hellp", "hello", "hello", "hello"},
                              .d_wrd_lens = {5, 5, 5, 5, 5, 5, 5, 5}};
  std::array<uint16_t, 8> result = levenshtein_myers_16x8(input);
  std::array<uint16_t, 8> expected = {1, 1, 1, 1, 1, 0, 0, 0};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyersTest, InsertionOperations) {
  auto input = Myers16x8Input{
      .q_wrd = "cat",
      .q_wrd_len = 3,
      .d_wrds = {"cart", "chat", "coat", "scat", "cat", "cat", "cat", "cat"},
      .d_wrd_lens = {4, 4, 4, 4, 3, 3, 3, 3}};
  std::array<uint16_t, 8> result = levenshtein_myers_16x8(input);
  std::array<uint16_t, 8> expected = {1, 1, 1, 1, 0, 0, 0, 0};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyersTest, DeletionOperations) {
  auto input = Myers16x8Input{.q_wrd = "hello",
                              .q_wrd_len = 5,
                              .d_wrds = {"hell", "hllo", "ello", "helo",
                                         "hello", "hello", "hello", "hello"},
                              .d_wrd_lens = {4, 4, 4, 4, 5, 5, 5, 5}};
  std::array<uint16_t, 8> result = levenshtein_myers_16x8(input);
  std::array<uint16_t, 8> expected = {1, 1, 1, 1, 0, 0, 0, 0};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyersTest, EmptyStrings) {
  auto input = Myers16x8Input{.q_wrd = "",
                              .q_wrd_len = 0,
                              .d_wrds = {"", "a", "ab", "abc", "", "", "", ""},
                              .d_wrd_lens = {0, 1, 2, 3, 0, 0, 0, 0}};
  std::array<uint16_t, 8> result = levenshtein_myers_16x8(input);
  std::array<uint16_t, 8> expected = {0, 1, 2, 3, 0, 0, 0, 0};
  EXPECT_THAT(result, ::testing::ContainerEq(expected));
}

TEST(LevenshteinMyersTest, SingleCharStrings) {
  auto input =
      Myers16x8Input{.q_wrd = "a",
                     .q_wrd_len = 1,
                     .d_wrds = {"a", "b", "c", "z", "a", "a", "a", "a"},
                     .d_wrd_lens = {1, 1, 1, 1, 1, 1, 1, 1}};
  std::array<uint16_t, 8> result = levenshtein_myers_16x8(input);
  std::array<uint16_t, 8> expected = {0, 1, 1, 1, 0, 0, 0, 0};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyersTest, MaxLengthStrings) {
  auto input = Myers16x8Input{
      .q_wrd = "abcdefghijklmnop",
      .q_wrd_len = 16,
      .d_wrds = {"abcdefghijklmnop", "abcdefghijklmnop", "abcdefghijklmnop",
                 "abcdefghijklmnop", "abcdefghijklmnop", "abcdefghijklmnop",
                 "abcdefghijklmnop", "abcdefghijklmnop"},
      .d_wrd_lens = {16, 16, 16, 16, 16, 16, 16, 16}};
  std::array<uint16_t, 8> result = levenshtein_myers_16x8(input);
  std::array<uint16_t, 8> expected = {0, 0, 0, 0, 0, 0, 0, 0};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyersTest, MaxLengthWithDifferences) {
  auto input = Myers16x8Input{
      .q_wrd = "abcdefghijklmnop",
      .q_wrd_len = 16,
      .d_wrds = {"xbcdefghijklmnop", "abcdefghijklmnox", "abcxefghijklmnop",
                 "xbcdefghijklmnox", "abcdefghijklmnop", "abcdefghijklmnop",
                 "abcdefghijklmnop", "abcdefghijklmnop"},
      .d_wrd_lens = {16, 16, 16, 16, 16, 16, 16, 16}};
  std::array<uint16_t, 8> result = levenshtein_myers_16x8(input);
  std::array<uint16_t, 8> expected = {1, 1, 1, 2, 0, 0, 0, 0};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyersTest, CompletelyDifferentStrings) {
  auto input = Myers16x8Input{
      .q_wrd = "abc",
      .q_wrd_len = 3,
      .d_wrds = {"xyz", "def", "ghi", "jkl", "abc", "abc", "abc", "abc"},
      .d_wrd_lens = {3, 3, 3, 3, 3, 3, 3, 3}};
  std::array<uint16_t, 8> result = levenshtein_myers_16x8(input);
  std::array<uint16_t, 8> expected = {3, 3, 3, 3, 0, 0, 0, 0};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyersTest, TranspositionLikePatterns) {
  auto input = Myers16x8Input{.q_wrd = "abcd",
                              .q_wrd_len = 4,
                              .d_wrds = {"abdc", "bacd", "acbd", "dcba", "abcd",
                                         "abcd", "abcd", "abcd"},
                              .d_wrd_lens = {4, 4, 4, 4, 4, 4, 4, 4}};
  std::array<uint16_t, 8> result = levenshtein_myers_16x8(input);
  // Note: Basic Levenshtein distance counts transpositions as 2 operations
  std::array<uint16_t, 8> expected = {2, 2, 2, 4, 0, 0, 0, 0};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyersTest, MixedLengths) {
  auto input = Myers16x8Input{
      .q_wrd = "test",
      .q_wrd_len = 4,
      .d_wrds = {"t", "te", "tes", "test", "tests", "testing", "test", "test"},
      .d_wrd_lens = {1, 2, 3, 4, 5, 7, 4, 4}};
  std::array<uint16_t, 8> result = levenshtein_myers_16x8(input);
  std::array<uint16_t, 8> expected = {3, 2, 1, 0, 1, 3, 0, 0};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyersTest, PrefixSuffixDifferences) {
  auto input =
      Myers16x8Input{.q_wrd = "middle",
                     .q_wrd_len = 6,
                     .d_wrds = {"xmiddle", "middlex", "xxmiddle", "middlexx",
                                "middle", "middle", "middle", "middle"},
                     .d_wrd_lens = {7, 7, 8, 8, 6, 6, 6, 6}};
  std::array<uint16_t, 8> result = levenshtein_myers_16x8(input);
  std::array<uint16_t, 8> expected = {1, 1, 2, 2, 0, 0, 0, 0};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyersTest, RepeatedCharacters) {
  auto input = Myers16x8Input{
      .q_wrd = "aaa",
      .q_wrd_len = 3,
      .d_wrds = {"aa", "aaaa", "aaaaaa", "bbb", "aaa", "aaa", "aaa", "aaa"},
      .d_wrd_lens = {2, 4, 6, 3, 3, 3, 3, 3}};
  std::array<uint16_t, 8> result = levenshtein_myers_16x8(input);
  std::array<uint16_t, 8> expected = {1, 1, 3, 3, 0, 0, 0, 0};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyersTest, AllDifferentTargets) {
  auto input = Myers16x8Input{.q_wrd = "alpha",
                              .q_wrd_len = 5,
                              .d_wrds = {"alpha", "beta", "gamma", "delta",
                                         "epsilon", "zeta", "eta", "theta"},
                              .d_wrd_lens = {5, 4, 5, 5, 7, 4, 3, 5}};
  std::array<uint16_t, 8> result = levenshtein_myers_16x8(input);
  std::array<uint16_t, 8> expected = {0, 4, 4, 4, 7, 4, 4, 4};
  EXPECT_EQ(result, expected);
}

TEST(LevenshteinMyersTest, LengthLimited) {
  auto input = Myers16x8Input{.q_wrd = "aaaa",
                              .q_wrd_len = 4,
                              .d_wrds = {"bbbbbbbbbbbbbbbb", "bbbbbbbbbbbbbbbb", "bbbbbbbbbbbbbbbb", "bbbbbbbbbbbbbbbb",
                                         "bbbbbbbbbbbbbbbb", "bbbbbbbbbbbbbbbb", "bbbbbbbbbbbbbbbb", "bbbbbbbbbbbbbbbb"},
                              .d_wrd_lens = {1, 2, 4, 8, 16, 0, 0, 0}};
  std::array<uint16_t, 8> result = levenshtein_myers_16x8(input);
  std::array<uint16_t, 8> expected = {4, 4, 4, 8, 16, 4, 4, 4};
  EXPECT_EQ(result, expected);
}
