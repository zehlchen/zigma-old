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

#ifndef ZIGMA_VERSION_STRING
#define ZIGMA_VERSION_STRING "ZIGMA " ZIGMA_VERSION
#endif

/* Define the length in bytes of the checksum. */
#ifndef ZIGMA_CHECKSUM_SIZE
#define ZIGMA_CHECKSUM_SIZE 32 /* 256 bits */
#endif

/*
 * Debug code ... respect no-debug requests.
 */
#ifndef NDEBUG
#define DEBUG_ASSERT(x)                            \
  if (!(x)) {                                      \
    fprintf(stderr, "assertion failed: %s\n", #x); \
    exit(EXIT_FAILURE);                            \
  }
#else
#define DEBUG_ASSERT(x)
#endif

/* Debug Verbosity */
typedef enum { DEBUG_NONE = 0, DEBUG_LOW, DEBUG_MEDIUM, DEBUG_HIGH } debug_level_t;

extern debug_level_t DEBUG_LEVEL;

void debug_printf(debug_level_t level, char const* format, ...);

#define DEBUG_PRINT_ARRAY(var, size)                    \
  {                                                     \
    fprintf(stderr, "(" #var ") = %d <", size);         \
    for (int _i = 0; _i < size; _i++) {                 \
      fprintf(stderr, "%02X", (unsigned char) var[_i]); \
    }                                                   \
    fprintf(stderr, ">\n");                             \
  }

/*
 * Useful function assists.
 */

/* Add a multiplicative value to a number K, M, G and so on*/
uint32 str2bytes(char const* str);

/* Duplicate a string safely */
char* safe_strdup(char const* str);

/* Get rid of something for good */
void memnull(void* ptr, uint32 size);

/*
 * The Zigma Cipher
 */

/* Cryptographic state machine handle. */
typedef struct zigma_t {
  /* Index A rotates smoothly between bytes. */
  uint8 index_A;

  /* Index B varies in an erratic manner. */
  uint8 index_B;

  /* Index C depends heavily on the previous input. */
  uint8 index_C;

  /* The last plaintext byte. */
  uint8 byte_X;

  /* The last ciphertext byte. */
  uint8 byte_Y;

  /* The permutation vector. */
  uint8 vektor[256]; /* 256-byte permutation vector */
} zigma_t;

/* Initializes and allocates a zigma object.
 * If the zigma object is NULL, it will be allocated. If the key is NULL, the
 * zigma object will be initialized with a hash function. If the key is not
 * NULL, the zigma object will be initialized with the key.
 *   @param handle The zigma object to initialize.
 *   @param key The key to initialize the zigma object with or NULL for hash.
 *   @param length The length of the key in bytes.
 *   @return The initialized zigma object.
 */
zigma_t* zigma_init(zigma_t* handle, uint8 const* key, uint32 length);

/* Initializes a non-NULL zigma object for use as a hash.
 *   @param handle The zigma object to initialize.
 *   @return The initialized zigma object.
 */
zigma_t* zigma_init_hash(zigma_t* handle);

/* Terminate the state for the purpose of generating a checksum.
 *   @param handle The zigma object to terminate.
 *   @param data The checksum value to be populated.
 *   @param length The length of the hash checksum in bytes.
 */
void zigma_hash_sign(zigma_t* handle, uint8* data, uint32 length);

/* Encrypt a single byte.
 *   @param handle The zigma object to encrypt with.
 *   @param byte The byte to encrypt.
 *   @return The encrypted byte.
 *   @note The zigma object must have been initialized with a key.
 */
unsigned char zigma_encrypt_byte(zigma_t* handle, uint32 z);

/* Decrypt a single byte.
 *   @param handle The zigma object to decrypt with.
 *   @param byte The byte to decrypt.
 *   @return The decrypted byte.
 *   @note The zigma object must have been initialized with a key.
 */
unsigned char zigma_decrypt_byte(zigma_t* handle, uint32 z);

/* Encrypt a string of data.
 *   @param handle The zigma object to encrypt with.
 *   @param data The data to encrypt.
 *   @param size The size of the data in bytes.
 *   @note The zigma object must have been initialized with a key.
 */
void zigma_encrypt(zigma_t* handle, uint8* data, uint32 size);

/* Decrypt a string of data.
 *   @param handle The zigma object to decrypt with.
 *   @param data The data to decrypt.
 *   @param size The size of the data in bytes.
 *   @note The zigma object must have been initialized with a key.
 */
void zigma_decrypt(zigma_t* handle, uint8* data, uint32 size);

/* Generate a random number from a key.
 *   @param handle The zigma object to generate with.
 *   @param limit The maximum value to generate.
 *   @param key The key to generate with.
 *   @param length The length of the key in bytes.
 *   @param rsum The random sum to generate with.
 *   @param keypos The key position to generate with.
 *   @return The generated random number.
 */
uint8 zigma_keyrand(zigma_t* handle, uint32 limit, uint8 const* key, uint32 length, uint8* rsum, uint32* keypos);

/* Print the state of a zigma object.
 *   @param handle The zigma object to print.
 *   @note This function is for debugging purposes only.
 */
void zigma_print(zigma_t* handle);

#endif // _ZIGMA_ZIGMA_H_
