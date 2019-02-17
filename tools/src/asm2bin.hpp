#ifndef ASM2BIN_PARSE_HPP
#define ASM2BIN_PARSE_HPP

#include "common.hpp"
 
/**
 * @file asm2bin.hpp
 * Defines functions and constants used by the asm2bin parser.
 */

/**
 * Deletes scheduling metadata instructions from all of the basic blocks.
 */
void stripSCHI();

/**
 * Inserts scheduling metadata instructions throughout code.
 * Note that this assumes there are no such instructions already present.
 */
void insertSCHIs();

/**
 * Creates an array of binary code equivalent to the assembly.
 * @return a structure containing the array of code
*/
kernelData * generateByteCode();

/**
 * Gets a list of functions within the code, indicated by CAL instructions.
 * Useful for fixing up the ELF after adding/removing code.
 * @return a list of ascending addresses corresponding to function starts
 */
node * getCallTargets();

/**
 * Called by parser after reaching the end of the assembly code.
 */
void finishParsing();

#endif

