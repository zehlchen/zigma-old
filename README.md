# ZIGMA - Zehl's Enigma Machine
## Introduction
In January, 1995, Michael Paul Johnson wrote a post on the newsgroup *sci.crypt.research* about
a novel stream cipher he had concieved and dedicated to the public domain that could most readily 
be visualized as an enigma machine with 256 rotors that was rewired after each byte of data. 

This program is the reimagination of this cipher and hopes to provide a reasonably secure way of
sending enciphered messages over plaintext networks like SMS and QR codes or posts to forums, social
media, etc. This program uses the cipher, called ZIGMA, and the feedback of its state machine (the POEM method) to create ciphertext and attempt to obfuscate the ciphertext output.

Without the key (either a passphrase or a 256-byte key file), the existence of encrypted data is
hard to prove, and the reshuffling of bytes in the output stream makes cryptanalysis much more
difficult and resource-intensive.

The program has been developed to function similarly to the UNIX utility 'dd'.

## Command Line Interface

~~~
$ zigma MODE [OPERAND KEY=VALUE]...
~~~

 `MODE` must be one of the following:
 * `e` or `E` (as in "encipher"): create a cryptogram 
 * `d` or `D` (as in "decipher"): restore a cryptogram
 * `h` or `H` (as in "hash"): generate a cryptographic checksum

and `OPERAND` may be any of the following
 * `if=FILE` stream the input from `FILE` instead of `<STDIN>`
 * `of=FILE` stream the output to `FILE` instead of `<STDOUT>`
 * `key=FILE` read *up to* the first 256 bytes of `FILE` instead of a passphrase
 * `fmt=BASE` one of `16` (hex dump), `64` (base-64 encoding), or `256` (no formatting, raw)

This should be familiar to anyone who has worked around a UNIX shell.

## Design Notes & Considerations
This program was written with the following assumptions (or caveats):

1. The maximum length of a cryptogram must be less than 4GB (2^32).
2. The key or passphrase is secret, unique, and secure.
3. The plaintext is encoded in UTF-8.
4. The ciphertext is transmitted over a plain, insecure network.
