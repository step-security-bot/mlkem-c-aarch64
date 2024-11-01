// SPDX-License-Identifier: Apache-2.0
#ifndef POLY_H
#define POLY_H

#include <stddef.h>
#include <stdint.h>
#include "cbmc.h"
#include "params.h"
#include "verify.h"

/*
 * Elements of R_q = Z_q[X]/(X^n + 1). Represents polynomial
 * coeffs[0] + X*coeffs[1] + X^2*coeffs[2] + ... + X^{n-1}*coeffs[n-1]
 */
typedef struct {
  int16_t coeffs[MLKEM_N];
} ALIGN poly;

/*
 * INTERNAL presentation of precomputed data speeding up
 * the base multiplication of two polynomials in NTT domain.
 */
// REF-CHANGE: This structure does not exist in the reference
// implementation.
typedef struct {
  int16_t coeffs[MLKEM_N >> 1];
} poly_mulcache;

#define scalar_compress_q_16 MLKEM_NAMESPACE(scalar_compress_q_16)
#define scalar_decompress_q_16 MLKEM_NAMESPACE(scalar_decompress_q_16)
#define scalar_compress_q_32 MLKEM_NAMESPACE(scalar_compress_q_32)
#define scalar_decompress_q_32 MLKEM_NAMESPACE(scalar_decompress_q_32)
#define scalar_signed_to_unsigned_q_16 \
  MLKEM_NAMESPACE(scalar_signed_to_unsigned_q_16)

static inline uint32_t scalar_compress_q_16(int32_t u)
    // clang-format off
REQUIRES(0 <= u && u <= (MLKEM_Q - 1))
ENSURES(RETURN_VALUE < 16)
ENSURES(RETURN_VALUE == (((uint32_t)u * 16 + MLKEM_Q / 2) / MLKEM_Q) % 16);
// clang-format on

static inline uint32_t scalar_decompress_q_16(uint32_t u)
    // clang-format off
REQUIRES(0 <= u && u < 16)
ENSURES(RETURN_VALUE <= (MLKEM_Q - 1));
// clang-format on

static inline uint32_t scalar_compress_q_32(int32_t u)
    // clang-format off
REQUIRES(0 <= u && u <= (MLKEM_Q - 1))
ENSURES(RETURN_VALUE < 32)
ENSURES(RETURN_VALUE == (((uint32_t)u * 32 + MLKEM_Q / 2) / MLKEM_Q) % 32);
// clang-format on

static inline uint32_t scalar_decompress_q_32(uint32_t u)
    // clang-format off
REQUIRES(0 <= u && u < 32)
ENSURES(RETURN_VALUE <= (MLKEM_Q - 1));
// clang-format on

static inline uint16_t scalar_signed_to_unsigned_q_16(int16_t c)
    // clang-format off
REQUIRES(c >= -(MLKEM_Q - 1) && c <= (MLKEM_Q - 1))
ENSURES(RETURN_VALUE >= 0 && RETURN_VALUE <= (MLKEM_Q - 1))
ENSURES(RETURN_VALUE == (int32_t)c + (((int32_t)c < 0) * MLKEM_Q));
// clang-format on

#define poly_compress MLKEM_NAMESPACE(poly_compress)
void poly_compress(uint8_t r[MLKEM_POLYCOMPRESSEDBYTES], const poly *a)
    // clang-format off
REQUIRES(r != NULL && IS_FRESH(r, MLKEM_POLYCOMPRESSEDBYTES))
REQUIRES(a != NULL && IS_FRESH(a, sizeof(poly)))
REQUIRES(ARRAY_IN_BOUNDS(int, k, 0, (MLKEM_N - 1), a->coeffs, 0, (MLKEM_Q - 1)))
ASSIGNS(OBJECT_WHOLE(r));
// clang-format on

/************************************************************
 * Name: scalar_compress_q_16
 *
 * Description: Computes round(u * 16 / q)
 *
 * Arguments: - u: Unsigned canonical modulus modulo q
 *                 to be compressed.
 ************************************************************/
static inline uint32_t scalar_compress_q_16(int32_t u) {
  uint32_t d0 = (uint32_t)u;
  d0 <<= 4;
  d0 += 1665;

/* This multiply will exceed UINT32_MAX and wrap around */
/* for large values of u. This is expected and required */
#ifdef CBMC
#pragma CPROVER check push
#pragma CPROVER check disable "unsigned-overflow"
#endif
  d0 *= 80635;
#ifdef CBMC
#pragma CPROVER check pop
#endif
  d0 >>= 28;
  d0 &= 0xF;
  return d0;
}

/************************************************************
 * Name: scalar_decompress_q_16
 *
 * Description: Computes round(u * q / 16)
 *
 * Arguments: - u: Unsigned canonical modulus modulo 16
 *                 to be decompressed.
 ************************************************************/
static inline uint32_t scalar_decompress_q_16(uint32_t u) {
  return ((u * MLKEM_Q) + 8) / 16;
}

/************************************************************
 * Name: scalar_compress_q_32
 *
 * Description: Computes round(u * 32 / q)
 *
 * Arguments: - u: Unsigned canonical modulus modulo q
 *                 to be compressed.
 ************************************************************/
static inline uint32_t scalar_compress_q_32(int32_t u) {
  uint32_t d0 = (uint32_t)u;
  d0 <<= 5;
  d0 += 1664;

/* This multiply will exceed UINT32_MAX and wrap around */
/* for large values of u. This is expected and required */
#ifdef CBMC
#pragma CPROVER check push
#pragma CPROVER check disable "unsigned-overflow"
#endif
  d0 *= 40318;
#ifdef CBMC
#pragma CPROVER check pop
#endif
  d0 >>= 27;
  d0 &= 0x1f;
  return d0;
}

/************************************************************
 * Name: scalar_decompress_q_32
 *
 * Description: Computes round(u * q / 32)
 *
 * Arguments: - u: Unsigned canonical modulus modulo 32
 *                 to be decompressed.
 ************************************************************/
static inline uint32_t scalar_decompress_q_32(uint32_t u) {
  return ((u * MLKEM_Q) + 16) / 32;
}

/************************************************************
 * Name: scalar_signed_to_unsigned_q_16
 *
 * Description: converts signed polynomial coefficient
 *              from signed (-3328 .. 3328) form to
 *              unsigned form (0 .. 3328).
 *
 * Note: Cryptographic constant time implementation
 *
 * Examples:       0 -> 0
 *                 1 -> 1
 *              3328 -> 3328
 *                -1 -> 3328
 *                -2 -> 3327
 *             -3328 -> 1
 *
 * Arguments: c: signed coefficient to be converted
 ************************************************************/
static inline uint16_t scalar_signed_to_unsigned_q_16(int16_t c) {
  // Add Q if c is negative, but in constant time
  cmov_int16(&c, c + MLKEM_Q, c < 0);

  ASSERT(c >= 0, "scalar_signed_to_unsigned_q_16 result lower bound");
  ASSERT(c < MLKEM_Q, "scalar_signed_to_unsigned_q_16 result upper bound");

  // and therefore cast to uint16_t is safe.
  return (uint16_t)c;
}

#define poly_decompress MLKEM_NAMESPACE(poly_decompress)
void poly_decompress(poly *r, const uint8_t a[MLKEM_POLYCOMPRESSEDBYTES])
    // clang-format off
REQUIRES(a != NULL && IS_FRESH(a, MLKEM_POLYCOMPRESSEDBYTES))
REQUIRES(r != NULL && IS_FRESH(r, sizeof(poly)))
ASSIGNS(OBJECT_WHOLE(r))
ENSURES(ARRAY_IN_BOUNDS(int, k, 0, (MLKEM_N - 1), r->coeffs, 0, (MLKEM_Q - 1)));
// clang-format on

#define poly_tobytes MLKEM_NAMESPACE(poly_tobytes)
void poly_tobytes(uint8_t r[MLKEM_POLYBYTES], const poly *a)
    // clang-format off
REQUIRES(a != NULL && IS_FRESH(a, sizeof(poly)))
REQUIRES(ARRAY_IN_BOUNDS(int, k, 0, (MLKEM_N - 1), a->coeffs, 0, (MLKEM_Q - 1)))
ASSIGNS(OBJECT_WHOLE(r));
// clang-format on


#define poly_frombytes MLKEM_NAMESPACE(poly_frombytes)
void poly_frombytes(poly *r, const uint8_t a[MLKEM_POLYBYTES]);

#define poly_frommsg MLKEM_NAMESPACE(poly_frommsg)
void poly_frommsg(poly *r, const uint8_t msg[MLKEM_INDCPA_MSGBYTES]);
#define poly_tomsg MLKEM_NAMESPACE(poly_tomsg)
void poly_tomsg(uint8_t msg[MLKEM_INDCPA_MSGBYTES], const poly *r);

#define poly_getnoise_eta1_4x MLKEM_NAMESPACE(poly_getnoise_eta1_4x)
void poly_getnoise_eta1_4x(poly *r0, poly *r1, poly *r2, poly *r3,
                           const uint8_t seed[MLKEM_SYMBYTES], uint8_t nonce0,
                           uint8_t nonce1, uint8_t nonce2, uint8_t nonce3);

#define poly_getnoise_eta2 MLKEM_NAMESPACE(poly_getnoise_eta2)
void poly_getnoise_eta2(poly *r, const uint8_t seed[MLKEM_SYMBYTES],
                        uint8_t nonce);

#define poly_getnoise_eta2_4x MLKEM_NAMESPACE(poly_getnoise_eta2_4x)
void poly_getnoise_eta2_4x(poly *r0, poly *r1, poly *r2, poly *r3,
                           const uint8_t seed[MLKEM_SYMBYTES], uint8_t nonce0,
                           uint8_t nonce1, uint8_t nonce2, uint8_t nonce3);

#define poly_getnoise_eta1122_4x MLKEM_NAMESPACE(poly_getnoise_eta1122_4x)
void poly_getnoise_eta1122_4x(poly *r0, poly *r1, poly *r2, poly *r3,
                              const uint8_t seed[MLKEM_SYMBYTES],
                              uint8_t nonce0, uint8_t nonce1, uint8_t nonce2,
                              uint8_t nonce3);

#define poly_basemul_montgomery_cached \
  MLKEM_NAMESPACE(poly_basemul_montgomery_cached)
void poly_basemul_montgomery_cached(poly *r, const poly *a, const poly *b,
                                    const poly_mulcache *b_cache);
#define poly_tomont MLKEM_NAMESPACE(poly_tomont)
void poly_tomont(poly *r);

// REF-CHANGE: This function does not exist in the reference implementation
#define poly_mulcache_compute MLKEM_NAMESPACE(poly_mulcache_compute)
void poly_mulcache_compute(poly_mulcache *x, const poly *a);

#define poly_reduce MLKEM_NAMESPACE(poly_reduce)
void poly_reduce(poly *r);

#define poly_add MLKEM_NAMESPACE(poly_add)
void poly_add(poly *r, const poly *a, const poly *b);
#define poly_sub MLKEM_NAMESPACE(poly_sub)
void poly_sub(poly *r, const poly *a, const poly *b);

#endif
