// SPDX-License-Identifier: Apache-2.0
#ifndef FIPS_202X4_H
#define FIPS_202X4_H

#include <stddef.h>
#include <stdint.h>
#include "keccakf1600.h"
#include "namespace.h"

#define shake128x4_absorb FIPS202_NAMESPACE(shake128x4_absorb)
void shake128x4_absorb(keccakx4_state *state, const uint8_t *in0,
                       const uint8_t *in1, const uint8_t *in2,
                       const uint8_t *in3, size_t inlen);

#define shake256x4_absorb FIPS202_NAMESPACE(shake256x4_absorb)
void shake256x4_absorb(keccakx4_state *state, const uint8_t *in0,
                       const uint8_t *in1, const uint8_t *in2,
                       const uint8_t *in3, size_t inlen);

#define shake128x4_squeezeblocks FIPS202_NAMESPACE(shake128x4_squeezeblocks)
void shake128x4_squeezeblocks(uint8_t *out0, uint8_t *out1, uint8_t *out2,
                              uint8_t *out3, size_t nblocks,
                              keccakx4_state *state);

#define shake256x4_squeezeblocks FIPS202_NAMESPACE(shake256x4_squeezeblocks)
void shake256x4_squeezeblocks(uint8_t *out0, uint8_t *out1, uint8_t *out2,
                              uint8_t *out3, size_t nblocks,
                              keccakx4_state *state);

#define shake256x4 FIPS202_NAMESPACE(shake256x4)
void shake256x4(uint8_t *out0, uint8_t *out1, uint8_t *out2, uint8_t *out3,
                size_t outlen, uint8_t *in0, uint8_t *in1, uint8_t *in2,
                uint8_t *in3, size_t inlen);

#endif
