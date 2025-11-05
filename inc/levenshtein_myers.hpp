#pragma once
#include <stdint.h>
#include <array>

#define ALPHABET_LEN 26

struct Myers8x16Input {
  const char *q_wrd;
  int q_wrd_len;
  const char *d_wrds[16];
  uint8_t d_wrd_lens[16];
};

struct Myers16x8Input {
  const char *q_wrd;
  int q_wrd_len;
  const char *d_wrds[8];
  uint16_t d_wrd_lens[8];
};

struct Myers32x4Input {
  const char *q_wrd;
  int q_wrd_len;
  const char *d_wrds[4];
  uint32_t d_wrd_lens[4];
};

struct Myers64x2Input {
  const char *q_wrd;
  int q_wrd_len;
  const char *d_wrds[2];
  uint64_t d_wrd_lens[2];
};

std::array<uint8_t, 16> levenshtein_myers_8x16(const Myers8x16Input &input);
std::array<uint16_t, 8> levenshtein_myers_16x8(const Myers16x8Input &input);
std::array<uint32_t, 4> levenshtein_myers_32x4(const Myers32x4Input &input);
std::array<uint64_t, 2> levenshtein_myers_64x2(const Myers64x2Input &input);
uint32_t levenshtein_myers_128x1(const char *q_wrd, int q_wrd_len, const char *d_wrd,
                       int d_wrd_len);
uint32_t levenshtein_myers_anyx1(const char *q_wrd, int q_wrd_len, const char *d_wrd,
                       int d_wrd_len);
