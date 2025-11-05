#include "levenshtein_myers.hpp"
#include <arm_neon.h>

static const uint32x4_t NULL_V = vdupq_n_u32(0);
static const uint32x4_t ONE_V = vdupq_n_u32(1);

std::array<uint32_t, 4> levenshtein_myers_32x4(const Myers32x4Input &input) {
  if (input.q_wrd_len == 0)
    return std::to_array(input.d_wrd_lens);

  uint32_t bm[ALPHABET_LEN] = {0}; // Bitmap for each letter in the alphabet

  const char *q_wrd = input.q_wrd;
  uint32_t q_wrd_len = input.q_wrd_len;
  uint32x4_t scores = vdupq_n_u32(q_wrd_len);

  uint32x4_t vp = vdupq_n_u32(0xFFFFFFFF);
  uint32x4_t vn = vdupq_n_u32(0);
  uint32x4_t x, y, hn, hp, d0;

  // Initialize the bitmap
  for (int i = 0; i < q_wrd_len; i++) {
    bm[q_wrd[i] - 'a'] |= 1 << i;
  }

  uint32x4_t d_wrd_lens = vld1q_u32(input.d_wrd_lens);

  uint32x4_t q_wrd_len_ls = vshlq_u32(ONE_V, vdupq_n_u32(q_wrd_len - 1));

  int max_d_wrd_len =std::ranges::max(input.d_wrd_lens);

  for (int i = 0; i < max_d_wrd_len; i++) {
    uint32_t c_bm_0 = bm[input.d_wrds[0][i] - 'a'];
    uint32_t c_bm_1 = bm[input.d_wrds[1][i] - 'a'];
    uint32_t c_bm_2 = bm[input.d_wrds[2][i] - 'a'];
    uint32_t c_bm_3 = bm[input.d_wrds[3][i] - 'a'];
    uint32x4_t c_bm = {c_bm_0, c_bm_1, c_bm_2, c_bm_3};

    x = vorrq_u32(c_bm, vn);
    d0 = vorrq_u32(veorq_u32(vaddq_u32(vandq_u32(vp, x), vp), vp), x);
    hn = vandq_u32(vp, d0);
    hp = vorrq_u32(vn, vmvnq_u32(vorrq_u32(vp, d0)));
    y = vorrq_u32(vshlq_n_u32(hp, 1), ONE_V);
    vn = vandq_u32(y, d0);
    vp = vorrq_u32(vshlq_n_u32(hn, 1), vmvnq_u32(vorrq_u32(y, d0)));

    uint32x4_t add_score = vandq_u32(hp, q_wrd_len_ls);
    uint32x4_t sub_score = vandq_u32(hn, q_wrd_len_ls);

    uint32x4_t continue_eval = vcltq_u32(vdupq_n_u32(i), d_wrd_lens);
    uint32x4_t should_add =
        vandq_u32(continue_eval, vmvnq_u32(vceqq_u32(add_score, NULL_V)));
    uint32x4_t should_not_add = vmvnq_u32(should_add);
    uint32x4_t should_sub =
        vandq_u32(continue_eval, vmvnq_u32(vceqq_u32(sub_score, NULL_V)));

    scores = vaddq_u32(scores, vandq_u32(should_add, ONE_V));
    scores = vsubq_u32(scores,
                       vandq_u32(vandq_u32(should_not_add, should_sub), ONE_V));
  }

  std::array<uint32_t, 4> out;
  vst1q_u32(out.data(), scores);
  return out;
}
