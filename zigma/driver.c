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

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __linux__
#include <termios.h>
#endif

#include "base64.h"
#include "kvlist.h"
#include "zigma.h"

enum command_mode_t {
  MODE_NONE = 0,
  MODE_ENCRYPT,
  MODE_DECRYPT,
  MODE_HASH,
  MODE_RANDOM,
};

/* Generalized callback for encrypt/decrypt */
typedef void(zigma_cb_t)(zigma_t*, uint8*, uint32);

debug_level_t DEBUG_LEVEL = DEBUG_HIGH;

/* Prints the command line usage to stderr */
void print_usage(char const* myself)
{
  fprintf(stderr, "usage: %s MODE [OPERAND...]\n", myself);
  fprintf(stderr,
          "  where MODE must be only one of:\n"
          "    e, encode     create a cryptogram\n"
          "    d, decode     restore a cryptogram\n"
          "    h, hash       compute standardized checksum\n"
          "    r, random     generate pseudorandom data\n"
          "\n"
          "  and OPERAND may be any of:\n"
          "    if=FILE       input file (instead of STDIN)\n"
          "    of=FILE       output file (instead of STDOUT)\n"
          "    key=FILE      use a key file instead of PASSPHRASE\n"
          "    fmt=BASE      force format base: 16, 64, or 256\n"
          "\n"
          "N and BYTES may use one of the following multiplicative suffixes:\n"
          " C=1, K=1024, M=1024*1024, G=1024*1024*1024\n"
          "\n"
          "ZIGMA (C) 1999, 2005, 2023 Chase Zehl O'Byrne <zehl@live.com>\n");
}

void import_defaults(kvlist_t** head)
{
/* A simple macro to help with setting default values */
#define _KV(k, v) kvlist_assign(head, k, v)
  /* Input file (default "": read from stdin) */
  _KV("if", "");

  /* Output file (default "": write to stdout) */
  _KV("of", "");

  /* Key file (default "": use a passphrase */
  _KV("key", "");

  /* Format override (normal: autodetect) binary, base16, base64 */
  _KV("fmt", "64");
#undef _KV
}

unsigned long get_passwd(uint8* buffer, uint8* prompt)
{
#ifdef __linux__
  struct termios old_term;
  struct termios new_term;

  tcgetattr(STDIN_FILENO, &old_term);
  new_term = old_term;
#endif /* __linux__ */

  int index = 0;

#ifdef __linux__
  new_term.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
#endif /* __linux__ */

  fprintf(stderr, "%s", prompt);

  while (1) {
    char ch = getchar();
    if (ch == '\n' || ch == '\r') { // Enter key
      buffer[index] = '\0';
      break;
    }
    else if (ch == 127 || ch == '\b') { // Backspace key
      if (index > 0) {
        index--;
        fputs("\b \b", stderr);
      }
    }
    else if (index < 255) {
      buffer[index++] = ch;
      fputc('*', stderr);
    }
  }

#ifdef __linux__
  tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
#endif /* __linux__ */

  fprintf(stderr, "\r\n");

  return index;
}

void debug_printf(debug_level_t level, char const* format, ...)
{
  if (level <= DEBUG_LEVEL) {
    fprintf(stderr, "*** DEBUG: ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
  }
}

/* Convert using multiplicative suffixes */
uint32 str2bytes(char const* str)
{
  int len = strlen(str);

  char suffix = str[len - 1];

  unsigned long value = strtoul(str, NULL, 0);

  switch (suffix) {
    case 'C':
    case 'c':
      return value;
    case 'K':
    case 'k':
      return 1024 * value;
    case 'M':
    case 'm':
      return 1024 * 1024 * value;
    case 'G':
    case 'g':
      return 1024 * 1024 * 1024 * value;
    default:
      return value;
  }
}

char* safe_strdup(char const* str)
{
  DEBUG_ASSERT(str != NULL);

  size_t len  = strlen(str) + 1;
  char*  copy = malloc(len);

  DEBUG_ASSERT(copy != NULL);

  strncpy(copy, str, len); /*  NOLINT */

  copy[len] = '\0';

  return copy;
}

int stricmp(char const* p1, char const* p2)
{
  if (p1 == p2)
    return 0;

  while (*p1 && *p2) {
    int ch1 = tolower((unsigned char) *p1);
    int ch2 = tolower((unsigned char) *p2);

    if (ch1 != ch2) {
      return ch1 - ch2;
    }

    p1++;
    p2++;
  }

  return tolower((unsigned char) *p1) - tolower((unsigned char) *p2);
}

void memnull(void* ptr, uint32 size)
{
  volatile unsigned char* _ptr = ptr;

  while (size--)
    *_ptr++ = 0;
}

int parse_command(kvlist_t** head, int argc, char const* argv[])
{
  import_defaults(head);

  for (int i = 2; i < argc; i++) {
    char* dupl    = safe_strdup(argv[i]);
    char* delimit = strchr(dupl, '=');
    char* key     = dupl;
    char* value   = "";

    if (delimit != NULL) {
      *delimit = '\0';
      value    = delimit + 1;
    }

    // Add or update the key-value pair.
    kvlist_assign(head, key, value);
    free(dupl);
  }

  enum command_mode_t command = MODE_NONE;

  char const* modus = argv[1];

  switch (modus[0]) {
    case 'e':
    case 'E':
      command = MODE_ENCRYPT;
      break;
    case 'd':
    case 'D':
      command = MODE_DECRYPT;
      break;
    case 'h':
    case 'H':
      command = MODE_HASH;
      break;
    case 'r':
    case 'R':
      command = MODE_RANDOM;
      break;
    default:
      command = MODE_NONE;
      break;
  }
  return command;
}

void handle_cipher(kvlist_t** head)
{
  kvlist_t* input  = kvlist_search(head, "if");
  kvlist_t* output = kvlist_search(head, "of");
  kvlist_t* key    = kvlist_search(head, "key");
  kvlist_t* fmt    = kvlist_search(head, "fmt");

  DEBUG_ASSERT(input != NULL);
  DEBUG_ASSERT(output != NULL);
  DEBUG_ASSERT(key != NULL);
  DEBUG_ASSERT(fmt != NULL);

  FILE* input_fp  = stdin;
  FILE* output_fp = stdout;

  /* Setup the input. */
  if (*input->value != 0) {
    input_fp = fopen(input->value, "r");

    if (input_fp == NULL) {
      fprintf(stderr, "ERROR: fopen(): unable to open input file '%s': %s\n", input->value, strerror(errno));
      exit(EXIT_FAILURE);
    }

    fprintf(stderr, "successfully opened input file '%s' for reading!\n", input->value);
  }

  /* Setup the output. */
  if (*output->value != 0) {
    output_fp = fopen(output->value, "w");

    if (output_fp == NULL) {
      fprintf(stderr, "ERROR: fopen(): unable to open output file '%s': %s!\n", output->value, strerror(errno));
      exit(EXIT_FAILURE);
    }

    fprintf(stderr, "successfully opened output file '%s' for writing!\n", output->value);
  }

  uint8 passkey[256]       = {0};
  uint8 passkey_retry[256] = {0};

  uint32 keylen       = get_passwd(passkey, (uint8*) "enter passphrase: ");
  uint32 keylen_retry = get_passwd(passkey_retry, (uint8*) "enter passphrase again: ");

  if (keylen != keylen_retry || strcmp((char*) passkey, (char*) passkey_retry) != 0) {
    fprintf(stderr, "PASSWORD MISMATCH!\n");

    memnull(passkey, 256);
    memnull(passkey_retry, 256);

    fclose(input_fp);
    fclose(output_fp);
    
    exit(EXIT_FAILURE);
  }

  zigma_t* poem = zigma_init(NULL, passkey, keylen);

  /* Purge passphrase from memory */

  zigma_print(poem);

  zigma_cb_t* zigma_callback = zigma_encrypt;

  uint8  buffer[768];
  uint32 total = 0;
  uint32 count;

  int output_base = strtoul(fmt->value, 0, 10);

  while ((count = fread(buffer, 1, 768, input_fp)) > 0) {
    total += count;
    zigma_callback(poem, buffer, count);

    if (output_base == 256) { /* output as raw binary data */
      fwrite(buffer, 1, count, output_fp);
    }
    else if (output_base == 16) { /* complete a hex dump */
      for (int i = 0; i < count; i++) {
        fprintf(output_fp, "%02X", (unsigned char) buffer[i]);
      }
    }
    else if (output_base == 64) { /* output as base64 encoded data */
      char* encoded = malloc(count * 2);
      base64_encode(encoded, (char*) buffer, count);

      fprintf(output_fp, "##### BEGIN BASE64 ENCODED DATA #####\n");

      for (int i = 0; i < count; i++) {
        fprintf(output_fp, "%c", encoded[i]);

        if ((i + 1) % 80 == 0)
          fprintf(output_fp, "\n");
      }

      fprintf(output_fp, "\n##### END BASE64 ENCODED DATA #####\n");

      free(encoded);
    }

    fflush(output_fp);
  }

  fprintf(stderr, "complete! total of %u bytes read/written\n", total);
  fclose(output_fp);
}

void handle_decipher(kvlist_t** head)
{
  kvlist_t* input  = kvlist_search(head, "if");
  kvlist_t* output = kvlist_search(head, "of");
  kvlist_t* key    = kvlist_search(head, "key");
  kvlist_t* fmt    = kvlist_search(head, "fmt");

  DEBUG_ASSERT(input != NULL);
  DEBUG_ASSERT(output != NULL);
  DEBUG_ASSERT(key != NULL);
  DEBUG_ASSERT(fmt != NULL);

  FILE* input_fp  = stdin;
  FILE* output_fp = stdout;

  /* Setup the input. */
  if (*input->value != 0) {
    input_fp = fopen(input->value, "r");

    if (input_fp == NULL) {
      fprintf(stderr, "ERROR: fopen(): unable to open input file '%s': %s\n", input->value, strerror(errno));
      exit(EXIT_FAILURE);
    }

    fprintf(stderr, "successfully opened input file '%s' for reading!\n", input->value);
  }

  /* Setup the output. */
  if (*output->value != 0) {
    output_fp = fopen(output->value, "w");

    if (output_fp == NULL) {
      fprintf(stderr, "ERROR: fopen(): unable to open output file '%s': %s!\n", output->value, strerror(errno));
      exit(EXIT_FAILURE);
    }

    fprintf(stderr, "successfully opened output file '%s' for writing!\n", output->value);
  }

  uint8    passkey[256] = {0};
  uint32   keylen       = get_passwd(passkey, (uint8*) "enter passphrase: ");
  zigma_t* poem         = zigma_init(NULL, passkey, keylen);

  zigma_print(poem);
  /* Clear the passphrase from memory */
  memnull(passkey, 256);

  zigma_cb_t* poem_callback = zigma_decrypt;

  uint8  buffer[1024];
  uint32 total = 0;
  uint32 count;

  int output_base = strtoul(fmt->value, 0, 10);

  if (output_base == 256) {
    while ((count = fread(buffer, 1, 1024, input_fp)) > 0) {
      total += count;
      poem_callback(poem, buffer, count);

      fwrite(buffer, 1, count, output_fp);
    }
  }

  fprintf(stderr, "complete! total of %u bytes read/written\n", total);
}

void handle_checksum(kvlist_t** head)
{
  kvlist_t* input = kvlist_search(head, "if");

  DEBUG_ASSERT(input != NULL);

  zigma_t* poem = zigma_init(NULL, NULL, 0);

  zigma_print(poem);

  FILE* input_fp = stdin;

  /* Setup the input. */
  if (*input->value != 0) {
    input_fp = fopen(input->value, "r");

    if (input_fp == NULL) {
      fprintf(stderr, "ERROR: fopen(): unable to open input file '%s': %s\n", input->value, strerror(errno));
      exit(EXIT_FAILURE);
    }

    fprintf(stderr, "successfully opened input file '%s' for reading!\n", input->value);
  }

  zigma_cb_t* zigma_callback = zigma_encrypt;

  uint8  buffer[1024] = {0};
  uint32 total        = 0;
  uint32 count;

  while ((count = fread(buffer, 1, 1024, input_fp)) > 0) {
    total += count;
    zigma_callback(poem, buffer, count);
  }

  uint8 checksum[32] = {0};

  zigma_hash_sign(poem, checksum, 32);

  fprintf(stderr, "%s (%u bytes): ", input->value, total);
  for (int j = 0; j < 24; j++)
    fprintf(stderr, "%02X", (unsigned char) checksum[j]);

  fprintf(stderr, "\n");
}

int main(int argc, char const* argv[])
{
  if (argc < 2) {
    print_usage(argv[0]);
    return 0;
  }

  fprintf(stderr, "--- ZIGMA version %s ... \n", ZIGMA_VERSION_STRING);
  fprintf(stderr, ">>> WARNING: SENSITIVE DIAGNOSTIC DATA. USE WITH CAUTION!\n");
  kvlist_t* opt = NULL;

  enum command_mode_t command = parse_command(&opt, argc, argv);

  kvlist_print(&opt);

  switch (command) {
    case MODE_NONE:
      print_usage(argv[0]);
      return 0;
      break;

    case MODE_ENCRYPT:
      handle_cipher(&opt);
      return 0;
      break;

    case MODE_DECRYPT:
      handle_decipher(&opt);
      return 0;
      break;

    case MODE_HASH:
      handle_checksum(&opt);
      return 0;
      break;
  }

  return 0;
}