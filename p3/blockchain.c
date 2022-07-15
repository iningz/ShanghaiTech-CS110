/*********************************************************************
Homework 5
CS 110: Computer Architecture, Spring 2021
ShanghaiTech University

* Last Modified: 03/28/2021
*********************************************************************/

#define _GNU_SOURCE
#include "blockchain.h"
#include "omp.h"
#include <immintrin.h>
#include <stdlib.h>
#include <string.h>

void blockchain_node_init(blk_t *node, uint32_t index, uint32_t timestamp,
                          unsigned char prev_hash[32], unsigned char *data,
                          size_t data_size) {
  if (!node || !data || !prev_hash)
    return;

  node->header.index = index;
  node->header.timestamp = timestamp;
  node->header.nonce = -1;

  memset(node->header.data, 0, sizeof(unsigned char) * 256);
  memcpy(node->header.prev_hash, prev_hash, HASH_BLOCK_SIZE);
  memcpy(node->header.data, data,
         sizeof(unsigned char) * ((data_size < 256) ? data_size : 256));
}

void blockchain_node_hash(blk_t *node, unsigned char hash_buf[HASH_BLOCK_SIZE],
                          hash_func func) {
  if (node)
    func((unsigned char *)node, sizeof(blkh_t), (unsigned char *)hash_buf);
}

BOOL blockchain_node_verify(blk_t *node, blk_t *prev_node, hash_func func) {
  unsigned char hash_buf[HASH_BLOCK_SIZE];

  if (!node || !prev_node)
    return False;

  blockchain_node_hash(node, hash_buf, func);
  if (memcmp(node->hash, hash_buf, sizeof(unsigned char) * HASH_BLOCK_SIZE))
    return False;

  blockchain_node_hash(prev_node, hash_buf, func);
  if (memcmp(node->header.prev_hash, hash_buf,
             sizeof(unsigned char) * HASH_BLOCK_SIZE))
    return False;

  return True;
}

typedef unsigned char BYTE;
typedef unsigned int WORD;
typedef struct {
  BYTE block[8][64];
  WORD datalen;
  unsigned long bitlen;
  __m256i state[8];
} CTX_V;

static const WORD k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

#define ROTLI_V(a, bint) (_mm256_or_si256(_mm256_slli_epi32(a, bint), _mm256_srli_epi32(a, 32 - (bint))))
#define ROTRI_V(a, bint) (_mm256_or_si256(_mm256_srli_epi32(a, bint), _mm256_slli_epi32(a, 32 - (bint))))
#define SIG0_V(x) (_mm256_xor_si256(_mm256_xor_si256(ROTRI_V(x, 7), ROTRI_V(x, 18)), _mm256_srli_epi32(x, 3)))
#define SIG1_V(x) (_mm256_xor_si256(_mm256_xor_si256(ROTRI_V(x, 17), ROTRI_V(x, 19)), _mm256_srli_epi32(x, 10)))
#define CH_V(x, y, z) (_mm256_xor_si256(_mm256_and_si256(x, y), _mm256_andnot_si256(x, z)))
#define MAJ_V(x, y, z) (_mm256_xor_si256(_mm256_xor_si256(_mm256_and_si256(x, y), _mm256_and_si256(x, z)), _mm256_and_si256(y, z)))
#define EP0_V(x) (_mm256_xor_si256(_mm256_xor_si256(ROTRI_V(x, 2), ROTRI_V(x, 13)), ROTRI_V(x, 22)))
#define EP1_V(x) (_mm256_xor_si256(_mm256_xor_si256(ROTRI_V(x, 6), ROTRI_V(x, 11)), ROTRI_V(x, 25)))
#define ROUND_V(a, b, c, d, e, f, g, h, iint, w) \
  t1 = _mm256_add_epi32(_mm256_add_epi32(_mm256_add_epi32(_mm256_add_epi32(h, EP1_V(e)), CH_V(e, f, g)), _mm256_set1_epi32(k[iint])), w); \
  d = _mm256_add_epi32(d, t1); \
  h = _mm256_add_epi32(t1, _mm256_add_epi32(EP0_V(a), MAJ_V(a, b, c)))

#define PACK(vi, wordi) ((ctx->block[vi][wordi * 4]) << 24 | (ctx->block[vi][wordi * 4 + 1]) << 16 | (ctx->block[vi][wordi * 4 + 2]) << 8 | (ctx->block[vi][wordi * 4 + 3]))
#define SUM_V(a, b, c, d) (_mm256_add_epi32(_mm256_add_epi32(a, b), _mm256_add_epi32(c, d)))

void simd_sha256_transform(CTX_V *ctx) {
  __m256i w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10, w11, w12, w13, w14, w15;
  __m256i t1;
  __m256i a, b, c, d, e, f, g, h;

//   WORD *input[8];
// #pragma GCC ivdep
//   for (int i = 0; i < 8; ++i) {
//     input[i] = (WORD *)ctx->block[i];
//   }
  w0 = _mm256_set_epi32(PACK(7, 0), PACK(6, 0), PACK(5, 0), PACK(4, 0),
                        PACK(3, 0), PACK(2, 0), PACK(1, 0), PACK(0, 0));
  w1 = _mm256_set_epi32(PACK(7, 1), PACK(6, 1), PACK(5, 1), PACK(4, 1),
                        PACK(3, 1), PACK(2, 1), PACK(1, 1), PACK(0, 1));
  w2 = _mm256_set_epi32(PACK(7, 2), PACK(6, 2), PACK(5, 2), PACK(4, 2),
                        PACK(3, 2), PACK(2, 2), PACK(1, 2), PACK(0, 2));
  w3 = _mm256_set_epi32(PACK(7, 3), PACK(6, 3), PACK(5, 3), PACK(4, 3),
                        PACK(3, 3), PACK(2, 3), PACK(1, 3), PACK(0, 3));
  w4 = _mm256_set_epi32(PACK(7, 4), PACK(6, 4), PACK(5, 4), PACK(4, 4),
                        PACK(3, 4), PACK(2, 4), PACK(1, 4), PACK(0, 4));
  w5 = _mm256_set_epi32(PACK(7, 5), PACK(6, 5), PACK(5, 5), PACK(4, 5),
                        PACK(3, 5), PACK(2, 5), PACK(1, 5), PACK(0, 5));
  w6 = _mm256_set_epi32(PACK(7, 6), PACK(6, 6), PACK(5, 6), PACK(4, 6),
                        PACK(3, 6), PACK(2, 6), PACK(1, 6), PACK(0, 6));
  w7 = _mm256_set_epi32(PACK(7, 7), PACK(6, 7), PACK(5, 7), PACK(4, 7),
                        PACK(3, 7), PACK(2, 7), PACK(1, 7), PACK(0, 7));
  w8 = _mm256_set_epi32(PACK(7, 8), PACK(6, 8), PACK(5, 8), PACK(4, 8),
                        PACK(3, 8), PACK(2, 8), PACK(1, 8), PACK(0, 8));
  w9 = _mm256_set_epi32(PACK(7, 9), PACK(6, 9), PACK(5, 9), PACK(4, 9),
                        PACK(3, 9), PACK(2, 9), PACK(1, 9), PACK(0, 9));
  w10 = _mm256_set_epi32(PACK(7, 10), PACK(6, 10), PACK(5, 10), PACK(4, 10),
                         PACK(3, 10), PACK(2, 10), PACK(1, 10), PACK(0, 10));
  w11 = _mm256_set_epi32(PACK(7, 11), PACK(6, 11), PACK(5, 11), PACK(4, 11),
                         PACK(3, 11), PACK(2, 11), PACK(1, 11), PACK(0, 11));
  w12 = _mm256_set_epi32(PACK(7, 12), PACK(6, 12), PACK(5, 12), PACK(4, 12),
                        PACK(3, 12), PACK(2, 12), PACK(1, 12), PACK(0, 12));
  w13 = _mm256_set_epi32(PACK(7, 13), PACK(6, 13), PACK(5, 13), PACK(4, 13),
                        PACK(3, 13), PACK(2, 13), PACK(1, 13), PACK(0, 13));
  w14 = _mm256_set_epi32(PACK(7, 14), PACK(6, 14), PACK(5, 14), PACK(4, 14),
                        PACK(3, 14), PACK(2, 14), PACK(1, 14), PACK(0, 14));
  w15 = _mm256_set_epi32(PACK(7, 15), PACK(6, 15), PACK(5, 15), PACK(4, 15),
                        PACK(3, 15), PACK(2, 15), PACK(1, 15), PACK(0, 15));

  a = ctx->state[0];
  b = ctx->state[1];
  c = ctx->state[2];
  d = ctx->state[3];
  e = ctx->state[4];
  f = ctx->state[5];
  g = ctx->state[6];
  h = ctx->state[7];

  ROUND_V(a, b, c, d, e, f, g, h, 0, w0);
  ROUND_V(h, a, b, c, d, e, f, g, 1, w1);
  ROUND_V(g, h, a, b, c, d, e, f, 2, w2);
  ROUND_V(f, g, h, a, b, c, d, e, 3, w3);
  ROUND_V(e, f, g, h, a, b, c, d, 4, w4);
  ROUND_V(d, e, f, g, h, a, b, c, 5, w5);
  ROUND_V(c, d, e, f, g, h, a, b, 6, w6);
  ROUND_V(b, c, d, e, f, g, h, a, 7, w7);
  ROUND_V(a, b, c, d, e, f, g, h, 8, w8);
  ROUND_V(h, a, b, c, d, e, f, g, 9, w9);
  ROUND_V(g, h, a, b, c, d, e, f, 10, w10);
  ROUND_V(f, g, h, a, b, c, d, e, 11, w11);
  ROUND_V(e, f, g, h, a, b, c, d, 12, w12);
  ROUND_V(d, e, f, g, h, a, b, c, 13, w13);
  ROUND_V(c, d, e, f, g, h, a, b, 14, w14);
  ROUND_V(b, c, d, e, f, g, h, a, 15, w15);

  w0 = SUM_V(SIG1_V(w14), w9, SIG0_V(w1), w0);
  ROUND_V(a, b, c, d, e, f, g, h, 16, w0);
  w1 = SUM_V(SIG1_V(w15), w10, SIG0_V(w2), w1);
  ROUND_V(h, a, b, c, d, e, f, g, 17, w1);
  w2 = SUM_V(SIG1_V(w0), w11, SIG0_V(w3), w2);
  ROUND_V(g, h, a, b, c, d, e, f, 18, w2);
  w3 = SUM_V(SIG1_V(w1), w12, SIG0_V(w4), w3);
  ROUND_V(f, g, h, a, b, c, d, e, 19, w3);
  w4 = SUM_V(SIG1_V(w2), w13, SIG0_V(w5), w4);
  ROUND_V(e, f, g, h, a, b, c, d, 20, w4);
  w5 = SUM_V(SIG1_V(w3), w14, SIG0_V(w6), w5);
  ROUND_V(d, e, f, g, h, a, b, c, 21, w5);
  w6 = SUM_V(SIG1_V(w4), w15, SIG0_V(w7), w6);
  ROUND_V(c, d, e, f, g, h, a, b, 22, w6);
  w7 = SUM_V(SIG1_V(w5), w0, SIG0_V(w8), w7);
  ROUND_V(b, c, d, e, f, g, h, a, 23, w7);
  w8 = SUM_V(SIG1_V(w6), w1, SIG0_V(w9), w8);
  ROUND_V(a, b, c, d, e, f, g, h, 24, w8);
  w9 = SUM_V(SIG1_V(w7), w2, SIG0_V(w10), w9);
  ROUND_V(h, a, b, c, d, e, f, g, 25,w9);
  w10 = SUM_V(SIG1_V(w8), w3, SIG0_V(w11), w10);
  ROUND_V(g, h, a, b, c, d, e, f, 26, w10);
  w11 = SUM_V(SIG1_V(w9), w4, SIG0_V(w12), w11);
  ROUND_V(f, g, h, a, b, c, d, e, 27, w11);
  w12 = SUM_V(SIG1_V(w10), w5, SIG0_V(w13), w12);
  ROUND_V(e, f, g, h, a, b, c, d, 28, w12);
  w13 = SUM_V(SIG1_V(w11), w6, SIG0_V(w14), w13);
  ROUND_V(d, e, f, g, h, a, b, c, 29, w13);
  w14 = SUM_V(SIG1_V(w12), w7, SIG0_V(w15), w14);
  ROUND_V(c, d, e, f, g, h, a, b, 30, w14);
  w15 = SUM_V(SIG1_V(w13), w8, SIG0_V(w0), w15);
  ROUND_V(b, c, d, e, f, g, h, a, 31, w15);

  w0 = SUM_V(SIG1_V(w14), w9, SIG0_V(w1), w0);
  ROUND_V(a, b, c, d, e, f, g, h, 32, w0);
  w1 = SUM_V(SIG1_V(w15), w10, SIG0_V(w2), w1);
  ROUND_V(h, a, b, c, d, e, f, g, 33, w1);
  w2 = SUM_V(SIG1_V(w0), w11, SIG0_V(w3), w2);
  ROUND_V(g, h, a, b, c, d, e, f, 34, w2);
  w3 = SUM_V(SIG1_V(w1), w12, SIG0_V(w4), w3);
  ROUND_V(f, g, h, a, b, c, d, e, 35, w3);
  w4 = SUM_V(SIG1_V(w2), w13, SIG0_V(w5), w4);
  ROUND_V(e, f, g, h, a, b, c, d, 36, w4);
  w5 = SUM_V(SIG1_V(w3), w14, SIG0_V(w6), w5);
  ROUND_V(d, e, f, g, h, a, b, c, 37, w5);
  w6 = SUM_V(SIG1_V(w4), w15, SIG0_V(w7), w6);
  ROUND_V(c, d, e, f, g, h, a, b, 38, w6);
  w7 = SUM_V(SIG1_V(w5), w0, SIG0_V(w8), w7);
  ROUND_V(b, c, d, e, f, g, h, a, 39, w7);
  w8 = SUM_V(SIG1_V(w6), w1, SIG0_V(w9), w8);
  ROUND_V(a, b, c, d, e, f, g, h, 40, w8);
  w9 = SUM_V(SIG1_V(w7), w2, SIG0_V(w10), w9);
  ROUND_V(h, a, b, c, d, e, f, g, 41,w9);
  w10 = SUM_V(SIG1_V(w8), w3, SIG0_V(w11), w10);
  ROUND_V(g, h, a, b, c, d, e, f, 42, w10);
  w11 = SUM_V(SIG1_V(w9), w4, SIG0_V(w12), w11);
  ROUND_V(f, g, h, a, b, c, d, e, 43, w11);
  w12 = SUM_V(SIG1_V(w10), w5, SIG0_V(w13), w12);
  ROUND_V(e, f, g, h, a, b, c, d, 44, w12);
  w13 = SUM_V(SIG1_V(w11), w6, SIG0_V(w14), w13);
  ROUND_V(d, e, f, g, h, a, b, c, 45, w13);
  w14 = SUM_V(SIG1_V(w12), w7, SIG0_V(w15), w14);
  ROUND_V(c, d, e, f, g, h, a, b, 46, w14);
  w15 = SUM_V(SIG1_V(w13), w8, SIG0_V(w0), w15);
  ROUND_V(b, c, d, e, f, g, h, a, 47, w15);

  w0 = SUM_V(SIG1_V(w14), w9, SIG0_V(w1), w0);
  ROUND_V(a, b, c, d, e, f, g, h, 48, w0);
  w1 = SUM_V(SIG1_V(w15), w10, SIG0_V(w2), w1);
  ROUND_V(h, a, b, c, d, e, f, g, 49, w1);
  w2 = SUM_V(SIG1_V(w0), w11, SIG0_V(w3), w2);
  ROUND_V(g, h, a, b, c, d, e, f, 50, w2);
  w3 = SUM_V(SIG1_V(w1), w12, SIG0_V(w4), w3);
  ROUND_V(f, g, h, a, b, c, d, e, 51, w3);
  w4 = SUM_V(SIG1_V(w2), w13, SIG0_V(w5), w4);
  ROUND_V(e, f, g, h, a, b, c, d, 52, w4);
  w5 = SUM_V(SIG1_V(w3), w14, SIG0_V(w6), w5);
  ROUND_V(d, e, f, g, h, a, b, c, 53, w5);
  w6 = SUM_V(SIG1_V(w4), w15, SIG0_V(w7), w6);
  ROUND_V(c, d, e, f, g, h, a, b, 54, w6);
  w7 = SUM_V(SIG1_V(w5), w0, SIG0_V(w8), w7);
  ROUND_V(b, c, d, e, f, g, h, a, 55, w7);
  w8 = SUM_V(SIG1_V(w6), w1, SIG0_V(w9), w8);
  ROUND_V(a, b, c, d, e, f, g, h, 56, w8);
  w9 = SUM_V(SIG1_V(w7), w2, SIG0_V(w10), w9);
  ROUND_V(h, a, b, c, d, e, f, g, 57,w9);
  w10 = SUM_V(SIG1_V(w8), w3, SIG0_V(w11), w10);
  ROUND_V(g, h, a, b, c, d, e, f, 58, w10);
  w11 = SUM_V(SIG1_V(w9), w4, SIG0_V(w12), w11);
  ROUND_V(f, g, h, a, b, c, d, e, 59, w11);
  w12 = SUM_V(SIG1_V(w10), w5, SIG0_V(w13), w12);
  ROUND_V(e, f, g, h, a, b, c, d, 60, w12);
  w13 = SUM_V(SIG1_V(w11), w6, SIG0_V(w14), w13);
  ROUND_V(d, e, f, g, h, a, b, c, 61, w13);
  w14 = SUM_V(SIG1_V(w12), w7, SIG0_V(w15), w14);
  ROUND_V(c, d, e, f, g, h, a, b, 62, w14);
  w15 = SUM_V(SIG1_V(w13), w8, SIG0_V(w0), w15);
  ROUND_V(b, c, d, e, f, g, h, a, 63, w15);

  ctx->state[0] = _mm256_add_epi32(ctx->state[0], a);
  ctx->state[1] = _mm256_add_epi32(ctx->state[1], b);
  ctx->state[2] = _mm256_add_epi32(ctx->state[2], c);
  ctx->state[3] = _mm256_add_epi32(ctx->state[3], d);
  ctx->state[4] = _mm256_add_epi32(ctx->state[4], e);
  ctx->state[5] = _mm256_add_epi32(ctx->state[5], f);
  ctx->state[6] = _mm256_add_epi32(ctx->state[6], g);
  ctx->state[7] = _mm256_add_epi32(ctx->state[7], h);
}

void simd_sha256_init(CTX_V *ctx) {
  ctx->datalen = 0;
  ctx->bitlen = 0;
  ctx->state[0] = _mm256_set1_epi32(0x6a09e667);
  ctx->state[1] = _mm256_set1_epi32(0xbb67ae85);
  ctx->state[2] = _mm256_set1_epi32(0x3c6ef372);
  ctx->state[3] = _mm256_set1_epi32(0xa54ff53a);
  ctx->state[4] = _mm256_set1_epi32(0x510e527f);
  ctx->state[5] = _mm256_set1_epi32(0x9b05688c);
  ctx->state[6] = _mm256_set1_epi32(0x1f83d9ab);
  ctx->state[7] = _mm256_set1_epi32(0x5be0cd19);
}

void simd_sha256_update(CTX_V *ctx, blkh_t headers[8]) {
#pragma GCC ivdep
  for (size_t i = 0; i < sizeof(blkh_t); ++i) {
#pragma GCC ivdep
    for (int j = 0; j < 8; ++j) {
      ctx->block[j][ctx->datalen] = ((BYTE *)(headers + j))[i];
    }
    ctx->datalen++;
    if (ctx->datalen == 64) {
      simd_sha256_transform(ctx);
      ctx->bitlen += 512;
      ctx->datalen = 0;
    }
  }
}

void simd_sha256_final(CTX_V *ctx, unsigned char output[8][HASH_BLOCK_SIZE]) {
  WORD i = ctx->datalen;
  if (ctx->datalen < 56) {
#pragma GCC ivdep
    for (int j = 0; j < 8; ++j) {
      ctx->block[j][i] = 0x80;
    }
    ++i;
#pragma GCC ivdep
    for (; i < 56; ++i) {
#pragma GCC ivdep
      for (int j = 0; j < 8; ++j) {
        ctx->block[j][i] = 0x00;
      }
    }
  } else {
#pragma GCC ivdep
    for (int j = 0; j < 8; ++j) {
      ctx->block[j][i] = 0x80;
    }
    ++i;
#pragma GCC ivdep
    for (; i < 64; ++i) {
#pragma GCC ivdep
      for (int j = 0; j < 8; ++j) {
        ctx->block[j][i] = 0x00;
      }
    }
    simd_sha256_transform(ctx);
#pragma GCC ivdep
    for (int j = 0; j < 8; ++j) {
      memset(ctx->block[j], 0, 56);
    }
  }
  ctx->bitlen += ctx->datalen * 8;
#pragma GCC ivdep
  for (int j = 0; j < 8; ++j) {
    ctx->block[j][63] = ctx->bitlen;
    ctx->block[j][62] = ctx->bitlen >> 8;
    ctx->block[j][61] = ctx->bitlen >> 16;
    ctx->block[j][60] = ctx->bitlen >> 24;
    ctx->block[j][59] = ctx->bitlen >> 32;
    ctx->block[j][58] = ctx->bitlen >> 40;
    ctx->block[j][57] = ctx->bitlen >> 48;
    ctx->block[j][56] = ctx->bitlen >> 56;
  }
  simd_sha256_transform(ctx);

  __m256i mask = _mm256_set1_epi32(0x000000ff);
  WORD res[8];
#pragma GCC ivdep
  for (int i = 0; i < 4; ++i) {
#pragma GCC ivdep
    for (int j = 0; j < 8; ++j) {
      _mm256_storeu_si256(
          (__m256i *)res,
          _mm256_and_si256(_mm256_srli_epi32(ctx->state[j], 24 - i * 8), mask));
#pragma GCC ivdep
      for (int k = 0; k < 8; ++k) {
        output[k][i + j * 4] = res[k];
      }
    }
  }
}

void simd_sha256(blkh_t headers[8], unsigned char output[4][HASH_BLOCK_SIZE]) {
  CTX_V ctx;
  simd_sha256_init(&ctx);
  simd_sha256_update(&ctx, headers);
  simd_sha256_final(&ctx, output);
}
//#include <stdio.h>
/* The sequiental implementation of mining implemented for you. */
void blockchain_node_mine(blk_t *node, unsigned char hash_buf[HASH_BLOCK_SIZE],
                          size_t diff, hash_func func) {
  unsigned char one_diff[HASH_BLOCK_SIZE];
  size_t diff_q, diff_m;
  diff_q = diff / 8;
  diff_m = diff % 8;
  memset(one_diff, 0xFF, sizeof(unsigned char) * HASH_BLOCK_SIZE);
  memset(one_diff, 0, sizeof(unsigned char) * diff_q);
  one_diff[diff_q] = ((uint8_t)0xFF) >> diff_m;

  func((unsigned char*)node, 0, hash_buf);

  size_t cmp1 = sizeof(unsigned char) * diff_q;
  size_t cmp2 = sizeof(unsigned char) * (HASH_BLOCK_SIZE - diff_q);
  int found = 0;
  //omp_set_num_threads(41);
#pragma omp parallel shared(found)
  {
    int intvl = omp_get_num_threads() * 8;
    int tid = omp_get_thread_num();
    blkh_t headers[8];
    for (int i = 0; i < 8; i++) {
      memcpy(headers + i, node, sizeof(blkh_t));
      headers[i].nonce = tid * 8 + i;
    }
    unsigned char buffer[8][HASH_BLOCK_SIZE];
#pragma GCC ivdep
    while (!found) {
      simd_sha256(headers, buffer);
      for (int i = 0; i < 8; i++) {
        if ((!memcmp(buffer[i], one_diff, cmp1)) &&
            memcmp(buffer[i] + diff_q, one_diff + diff_q, cmp2) <= 0) {
#pragma omp atomic
          found++;
          node->header.nonce = headers[i].nonce;
          memcpy(node->hash, buffer[i], sizeof(unsigned char) * HASH_BLOCK_SIZE);
          memcpy(&hash_buf[0], buffer[i], sizeof(unsigned char) * HASH_BLOCK_SIZE);
          break;
        }
        headers[i].nonce += intvl;
      }
    }
  }
}