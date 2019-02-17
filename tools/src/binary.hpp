#ifndef BINARY_HPP
#define BINARY_HPP
#include "common.hpp"

/**
 * @file binary.hpp
 * Defines functions used to convert assembly-style instructions to hex/binary.
 */

/**
 * Gets the difference between two values.
 * Used to determine jump sizes when converting instructions to binary/hex.
 * @param val1 The first value
 * @param val2 The second value
 * @param sizeLimit The number of bits in the difference
 * @return a 64-bit integer whose lowest bits equate to (val1-val2)
 */
long long getBinaryDifference(unsigned long long val1, int val2, char sizeLimit);

/**
 * Sets bits inside of a 64-bit unsigned integer, which represents a binary instruction.
 * @param binary The address of the value being altered
 * @param location The index of the first bit being set
 * @param value The bits used to set binary
 * @param size The number of bits in value
 * @param overwrite Indicates whether to overwrite 1's with 0's
 */
void setBinary(unsigned long long * binary, int location, long long value, int size, char overwrite);

/**
 * Converts an assembly instruction to a hex string, equivalent to binary code.
 * @param inst The instruction being converted
 * @param arch The kernel function's architecture (the value of xx in sm_xx).
 * @param cuobjdump_version The cuobjdump version with which the assembly code was generated.
 * @return a char array containing 16 hexadecimal characters
 */
char* instructionToHexString(instruction * inst, int arch, int cubojdump_version);

#endif

