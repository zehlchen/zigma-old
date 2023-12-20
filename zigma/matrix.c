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

#include "matrix.h"
#include "zigma.h"

matrix_t* matrix_init(matrix_t* matrix, uint32 size_request)
{
  if (matrix == NULL)
    matrix = (matrix_t*) malloc(sizeof(matrix_t));

  DEBUG_ASSERT(matrix != NULL);

  if (size_request == 0) {
    matrix->length    = 0;
    matrix->capacity  = 0;
    matrix->magnitude = 0;
    matrix->data      = NULL;

    return matrix;
  }

  uint32 magnitude = matrix_smallest_magnitude(size_request);
  uint32 capacity  = (magnitude * magnitude) > 1024 * 1024 ? magnitude * magnitude : 1024 * 1024; // 1MB minimum

  matrix->data = (uint8*) malloc(capacity * sizeof(uint8));

  DEBUG_ASSERT(matrix->data != NULL);

  matrix->length    = size_request;
  matrix->capacity  = capacity;
  matrix->magnitude = magnitude;

  return matrix;
}

matrix_t* matrix_destroy(matrix_t* matrix)
{
  DEBUG_ASSERT(matrix != NULL);

  memnull(matrix->data, matrix->capacity * sizeof(uint8));
  memnull(matrix, sizeof(matrix_t));

  free(matrix->data);
  free(matrix);

  return NULL;
}

matrix_t* matrix_resize(matrix_t* matrix, uint32 size_request)
{
  DEBUG_ASSERT(matrix != NULL);

  uint32 magnitude = matrix_smallest_magnitude(size_request);

  if (size_request <= matrix->capacity) {
    matrix->length    = size_request;
    matrix->magnitude = magnitude;

    return matrix;
  }

  uint32 capacity = (magnitude * magnitude) > 1024 * 1024 ? magnitude * magnitude : 1024 * 1024; // 1MB minimum

  matrix->data = (uint8*) realloc(matrix->data, capacity * sizeof(uint8));

  DEBUG_ASSERT(matrix->data != NULL);

  matrix->capacity  = capacity;
  matrix->magnitude = magnitude;

  return matrix;
}

uint32 matrix_smallest_magnitude(uint32 request_size)
{
  uint32 magnitude = 1;

  if (request_size == 0)
    return 0;

  while (magnitude * magnitude < request_size)
    magnitude++;

  return magnitude;
}

void matrix_print(matrix_t* matrix)
{
  DEBUG_ASSERT(matrix != NULL);

  fprintf(stderr, "matrix[] = {\n");
  fprintf(stderr, "  length = %d\n", matrix->length);
  fprintf(stderr, "  capacity = %d\n", matrix->capacity);
  fprintf(stderr, "  magnitude = %d (%d^2^)\n", matrix->magnitude, matrix->magnitude * matrix->magnitude);
  fprintf(stderr, "  data = %p\n", matrix->data);
  fprintf(stderr, "}\n");
}