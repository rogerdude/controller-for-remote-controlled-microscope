/** 
 **************************************************************
 * @file mylib/s4743527_hamming.c
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

#include "stdint.h"

/**
 * Encodes 4 bits into a hamming encoded byte.
 * 
 * value: the 4 bits to encode.
 * 
 * Returns: encoded byte including parity bit, data bits, and hamming bits.
 */
unsigned char hamming_hbyte_encode(unsigned char value) {

    uint8_t d0, d1, d2, d3; // Bits 0-3 of value.
    uint8_t h0, h1, h2; // Hamming bits
    uint8_t p0 = 0; // Even parity bit
    uint16_t out; // Output byte 

    // Extract bits
    d0 = !!(value & (0x01 << 0));
    d1 = !!(value & (0x01 << 1));
    d2 = !!(value & (0x01 << 2));
    d3 = !!(value & (0x01 << 3));

    // Calculate hamming bits
    h0 = d1 ^ d2 ^ d3;
    h1 = d0 ^ d2 ^ d3;
    h2 = d0 ^ d1 ^ d3;

    // Make output byte without parity bit.
    out = (h0 << 1) | (h1 << 2) | (h2 << 3) |
            (d0 << 4) | (d1 << 5) | (d2 << 6) | (d3 << 7);

    // Calculate even parity bit.
    for (uint8_t bit = 1; bit < 8; bit++) {
        p0 = p0 ^ !!(out & (1 << bit));
    }
    
    // Add parity bit
    out |= p0;

    return out;
}

/**
 * Encodes a byte into a 16 bit hamming encoded value.
 * 
 * value: the byte to encode.
 * 
 * Returns: the 16 bit hamming encoded value.
 */
extern unsigned short s4743527_lib_hamming_byte_encode(unsigned char value) {
    
    uint16_t encoded;

    encoded = hamming_hbyte_encode(value & 0x0F) |
            (hamming_hbyte_encode((value & 0xF0) >> 4) << 8);
    
    return encoded;
}

/**
 * Decodes a hamming encoded byte into a 4 bit value.
 * 
 * value: the hamming encoded byte to decode.
 * 
 * Returns: the 4 bit value from the encoded byte.
 */
extern unsigned char s4743527_lib_hamming_byte_decode(unsigned char value) {
    uint8_t d0, d1, d2, d3; // Bits 0-3 of value.
    uint8_t h0, h1, h2; // Hamming bits
    uint8_t s0, s1, s2; // Syndrome bits
    uint8_t decoded = value;

    // Extract value bits
    d0 = !!(value & (0x01 << 4));
    d1 = !!(value & (0x01 << 5));
    d2 = !!(value & (0x01 << 6));
    d3 = !!(value & (0x01 << 7));

    // Extract hamming bits
    h0 = !!(value & (0x01 << 1));
    h1 = !!(value & (0x01 << 2));
    h2 = !!(value & (0x01 << 3));

    // Calculate syndrome bits
    s0 = d1 ^ d2 ^ d3 ^ h0;
    s1 = d0 ^ d2 ^ d3 ^ h1;
    s2 = d0 ^ d1 ^ d3 ^ h2;

    // Check for error
    uint8_t errorBit;
    if (!(!(s0) && !(s1) && !(s2))) {
        if (s0 && !(s1) && !(s2)) {
            errorBit = 1; // Error in h0
        } else if (!(s0) && s1 && !(s2)) {
            errorBit = 2; // Error in h1
        } else if (!(s0) && !(s1) && s2) {
            errorBit = 3; // Error in h2
        } else if (!(s0) && s1 && s2) {
            errorBit = 4; // Error in d0
        } else if (s0 && !(s1) && s2) {
            errorBit = 5; // Error in d1
        } else if (s0 && s1 && !(s2)) {
            errorBit = 6; // Error in d2
        } else if (s0 && s1 && s2) {
            errorBit = 7; // Error in d3
        }

        // Fix error
        decoded ^= (0x01 << errorBit);
    }

    return (decoded & 0xF0) >> 4;
}

/**
 * Checks if there is a parity error in the hamming encoded byte.
 * 
 * value: the hamming encoded byte to check.
 * 
 * Returns: 0 if no parity error, 1 if there is parity error.
 */
extern int s4743527_lib_hamming_parity_error(unsigned char value) {

    uint8_t receivedP0, expectedP0 = 0;; // Even parity bits

    // Get received parity bit.
    receivedP0 = !!(value & 0x01);

    // Calculate expected even parity bit
    for (uint8_t bit = 1; bit < 8; bit++) {
        expectedP0 = expectedP0 ^ !!(value & (0x01 << bit));
    }

    if (expectedP0 == receivedP0) {
        return 0;
    }
    return 1;
}
