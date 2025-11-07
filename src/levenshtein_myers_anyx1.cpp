#include "levenshtein_myers.hpp"
#include <arm_neon.h>

void bitshift_left(uint8_t *in_ptr, uint8_t *out_ptr, int ls, size_t size) {
  size_t byte_shift = ls / 8;
  unsigned int bit_shift = ls % 8;

  // Shift by whole bytes (assuming little-endian: LSB at index 0)
  for (size_t i = 0; i < size; i++) {
    if (i < byte_shift) {
      out_ptr[i] = 0;
    } else {
      out_ptr[i] = in_ptr[i - byte_shift];
    }
  }

  // Shift remaining bits
  if (bit_shift > 0) {
    uint8_t carry = 0;
    for (size_t i = 0; i < size; i++) {
      uint8_t new_carry = out_ptr[i] >> (8 - bit_shift);
      out_ptr[i] = (out_ptr[i] << bit_shift) | carry;
      carry = new_carry;
    }
  }
}

void bitwise_or(uint8_t *in_ptr1, uint8_t *in_ptr2, uint8_t *out_ptr,
                size_t size) {
  for (size_t i = 0; i < size; i++) {
    out_ptr[i] = in_ptr1[i] | in_ptr2[i];
  }
}

void bitwise_and(uint8_t *in_ptr1, uint8_t *in_ptr2, uint8_t *out_ptr,
                 size_t size) {
  for (size_t i = 0; i < size; i++) {
    out_ptr[i] = in_ptr1[i] & in_ptr2[i];
  }
}

void bitwise_xor(uint8_t *in_ptr1, uint8_t *in_ptr2, uint8_t *out_ptr,
                 size_t size) {
  for (size_t i = 0; i < size; i++) {
    out_ptr[i] = in_ptr1[i] ^ in_ptr2[i];
  }
}

void bitwise_not(uint8_t *in_ptr, uint8_t *out_ptr, size_t size) {
  for (size_t i = 0; i < size; i++) {
    out_ptr[i] = ~in_ptr[i];
  }
}

void add(uint8_t *in_ptr1, uint8_t *in_ptr2, uint8_t *out_ptr, size_t size) {
  uint16_t carry = 0;

  // Add byte by byte from LSB to MSB (assuming little-endian)
  for (size_t i = 0; i < size; i++) {
    uint16_t sum = (uint16_t)in_ptr1[i] + (uint16_t)in_ptr2[i] + carry;
    out_ptr[i] = (uint8_t)(sum & 0xFF);
    carry = sum >> 8;
  }
}

bool is_not_zero(uint8_t *in_ptr, size_t size) {
  for (size_t i = 0; i < size; i++) {
    if (in_ptr[i] != 0)
      return true;
  }
  return false;
}

uint32_t levenshtein_myers_anyx1(const char *q_wrd, int q_wrd_len,
                                 const char *d_wrd, int d_wrd_len) {
  if (q_wrd_len == 0)
    return d_wrd_len;

  int max_wrd_len = std::max(q_wrd_len, d_wrd_len);
  size_t word_bytes = (max_wrd_len + 7) / 8;

  uint8_t *mem = (uint8_t *)std::calloc(
      ALPHABET_LEN + 10, word_bytes);

  // Initialize variables
  uint8_t *bm = mem;
  uint8_t *vp = bm + (ALPHABET_LEN * word_bytes);
  uint8_t *vn = vp + word_bytes;
  uint8_t *x = vn + word_bytes;
  uint8_t *y = x + word_bytes;
  uint8_t *hn = y + word_bytes;
  uint8_t *hp = hn + word_bytes;
  uint8_t *d0 = hp + word_bytes;
  uint8_t *one = d0 + word_bytes;
  uint8_t *tmp0 = one + word_bytes;   // Used for temporary storage
  uint8_t *tmp1 = tmp0 + word_bytes; // Used for temporary storage

  // Set up one and vp.
  // vn is already initialized because its set to 0 with calloc
  for (int i = 0; i < word_bytes; i++) {
    *(vp + i) = 0xFF;
  }
  *one = 1;

  // Initialize the bitmap
  for (int i = 0; i < q_wrd_len; i++) {
    uint8_t char_idx = q_wrd[i] - 'a';
    uint8_t *bm_ptr = bm + char_idx * word_bytes;

    bitshift_left(one, tmp0, i, word_bytes);
    bitwise_or(tmp0, bm_ptr, bm_ptr, word_bytes);
  }

  uint32_t score = q_wrd_len;
  for (int i = 0; i < d_wrd_len; i++) {
    uint8_t char_idx = d_wrd[i] - 'a';
    uint8_t *c_bm = bm + char_idx * word_bytes;

    // Set x
    // uint64_t x = c_bm | vn;
    bitwise_or(c_bm, vn, x, word_bytes);

    // Set d0
    // uint64_t d0 = ((vp + (x & vp)) ^ vp) | x;
    bitwise_and(x, vp, tmp0, word_bytes);
    add(tmp0, vp, tmp0, word_bytes);
    bitwise_xor(tmp0, vp, tmp0, word_bytes);
    bitwise_or(tmp0, x, d0, word_bytes);

    // Set hn
    // uint64_t hn = vp & d0;
    bitwise_and(vp, d0, hn, word_bytes);

    // Set hp
    // uint64_t hp = vn | ~(vp | d0);
    bitwise_or(vp, d0, tmp0, word_bytes);
    bitwise_not(tmp0, tmp0, word_bytes);
    bitwise_or(tmp0, vn, hp, word_bytes);

    // Set y
    // uint64_t y = (hp << 1) | 1;
    bitshift_left(hp, y, 1, word_bytes);
    *y |= 1;

    // Set vn
    // vn = y & d0;
    bitwise_and(y, d0, vn, word_bytes);

    // Set vp
    // vp = (hn << 1) | ~(y | d0);
    bitshift_left(hn, tmp0, 1, word_bytes);
    bitwise_or(y, d0, tmp1, word_bytes);
    bitwise_not(tmp1, tmp1, word_bytes);
    bitwise_or(tmp0, tmp1, vp, word_bytes);

    // Set up t0 and t1 as comp values
    bitshift_left(one, tmp0, q_wrd_len - 1, word_bytes);
    bitwise_and(hn, tmp0, tmp1, word_bytes); // (hn & (uint64_t(1) << (q_wrd_len - 1))
    bitwise_and(hp, tmp0, tmp0, word_bytes); // (hp & (uint64_t(1) << (q_wrd_len - 1))

    if (is_not_zero(tmp0, word_bytes)) {
      score++;
    } else if (is_not_zero(tmp1, word_bytes)) {
      score--;
    }
  }

  return score;
}
