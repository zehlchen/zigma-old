/*
 * ZIGMA, Copyright (C) 1999, 2005, 2023 Chase Zehl O'Byrne
 *  <mail: zehl@live.com> http://zehlchen.com/
 *
 * This file is part of ZIGMA.
 *
 * ZIGMA is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ZIGMA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ZIGMA; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#pragma once

#ifndef _ZIGMA_ZIGMA_H_
#define _ZIGMA_ZIGMA_H_

typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef unsigned long long uint64;

typedef char      sint8;
typedef short     sint16;
typedef int       sint32;
typedef long long sint64;

#define ZIGMA_VERSION "1.2.0"

#ifndef NDEBUG
#define DEBUG_ASSERT(x)                            \
  if (!(x)) {                                      \
    fprintf(stderr, "assertion failed: %s\n", #x); \
    exit(EXIT_FAILURE);                            \
  }
#else
#define DEBUG_ASSERT(x)
#endif

/* The cryptographic state, essentially an LFSR */
typedef struct {
  uint8 radix;   /* The first index (smooth, gradual) */
  uint8 pride;   /* The second index (erratic shift) */
  uint8 chasm;   /* The third index (highly dependent) */
  uint8 left;    /* The last plaintext state */
  uint8 right;   /* The last ciphertext state */
  uint8 pv[256]; /* 256-byte permutation vector */
} zigma_t;

zigma_t* zigma_init(zigma_t* handle, uint8 const* key, uint32 length);
zigma_t* zigma_init_hash(zigma_t* handle);

void zigma_hash_sign(zigma_t* handle, char* data, uint32 length);

unsigned char zigma_encrypt_byte(zigma_t* handle, uint32 z);
unsigned char zigma_decrypt_byte(zigma_t* handle, uint32 z);

void zigma_encrypt(zigma_t* handle, uint8* data, uint32 size);
void zigma_decrypt(zigma_t* handle, uint8* data, uint32 size);

uint8 zigma_keyrand(zigma_t* zigma, uint32 limit, uint8 const* key, uint32 length, uint8* rsum, uint32* keypos);
void  zigma_print(zigma_t* zigma);

#endif // _ZIGMA_ZIGMA_H_
