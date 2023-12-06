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
#include "kvlist.h"

/* Create a kvlist_t node and populate it. */
kvlist_t* kvlist_create_node(char const* key, char const* value)
{
  kvlist_t* node = malloc(sizeof(kvlist_t));

  if (node == NULL)
    return NULL;

  node->key   = safe_strdup(key);
  node->value = safe_strdup(value);

  node->prev = node->next = NULL;

  return node;
}

/* Find a node by name. */
kvlist_t* kvlist_search(kvlist_t** head, char const* key)
{
  kvlist_t* current = *head;

  while (current != NULL) {
    if (strcmp(current->key, key) == 0) {
      return current;
    }
    current = current->next;
  }

  return NULL;
}

/* Update registry to reflect newest addition or modification*/
kvlist_t* kvlist_assign(kvlist_t** head, char const* key, char const* value)
{
  kvlist_t* current = *head;

  /* Check to see whether the key already exists and update as necessary. */
  while (current != NULL) {
    if (strcmp(current->key, key) == 0) {
      free(current->value);

      current->value = safe_strdup(value);

      return current;
    }
    current = current->next;
  }

  /* Create a new node and insert at the end of the list. */
  kvlist_t* node = kvlist_create_node(key, value);

  if (node == NULL)
    return NULL;

  /* Update the node/list structure. */
  if (*head == NULL) {
    *head = node;
  }
  else {
    current = *head;

    while (current->next != NULL)
      current = current->next;

    current->next = node;
    node->prev    = current;
  }
  return current;
}

void kvlist_print(kvlist_t** head)
{
  kvlist_t* current = *head;

  fprintf(stderr, "kv[] = {");

  while (current != NULL) {
    fprintf(stderr, "{ '%s' -> '%s' }", current->key, current->value);

    current = current->next;
  }

  fprintf(stderr, "}\n");
}
