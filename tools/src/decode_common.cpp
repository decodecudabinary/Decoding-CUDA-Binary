#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode_common.hpp"

/**
 * Names of special registers for sm_2x.
 * Corresponds one-to-one with the specialIDs array.
 */
std::string _specialNames20[] = {"laneid", "virtcfg", "virtid", "pm0", "pm1", "pm2", "pm3", "pm4", "pm5", "pm6", "pm7", "prim_type", "invocation_id", "y_direction", "machine_id_0", "machine_id_1", "machine_id_2", "machine_id_3", "affinity", "tid", "tid.x", "tid.y", "tid.z", "ctaparam", "ctaid.x", "ctaid.y", "ctaid.z", "ntid", "ntid.x", "ntid.y", "ntid.z", "gridparam", "nctaid.x", "nctaid.y", "nctaid.z", "swinlo", "swinsz", "smemsz", "smembanks", "lwinlo", "lwinsz", "lmemlosz", "lmemhioff", "eqmask", "ltmask", "lemask", "gtmask", "gemask", "clocklo", "clockhi"};

/**
 * Values of special registers for sm_2x.
 * Corresponds one-to-one with the specialNames array.
 */
const int _specialIDs20[] = {0,2,3,4,5,6,7,8,9,10,11,16,17,18,24,25,26,27,28,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,80,81};

/**
 * Number of recognized special registers for sm_2x.
 */
const int _numSpecials20 = 62;

/**
 * Names of special registers for sm_3x through sm_6x
 * //TODO deal with SM_SHADER_TYPE, which does not start with SR_
 */
std::string _specialNames35[] = {"laneid", "clock", "virtcfg", "virtid", "pm0", "pm1", "pm2", "pm3", "pm4", "pm5", "pm6", "pm7", "", "", "", "", "prim_type", "invocation_id", "y_direction", "thread_kill", "shader_type", "directcbewriteaddresslow", "directcbewriteaddresshigh", "directcbewriteenabled", "machine_id_0", "machine_id_1", "machine_id_2", "machine_id_3", "affinity", "invocation_info", "wscalefactor_xy", "wscalefactor_z", "tid", "tid.x", "tid.y", "tid.z", "cta_param", "ctaid.x", "ctaid.y", "ctaid.z", "ntid", "cirqueueincrminusone", "nlatc", "", "", "", "", "", "swinlo", "swinsz", "smemsz", "smembanks", "lwinlo", "lwinsz", "lmemlosz", "lmemhioff", "eqmask", "ltmask", "lemask", "gtmask", "gemask", "regalloc", "ctxaddr", "", "globalerrorstatus", "", "warperrorstatus", "warperrorstatusclear", "", "", "", "", "pm_hi0", "pm_hi1", "pm_hi2", "pm_hi3", "pm_hi4", "pm_hi5", "pm_hi6", "pm_hi7", "clocklo", "clockhi", "globaltimerlo", "globaltimerhi", "", "", "", "", "", "", "", "", "", "", "", "", "hwtaskid", "circularqueueentryindex", "circularqueueentryaddresslow", "circularqueueentryaddresshigh"};

/**
 * Names of texture operands.
 * Corresponds one-to-one with the textureIDs array.
 */
const char * _textureNames[] = {"1D", "ARRAY_1D", "RECT", "2D", "ARRAY_2D", "3D", "CUBE", "ARRAY_CUBE"};

/**
 * Values of texture operands.
 * Corresponds one-to-one with the textureNames array.
 */
const int _textureIDs[] = {0,1,2,2,3,4,6,7};

/**
 * Number of recognized texture operands.
 */
const int _numTextures = 8;

/**
 * Names of texture operands.
 * Corresponds one-to-one with the textureIDs array.
 */
const char * _channelNames[] = {"R", "G", "B", "A", "RG", "RA", "GA", "BA", "RGB", "RGA", "RBA", "GBA", "RGBA", "INVALID5", "INVALID6", "INVALID7", };

/**
 * Values of texture operands.
 * Corresponds one-to-one with the textureNames array.
 */
const int _channelIDs[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

/**
 * Number of recognized texture operands.
 */
const int _numChannels = 16;

/**
 * Names of miscellaneous operands.
 */
const char * _otherNames[] = {"CC", "PR"};

/**
 * Number of items in otherNames array.
 */
const int _numOthers = 2;

/**
 * List of opcode names.
 * An opcode's index in this array should be the same as the the corresponding 'opcode' enum value.
 */
const char * opcodes[] = {"MOV", "MOV32I", "LD", "LDU", "LDL", "LDS", "LDC", "ST", "STL", "STS", "LDLK", "LDSLK", "STUL", "STSUL", "FADD", "FADD32I", "FMUL", "FMUL32I", "FFMA", "FSET", "FSETP", "DSETP", "FCMP", "MUFU", "DADD", "DMUL", "DFMA", "IADD", "IADD32I", "IMNMX", "IMUL", "IMUL32I", "IMAD", "ISCADD", "ISET", "ISETP", "ICMP", "I2F", "I2I", "F2I", "F2F", "LOP", "LOP32I", "SHL", "SHR", "BFE", "BFI", "SEL", "SCHI", "SSY", "BRA", "BRX", "PCNT", "CONT", "PBK", "BRK", "CAL", "RET", "EXIT", "NOP", "BAR", "BPT", "B2R", "S2R", "PSETP", "PSET", "FLO", "P2R", "R2P", "TEX", "TEXDEPBAR", "RRO", "PRMT", "VADD", "DMNMX", "FMNMX", "RED", "VOTE", "POPC", "MEMBAR", "STSCUL", "LEPC", "CSETP", "ISCADD32I", "VMNMX", "TLD", "SHF", "FCHK", "ISUB", "JCAL", "SHFL", "LDG", "LD_LDU", "ATOM", "CCTL", "XMAD", "SYNC", "STG", "IADD3", "VABSDIFF", "DEPBAR", "LOP3", "TLDS", "TEXS", "LEA", "DSET", 0};

int getSpecialID(const char * lexeme, bool format, int arch) {
	if(arch <= 30) {
		for(int x = 0; x < _numSpecials20; x++) {
			for(int y = 0; y >= 0; y++) {
				if(lexeme[y] == _specialNames20[x][y]) {
					if(lexeme[y] == 0) {
						//Make them match perfectly for printing out later:
						if(format) {
							for(int z = 0; z < y; z++) {
								_specialNames20[x][z] = lexeme[z];
							}
						}
					
						//Return value:
						return _specialIDs20[x];
					}
				} else if(lexeme[y] == '_' &&  _specialNames20[x][y] == '.') {
					//interchangeable; format varies depending on CUDA version
				} else if(lexeme[y] == '.' &&  _specialNames20[x][y] == '_') {
					//interchangeable; format varies depending on CUDA version
				} else if(lexeme[y] >= 'A' && lexeme[y] <= 'Z' && lexeme[y] == _specialNames20[x][y] - ('a' - 'A')) {
					//interchangeable; format varies depending on CUDA version
				} else if(lexeme[y] >= 'a' && lexeme[y] <= 'z' && lexeme[y] == _specialNames20[x][y] - ('A' - 'a')) {
					//interchangeable; format varies depending on CUDA version
				} else {
					break;
				}
			}
		}
	} else if(arch < 70) {
		int numSpecials = sizeof(_specialNames35) / sizeof(std::string);
		for(int x = 0; x < numSpecials; x++) {
			for(int y = 0; y >= 0; y++) {
				if(lexeme[y] == _specialNames35[x][y]) {
					if(lexeme[y] == 0) {
						//Make them match perfectly for printing out later:
						if(format) {
							for(int z = 0; z < y; z++) {
								_specialNames35[x][z] = lexeme[z];
							}
						}
					
						//Return value:
						return x;
					}
				} else if(lexeme[y] == '_' &&  _specialNames35[x][y] == '.') {
					//interchangeable; format varies depending on CUDA version
				} else if(lexeme[y] == '.' &&  _specialNames35[x][y] == '_') {
					//interchangeable; format varies depending on CUDA version
				} else if(lexeme[y] >= 'A' && lexeme[y] <= 'Z' && lexeme[y] == _specialNames35[x][y] - ('a' - 'A')) {
					//interchangeable; format varies depending on CUDA version
				} else if(lexeme[y] >= 'a' && lexeme[y] <= 'z' && lexeme[y] == _specialNames35[x][y] - ('A' - 'a')) {
					//interchangeable; format varies depending on CUDA version
				} else {
					break;
				}
			}
		}
	} else {
		fprintf(stderr, "ERROR c~98: Unimplemented.\n");
	}
	
	fprintf(stderr,"WARNING: Found unrecognized special register \"%s\".\n",lexeme);
	return -1;
}

int getOtherID(const char * lexeme) {
	for(int x = 0; x < _numOthers; x++) {
		if(!strcmp(lexeme, _otherNames[x])) {
			return x;
		}
	}
	
	fprintf(stderr,"\n*SANITY CHECK ERROR ~22* This message is unreachable! \"%s\".\n\n",lexeme);
	return -1;
}

int getTextureID(const char * lexeme) {
	for(int x = 0; x < _numTextures; x++) {
		if(!strcmp(lexeme, _textureNames[x])) {
			return _textureIDs[x];
		}
	}
	
	fprintf(stderr,"\n*SANITY CHECK ERROR ~33* Found unrecognized texture operand \"%s\".\n\n",lexeme);
	return -1;
}

int getChannelID(const char * lexeme) {
	for(int x = 0; x < _numChannels; x++) {
		if(!strcmp(lexeme, _channelNames[x])) {
			return _channelIDs[x];
		}
	}
	
	fprintf(stderr,"\n*SANITY CHECK ERROR c~162: Found unrecognized channel operand \"%s\".\n\n",lexeme);
	return -1;
}

instruction* newInstruction(opcode op, node * mods, node * operands) {
	instruction * inst = (instruction*) malloc(sizeof(instruction));
	inst->label = 0;
	inst->op = op;
	inst->mods = mods;
	inst->guard = 0;
	inst->num_operands = 0;
	inst->operands = 0;
	
	if(operands) {
		inst->num_operands = listSize(operands);
		inst->operands = (operand**) malloc(inst->num_operands*sizeof(operand*));
		node *o = operands;
		int x = 0;
		while(o) {
			inst->operands[x] = (operand*) o->value;
			o = o->next;
			x++;
		}
		cleanNodes(operands);
	}
	
	return inst;
}

instruction* newInstruction(opcode op, node * mods) {
	return newInstruction(op, mods, (node*)0);
}

instruction* newInstruction(opcode op, node * mods, operand * o1) {
	instruction * inst = newInstruction(op, mods);
	inst->num_operands = 1;
	inst->operands = (operand**) malloc(1*sizeof(operand*));
	inst->operands[0] = o1;
	
	return inst;
}

instruction* newInstruction(opcode op, node * mods, operand * o1, operand * o2) {
	instruction * inst = newInstruction(op, mods);
	inst->num_operands = 2;
	inst->operands = (operand**) malloc(2*sizeof(operand*));
	inst->operands[0] = o1;
	inst->operands[1] = o2;
	
	return inst;
}

instruction* newInstruction(opcode op, node * mods, operand * o1, operand * o2, operand * o3) {
	instruction * inst = newInstruction(op, mods);
	inst->num_operands = 3;
	inst->operands = (operand**) malloc(3*sizeof(operand*));
	inst->operands[0] = o1;
	inst->operands[1] = o2;
	inst->operands[2] = o3;
	
	return inst;
}

instruction* newInstruction(opcode op, node * mods, operand * o1, operand * o2, operand * o3, operand * o4) {
	instruction * inst = newInstruction(op, mods);
	inst->num_operands = 4;
	inst->operands = (operand**) malloc(4*sizeof(operand*));
	inst->operands[0] = o1;
	inst->operands[1] = o2;
	inst->operands[2] = o3;
	inst->operands[3] = o4;
	return inst;
}

operand* newOperand(token_type type, long long val1, int val2, int val3) {
	operand * op = (operand*) malloc(sizeof(operand));
	op->type = type;
	op->val1 = val1;
	op->val2 = val2;
	op->val3 = val3;
	op->reg = -1;
	op->mods = 0;
	op->properties = (operand_prop) 0;
	op->converted = false;
	op->bad = false;
	op->decimal = false;
	
	if(type == type_register || type == type_const_mem || type == type_mem) {
		if(val1 >= 0) {	
			op->reg = val1;
		}
	}
	
	return op;
}

operand* newOperand(token_type type, operand_prop props, long long val1, int val2, int val3)  {
	operand * op = newOperand(type, val1, val2, val3);
	op->properties = props;
	return op;
}

node *addNode(node **first, void * value, long (*comparator)(void*, void*)) {
	//check if new node replaces first
	if(!*first || comparator((*first)->value,value) > 0) {
		node * newNode = (node*) malloc(sizeof(node));
		newNode->value = value;
		newNode->next = *first;
		*first = newNode;
		return newNode;
	}
	
	if(!comparator((*first)->value,value)) {
		return 0;
	}
	
	//find location to place new node
	node* iterator = *first;
	while(iterator->next && comparator(iterator->next->value,value) < 0) {
		iterator = iterator->next;
	}
	
	//if already in list, return
	if(iterator->next && !comparator(iterator->next->value,value)) {
		return 0;
	}

	//add new node to list
	node * newNode = (node*) malloc(sizeof(node));
	newNode->value = value;
	newNode->next = iterator->next;
	iterator->next = newNode;
	return newNode;
}

void addLast(node ** first, void * value) {
	//check if new node replaces first
	if(!*first) {
		node * newNode = (node*) malloc(sizeof(node));
		newNode->value = value;
		newNode->next = *first;
		*first = newNode;
		return;
	}
	
	//find last
	node* iterator = *first;
	while(iterator->next != 0) {
		iterator = iterator->next;
	}
	
	//add new node to list
	node * newNode = (node*) malloc(sizeof(node));
	newNode->value = value;
	newNode->next = iterator->next;
	iterator->next = newNode;
	return;
}

void addFirst(node ** first, void * value) {
	node * newNode = (node*) malloc(sizeof(node));
	newNode->value = value;
	newNode->next = *first;
	*first = newNode;
}

void addAfter(node **first, node * afterMe, void * value) {
	node * newNode = (node*) malloc(sizeof(node));
	newNode->value = value;
	if(afterMe != 0) {
		newNode->next = afterMe->next;
		afterMe->next = newNode;
	}
	else {
		newNode->next = *first;
		*first = newNode;
	}
}

void addBefore(node **first, node * beforeMe, void * value) {
	if(!*first || (*first) == beforeMe) {
		node * newNode = (node*) malloc(sizeof(node));
		newNode->value = value;
		newNode->next = *first;
		*first = newNode;
		return;
	}
	
	//find location to place new node
	node* iterator = *first;
	while(iterator->next != beforeMe) {
		iterator = iterator->next;
	}

	//add new node to list
	node * newNode = (node*) malloc(sizeof(node));
	newNode->value = value;
	newNode->next = iterator->next;
	iterator->next = newNode;
}

void addAtIndex(node ** first, void * value, int index) {
	if(index < 1) {
		addFirst(first, value);
		return;
	}
	
	//Find place to add to:
	node* iterator = *first;
	while(iterator->next != 0 && index > 1) {
		iterator = iterator->next;
		index--;
	}
	
	//add new node to list
	node * newNode = (node*) malloc(sizeof(node));
	newNode->value = value;
	newNode->next = iterator->next;
	iterator->next = newNode;
	return;
}

node * removeNode(node **first, void * value, long (*comparator)(void*, void*)) {
	//make sure list is nonempty
	if(!*first) {
		return 0;
	}
	
	//check if first node is deleted
	if(!comparator((*first)->value,value)) {
		node * n = *first;
		*first = (*first)->next;
		return n;
	}
	
	//find location of node to delete
	node* iterator = *first;
	while(iterator->next != 0 && comparator(iterator->next->value,value)) {
		iterator = iterator->next;
	}
	
	//if node not found, return 0
	if(iterator->next == 0) {
		return 0;
	}

	//delete node
	node * n = iterator->next;
	iterator->next = iterator->next->next;
	n->next = 0;
	return n;
}

bool deleteNode(node **first, void * value, long long (*comparator)(void*, void*)) {
	//make sure list is nonempty
	if(!*first) {
		return false;
	}
	
	//check if first node is deleted
	if(!comparator((*first)->value,value)) {
		node * n = *first;
		*first = (*first)->next;
		free(n);
		return true;
	}
	
	//find location of node to delete
	node* iterator = *first;
	while(iterator->next && comparator(iterator->next->value,value)) {
		iterator = iterator->next;
	}
	
	//if node not found, return 0
	if(!iterator->next) {
		return false;
	}

	//delete node
	node * n = iterator->next;
	iterator->next = iterator->next->next;
	free(n);
	return true;
}

bool deleteNode(node **first, void * value, long long (*comparator)(void*, void*), void (*valueCleaner)(void *)) {
	//make sure list is nonempty
	if(!*first) {
		return false;
	}
	
	//check if first node is deleted
	if(!comparator((*first)->value,value)) {
		node * n = *first;
		*first = (*first)->next;
		valueCleaner(n->value);
		free(n);
		return true;
	}
	
	//find location of node to delete
	node* iterator = *first;
	while(iterator->next && comparator(iterator->next->value,value)) {
		iterator = iterator->next;
	}
	
	//if node not found, return 0
	if(!iterator->next) {
		return false;
	}

	//delete node
	node * n = iterator->next;
	iterator->next = iterator->next->next;
	valueCleaner(n->value);
	free(n);
	return true;
}

bool deleteNode(node **first, node * del) {
	//make sure list is nonempty
	if(!*first) {
		return false;
	}
	
	//check if first node is deleted
	if(*first == del) {
		node * n = *first;
		*first = (*first)->next;
		free(n);
		return true;
	}
	
	//find location of node to delete
	node* iterator = *first;
	while(iterator->next && iterator->next != del) {
		iterator = iterator->next;
	}
	
	//if node not found, return 0
	if(!iterator->next) {
		return false;
	}

	//delete node
	node * n = iterator->next;
	iterator->next = iterator->next->next;
	free(n);
	return true;
}

long long intorder(void * a, void * b) {
	long long i = (long long) a;
	long long j = (long long) b;
	return i - j;
}

long long charstarorder(void * a, void * b) {
	char * i = (char*) a;
	char * j = (char*) b;
	return (long)strcmp(i, j);
}

long long lexicographicorder(void * a, void * b) {
	char * i = (char*) a;
	char * j = (char*) b;
	
	return strcmp(i,j);
}

int listSize(node *first) {
	int count = 0;

	while(first) {
		count++;
		first = first->next;
	}
	
	return count;
}

node * containsValue(node *first, void * val) {
	while(first && first->value != val) {
		first = first->next;
	}
	
	return first;
}

void cleanNodes(node *first) {
	node * next;
	while(first) {
		next = first->next;
		free(first);
		first = next;
	}
}

void cleanNodesFully(node *first, void (*valueCleaner)(void *)) {
	node * next;
	while(first) {
		next = first->next;
		valueCleaner(first->value);
		free(first);
		first = next;
	}
}

void cleanOperand(operand * o) {
	cleanNodesFully(o->mods,&free);
	free(o);
}

void cleanInstruction(instruction* i) {
	if(i->guard) {
		free(i->guard);
	}
	cleanNodesFully(i->mods, &free);
	if(i->num_operands > 0) {
		int x;
		for(x = 0; x < i->num_operands; x++) {
			cleanOperand(i->operands[x]);
		}
		free(i->operands);
	}
	
	free(i);
}

