/** 
 **************************************************************
 * @file mylib/s4743527_hamming.h
 * @author Hamza K
 * @date 03042024
 * @brief Hamming Encoding and Decoding Library
 * REFERENCE: csse3010_mylib_lib_hamming.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743527_lib_hamming_byte_encode() - Encodes byte.
 * s4743527_lib_hamming_byte_decode() - Decodes byte.
 * s4743527_lib_hamming_parity_error() - Checks for parity error.
 *************************************************************** 
 */

#ifndef S4743527_HAMMING_H
#define S4743527_HAMMING_H

// Function prototypes
// Encodes a byte into a 16 bit hamming encoded value.
extern unsigned short s4743527_lib_hamming_byte_encode(unsigned char value);

// Decodes a hamming encoded byte into a 4 bit value.
extern unsigned char s4743527_lib_hamming_byte_decode(unsigned char value);

// Checks if there is a parity error in the hamming encoded byte.
extern int s4743527_lib_hamming_parity_error(unsigned char value);

#endif