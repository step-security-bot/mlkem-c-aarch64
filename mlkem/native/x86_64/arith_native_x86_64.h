// SPDX-License-Identifier: Apache-2.0
#ifndef MLKEM_X86_64_NATIVE_H
#define MLKEM_X86_64_NATIVE_H

#include <stdint.h>
#include "config.h"
#include "fips202.h"
#include "params.h"
#include "polyvec.h"

#if defined(MLKEM_USE_NATIVE_X86_64) && defined(SYS_X86_64_AVX2)

#include <immintrin.h>
#include <stdint.h>

#define REJ_UNIFORM_AVX_NBLOCKS \
  ((12 * MLKEM_N / 8 * (1 << 12) / MLKEM_Q + SHAKE128_RATE) / SHAKE128_RATE)
#define REJ_UNIFORM_AVX_BUFLEN (REJ_UNIFORM_AVX_NBLOCKS * SHAKE128_RATE)

// TODO: Document buffer constraints
#define rej_uniform_avx2 MLKEM_NAMESPACE(rej_uniform_avx2)
unsigned int rej_uniform_avx2(int16_t *r, const uint8_t *buf);

#define ntt_avx2 MLKEM_NAMESPACE(ntt_avx2)
void ntt_avx2(__m256i *r, const __m256i *qdata);

#define invntt_avx2 MLKEM_NAMESPACE(invntt_avx2)
void invntt_avx2(__m256i *r, const __m256i *qdata);

#define nttpack_avx2 MLKEM_NAMESPACE(nttpack_avx2)
void nttpack_avx2(__m256i *r, const __m256i *qdata);

#define nttunpack_avx2 MLKEM_NAMESPACE(nttunpack_avx2)
void nttunpack_avx2(__m256i *r, const __m256i *qdata);

#define reduce_avx2 MLKEM_NAMESPACE(reduce_avx2)
void reduce_avx2(__m256i *r, const __m256i *qdata);

#define basemul_avx2 MLKEM_NAMESPACE(basemul_avx2)
void basemul_avx2(__m256i *r, const __m256i *a, const __m256i *b,
                  const __m256i *qdata);

#define polyvec_basemul_acc_montgomery_cached_avx2 \
  MLKEM_NAMESPACE(polyvec_basemul_acc_montgomery_cached_avx2)
void polyvec_basemul_acc_montgomery_cached_avx2(
    poly *r, const polyvec *a, const polyvec *b,
    const polyvec_mulcache *b_cache);

#define ntttobytes_avx2 MLKEM_NAMESPACE(ntttobytes_avx2)
void ntttobytes_avx2(uint8_t *r, const __m256i *a, const __m256i *qdata);

#define nttfrombytes_avx2 MLKEM_NAMESPACE(nttfrombytes_avx2)
void nttfrombytes_avx2(__m256i *r, const uint8_t *a, const __m256i *qdata);

#define tomont_avx2 MLKEM_NAMESPACE(tomont_avx2)
void tomont_avx2(__m256i *r, const __m256i *qdata);

#endif /* MLKEM_USE_NATIVE_X86_64 && SYS_X86_64_AVX2 */

#endif /* MLKEM_X86_64_NATIVE_H */
