#ifndef CFGHELPERS_HPP
#define CFGHELPERS_HPP
#include "common.hpp"
#include <vector>
#include <stack>
#include <utility>
using namespace std;

/**
 * @file cfghelpers.hpp
 * Defines functions used to create and modify the control flow graph.
 */

/**
 * Adds a branch from a given block to a given line.
 * If the line is in the middle of a basic block, it will be split into two blocks.
 * If block is NULL, this function simply splits the block which contains the target line.
 * @param a The block being branched from, or NULL.
 * @param line The target instruction.
 * @return true if the branch was added; false if it already existed
 */
bool addBranchToLine(blockNode * a, instruction * line);

/**
 * Adds a branch from a given block to a given line number.
 * If the line is in the middle of a basic block, it will be split into two blocks.
 * If block is NULL, this function simply splits the block which contains the target line.
 * @param a The block being branched from, or NULL.
 * @param line The target line number.
 * @return true if the branch was added; false if it already existed
 */
bool addBranchToLine(blockNode * a, int line);

/**
 * Adds a branch from a given block to a given address.
 * Finds line number for address, then calls #addBranchToLine
 * @param a The block being branched from, or NULL.
 * @param address The target address.
 * @return true if the branch was added; false if it already existed
 */
bool addBranchToAddress(blockNode * a, int address);

/**
 * Finds addresses for labels, and adds the addresses to metadata as appropriate.
 */
void labelsToAddresses();

/**
 * Sets up labels for blocks; replaces hex addresses with label operands.
 * Overwrites existing labels.
 */
void addressesToLabels();

/**
 * Combines adjacent blocks that should together form a basic block if predicate guards are ignored.
 */
void combineBlocks();

/**
 * Helper to fix ptrs (jump targets) after a block is deleted.
 * @param oldVal The ptr value to change
 * @param newVal The ptr value to replace oldVal with
 */
void changePtrs(blockNode* oldVal, blockNode* newVal);

/**
 * Deletes a basic block, adding its successors to its predecessors' successors.
 * @param deleted The block to delete
 */
void deleteBlock(blockNode* deleted);

/**
 * Fixes line numbers & addresses after addition/deletion of instructions.
 */
void fixAssembly();

/**
 * Adds successors to blocks with the specified end type.
 * @param The type of instruction the blocks end with.
 */
void propogateBranches(blockEnd branch_type);

/**
 * Helper for propogateCallReturns.
 * Recursively adds successors to blocks that end with a RET.
 * @param search The current block in which we're searching for a RET
 * @param target The address to jump to after a RET
 * @param tag A unique value to avoid infinite recursion
 * @return true iff changes were made
 */
bool propogateReturns(blockNode * search, int target, int tag);

/**
 * Add successors to blocks that end in a CAL or RET.
 * @return true iff changes were made, false otherwise
 */
bool propogateCallReturns();

/**
 * Adds pointers based on thread divergence, break, and continue instructions.
 * @param search The current block in which we're searching for SSY/.S|SYNC, PBK/BRK, and PCNT/CONT
 * @param ptrs A stack of pointers, as <opcode, address> pairs
 * @param tag A unique value used to avoid double visiting any blocks
 * @param firstCall True iff at depth 0 of recursion
 * @return true iff changes were made, false otherwise
 */
bool propogatePointersHelper(blockNode * search, stack<pair<opcode, long long> > ptrs, int tag, bool firstCall);

/**
 * Adds pointers based on thread divergence, break, and continue instructions.
 * @return true iff changes were made, false otherwise
 */
bool propogatePointers();

/**
 * Sets target blocks for relevant instructions.
 * Call this after first setting up all the basic blocks.
 */
void setPtr();

#endif
