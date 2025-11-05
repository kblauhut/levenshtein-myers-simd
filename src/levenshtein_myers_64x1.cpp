#include "levenshtein_myers.hpp"
#include <arm_neon.h>

uint32_t levenshtein_myers_64x1(const char *q_wrd, int q_wrd_len,
                                const char *d_wrd, int d_wrd_len) {
  if (q_wrd_len == 0)
    return d_wrd_len;

  uint64_t bm[ALPHABET_LEN] = {0}; // Bitmap for each letter in the alphabet

  // Initialize the bitmap
  for (int i = 0; i < q_wrd_len; i++) {
    bm[q_wrd[i] - 'a'] |= (uint64_t(1) << i);
  }

  uint64_t vp = ~0ULL;
  uint64_t vn = 0;
  uint32_t score = q_wrd_len;

  for (int i = 0; i < d_wrd_len; i++) {
    uint64_t c_bm = bm[d_wrd[i] - 'a'];

    uint64_t x = c_bm | vn;
    uint64_t d0 = ((vp + (x & vp)) ^ vp) | x;
    uint64_t hn = vp & d0;
    uint64_t hp = vn | ~(vp | d0);
    uint64_t y = (hp << 1) | 1;
    vn = y & d0;
    vp = (hn << 1) | ~(y | d0);

    if ((hp & (uint64_t(1) << (q_wrd_len - 1))) != 0) {
      score++;
    } else if ((hn & (uint64_t(1) << (q_wrd_len - 1))) != 0) {
      score--;
    }
  }

  return score;
}
