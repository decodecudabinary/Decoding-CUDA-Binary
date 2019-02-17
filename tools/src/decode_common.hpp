#ifndef DECODE_COMMON_HPP
#define DECODE_COMMON_HPP
#include <set>
#include <string>

/**
 * @file decode_common.hpp
 * Defines enums, structs, and functions that are used in multiple parts of the program.
 */

#define MAXREGISTERS 255

/**
 * Identifies a token's 'type' when parsing.
 * Also used to keep track of operand types.
 */
typedef enum {
	type_opcode,
	type_register,
	type_special_reg,
	type_predicate,
	type_guard,
	type_sb,
	type_hex,
	type_bit_list,
	type_mod,
	type_const_mem,
	type_mem,
	type_texture_operand,
	type_channel,
	type_other_operand,
	type_label,
} token_type;

/**
 * Used to build linked lists.
 */
typedef struct node {
	void * value;
	struct node * next;
} node;

/**
 * Mostly used to hold data during parsing.
 */
typedef struct {
	token_type type;
	char* lexeme;
} token;

/**
 * Used to note attributes and unary operations of an operand.
 */
typedef enum {
	prop_minus = 1,
	prop_bitwise_complement = 2,
	prop_absolute_value = 4,
	prop_not = 8,
	
	prop_float = 128,//not a real property; used here for accurate asm->asm
	prop_double = 256,//not a real property; used here for accurate asm->asm
	prop_exp = 512,//not a real property; used here for accurate asm->asm
	prop_float32i = 1024,//not a real property; used here for accurate asm->asm
} operand_prop;
inline operand_prop operator|(operand_prop a, operand_prop b) {
	return static_cast<operand_prop>(static_cast<int>(a) | static_cast<int>(b));
}
inline operand_prop operator-(operand_prop a, operand_prop b) {
	return static_cast<operand_prop>(static_cast<int>(a) - static_cast<int>(b));
}

/**
 * Represents a parsed operand.
 */
typedef struct {
	/**
	 * The list of mods applies to this operand, such as ".CC".
	 */
	node * mods;
	
	/**
	 * The operand's type.
	 */
	token_type type;
	
	/**
	 * Part of the operand's value.
	 * Used for label name for labels.
	 */
	char * strval;
	
	/**
	 * Part of the operand's value.
	 * This is the register ID in register, predicate, and memory operands.
	 * This is an integer or bit-shifted float value for hex operands.
	 */
	long long val1;
	
	/**
	 * Part of the operand's value.
	 * This is the offset for global/local/shared memory operands.
	 * This is the memory bank value (the first hex value) for constant memory operands.
	 */
	int val2;
	
	/**
	 * Part of the operand's value.
	 * This is the offset (the second hex value) for constant memory operands.
	 */
	int val3;
	
	/**
	 * The ID of the first register inside this operand, or -1.
	 * The RZ register results in a value of -1 for this.
	 */
	int reg;
	
	/**
	 * The operand's properties, such as negative and/or absolute value.
	 * There are some fake properties used for accurate output of assembly code.
	 */
	operand_prop properties;
	
	/**
	 * For base-10 operands (used in float/double instructions), this is the number of digits after the decimal point.
	 * Used for accurate assembly output.
	 */
	int precision;
	
	/**
	 * Set to true for the last operand in I2F, F2I, I2I, or F2F.
	 * Used for accurate assembly output.
	 */
	bool converted;
	
	bool bad;
	
	bool decimal;
} operand;

/**
 * Unique ID for each opcode.
 */
typedef enum {
	opcode_MOV       	= 0,
	opcode_MOV32I       	= 1,
	opcode_LD       	= 2,
	opcode_LDU       	= 3,
	opcode_LDL       	= 4,
	opcode_LDS       	= 5,
	opcode_LDC       	= 6,
	opcode_ST       	= 7,
	opcode_STL       	= 8,
	opcode_STS       	= 9,
	opcode_LDLK       	= 10,
	opcode_LDSLK       	= 11,
	opcode_STUL       	= 12,
	opcode_STSUL       	= 13,
	opcode_FADD       	= 14,
	opcode_FADD32I       	= 15,
	opcode_FMUL       	= 16,
	opcode_FMUL32I       	= 17,
	opcode_FFMA       	= 18,
	opcode_FSET       	= 19,
	opcode_FSETP       	= 20,
	opcode_DSETP       	= 21,
	opcode_FCMP       	= 22,
	opcode_MUFU       	= 23,
	opcode_DADD       	= 24,
	opcode_DMUL       	= 25,
	opcode_DFMA       	= 26,
	opcode_IADD       	= 27,
	opcode_IADD32I       	= 28,
	opcode_IMNMX       	= 29,
	opcode_IMUL       	= 30,
	opcode_IMUL32I       	= 31,
	opcode_IMAD       	= 32,
	opcode_ISCADD       	= 33,
	opcode_ISET       	= 34,
	opcode_ISETP       	= 35,
	opcode_ICMP       	= 36,
	opcode_I2F       	= 37,
	opcode_I2I       	= 38,
	opcode_F2I       	= 39,
	opcode_F2F       	= 40,
	opcode_LOP       	= 41,
	opcode_LOP32I       	= 42,
	opcode_SHL       	= 43,
	opcode_SHR       	= 44,
	opcode_BFE       	= 45,
	opcode_BFI       	= 46,
	opcode_SEL       	= 47,
	opcode_SCHI       	= 48,//this is asfermi's name for the fake instructions which contain scheduling information in sm_30 and up
	opcode_SSY       	= 49,
	opcode_BRA       	= 50,
	opcode_BRX       	= 51,
	opcode_PCNT       	= 52,
	opcode_CONT       	= 53,
	opcode_PBK       	= 54,
	opcode_BRK       	= 55,
	opcode_CAL       	= 56,
	opcode_RET       	= 57,
	opcode_EXIT       	= 58,
	opcode_NOP       	= 59,
	opcode_BAR       	= 60,
	opcode_BPT       	= 61,
	opcode_B2R       	= 62,
	opcode_S2R       	= 63,
	opcode_PSETP       	= 64,
	opcode_PSET       	= 65,
	opcode_FLO       	= 66,
	opcode_P2R       	= 67,
	opcode_R2P       	= 68,
	opcode_TEX       	= 69,
	opcode_TEXDEPBAR     	= 70,
	opcode_RRO       	= 71,
	opcode_PRMT       	= 72,
	opcode_VADD       	= 73,
	opcode_DMNMX       	= 74,
	opcode_FMNMX       	= 75,
	opcode_RED			= 76,
	opcode_VOTE			= 77,
	opcode_POPC			= 78,
	opcode_MEMBAR		= 79,
	opcode_STSCUL		= 80,
	opcode_LEPC		= 81,
	opcode_CSETP		= 82,
	opcode_ISCADD32I		= 83,
	opcode_VMNMX		= 84,
	opcode_TLD		= 85,
	opcode_SHF		= 86,
	opcode_FCHK		= 87,
	opcode_ISUB		= 88,
	opcode_JCAL		= 89, //calls extern functions like printf
	opcode_SHFL		= 90,
	opcode_LDG		= 91,
	opcode_LD_LDU		= 92,
	opcode_ATOM		= 93,
	opcode_CCTL		= 94,
	opcode_XMAD		= 95,
	opcode_SYNC		= 96,
	opcode_STG		= 97,
	opcode_IADD3		= 98,
	opcode_VABSDIFF		= 99,
	opcode_DEPBAR		= 100,
	opcode_LOP3		= 101,
	opcode_TLDS		= 102,
	opcode_TEXS		= 103,
	opcode_LEA		= 104,
	opcode_DSET		= 105,
} opcode;

/**
 * Represents an assembly instruction.
 */
typedef struct {
	/**
	 * A label representing this instruction's address, or 0.
	 */
	char * label;
	
	/**
	 * The predicate guard, or 0 of there isn't one.
	 */
	operand * guard;
	
	/**
	 * A unique ID representing the instruction's opcode.
	 */
	opcode op;
	
	/**
	 * A list of mods (such as ".LU" or ".128") attached to the opcode.
	 */
	node * mods;
	
	/**
	 * The operands for this instruction.
	 */
	operand ** operands;
	
	/**
	 * The number of operands for this instruction.
	 */
	int num_operands;
	
	int address;
} instruction;

/**
 * Names of special registers.
 * Corresponds one-to-one with the specialIDs array.
 */
extern std::string _specialNames20[];

/**
 * Values of special registers.
 * Corresponds one-to-one with the specialNames array.
 */
extern const int _specialIDs20[];

/**
 * Number of recognized special registers.
 */
extern const int _numSpecials20;

/**
 * Names of texture operands.
 * Corresponds one-to-one with the textureIDs array.
 */
extern const char * _textureNames[];

/**
 * Values of texture operands.
 * Corresponds one-to-one with the textureNames array.
 */
extern const int _textureIDs[];

/**
 * Number of recognized texture operands.
 */
extern const int _numTextures;

/**
 * Names of channel operands.
 * Corresponds one-to-one with the _channelIDs array.
 */
extern const char * _channelNames[];

/**
 * Values of channel operands.
 * Corresponds one-to-one with the _channelNames array.
 */
extern const int _channelIDs[];

/**
 * Number of recognized channel operands.
 */
extern const int _numChannels;

/**
 * Names of miscellaneous operands.
 */
extern const char * _otherNames[];

/**
 * Number of items in otherNames array.
 */
extern const int _numOthers;

/**
 * List of opcode names.
 */
extern const char * opcodes[];

 /**
 * Returns the value which represents a given special register.
 * @param lexeme The special register's name
 * @param format If true, change special register's name to match given lexeme
 * @param arch The architecture (e.g. 20 for sm_20)
 */
int getSpecialID(const char * lexeme, bool format, int arch);

/**
 * Returns a unique ID for certain miscellaneous operand types.
 * @param lexeme The operand's name
 */
int getOtherID(const char * lexeme);

/**
 * Returns the value which represents a given texture operand.
 * @param lexeme The texture operand's name
 */
int getTextureID(const char * lexeme);

/**
 * Returns the value which represents a given channel operand.
 * @param lexeme The channel operand's name
 */
int getChannelID(const char * lexeme);


/**
 * Create an instruction with given values.
 * Frees operand nodes after putting data into proper array.
 * @param op The opcode's ID
 * @param mods The list of mods for the opcode
 * @param operands The list of operands
 * @return an instruction
 */
instruction* newInstruction(opcode op, node * mods, node * operands);

/**
 * Create an instruction with no operands.
 * Frees operand nodes after putting data into proper array.
 * @param op The opcode's ID
 * @param mods The list of mods for the opcode
 * @return an instruction
 */
instruction* newInstruction(opcode op, node * mods);

/**
 * Create an instruction with one operand.
 * Frees operand nodes after putting data into proper array.
 * @param op The opcode's ID
 * @param mods The list of mods for the opcode
 * @param o1 The operand
 * @return an instruction
 */
instruction* newInstruction(opcode op, node * mods, operand * o1);

/**
 * Create an instruction with two operands.
 * Frees operand nodes after putting data into proper array.
 * @param op The opcode's ID
 * @param mods The list of mods for the opcode
 * @param o1 The first operand
 * @param o2 The second operand
 * @return an instruction
 */
instruction* newInstruction(opcode op, node * mods, operand * o1, operand * o2);

/**
 * Create an instruction with three operands.
 * Frees operand nodes after putting data into proper array.
 * @param op The opcode's ID
 * @param mods The list of mods for the opcode
 * @param o1 The first operand
 * @param o2 The second operand
 * @param o3 The third operand
 * @return an instruction
 */
instruction* newInstruction(opcode op, node * mods, operand * o1, operand * o2, operand * o3);

/**
 * Create an instruction with four operands.
 * Frees operand nodes after putting data into proper array.
 * @param op The opcode's ID
 * @param mods The list of mods for the opcode
 * @param o1 The first operand
 * @param o2 The second operand
 * @param o3 The third operand
 * @param o4 The fourth operand
 * @return an instruction
 */
instruction* newInstruction(opcode op, node * mods, operand * o1, operand * o2, operand * o3, operand * o4);

/**
 * Create an operand with given values.
 * Sets unspecified values to appropriate defaults.
 * @param type The operand type
 * @param val1 (defaults to 0)
 * @param val2 (defaults to 0)
 * @param val3 (defaults to 0)
 * @return the new operand
 */
operand* newOperand(token_type type, long long val1 = 0, int val2 = 0, int val3 = 0);

/**
 * Create an operand with given values.
 * Sets unspecified values to appropriate defaults.
 * @param type The operand type
 * @param props The operand's properties
 * @param val1
 * @param val2
 * @param val3
 * @return the new operand
 */
operand* newOperand(token_type type, operand_prop props, long long val1, int val2, int val3);

/**
 * Add a value to a sorted list of nodes, treated as a set.
 * Cannot add same value a second time (unless comparator erroneously varies).
 * @param first The address of the list
 * @param value The item to add to the list
 * @param comparator A comparison function, for sorting the list's items
 * @return the added node, or 0 if no change was made
 */
node* addNode(node **first, void * value, long long (*comparator)(void*, void*));

/**
 * Adds node to end of list, no questions asked.
 * @param first The address of the list
 * @param value The item to add to the list
 */
void addLast(node ** first, void * value);

/**
 * Adds node to start of list, no questions asked.
 * @param first The address of the list
 * @param value The item to add to the list
 */
void addFirst(node ** first, void * value);

/**
 * Adds value to list, immediately after specified predecessor.
 * If predecessor is 0, value is added to start of list instead.
 * @param first The address of the list
 * @param afterMe The node we're inserting things after
 * @param value The item to add to the list
 */
void addAfter(node ** first, node * afterMe, void * value);

/**
 * Adds value to list, immediately before specified node.
 * If node is 0, value is added to start of list.
 * @param first The address of the list
 * @param beforeMe The node we're inserting things before
 * @param value The item to add to the list
 */
void addBefore(node **first, node * beforeMe, void * value);

/**
 * Adds node to list, at specified zero-based index
 * @param first The address of the list
 * @param value The item to add to the list
 * @param index The location in the list to add the element to
 */
void addAtIndex(node ** first, void * value, int index);

/**
 * Removes a node from a list.
 * @param first The address of the list
 * @param value The item to remove from the list
 * @param comparator A comparison function, used to identify the correct item
 * @return the removed node on success, or 0 if the node does not exist
 */
node * removeNode(node **first, void * value, long long (*comparator)(void*, void*));

/**
 * Removes a node from a list and frees it.
 * @param first The address of the list
 * @param value The item to remove from the list
 * @param comparator A comparison function, to identify the corrent item
 * @return true on success, false if node does not exist
 */
bool deleteNode(node **first, void * value, long long (*comparator)(void*, void*));

/**
 * Removes a node from a list, and frees it and its contents.
 * @param first The address of the list
 * @param value The item to remove from the list
 * @param comparator A comparison function, to identify the corrent item
 * @param valueCleaner The free function to use on the node's contents
 * @return true on success, false if node does not exist
 */
bool deleteNode(node **first, void * value, long long (*comparator)(void*, void*), void (*valueCleaner)(void *));

/**
 * Removes a node from a list and frees it.
 * @param first The address of the list
 * @param del The node to delete
 * @return true on success, false if node is not in list
 */
bool deleteNode(node **first, node * del);

/**
 * Comparator for addNode; compares long integers by value.
 * @param a The first long being compared
 * @param b The second long being compared
 * @return negative number iff a<b, 0 iff a==b, positive number otherwise
 */
long long intorder(void * a, void * b);

/**
 * Comparator for addNode; compares char* using strcmp.
 * @param a The first char* being compared
 * @param b The second char* being compared
 * @return strcmp result casted to a long
 */
long long charstarorder(void * a, void * b);

/**
 * Comparator for addNode; compares char* values.
 * @param a The first char* string being compared
 * @param b The second char* string being compared
 * @return negative number iff a comes before b, 0 iff equal, positive number otherwise
 */
long long lexicographicorder(void * a, void * b);

/**
 * Gets the size of a list.
 * @param first The first node in the list
 * @return the integer size
 */
int listSize(node *first);

/**
 * Finds an item inside a list.
 * @param first The list
 * @param val The item being searched for
 * @return the node which contains the item, or 0 on failure
 */
node * containsValue(node *first, int * val);

/**
 * Frees a list of nodes, but does not free the contents.
 * @param first The list
 */
void cleanNodes(node *first);

/**
 * Frees memory used by list, including its contents.
 * @param first The first node in list
 * @param valueCleaner The cleanup function for each item in the list
 */
void cleanNodesFully(node * first, void (*valueCleaner)(void*));

/**
 * Frees memory used by #operand, including its contents.
 * @param o The operand
 */
void cleanOperand(operand * o);

/**
 * Frees memory used by #instruction, including its contents.
 * @param i The instruction
 */
void cleanInstruction(instruction* i);

#endif
