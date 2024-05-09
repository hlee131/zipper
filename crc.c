#include <stdio.h> 

#include "includes/crc.h" 
#include "includes/helper.h"

// https://en.wikipedia.org/wiki/Cyclic_redundancy_check#:~:text=WEP)%20protocol.%5B7%5D-,Computation,-%5Bedit%5D 
uint32_t crc32(uint8_t* data, size_t length) {
    // crc32 is the remainder of polynomial division  
    uint32_t remainder = 0xFFFFFFFF; 

    // for each byte 
    for (int i = 0; i < length; i++) {
        uint8_t byte = data[i];
        // for each bit in byte 
        for (int j = 0; j < 8; j++) {
            // check if there's a remainder
            int has_rem = (byte ^ remainder) & 1;  
            // shift remainder over to the next place
            remainder >>= 1;
            // if the remainder is non-zero,
            // divide by the polynomial 
            if (has_rem) remainder ^= 0xEDB88320; 
            // shift the byte over to the next bit 
            byte >>= 1; 
        }
    }

    // return inverse of remainder because remainder started with -1 not 0 
    return ~remainder; 
}

uint8_t crc8(uint8_t* data, size_t length) {
    uint8_t remainder = 0xFF; 

    // for each byte 
    for (int i = 0; i < length; i++) {
        uint8_t byte = data[i];
        // for each bit in byte 
        for (int j = 0; j < 8; j++) {
            // check if there's a remainder
            int has_rem = (byte ^ remainder) & 1;  
            // shift remainder over to the next place
            remainder >>= 1;
            // if the remainder is non-zero,
            // divide by the polynomial 
            if (has_rem) remainder ^= 0x97; 
            // shift the byte over to the next bit 
            byte >>= 1; 
        }
    }

    // return inverse of remainder because remainder started with -1 not 0 
    return ~remainder; 
}