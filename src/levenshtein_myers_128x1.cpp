#include "levenshtein_myers.hpp"
#include <arm_neon.h>

uint32_t levenshtein_myers_128x1(const char *q_wrd, int q_wrd_len,
                                 const char *d_wrd, int d_wrd_len) {
  if (q_wrd_len == 0)
    return d_wrd_len;


  return 0;
}
