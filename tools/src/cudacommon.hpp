#ifndef CUDACOMMON_HPP
#define CUDACOMMON_HPP
#include "common.hpp"

/**
 * @file cudacommon.hpp
 * Defines functions used to analyze CUDA code.
 */

/**
 * Checks if instruction has given mod attached to opcode.
 * @param inst The instruction
 * @param mod The mod string
 * @return true iff mod is present, false iff not
 */
bool hasMod(instruction* inst, const char* mod);

/**
 * Checks if operand has given mod.
 * @param op The operand
 * @param mod The mod string
 * @return true iff mod is present, false iff not
 */
bool hasMod(operand* op, const char* mod);

/**
 * Checks if instruction has given mod in appropriate place, associated with the opcode.
 * @param inst The instruction
 * @param mod The mod string
 * @param later Should be 1 iff target mod is after a type mod; 0 if before
 * @return true iff mod is present, false iff not
 */
bool hasTypeMod(instruction* inst, const char* mod, char later);

/**
 * Gets the basic block which contains the given line.
 * @param line The line number for an instruction
 * @return the blockNode which contains the line, or 0 if no such line exists
 */
blockNode * getBlock(int line);

/**
 * Gets the basic block which contains the given instruction.
 * @param inst The instruction
 * @return the blockNode which contains the instruction, or 0 if no such line exists
 */
blockNode * getBlock(instruction * inst);

/**
 * Gets the instruction with the given line number.
 * @param line The line number for an instruction
 * @return the desired instruction
 */
instruction * getLine(int line);

#endif
