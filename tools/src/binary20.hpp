#ifndef BINARY20_HPP
#define BINARY20_HPP
#include "common.hpp"

/**
 * @file binary20.hpp
 * Defines functions used to convert assembly-style instructions to hex/binary for sm_20, sm_21, sm_30.
 */

/**
 * Get the binary value of an 'imme' operand.
 * An imme is usually an operand than could be either a hex value, constant memory, or a register.
 * @param op is the operand
 * @param sizeLimit is the number of bits the value can use (excluding the two bits that identify the type)
 * @param special indicates a unique value type: special & 1 means ignore negative; special & 2 means its the 4th operand and constant memory
 * @return a 64-bit integer whose lowest bits are the binary value
 */
long long getImme20(operand * op, char sizeLimit, char special);

/**
 * Converts an assembly instruction to a hex string, equivalent to binary code for sm_20, sm_21, sm_30.
 * @param inst The instruction being converted
 * @param arch The sm_xx architecture of the kernel code.
 * @param cuobjdump_version The cuobjdump version with which the assembly code was generated.
 * @return a char array containing 16 hexadecimal characters
 */
char* instructionToHexString20(instruction * inst, int arch, int cuobjdump_version);

#endif

