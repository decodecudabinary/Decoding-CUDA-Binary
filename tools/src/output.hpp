#ifndef OUTPUT_HPP
#define OUTPUT_HPP

#include "common.hpp"

/**
 * @file output.hpp
 * Defines functions used for output to stdout.
 */

/**
 * Outputs an instruction.
 * @param out The output stream.
 * @param inst The instruction to output.
 */
void fprintfInstruction(std::ostream & out, instruction * inst);

/**
 * Outputs the assembly code, split into basic blocks, to stdout
 * @param metadata If true, prints out the metadata.
 */
void printfBlocks(bool metadata);

/**
 * Outputs assembly code in a format optimized for ease of reading the dispatch values.
 */
void printfSCHIVals();

/**
 * Outputs the assembly code, together with hexadecimal equivalent, to stdout
 */
void printfHex();

/**
 * Prints the assembly to stdout, in appropriate format for parsing.
 * @param metadata True iff metadata for parsing should be included
 */
void printfAssembly(bool metadata);

#endif
