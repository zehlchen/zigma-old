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
#ifndef _ZIGMA_KVLIST_H_
#define _ZIGMA_KVLIST_H_

typedef struct kvlist_t {
  char* key;
  char* value;

  struct kvlist_t* next;
  struct kvlist_t* prev;
} kvlist_t;

kvlist_t* kvlist_create_node(char const* key, char const* value);

kvlist_t* kvlist_assign(kvlist_t** head, char const* key, char const* value);
kvlist_t* kvlist_search(kvlist_t** head, char const* key);

void kvlist_print(kvlist_t** head);

#endif /* _ZIGMA_KVLIST_H_ */
