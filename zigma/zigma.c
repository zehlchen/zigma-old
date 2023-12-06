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

zigma_t* zigma_init(zigma_t* zigma, uint8 const* key, uint32 length)
{
  if (zigma == NULL)
    zigma = (zigma_t*) malloc(sizeof(zigma_t));

  if (key == NULL) {
    zigma_init_hash(zigma);
    return zigma;
  }

  uint8  toswap   = 0;
  uint8  swaptemp = 0;
  uint8  rsum     = 0;
  uint32 keypos   = 0;

  for (int i = 0; i < 256; i++)
    zigma->pv[i] = i;

  for (int i = 255; i >= 0; i--) {
    toswap = zigma_keyrand(zigma, i, key, length, &rsum, &keypos);

    swaptemp          = zigma->pv[i];
    zigma->pv[i]      = zigma->pv[toswap];
    zigma->pv[toswap] = swaptemp;
  }

  zigma->radix = zigma->pv[1];
  zigma->pride = zigma->pv[3];
  zigma->chasm = zigma->pv[5];
  zigma->left  = zigma->pv[7];
  zigma->right = zigma->pv[rsum];

  return zigma;
}

zigma_t* zigma_init_hash(zigma_t* zigma)
{
  zigma->radix = 1;
  zigma->pride = 3;
  zigma->chasm = 5;
  zigma->left  = 7;
  zigma->right = 11;

  for (int i = 0, j = 255; i < 256; i++, j--) {
    zigma->pv[i] = (uint8) j;
  }

  return zigma;
}

void zigma_hash_sign(zigma_t* zigma, char* data, uint32 length)
{
  for (int i = 255; i >= 0; i--) {
    zigma_encrypt_byte(zigma, i);
  }

  for (int i = 0; i < length; i++)
    data[i] = zigma_encrypt_byte(zigma, 0);
}

uint8 zigma_encrypt_byte(zigma_t* zigma, uint32 byte)
{
  uint8 swaptemp;

  zigma->pride += zigma->pv[zigma->radix++];

  swaptemp                = zigma->pv[zigma->right];
  zigma->pv[zigma->right] = zigma->pv[zigma->pride];
  zigma->pv[zigma->pride] = zigma->pv[zigma->left];
  zigma->pv[zigma->left]  = zigma->pv[zigma->radix];
  zigma->pv[zigma->radix] = swaptemp;

  zigma->chasm += zigma->pv[swaptemp];

  zigma->right =
      byte ^ zigma->pv[(zigma->pv[zigma->pride] + zigma->pv[zigma->radix]) & 0xFF] ^
      zigma->pv[zigma->pv[(zigma->pv[zigma->left] + zigma->pv[zigma->right] + zigma->pv[zigma->chasm]) & 0xFF]];

  zigma->left = byte;

  return zigma->right;
}

uint8 zigma_decrypt_byte(zigma_t* zigma, uint32 byte)
{
  uint8 swaptemp;

  zigma->pride += zigma->pv[zigma->radix++];

  swaptemp                = zigma->pv[zigma->right];
  zigma->pv[zigma->right] = zigma->pv[zigma->pride];
  zigma->pv[zigma->pride] = zigma->pv[zigma->left];
  zigma->pv[zigma->left]  = zigma->pv[zigma->radix];
  zigma->pv[zigma->radix] = swaptemp;

  zigma->chasm += zigma->pv[swaptemp];

  zigma->left =
      byte ^ zigma->pv[(zigma->pv[zigma->pride] + zigma->pv[zigma->radix]) & 0xFF] ^
      zigma->pv[zigma->pv[(zigma->pv[zigma->left] + zigma->pv[zigma->right] + zigma->pv[zigma->chasm]) & 0xFF]];

  zigma->right = byte;

  return zigma->left;
}

void zigma_encrypt(zigma_t* zigma, uint8* data, uint32 size)
{
  for (int i = 0; i < size; i++)
    data[i] = zigma_encrypt_byte(zigma, data[i]);
}

void zigma_decrypt(zigma_t* zigma, uint8* data, uint32 size)
{
  for (int i = 0; i < size; i++)
    data[i] = zigma_decrypt_byte(zigma, data[i]);
}

uint8 zigma_keyrand(zigma_t* zigma, uint32 limit, uint8 const* key, uint32 length, uint8* rsum, uint32* keypos)
{
  uint32 u;
  uint32 retry_limiter = 0;
  uint32 mask          = 1;

  while (mask < limit)
    mask = (mask << 1) + 1;

  do {
    *rsum = zigma->pv[(unsigned) *rsum] + key[(*keypos)++];

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

void zigma_print(zigma_t* zigma)
{
  fprintf(stderr, "*** DANGER: NEVER SHARE PERMUTATION VECTOR OR INDEXES! EVER!\n");
  fprintf(stderr, "zigma_state_machasmne[] = { /* DEBUG PRINT */\n");
  fprintf(stderr, "  radix/pride/chasm = %02X/%02X/%02X\n", zigma->radix, zigma->pride, zigma->chasm);
  fprintf(stderr, "  pre/post   = %02X/%02X\n", zigma->left, zigma->right);
  fprintf(stderr, "  pvector[]  = {\n    ");

  /* Print pv[] in base 16 */
  for (int i = 0; i < 256; i++) {
    fprintf(stderr, "%02x", zigma->pv[i]);

    if ((i + 1) % 8 == 0) {
      fprintf(stderr, " ");
    }

    if ((i + 1) % 32 == 0) {
      fprintf(stderr, "\n    ");
    }
  }
  fprintf(stderr, "\r  }\n}\n");
}