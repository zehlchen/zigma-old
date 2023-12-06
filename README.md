# ZIGMA - Zehl's Enigma Machine
ZIGMA is the implementation and evolution of a stream cipher dedicated to the public domain
by Michael Paul Johnson on the newsgroup *sci.crypt.research* in 1995. 

The project has two parallel avenues of research and development.
 * The ZIGMA Cipher
 * The POEM Method

The combination of these two paths will (ideally) allow the user to store their data in a dense 
matrix that is both encrypted and (theoretically) highly resistant to cryptanalysis.

## The ZIGMA Cipher
The ZIGMA cipher is based on the concept of a a linear feedback shift register and boasts a
256-byte permutation vector with five indexes. The source code aims to document the basic
functionality of this cipher in the C programming language.  
 
## The POEM Method
The POEM method (Pattern Obfuscation and Encapsulation Matrix) is a technique for hiding data
inside a cryptographic-quality container. The method involes manipulation of a matrix in a four
dimensional field.