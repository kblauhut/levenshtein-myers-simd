#include "levenshtein_myers.hpp"
#include <arm_neon.h>

static const uint64x2_t NULL_V = vdupq_n_u64(0);
static const uint64x2_t ONE_V = vdupq_n_u64(1);

uint64x2_t not_u64(uint64x2_t a) { return veorq_u64(a, vdupq_n_u64(~0ULL)); }

std::array<uint64_t, 2> levenshtein_myers_64x2(const Myers64x2Input &input) {
  if (input.q_wrd_len == 0)
    return std::to_array(input.d_wrd_lens);

  uint64_t bm[ALPHABET_LEN] = {0}; // Bitmap for each letter in the alphabet

  const char *q_wrd = input.q_wrd;
  uint64_t q_wrd_len = input.q_wrd_len;
  uint64x2_t scores = vdupq_n_u64(q_wrd_len);

  uint64x2_t vp = vdupq_n_u64(~0ULL);
  uint64x2_t vn = vdupq_n_u64(0);
  uint64x2_t x, y, hn, hp, d0;

  // Initialize the bitmap
  for (int i = 0; i < q_wrd_len; i++) {
      bm[(unsigned char)q_wrd[i] - 'a'] |= (uint64_t(1) << i);
  }

  uint64x2_t d_wrd_lens = vld1q_u64(input.d_wrd_lens);

  uint64x2_t q_wrd_len_ls = vshlq_u64(ONE_V, vdupq_n_u64(q_wrd_len - 1));

  int max_d_wrd_len = std::max(input.d_wrd_lens[0], input.d_wrd_lens[1]);

  for (int i = 0; i < max_d_wrd_len; i++) {
    uint64_t c_bm_0 = bm[input.d_wrds[0][i] - 'a'];
    uint64_t c_bm_1 = bm[input.d_wrds[1][i] - 'a'];
    uint64x2_t c_bm = {c_bm_0, c_bm_1};

    x = vorrq_u64(c_bm, vn);
    d0 = vorrq_u64(veorq_u64(vaddq_u64(vandq_u64(vp, x), vp), vp), x);
    hn = vandq_u64(vp, d0);
    hp = vorrq_u64(vn, not_u64(vorrq_u64(vp, d0)));
    y = vorrq_u64(vshlq_n_u64(hp, 1), ONE_V);
    vn = vandq_u64(y, d0);
    vp = vorrq_u64(vshlq_n_u64(hn, 1), not_u64(vorrq_u64(y, d0)));

    uint64x2_t add_score = vandq_u64(hp, q_wrd_len_ls);
    uint64x2_t sub_score = vandq_u64(hn, q_wrd_len_ls);

    uint64x2_t continue_eval = vcltq_u64(vdupq_n_u64(i), d_wrd_lens);
    uint64x2_t should_add =
        vandq_u64(continue_eval, not_u64(vceqq_u64(add_score, NULL_V)));
    uint64x2_t should_not_add = not_u64(should_add);
    uint64x2_t should_sub =
        vandq_u64(continue_eval, not_u64(vceqq_u64(sub_score, NULL_V)));

    scores = vaddq_u64(scores, vandq_u64(should_add, ONE_V));
    scores = vsubq_u64(scores,
                       vandq_u64(vandq_u64(should_not_add, should_sub), ONE_V));
  }

  return std::array<uint64_t, 2>{vgetq_lane_u64(scores, 0),
                                 vgetq_lane_u64(scores, 1)};
}
