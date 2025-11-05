#include "levenshtein_myers.hpp"
#include <arm_neon.h>

static const uint8x16_t NULL_V_8 = vdupq_n_u8(0);
static const uint8x16_t ONE_V_8 = vdupq_n_u8(1);

std::array<uint8_t, 16> levenshtein_myers_8x16(const Myers8x16Input &input) {
  if (input.q_wrd_len == 0)
    return std::to_array(input.d_wrd_lens);

  uint8_t bm[ALPHABET_LEN] = {0}; // Bitmap for each letter in the alphabet

  const char *q_wrd = input.q_wrd;
  uint8_t q_wrd_len = input.q_wrd_len;
  uint8x16_t scores = vdupq_n_u8(q_wrd_len);

  uint8x16_t vp = vdupq_n_u8(0xFF);
  uint8x16_t vn = vdupq_n_u8(0);
  uint8x16_t x, y, hn, hp, d0;

  // Initialize the bitmap
  for (int i = 0; i < q_wrd_len; i++) {
    bm[q_wrd[i] - 'a'] |= 1 << i;
  }

  uint8x16_t d_wrd_lens = vld1q_u8(input.d_wrd_lens);

  uint8x16_t q_wrd_len_ls = vshlq_u8(ONE_V_8, vdupq_n_u8(q_wrd_len - 1));

  int max_d_wrd_len = std::ranges::max(input.d_wrd_lens);

  for (int i = 0; i < max_d_wrd_len; i++) {
    uint8_t c_bm_0 = bm[input.d_wrds[0][i] - 'a'];
    uint8_t c_bm_1 = bm[input.d_wrds[1][i] - 'a'];
    uint8_t c_bm_2 = bm[input.d_wrds[2][i] - 'a'];
    uint8_t c_bm_3 = bm[input.d_wrds[3][i] - 'a'];
    uint8_t c_bm_4 = bm[input.d_wrds[4][i] - 'a'];
    uint8_t c_bm_5 = bm[input.d_wrds[5][i] - 'a'];
    uint8_t c_bm_6 = bm[input.d_wrds[6][i] - 'a'];
    uint8_t c_bm_7 = bm[input.d_wrds[7][i] - 'a'];
    uint8_t c_bm_8 = bm[input.d_wrds[8][i] - 'a'];
    uint8_t c_bm_9 = bm[input.d_wrds[9][i] - 'a'];
    uint8_t c_bm_10 = bm[input.d_wrds[10][i] - 'a'];
    uint8_t c_bm_11 = bm[input.d_wrds[11][i] - 'a'];
    uint8_t c_bm_12 = bm[input.d_wrds[12][i] - 'a'];
    uint8_t c_bm_13 = bm[input.d_wrds[13][i] - 'a'];
    uint8_t c_bm_14 = bm[input.d_wrds[14][i] - 'a'];
    uint8_t c_bm_15 = bm[input.d_wrds[15][i] - 'a'];

    uint8x16_t c_bm = {c_bm_0,  c_bm_1,  c_bm_2,  c_bm_3, c_bm_4,  c_bm_5,
                       c_bm_6,  c_bm_7,  c_bm_8,  c_bm_9, c_bm_10, c_bm_11,
                       c_bm_12, c_bm_13, c_bm_14, c_bm_15};

    x = vorrq_u8(c_bm, vn);
    d0 = vorrq_u8(veorq_u8(vaddq_u8(vandq_u8(vp, x), vp), vp), x);
    hn = vandq_u8(vp, d0);
    hp = vorrq_u8(vn, vmvnq_u8(vorrq_u8(vp, d0)));
    y = vorrq_u8(vshlq_n_u8(hp, 1), ONE_V_8);
    vn = vandq_u8(y, d0);
    vp = vorrq_u8(vshlq_n_u8(hn, 1), vmvnq_u8(vorrq_u8(y, d0)));

    uint8x16_t add_score = vandq_u8(hp, q_wrd_len_ls);
    uint8x16_t sub_score = vandq_u8(hn, q_wrd_len_ls);

    uint8x16_t continue_eval = vcltq_u8(vdupq_n_u8(i), d_wrd_lens);
    uint8x16_t should_add =
        vandq_u8(continue_eval, vmvnq_u8(vceqq_u8(add_score, NULL_V_8)));
        uint8x16_t should_not_add = vmvnq_u8(should_add);
    uint8x16_t should_sub =
        vandq_u8(continue_eval, vmvnq_u8(vceqq_u8(sub_score, NULL_V_8)));

        scores = vaddq_u8(scores, vandq_u8(should_add, ONE_V_8));
        scores = vsubq_u8(
            scores, vandq_u8(vandq_u8(should_not_add, should_sub), ONE_V_8));
  }

  std::array<uint8_t, 16> out;
  vst1q_u8(out.data(), scores);
  return out;
}
