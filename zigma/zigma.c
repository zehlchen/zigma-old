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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zigma.h"

zigma_t* zigma_init(zigma_t* handle, uint8 const* key, uint32 length)
{
  if (handle == NULL)
    handle = (zigma_t*) malloc(sizeof(zigma_t));

  if (key == NULL) {
    zigma_init_hash(handle);
    return handle;
  }

  uint8  toswap   = 0;
  uint8  swaptemp = 0;
  uint8  rsum     = 0;
  uint32 keypos   = 0;

  /* Populate the permutation vector. */
  for (int i = 0, j = 255; i < 256; i++, j--)
    handle->vektor[i] = j;

  /* Randomize the permutation vector with the key. */
  for (int i = 255; i >= 0; i--) {
    toswap = zigma_keyrand(handle, i, key, length, &rsum, &keypos);

    swaptemp               = handle->vektor[i];
    handle->vektor[i]      = handle->vektor[toswap];
    handle->vektor[toswap] = swaptemp;
  }

  handle->index_A = handle->vektor[1];
  handle->index_B = handle->vektor[3];
  handle->index_C = handle->vektor[5];
  handle->byte_X  = handle->vektor[7];
  handle->byte_Y  = handle->vektor[rsum];

  return handle;
}

zigma_t* zigma_init_hash(zigma_t* handle)
{
  DEBUG_ASSERT(handle != NULL);

  handle->index_A = 1;
  handle->index_B = 3;
  handle->index_C = 5;
  handle->byte_X  = 7;
  handle->byte_Y  = 11;

  for (int i = 0, j = 255; i < 256; i++, j--) {
    handle->vektor[i] = (uint8) j;
  }

  return handle;
}

void zigma_hash_sign(zigma_t* handle, uint8* data, uint32 length)
{
  /* Advance the permutation vector. */
  for (int i = 255; i >= 0; i--) {
    zigma_encrypt_byte(handle, i);
  }

  /* Encrypt 0 to desired length to populate hash value */
  for (int i = 0; i < length; i++)
    data[i] = zigma_encrypt_byte(handle, 0);
}

uint8 zigma_encrypt_byte(zigma_t* handle, uint32 byte)
{
  uint8 swaptemp;

  handle->index_B += handle->vektor[handle->index_A++];

  swaptemp                        = handle->vektor[handle->byte_Y];
  handle->vektor[handle->byte_Y]  = handle->vektor[handle->index_B];
  handle->vektor[handle->index_B] = handle->vektor[handle->byte_X];
  handle->vektor[handle->byte_X]  = handle->vektor[handle->index_A];
  handle->vektor[handle->index_A] = swaptemp;

  handle->index_C += handle->vektor[swaptemp];

  handle->byte_Y = byte ^ handle->vektor[(handle->vektor[handle->index_B] + handle->vektor[handle->index_A]) & 0xFF] ^
                   handle->vektor[handle->vektor[(handle->vektor[handle->byte_X] + handle->vektor[handle->byte_Y] +
                                                  handle->vektor[handle->index_C]) &
                                                 0xFF]];

  handle->byte_X = byte;

  return handle->byte_Y;
}

uint8 zigma_decrypt_byte(zigma_t* handle, uint32 byte)
{
  uint8 swaptemp;

  handle->index_B += handle->vektor[handle->index_A++];

  swaptemp                        = handle->vektor[handle->byte_Y];
  handle->vektor[handle->byte_Y]  = handle->vektor[handle->index_B];
  handle->vektor[handle->index_B] = handle->vektor[handle->byte_X];
  handle->vektor[handle->byte_X]  = handle->vektor[handle->index_A];
  handle->vektor[handle->index_A] = swaptemp;

  handle->index_C += handle->vektor[swaptemp];

  handle->byte_X = byte ^ handle->vektor[(handle->vektor[handle->index_B] + handle->vektor[handle->index_A]) & 0xFF] ^
                   handle->vektor[handle->vektor[(handle->vektor[handle->byte_X] + handle->vektor[handle->byte_Y] +
                                                  handle->vektor[handle->index_C]) &
                                                 0xFF]];

  handle->byte_Y = byte;

  return handle->byte_X;
}

void zigma_encrypt(zigma_t* handle, uint8* data, uint32 size)
{
  DEBUG_ASSERT(handle != NULL);
  DEBUG_ASSERT(data != NULL);

  for (int i = 0; i < size; i++)
    data[i] = zigma_encrypt_byte(handle, data[i]);
}

void zigma_decrypt(zigma_t* handle, uint8* data, uint32 size)
{
  DEBUG_ASSERT(handle != NULL);
  DEBUG_ASSERT(data != NULL);

  for (int i = 0; i < size; i++)
    data[i] = zigma_decrypt_byte(handle, data[i]);
}

uint8 zigma_keyrand(zigma_t* handle, uint32 limit, uint8 const* key, uint32 length, uint8* rsum, uint32* keypos)
{
  uint32 u;
  uint32 retry_limiter = 0;
  uint32 mask          = 1;

  while (mask < limit)
    mask = (mask << 1) + 1;

  do {
    *rsum = handle->vektor[(unsigned) *rsum] + key[(*keypos)++];

    if (*keypos >= length) {
      *keypos = 0;
      *rsum += length;
    }

    u = mask & *rsum;

    if (++retry_limiter > 11)
      u %= limit;

  } while (u > limit);

  return u;
}

void zigma_print(zigma_t* handle)
{
  DEBUG_ASSERT(handle != NULL);

  fprintf(stderr, ">>> DANGER: NEVER SHARE PERMUTATION VECTOR OR INDEXES! EVER!\n");
  fprintf(stderr, "zigma_state_machine[] = { /* DEBUG PRINT */\n");
  fprintf(stderr, "  index_A/index_B/index_C = %02X/%02X/%02X\n", handle->index_A, handle->index_B, handle->index_C);
  fprintf(stderr, "  pre/post = %02X/%02X\n", handle->byte_X, handle->byte_Y);
  fprintf(stderr, "  vektor[] = {\n    ");

  /* Print vektor[] in base 16 */
  for (int i = 0; i < 256; i++) {
    fprintf(stderr, "%02x", handle->vektor[i]);

    if ((i + 1) % 8 == 0) {
      fprintf(stderr, " ");
    }

    if ((i + 1) % 32 == 0) {
      fprintf(stderr, "\n    ");
    }
  }
  fprintf(stderr, "\r  }\n}\n");
}