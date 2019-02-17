#ifndef DECODE_PARSE_HPP
#define DECODE_PARSE_HPP

#include "decode_common.hpp"
 
/**
 * @file decode.hpp
 * Defines some functions, structs, and constants used to analyze encodings and generate assemblers.
 */

//Various modifier types:
#define NUM_MOD_TYPES 12
#define MOD_FLAG 0
#define MOD_TYPE 1
#define MOD_PROPOSITION 2
#define MOD_COMPARE 3
#define MOD_ROUND 4
#define MOD_SHIFTDIRECTION 5
#define MOD_INVALIDCHKMODE 6
#define MOD_IADD3 7
#define MOD_MUFU 8
#define MOD_XMAD 9
#define MOD_IMNMX 10
#define MOD_FTZ 11

/**
 * Struct representing a modifier's encoding info.
 */
typedef struct {
	/**
	 * The modifier's ASCII name inside the assembly.
	 */
	char * token;
	
	/**
	 * The value of the binary instruction when this modifier was first observed.
	 */
	bool vals[64];
	
	/**
	 * Whether or not each bit is part of this modifier's encoding.
	 * Initialized to all true, then set to false for bits confirmed not to matter.
	 */
	bool matters[64];
	
	/**
	 * Possible value of the binary instruction when this modifier was *not* present.
	 */
	bool antivals[64];
	
	/**
	 * True iff the antivals array has been initialized.
	 */
	bool seenAntiVal;
	
	/**
	 * The modifier's type, such as MOD_FLAG, MOD_TYPE, MOD_ROUND, etc.
	 * MOD_FLAG is used for modifiers not known to be associated with a particular type.
	 */
	int type;
	
	/**
	 * The relative order of this modifier, with respect to its type.
	 * For example, the second MOD_ROUND type modifier has count=1 (the default value is count=0).
	 */
	int count;//0 for generic or 1st non-generic of its own type, 1 for second non-generic of its own type
	
	/**
	 * Whether we want to optimize number of lines for this modifier in generated assembler.
	 * If true, then this is combined with matching modifier from operations that have the same opcode.
	 */
	bool combine;//false iff we should avoid combining this with same mod from other versions of same operation
} operationMod;

/**
 * Struct representing an operand's encoding info.
 */
typedef struct {
	/**
	 * The operand's type.
	 */
	token_type type;
	
	/**
	 * Possible starting locations for the encoding of the operand's first component.
	 */
	bool possibleStart1[64];
	
	/**
	 * At each location in possibleStart1, the max number of bits that seem to match.
	 */
	int maxBits1[64];
	
	/**
	 * Possible starting locations for the encoding of the operand's second component.
	 * (I treat literal offset in memory operands as the second component.)
	 */
	bool possibleStart2[64];
	
	/**
	 * At each location in possibleStart2, the max number of bits that seem to match.
	 */
	int maxBits2[64];
	
	/**
	 * Possible starting locations for the encoding of the operand's third component.
	 * (I treat constant memory bank in const memory operands as the third component.)
	 */
	bool possibleStart3[64];
	
	/**
	 * At each location in possibleStart3, the max number of bits that seem to match.
	 */
	int maxBits3[64];
	
	/**
	 * The number of components in the operand.
	 * Set to 1 for most types, 2 for memory, 3 for constant memory.
	 */
	int components;
	
	/**
	 * A linked list of modifier encodings attached to this operand.
	 */
	node * mods;
	
	/**
	 * Indicates which unary functions we've seen attached to this operand.
	 * Has 1 bit for each unary function.
	 */
	operand_prop properties;
	
	/**
	 * For the four known unary functions, which bits are used in its encoding.
	 */
	bool propMatters[4][64];
	
	/**
	 * For the four known unary functions, the instruction's binary when they were seen.
	 */
	bool propVals[4][64];
	
	/**
	 * True if this (hex literal) operand was actually written as a float/double literal in decimal.
	 */
	bool decimal;
	
	/**
	 * True if this (hex literal) operand is encoded as a relative address.
	 */
	bool relative;
	
	/**
	 * True if the arithmetic negation unary function uses twos complement.
	 * False otherwise (e.g. if it's encoded as a single bit somewhere).
	 */
	bool incNegative;//true iff negative prop is applied to hex operand instead of flipping a bit
	
	/**
	 * True if this (hex literal) operand is encoded as its opposite.
	 * We've seen this happen with the last operand in ISUB instructions, since ISUB is really a special case of IADD.
	 */
	bool addlNegative;
	
	/**
	 * True if the encoding for the third component is bit-shifted due to limited space in binary.
	 */
	bool shiftComp3;
} operationOperand;

/**
 * Struct representing an operation's encoding.
 * If two instructions have the same opcode name AND the same operand types, they are considered the same operation.
 */
typedef struct {
	/**
	 * The opcode, using our own arbitrary identifiers.
	 */
	int op;//uses enum id from common.hpp
	
	/**
	 * True iff we've confirmed that this instruction can have predicate guards.
	 * Some instructions, such as CAL, cannot.
	 * Note that depending on the nvcc compiler version, NVIDIA's encoding will vary for cases where guards are disallowed.
	 * (I.e. it's sometimes encoded the same as the null predicate PT, and sometimes the same as P0).
	 */
	bool confirmedGuard;
	
	/**
	 * The instruction's binary the first time that we saw this operation.
	 */
	bool binid[64];
	
	/**
	 * Which bits control the opcode for this operation.
	 */
	bool binidmatters[64];
	
	/**
	 * Array of operand encodings.
	 */
	operationOperand * operands[8];
	
	/**
	 * The number of operands for this operation.
	 */
	int numOperands;
	
	/**
	 * Linked list of modifier encodings that can be attached to the opcode.
	 */
	node * mods;
	
	/**
	 * Value of instruction's binary in cases where each particular modifier type was completely absent.
	 */
	int noModBits[NUM_MOD_TYPES][64];
} operation;

/**
 * Returns the kind of mod a mod string corresponds to.
 * @param modstr The mod string
 * @return modifier's type if known, or MOD_FLAG if assumed to be generic
 */
int getModType(const char * modstr);

/**
 * Returns the unique integer ID we associate with the given opcode string.
 * @param opname The opcode's string
 * @return opcode's integer ID, or -1 on failure
 */
int getOpcode(const char * opname);

/**
 * Performs analysis of an instruction, updating the operation list.
 * @param inst The parsed assembly code
 * @param hexstring the binary code (in hexadecimal format)
 */
void analyze(instruction * inst, char * hexstring);

#endif

