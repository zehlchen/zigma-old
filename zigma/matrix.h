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
#ifndef _ZIGMA_MATRIX_H_
#define _ZIGMA_MATRIX_H_

#include "zigma.h"

/* The matrix structure, used to store data in a square.
 * The data is stored in a one-dimensional array, but is accessed as a
 * two-dimensional array. The data is stored in a square, so the length of
 * the data block is the square of the magnitude.
 */
typedef struct matrix_t {
  /* Actual length of data block. */
  uint32 length;

  /* Maximum length of data block. */
  uint32 capacity;

  /* The square root of the length. */
  uint32 magnitude;

  /* The data block. */
  uint8* data;
} matrix_t;

matrix_t* matrix_init(matrix_t* matrix, uint32 size_request);
matrix_t* matrix_destroy(matrix_t* matrix);
matrix_t* matrix_resize(matrix_t* matrix, uint32 size_request);

uint32 matrix_smallest_magnitude(uint32 request_size);

void matrix_print(matrix_t* matrix);

#endif // _ZIGMA_MATRIX_H_
