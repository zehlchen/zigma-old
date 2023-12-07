# ZIGMA - Zehl's Enigma Machine
ZIGMA is the implementation and evolution of a stream cipher dedicated to the public domain
by Michael Paul Johnson on the newsgroup *sci.crypt.research* in 1995. ZIGMA is used to send
secure messages over insecure plaintext networks like SMS, HTTP, SMTP. The program works in a
way similar to the UNIX command 'dd'.

While the goal of this project is to provide for encrypted ASCII bar codes, the software is of
course equally useful with binary data. Instead of the customary Base64 encoding, the program
allows for the user to have input and output as Base256 (raw) or Base16 (hex). 

This project has two parallel avenues or pillars of research and development, and associated 
theoretical applications.
 * The ZIGMA Cipher, a fully-functional LFSR/stream cipher
 * The POEM Method, a useful tool for transmitting over insecure plaintext networks like SMS.

The combination of these two paths will (ideally) allow the user to store their data in a dense 
matrix that is both encrypted and (theoretically) highly resistant to cryptanalysis. These messages
can then be transmitted over insecure media with the security of a one-time pad. 

## The ZIGMA Cipher
This stream cipher is a state machine with five indexes and a 2048-bit permutation vector. Each
byte encrypted causes "perturbations" to the state machine. There is plaintext and ciphertext
feedback.  

## The POEM Method
The POEM method (Pattern Obfuscation and Encapsulation Matrix) is a technique for hiding data
inside a cryptographic-quality container. The method involes manipulation of a matrix in a four
dimensional field.