#include "levenshtein_myers.hpp"
#include <arm_neon.h>

uint32_t levenshtein_myers_anyx1(const char *q_wrd, int q_wrd_len,
                                 const char *d_wrd, int d_wrd_len) {
  if (q_wrd_len == 0)
    return d_wrd_len;

  int max_wrd_len = std::max(q_wrd_len, d_wrd_len);
  int bitmap_bytes = (max_wrd_len + 7) / 8;

  return 0;
}
