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
#include <termios.h>
#include <unistd.h>


#include "zigma.h"
#include "kvlist.h"


enum mode_t {
  MODE_NONE = 0,
  MODE_ENCRYPT,
  MODE_DECRYPT,
  MODE_HASH,
  MODE_RANDOM,
};

/* Prints the command line usage to stderr */
void print_usage(char const* myself)
{
  fprintf(stderr, "usage: %s MODE [OPERAND...]\n", myself);
  fprintf(stderr,
          "  where MODE must be one of:\n"
          "    encode      create a cryptogram\n"
          "    decode      restore a cryptogram\n"
          "    hash        compute standardized checksum\n"
          "  and OPERAND may be any of:\n"
          "    if=FILE     input file (instead of STDIN)\n"
          "    of=FILE     output file (instead of STDOUT)\n"
          "    key=FILE    use a key file instead of PASSPHRASE\n"
          "    fmt=BASE    format: 16 (hex dump) or 256 (raw binary)\n"
          ""
          "\n"
          "N and BYTES may use one of the following multiplicative suffixes:\n"
          " C=1, K=1024, M=1024*1024, G=1024*1024*1024\n"
          "\n"
          "ZIGMA (C) 1999, 2005, 2023 Chase Zehl O'Byrne <zehl@live.com>\n");
}




int main(int argc, char* argv[])
{
  if (argc < 2) {
    print_usage(argv[0]);
    return 0;
  }

  return 0;
}
