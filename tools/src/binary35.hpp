#ifndef BINARY35_HPP
#define BINARY35_HPP
#include "common.hpp"

/**
 * @file binary35.hpp
 * Defines functions used to convert assembly-style instructions to hex/binary for sm_3x.
 */

/**
 * Converts an assembly instruction to a hex string, equivalent to binary code for sm_3x.
 * @param inst The instruction being converted
 * @param cuobjdump_version The cuobjdump version with which the assembly code was generated.
 * @return a char array containing 16 hexadecimal characters
 */
char* instructionToHexString35(instruction * inst, int cuobjdump_version);

#endif
