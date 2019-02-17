#ifndef DECODE_OUTPUT_HPP
#define DECODE_OUTPUT_HPP

#include "decode_common.hpp"

/**
 * @file decode_output.hpp
 * Defines functions used for output to stdout.
 */

/**
 * Outputs an operand to stdout
 * @param inst The operand's instruction
 * @param op The operand
 */
void printfOperand(instruction * inst, operand * op);

/**
 * Outputs an instruction.
 * @param out The output stream.
 * @param inst The instruction to output.
 */
void fprintfInstruction(std::ostream & out, instruction * inst);

/**
 * Prints the assembly's metadata to stdout, in appropriate format for parsing.
 */
void printfMetadata();

#endif
