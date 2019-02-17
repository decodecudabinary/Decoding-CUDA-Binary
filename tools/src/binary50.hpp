#ifndef BINARY50_HPP
#define BINARY50_HPP
#include "common.hpp"

/**
 * @file binary50.hpp
 * Defines functions used to convert assembly-style instructions to hex/binary for sm_5x and sm_6x.
 */

/**
 * Converts an assembly instruction to a hex string, equivalent to binary code for sm_5x or sm_6x.
 * @param inst The instruction being converted
 * @param cuobjdump_version The cuobjdump version with which the assembly code was generated.
 * @return a char array containing 16 hexadecimal characters
 */
char* instructionToHexString50(instruction * inst, int cuobjdump_version);

#endif
