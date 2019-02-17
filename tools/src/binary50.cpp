#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binary.hpp"
#include "binary50.hpp"
#include "common.hpp"
using namespace std;

///used for debugging, etcetera
bool * hextobool(char * hexstring) {
	bool * hex = (bool*) malloc(sizeof(bool) * 64);
	
	for(int x = 0; x < 16; x++) {
		int index = 63 - (4*x);
		switch(hexstring[x]) {
			case '0':
				hex[index-0] = false;
				hex[index-1] = false;
				hex[index-2] = false;
				hex[index-3] = false;
				break;
			case '1':
				hex[index-0] = false;
				hex[index-1] = false;
				hex[index-2] = false;
				hex[index-3] = true;
				break;
			case '2':
				hex[index-0] = false;
				hex[index-1] = false;
				hex[index-2] = true;
				hex[index-3] = false;
				break;
			case '3':
				hex[index-0] = false;
				hex[index-1] = false;
				hex[index-2] = true;
				hex[index-3] = true;
				break;
			case '4':
				hex[index-0] = false;
				hex[index-1] = true;
				hex[index-2] = false;
				hex[index-3] = false;
				break;
			case '5':
				hex[index-0] = false;
				hex[index-1] = true;
				hex[index-2] = false;
				hex[index-3] = true;
				break;
			case '6':
				hex[index-0] = false;
				hex[index-1] = true;
				hex[index-2] = true;
				hex[index-3] = false;
				break;
			case '7':
				hex[index-0] = false;
				hex[index-1] = true;
				hex[index-2] = true;
				hex[index-3] = true;
				break;
			case '8':
				hex[index-0] = true;
				hex[index-1] = false;
				hex[index-2] = false;
				hex[index-3] = false;
				break;
			case '9':
				hex[index-0] = true;
				hex[index-1] = false;
				hex[index-2] = false;
				hex[index-3] = true;
				break;
			case 'A':
			case 'a':
				hex[index-0] = true;
				hex[index-1] = false;
				hex[index-2] = true;
				hex[index-3] = false;
				break;
			case 'B':
			case 'b':
				hex[index-0] = true;
				hex[index-1] = false;
				hex[index-2] = true;
				hex[index-3] = true;
				break;
			case 'C':
			case 'c':
				hex[index-0] = true;
				hex[index-1] = true;
				hex[index-2] = false;
				hex[index-3] = false;
				break;
			case 'D':
			case 'd':
				hex[index-0] = true;
				hex[index-1] = true;
				hex[index-2] = false;
				hex[index-3] = true;
				break;
			case 'E':
			case 'e':
				hex[index-0] = true;
				hex[index-1] = true;
				hex[index-2] = true;
				hex[index-3] = false;
				break;
			case 'F':
			case 'f':
				hex[index-0] = true;
				hex[index-1] = true;
				hex[index-2] = true;
				hex[index-3] = true;
				break;
			default:
				cerr << "ERROR b50~120.\n";
		}
	}
	
	return hex;
}

char* instructionToHexString50(instruction * inst, int cuobjdump_version) {
	int checkrandmods = 0;
	int guardLoc = 16;
	bool binary[64];
	for(int x = 0; x < 64; x++) {
		binary[x] = 0;
	}
	bool canGuard = true;
	bool seenModType[11];
	for(int x = 0; x < 11; x++) {
		seenModType[x] = false;
	}

	if(opcode_MOV == inst->op) {
		binary[51] = true;
		binary[52] = true;
		binary[55] = true;
		binary[58] = true;
		binary[59] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_const_mem && inst->operands[2]->type == type_hex) {
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[1]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[1]->val2 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 39] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			binary[60] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 4; randsize++) {
				binary[randsize + 39] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register) {
			binary[39] = true;
			binary[40] = true;
			binary[41] = true;
			binary[42] = true;
			binary[60] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_const_mem) {
			binary[39] = true;
			binary[40] = true;
			binary[41] = true;
			binary[42] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[1]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[1]->val2 & (0x1 << randsize));
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction MOV.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction MOV.\n";
			mods = mods->next;
		}
	} else if(opcode_MOV32I == inst->op) {
		binary[56] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 32; randsize++) {
			binary[randsize + 20] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_hex && inst->operands[2]->type == type_hex) {
			for(int randsize = 0; randsize < 4; randsize++) {
				binary[randsize + 12] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_hex) {
			binary[12] = true;
			binary[13] = true;
			binary[14] = true;
			binary[15] = true;
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction MOV32I.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction MOV32I.\n";
			mods = mods->next;
		}
	} else if(opcode_LD == inst->op) {
		binary[52] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 32; randsize++) {
			binary[randsize + 20] = (inst->operands[1]->val2 & (0x1 << randsize));
		}
		if(inst->num_operands > 2) {
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 58] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
		} else {
			binary[58] = false;
			binary[59] = true;
			binary[60] = true;
			binary[61] = true;
		}
		if(true || (inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_mem && inst->operands[2]->type == type_predicate)) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "E")) {
				} else if(!strcmp((char*)mods->value, "64")) {
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "U8") && !seenModType[1]) {
					seenModType[1] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LD.\n";
				}

				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[55] = true;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction LD.\n";
		}
	} else if(opcode_LDU == inst->op) {
		cerr << "ERROR: unimplemented operation LDU.\n";
	} else if(opcode_LDL == inst->op) {
		binary[54] = true;
		binary[56] = true;
		binary[57] = true;
		binary[58] = true;
		binary[59] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 24; randsize++) {
			binary[randsize + 20] = (inst->operands[1]->val2 & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_mem) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "64")) {
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "LU")) {
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "CI")) {
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "128")) {
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "U8") && !seenModType[1]) {
					seenModType[1] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LDL.\n";
				}

				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[50] = true;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction LDL.\n";
		}
	} else if(opcode_LDS == inst->op) {
		binary[51] = true;
		binary[54] = true;
		binary[56] = true;
		binary[57] = true;
		binary[58] = true;
		binary[59] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 24; randsize++) {
			binary[randsize + 20] = (inst->operands[1]->val2 & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_mem) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "U")) {
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "32")) {
				} else if(!strcmp((char*)mods->value, "128")) {
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "S16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[44] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "U16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[44] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "64")) {
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "U8") && !seenModType[1]) {
					seenModType[1] = true;
					binary[44] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LDS.\n";
				}

				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[50] = true;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction LDS.\n";
		}
	} else if(opcode_LDC == inst->op) {
		binary[52] = true;
		binary[55] = true;
		binary[56] = true;
		binary[57] = true;
		binary[58] = true;
		binary[59] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 16; randsize++) {
			binary[randsize + 20] = (inst->operands[1]->val3 & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 5; randsize++) {
			binary[randsize + 36] = (inst->operands[1]->val2 & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_const_mem) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "U16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "S16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "64")) {
				} else if(!strcmp((char*)mods->value, "S8") && !seenModType[1]) {
					seenModType[1] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "U8") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "IL")) {
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "IS")) {
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDSIZE6")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LDC.\n";
				}

				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[36] = true;
				binary[37] = true;
				binary[50] = true;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction LDC.\n";
		}
	} else if(opcode_ST == inst->op) {
		binary[61] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 32; randsize++) {
			binary[randsize + 20] = (inst->operands[0]->val2 & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 58] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_mem && inst->operands[1]->type == type_register && inst->operands[2]->type == type_predicate) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "E")) {
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "64")) {
				} else if(!strcmp((char*)mods->value, "U8") && !seenModType[1]) {
					seenModType[1] = true;
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDSIZE7")) {
					binary[54] = true;
				} else if(!strcmp((char*)mods->value, "S8") && !seenModType[1]) {
					seenModType[1] = true;
					binary[52] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "CG")) {
					binary[56] = true;
				} else if(!strcmp((char*)mods->value, "CS")) {
					binary[57] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ST.\n";
				}

				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[55] = true;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction ST.\n";
		}
	} else if(opcode_STL == inst->op) {
		binary[52] = true;
		binary[54] = true;
		binary[56] = true;
		binary[57] = true;
		binary[58] = true;
		binary[59] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 24; randsize++) {
			binary[randsize + 20] = (inst->operands[0]->val2 & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_mem && inst->operands[1]->type == type_register) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "64")) {
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "CG")) {
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "CS")) {
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDSIZE7")) {
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "S8") && !seenModType[1]) {
					seenModType[1] = true;
					binary[48] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction STL.\n";
				}

				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[50] = true;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction STL.\n";
		}
	} else if(opcode_STS == inst->op) {
		binary[51] = true;
		binary[52] = true;
		binary[54] = true;
		binary[56] = true;
		binary[57] = true;
		binary[58] = true;
		binary[59] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 24; randsize++) {
			binary[randsize + 20] = (inst->operands[0]->val2 & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_mem && inst->operands[1]->type == type_register) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "U16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "64")) {
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "U8") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "128")) {
					binary[49] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction STS.\n";
				}

				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[50] = true;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction STS.\n";
		}
	} else if(opcode_LDLK == inst->op) {
		cerr << "ERROR: unimplemented operation LDLK.\n";
	} else if(opcode_LDSLK == inst->op) {
		cerr << "ERROR: unimplemented operation LDSLK.\n";
	} else if(opcode_STUL == inst->op) {
		cerr << "ERROR: unimplemented operation STUL.\n";
	} else if(opcode_STSUL == inst->op) {
		cerr << "ERROR: unimplemented operation STSUL.\n";
	} else if(opcode_FADD == inst->op) {
		binary[51] = true;
		binary[52] = true;
		binary[54] = true;
		binary[59] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			checkrandmods = 1;
			binary[58] = true;
			binary[62] = true;
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[46] = inst->operands[1]->properties & prop_absolute_value;
			binary[45] = inst->operands[2]->properties & prop_minus;
			binary[49] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[39] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
					binary[40] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "FTZ")) {
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[58] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FADD:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 3;
			binary[60] = true;
			binary[61] = true;
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[46] = inst->operands[1]->properties & prop_absolute_value;
			binary[56] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "FTZ")) {
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[39] = true;
					binary[56] = true;
				} else if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
					binary[40] = true;
					binary[56] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[60] = true;
				binary[61] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FADD:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FADD:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "ABS")) {
					binary[49] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FADD:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			checkrandmods = 1;
			binary[58] = true;
			binary[60] = true;
			binary[62] = true;
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[46] = inst->operands[1]->properties & prop_absolute_value;
			binary[45] = inst->operands[2]->properties & prop_minus;
			binary[49] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "FTZ")) {
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
					binary[40] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[58] = true;
				binary[60] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FADD:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FADD.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "RM")) {
			} else if(!strcmp((char*)mods->value, "RP")) {
			} else if(!strcmp((char*)mods->value, "FTZ")) {
			} else if(!strcmp((char*)mods->value, "SAT")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FADD.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_FADD32I == inst->op) {
		binary[59] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 32; randsize++) {
			binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			binary[56] = inst->operands[1]->properties & prop_minus;
			node * mods = inst->mods;
			while(mods) {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FADD32I.\n";

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FADD32I.\n";
		}
	} else if(opcode_FMUL == inst->op) {
		binary[51] = true;
		binary[53] = true;
		binary[54] = true;
		binary[59] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			checkrandmods = 1;
			binary[58] = true;
			binary[62] = true;
			binary[48] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "D2")) {
					binary[41] = true;
				} else if(!strcmp((char*)mods->value, "D4")) {
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "M8")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "FTZ")) {
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "FMZ")) {
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[58] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMUL:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 3;
			binary[60] = true;
			binary[61] = true;
			binary[56] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "FTZ")) {
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "D2")) {
					binary[41] = true;
				} else if(!strcmp((char*)mods->value, "D4")) {
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "M8")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "FMZ")) {
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[60] = true;
				binary[61] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMUL:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMUL:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[48] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMUL:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			checkrandmods = 1;
			binary[58] = true;
			binary[60] = true;
			binary[62] = true;
			binary[48] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "FTZ")) {
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "D2")) {
					binary[41] = true;
				} else if(!strcmp((char*)mods->value, "D4")) {
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "M8")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "FMZ")) {
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[58] = true;
				binary[60] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMUL:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FMUL.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "RM")) {
			} else if(!strcmp((char*)mods->value, "RP")) {
			} else if(!strcmp((char*)mods->value, "D2")) {
			} else if(!strcmp((char*)mods->value, "D4")) {
			} else if(!strcmp((char*)mods->value, "M8")) {
			} else if(!strcmp((char*)mods->value, "FTZ")) {
			} else if(!strcmp((char*)mods->value, "FMZ")) {
			} else if(!strcmp((char*)mods->value, "SAT")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMUL.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_FMUL32I == inst->op) {
		binary[57] = true;
		binary[58] = true;
		binary[59] = true;
		binary[60] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 32; randsize++) {
			binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			node * mods = inst->mods;
			while(mods) {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMUL32I.\n";

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FMUL32I.\n";
		}
	} else if(opcode_FFMA == inst->op) {
		binary[55] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_register) {
			checkrandmods = 3;
			binary[57] = true;
			binary[60] = true;
			binary[61] = true;
			binary[56] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[49] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "RZ") && !seenModType[4]) {
					seenModType[4] = true;
				} else if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
				} else if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
				} else if(!strcmp((char*)mods->value, "FTZ")) {
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[57] = true;
				binary[60] = true;
				binary[61] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FFMA:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FFMA:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[48] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FFMA:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			binary[56] = true;
			binary[59] = true;
			binary[60] = true;
			binary[62] = true;
			binary[48] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[49] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "RZ") && !seenModType[4]) {
					seenModType[4] = true;
					binary[51] = true;
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "FTZ")) {
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[56] = true;
				binary[59] = true;
				binary[60] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FFMA:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_const_mem) {
			checkrandmods = 1;
			binary[56] = true;
			binary[60] = true;
			binary[62] = true;
			binary[48] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[49] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[3]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[3]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[56] = true;
				binary[60] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FFMA:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			binary[56] = true;
			binary[59] = true;
			binary[62] = true;
			binary[48] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			binary[49] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[56] = true;
				binary[59] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FFMA:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FFMA.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "SAT")) {
			} else if(!strcmp((char*)mods->value, "RZ")) {
			} else if(!strcmp((char*)mods->value, "RP")) {
			} else if(!strcmp((char*)mods->value, "RM")) {
			} else if(!strcmp((char*)mods->value, "FTZ")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FFMA.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_FSET == inst->op) {
		binary[60] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[59] = true;
			binary[62] = true;
			binary[43] = inst->operands[1]->properties & prop_minus;
			binary[44] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[42] = inst->operands[3]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "FTZ")) {
					binary[55] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[47] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[45] = true;
					binary[48] = true;
					binary[50] = true;
					binary[51] = true;
					binary[55] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[46] = true;
					binary[48] = true;
					binary[50] = true;
					binary[51] = true;
					binary[55] = true;
				} else if(!strcmp((char*)mods->value, "GTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
					binary[51] = true;
					binary[55] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
					binary[50] = true;
					binary[51] = true;
					binary[55] = true;
				} else if(!strcmp((char*)mods->value, "LTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[51] = true;
					binary[55] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[55] = true;
				} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "GEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "NAN") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[55] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FSET:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_predicate) {
			checkrandmods = 3;
			binary[61] = true;
			binary[43] = inst->operands[1]->properties & prop_minus;
			binary[54] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[42] = inst->operands[3]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
					binary[51] = true;
					binary[54] = true;
				} else if(!strcmp((char*)mods->value, "FTZ")) {
					binary[55] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[55] = true;
				} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "GEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[45] = true;
					binary[50] = true;
					binary[51] = true;
					binary[54] = true;
					binary[55] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[46] = true;
					binary[50] = true;
					binary[51] = true;
					binary[54] = true;
					binary[55] = true;
				} else if(!strcmp((char*)mods->value, "NEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[50] = true;
					binary[51] = true;
					binary[54] = true;
					binary[55] = true;
				} else if(!strcmp((char*)mods->value, "NAN") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[54] = true;
					binary[55] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FSET:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FSET:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "ABS")) {
					binary[44] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FSET:2.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FSET.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "NEU")) {
			} else if(!strcmp((char*)mods->value, "FTZ")) {
			} else if(!strcmp((char*)mods->value, "AND")) {
			} else if(!strcmp((char*)mods->value, "GT")) {
			} else if(!strcmp((char*)mods->value, "OR")) {
			} else if(!strcmp((char*)mods->value, "XOR")) {
			} else if(!strcmp((char*)mods->value, "GTU")) {
			} else if(!strcmp((char*)mods->value, "T")) {
			} else if(!strcmp((char*)mods->value, "LTU")) {
			} else if(!strcmp((char*)mods->value, "EQ")) {
			} else if(!strcmp((char*)mods->value, "LT")) {
			} else if(!strcmp((char*)mods->value, "GEU")) {
			} else if(!strcmp((char*)mods->value, "NAN")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FSET.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_FSETP == inst->op) {
		binary[52] = true;
		binary[53] = true;
		binary[55] = true;
		binary[57] = true;
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 3] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 39] = ((inst->operands[4]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_register && inst->operands[3]->type == type_const_mem && inst->operands[4]->type == type_predicate) {
			binary[56] = true;
			binary[59] = true;
			binary[62] = true;
			binary[43] = inst->operands[2]->properties & prop_minus;
			binary[7] = inst->operands[2]->properties & prop_absolute_value;
			binary[6] = inst->operands[3]->properties & prop_minus;
			binary[44] = inst->operands[3]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[3]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[3]->val2 & (0x1 << randsize));
			}
			binary[42] = inst->operands[4]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[7] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "GEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "NEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "FTZ")) {
					binary[47] = true;
				} else if(!strcmp((char*)mods->value, "LTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[7] = true;
					binary[48] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[45] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[46] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "NE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "GE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "F") && !seenModType[3]) {
					seenModType[3] = true;
				} else if(!strcmp((char*)mods->value, "LEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "GTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDBOP3")) {
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[45] = true;
					binary[47] = true;
					binary[48] = true;
					binary[49] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "NAN") && !seenModType[3]) {
					seenModType[3] = true;
					binary[7] = true;
					binary[47] = true;
					binary[51] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register && inst->operands[4]->type == type_predicate) {
			binary[56] = true;
			binary[59] = true;
			binary[60] = true;
			binary[62] = true;
			binary[43] = inst->operands[2]->properties & prop_minus;
			binary[7] = inst->operands[2]->properties & prop_absolute_value;
			binary[6] = inst->operands[3]->properties & prop_minus;
			binary[44] = inst->operands[3]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			binary[42] = inst->operands[4]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "FTZ")) {
					binary[47] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "GEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "LEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "LTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "GTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "GE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDBOP3")) {
					binary[46] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[45] = true;
					binary[47] = true;
					binary[48] = true;
					binary[49] = true;
					binary[50] = true;
					binary[51] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[2]) {
				binary[0] = true;
				binary[1] = true;
				binary[2] = true;
				binary[8] = true;
				binary[10] = true;
				binary[20] = true;
				binary[21] = true;
				binary[22] = true;
				binary[23] = true;
				binary[45] = true;
				binary[46] = true;
				binary[47] = true;
				binary[48] = true;
				binary[50] = true;
				binary[51] = true;
				binary[56] = true;
				binary[59] = true;
				binary[60] = true;
				binary[62] = true;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_register && inst->operands[3]->type == type_hex && inst->operands[4]->type == type_predicate) {
			checkrandmods = 4;
			binary[58] = true;
			binary[60] = true;
			binary[61] = true;
			binary[43] = inst->operands[2]->properties & prop_minus;
			binary[7] = inst->operands[2]->properties & prop_absolute_value;
			binary[56] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[3]->val1 ^ ((inst->operands[3]->properties & prop_minus ? 1 : 0) << 19)) & (0x1 << randsize));
			}
			binary[42] = inst->operands[4]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "GEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[7] = true;
					binary[49] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "FTZ")) {
					binary[47] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "GTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "LEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[7] = true;
					binary[48] = true;
					binary[49] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "LTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "NEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[7] = true;
					binary[46] = true;
					binary[47] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "NAN") && !seenModType[3]) {
					seenModType[3] = true;
					binary[7] = true;
					binary[47] = true;
					binary[51] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FSETP:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FSETP:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FSETP:2.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[3]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[6] = true;
				} else if(!strcmp((char*)mods->value, "ABS")) {
					binary[44] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FSETP:3.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FSETP.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "LT")) {
			} else if(!strcmp((char*)mods->value, "AND")) {
			} else if(!strcmp((char*)mods->value, "GEU")) {
			} else if(!strcmp((char*)mods->value, "GT")) {
			} else if(!strcmp((char*)mods->value, "NEU")) {
			} else if(!strcmp((char*)mods->value, "FTZ")) {
			} else if(!strcmp((char*)mods->value, "LTU")) {
			} else if(!strcmp((char*)mods->value, "EQ")) {
			} else if(!strcmp((char*)mods->value, "OR")) {
			} else if(!strcmp((char*)mods->value, "XOR")) {
			} else if(!strcmp((char*)mods->value, "NE")) {
			} else if(!strcmp((char*)mods->value, "GE")) {
			} else if(!strcmp((char*)mods->value, "F")) {
			} else if(!strcmp((char*)mods->value, "LEU")) {
			} else if(!strcmp((char*)mods->value, "GTU")) {
			} else if(!strcmp((char*)mods->value, "INVALIDBOP3")) {
			} else if(!strcmp((char*)mods->value, "T")) {
			} else if(!strcmp((char*)mods->value, "LE")) {
			} else if(!strcmp((char*)mods->value, "NAN")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FSETP.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_DSETP == inst->op) {
		binary[55] = true;
		binary[57] = true;
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 3] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 39] = ((inst->operands[4]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_register && inst->operands[3]->type == type_hex && inst->operands[4]->type == type_predicate) {
			checkrandmods = 4;
			binary[58] = true;
			binary[60] = true;
			binary[61] = true;
			binary[43] = inst->operands[2]->properties & prop_minus;
			binary[7] = inst->operands[2]->properties & prop_absolute_value;
			binary[56] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			binary[42] = inst->operands[4]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "GTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[45] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[7] = true;
					binary[46] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[7] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "NUM") && !seenModType[3]) {
					seenModType[3] = true;
					binary[7] = true;
					binary[48] = true;
					binary[49] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "LTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "NEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "GEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "NAN") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "GE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "LEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "NE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[45] = true;
					binary[48] = true;
					binary[50] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DSETP:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DSETP:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DSETP:2.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[3]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[6] = true;
				} else if(!strcmp((char*)mods->value, "ABS")) {
					binary[44] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DSETP:3.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_register && inst->operands[3]->type == type_const_mem && inst->operands[4]->type == type_predicate) {
			binary[56] = true;
			binary[59] = true;
			binary[62] = true;
			binary[43] = inst->operands[2]->properties & prop_minus;
			binary[7] = inst->operands[2]->properties & prop_absolute_value;
			binary[6] = inst->operands[3]->properties & prop_minus;
			binary[44] = inst->operands[3]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[3]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[3]->val2 & (0x1 << randsize));
			}
			binary[42] = inst->operands[4]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "LTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "NEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[45] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[46] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "GEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "NAN") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register && inst->operands[4]->type == type_predicate) {
			binary[56] = true;
			binary[59] = true;
			binary[60] = true;
			binary[62] = true;
			binary[43] = inst->operands[2]->properties & prop_minus;
			binary[7] = inst->operands[2]->properties & prop_absolute_value;
			binary[6] = inst->operands[3]->properties & prop_minus;
			binary[44] = inst->operands[3]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			binary[42] = inst->operands[4]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NUM") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "NAN") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "LTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "GTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "GE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "GEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "NEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "LEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "NE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[45] = true;
					binary[48] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[45] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[46] = true;
					binary[48] = true;
					binary[51] = true;
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction DSETP.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "LE")) {
			} else if(!strcmp((char*)mods->value, "AND")) {
			} else if(!strcmp((char*)mods->value, "EQ")) {
			} else if(!strcmp((char*)mods->value, "GTU")) {
			} else if(!strcmp((char*)mods->value, "OR")) {
			} else if(!strcmp((char*)mods->value, "GT")) {
			} else if(!strcmp((char*)mods->value, "XOR")) {
			} else if(!strcmp((char*)mods->value, "LT")) {
			} else if(!strcmp((char*)mods->value, "NUM")) {
			} else if(!strcmp((char*)mods->value, "LTU")) {
			} else if(!strcmp((char*)mods->value, "NEU")) {
			} else if(!strcmp((char*)mods->value, "GEU")) {
			} else if(!strcmp((char*)mods->value, "NAN")) {
			} else if(!strcmp((char*)mods->value, "GE")) {
			} else if(!strcmp((char*)mods->value, "LEU")) {
			} else if(!strcmp((char*)mods->value, "NE")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DSETP.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_FCMP == inst->op) {
		binary[53] = true;
		binary[55] = true;
		binary[57] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_register) {
			binary[51] = true;
			binary[58] = true;
			binary[60] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "FTZ")) {
					binary[47] = true;
				} else if(!strcmp((char*)mods->value, "NAN") && !seenModType[3]) {
					seenModType[3] = true;
				} else if(!strcmp((char*)mods->value, "LEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "NEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "GTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "GEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "EQU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "F") && !seenModType[3]) {
					seenModType[3] = true;
				} else if(!strcmp((char*)mods->value, "NE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[50] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_register) {
			binary[51] = true;
			binary[56] = true;
			binary[59] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "GTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "GEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "FTZ")) {
					binary[47] = true;
				} else if(!strcmp((char*)mods->value, "EQU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "LTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
					binary[50] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register) {
			binary[56] = true;
			binary[59] = true;
			binary[60] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "FTZ")) {
					binary[47] = true;
				} else if(!strcmp((char*)mods->value, "F") && !seenModType[3]) {
					seenModType[3] = true;
				} else if(!strcmp((char*)mods->value, "NE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[50] = true;
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FCMP.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "LTU")) {
			} else if(!strcmp((char*)mods->value, "FTZ")) {
			} else if(!strcmp((char*)mods->value, "NAN")) {
			} else if(!strcmp((char*)mods->value, "LEU")) {
			} else if(!strcmp((char*)mods->value, "NEU")) {
			} else if(!strcmp((char*)mods->value, "GTU")) {
			} else if(!strcmp((char*)mods->value, "GEU")) {
			} else if(!strcmp((char*)mods->value, "EQU")) {
			} else if(!strcmp((char*)mods->value, "T")) {
			} else if(!strcmp((char*)mods->value, "LE")) {
			} else if(!strcmp((char*)mods->value, "LT")) {
			} else if(!strcmp((char*)mods->value, "F")) {
			} else if(!strcmp((char*)mods->value, "NE")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FCMP.\n";
			}
			mods = mods->next;
		}
		//TODO: make sure this is correct:
		if(!seenModType[3]) {
			binary[50] = true;
		}
	} else if(opcode_MUFU == inst->op) {
		binary[55] = true;
		binary[60] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register) {
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[46] = inst->operands[1]->properties & prop_absolute_value;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "EX2") && !seenModType[8]) {
					seenModType[8] = true;
					binary[21] = true;
				} else if(!strcmp((char*)mods->value, "RCP") && !seenModType[8]) {
					seenModType[8] = true;
					binary[22] = true;
				} else if(!strcmp((char*)mods->value, "RSQ") && !seenModType[8]) {
					seenModType[8] = true;
					binary[20] = true;
					binary[22] = true;
				} else if(!strcmp((char*)mods->value, "RCP64H") && !seenModType[8]) {
					seenModType[8] = true;
					binary[21] = true;
					binary[22] = true;
				} else if(!strcmp((char*)mods->value, "RSQ64H") && !seenModType[8]) {
					seenModType[8] = true;
					binary[20] = true;
					binary[21] = true;
					binary[22] = true;
				} else if(!strcmp((char*)mods->value, "LG2") && !seenModType[8]) {
					seenModType[8] = true;
					binary[20] = true;
					binary[21] = true;
				} else if(!strcmp((char*)mods->value, "SIN") && !seenModType[8]) {
					seenModType[8] = true;
					binary[20] = true;
				} else if(!strcmp((char*)mods->value, "COS") && !seenModType[8]) {
					seenModType[8] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDMUFUOPCODEC")) {
					binary[23] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction MUFU.\n";
				}

				mods = mods->next;
			}
			if(!seenModType[8]) {
				binary[4] = true;
				binary[8] = true;
				binary[10] = true;
				binary[22] = true;
				binary[23] = true;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction MUFU.\n";
		}
	} else if(opcode_DADD == inst->op) {
		binary[52] = true;
		binary[53] = true;
		binary[54] = true;
		binary[59] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			checkrandmods = 1;
			binary[58] = true;
			binary[62] = true;
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[46] = inst->operands[1]->properties & prop_absolute_value;
			binary[45] = inst->operands[2]->properties & prop_minus;
			binary[49] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
					binary[40] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[58] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DADD:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 3;
			binary[60] = true;
			binary[61] = true;
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[46] = inst->operands[1]->properties & prop_absolute_value;
			binary[56] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
					binary[40] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[60] = true;
				binary[61] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DADD:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DADD:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "ABS")) {
					binary[49] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DADD:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			checkrandmods = 1;
			binary[58] = true;
			binary[60] = true;
			binary[62] = true;
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[46] = inst->operands[1]->properties & prop_absolute_value;
			binary[45] = inst->operands[2]->properties & prop_minus;
			binary[49] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[39] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[58] = true;
				binary[60] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DADD:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction DADD.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "RM")) {
			} else if(!strcmp((char*)mods->value, "RP")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DADD.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_DMUL == inst->op) {
		binary[55] = true;
		binary[59] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			checkrandmods = 1;
			binary[58] = true;
			binary[60] = true;
			binary[62] = true;
			binary[48] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
					binary[40] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[58] = true;
				binary[60] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DMUL:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 3;
			binary[60] = true;
			binary[61] = true;
			binary[56] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[39] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[60] = true;
				binary[61] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DMUL:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DMUL:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[48] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DMUL:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			checkrandmods = 1;
			binary[58] = true;
			binary[62] = true;
			binary[48] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
					binary[40] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[58] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DMUL:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction DMUL.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "RM")) {
			} else if(!strcmp((char*)mods->value, "RP")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DMUL.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_DFMA == inst->op) {
		binary[52] = true;
		binary[53] = true;
		binary[54] = true;
		binary[57] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_register) {
			checkrandmods = 3;
			binary[58] = true;
			binary[60] = true;
			binary[61] = true;
			binary[56] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[49] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "RZ") && !seenModType[4]) {
					seenModType[4] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[58] = true;
				binary[60] = true;
				binary[61] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DFMA:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DFMA:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[48] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DFMA:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_const_mem) {
			checkrandmods = 1;
			binary[56] = true;
			binary[60] = true;
			binary[62] = true;
			binary[48] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[49] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[3]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[3]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[48] = true;
					binary[50] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[56] = true;
				binary[60] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DFMA:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			binary[56] = true;
			binary[59] = true;
			binary[62] = true;
			binary[48] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			binary[49] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[50] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[56] = true;
				binary[59] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DFMA:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			binary[56] = true;
			binary[59] = true;
			binary[60] = true;
			binary[62] = true;
			binary[48] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[49] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "RZ") && !seenModType[4]) {
					seenModType[4] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
					binary[51] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[56] = true;
				binary[59] = true;
				binary[60] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DFMA:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction DFMA.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "RM")) {
			} else if(!strcmp((char*)mods->value, "RZ")) {
			} else if(!strcmp((char*)mods->value, "RP")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DFMA.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_IADD == inst->op) {
		binary[52] = true;
		binary[59] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 3;
			binary[60] = true;
			binary[61] = true;
			binary[49] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "X")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[48] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			checkrandmods = 1;
			binary[58] = true;
			binary[60] = true;
			binary[62] = true;
			binary[49] = inst->operands[1]->properties & prop_minus;
			binary[48] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "X")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			checkrandmods = 1;
			binary[58] = true;
			binary[62] = true;
			binary[49] = inst->operands[1]->properties & prop_minus;
			binary[48] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "X")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction IADD.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "X")) {
			} else if(!strcmp((char*)mods->value, "SAT")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_IADD32I == inst->op) {
		binary[58] = true;
		binary[59] = true;
		binary[60] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 32; randsize++) {
			binary[randsize + 20] = ((inst->operands[2]->val1 * (inst->operands[2]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 1;
			binary[56] = inst->operands[1]->properties & prop_minus;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "X")) {
					binary[53] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD32I.\n";
				}

				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[52] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD32I:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction IADD32I.\n";
		}
	} else if(opcode_IMNMX == inst->op) {
		binary[53] = true;
		binary[59] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[60] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[42] = inst->operands[3]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "XLO") && !seenModType[10]) {
					seenModType[10] = true;
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "XMED") && !seenModType[10]) {
					seenModType[10] = true;
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "XHI") && !seenModType[10]) {
					seenModType[10] = true;
					binary[43] = true;
					binary[44] = true;
					binary[47] = true;
					binary[48] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[39] = true;
				binary[40] = true;
				binary[41] = true;
				binary[48] = true;
				binary[60] = true;
				binary[61] = true;
			}
			if(!seenModType[10]) {
				binary[60] = true;
				binary[61] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IMNMX:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[58] = true;
			binary[60] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[42] = inst->operands[3]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "XLO") && !seenModType[10]) {
					seenModType[10] = true;
					binary[42] = true;
					binary[43] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "XMED") && !seenModType[10]) {
					seenModType[10] = true;
					binary[42] = true;
					binary[44] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[42] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[48] = true;
				binary[58] = true;
				binary[60] = true;
				binary[62] = true;
			}
			if(!seenModType[10]) {
				binary[58] = true;
				binary[60] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IMNMX:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[58] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			binary[42] = inst->operands[3]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "XHI") && !seenModType[10]) {
					seenModType[10] = true;
					binary[43] = true;
					binary[44] = true;
					binary[47] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "XLO") && !seenModType[10]) {
					seenModType[10] = true;
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "XMED") && !seenModType[10]) {
					seenModType[10] = true;
					binary[44] = true;
					binary[48] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[48] = true;
				binary[58] = true;
				binary[62] = true;
			}
			if(!seenModType[10]) {
				binary[58] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IMNMX:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction IMNMX.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "U32")) {
			} else if(!strcmp((char*)mods->value, "XLO")) {
			} else if(!strcmp((char*)mods->value, "XMED")) {
			} else if(!strcmp((char*)mods->value, "XHI")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IMNMX.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_IMUL == inst->op) {
		cerr << "ERROR: unimplemented operation IMUL.\n";
	} else if(opcode_IMUL32I == inst->op) {
		cerr << "ERROR: unimplemented operation IMUL32I.\n";
	} else if(opcode_IMAD == inst->op) {
		cerr << "ERROR: unimplemented operation IMAD.\n";
	} else if(opcode_ISCADD == inst->op) {
		binary[51] = true;
		binary[52] = true;
		binary[59] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 5; randsize++) {
			binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_hex) {
			checkrandmods = 1;
			binary[58] = true;
			binary[62] = true;
			binary[49] = inst->operands[1]->properties & prop_minus;
			binary[48] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "PO")) {
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISCADD:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_hex) {
			checkrandmods = 3;
			binary[60] = true;
			binary[61] = true;
			binary[49] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISCADD:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISCADD:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[48] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISCADD:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_hex) {
			checkrandmods = 1;
			binary[58] = true;
			binary[60] = true;
			binary[62] = true;
			binary[49] = inst->operands[1]->properties & prop_minus;
			binary[48] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "PO")) {
					binary[48] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISCADD:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction ISCADD.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "PO")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISCADD.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_ISET == inst->op) {
		binary[52] = true;
		binary[54] = true;
		binary[57] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[56] = true;
			binary[59] = true;
			binary[60] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[42] = inst->operands[3]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "NE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "BF")) {
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[45] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[46] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "F") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "GE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[50] = true;
					binary[51] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[48] = true;
				binary[56] = true;
				binary[59] = true;
				binary[60] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISET:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[58] = true;
			binary[60] = true;
			binary[61] = true;
			binary[56] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1 * (inst->operands[2]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
			binary[42] = inst->operands[3]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[48] = false;
					binary[56] = false;//not sure about this
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[51] = true;
					binary[56] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "BF")) {
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[45] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[46] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "F") && !seenModType[3]) {
					seenModType[3] = true;
				} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[50] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[2] = true;
				binary[48] = true;
				binary[58] = true;
				binary[60] = true;
				binary[61] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISET:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[56] = true;
			binary[59] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			binary[42] = inst->operands[3]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "NE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[48] = false;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "BF")) {
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[45] = true;
					binary[48] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[46] = true;
					binary[48] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "GE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[50] = true;
					binary[51] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[48] = true;
				binary[56] = true;
				binary[59] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISET:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction ISET.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "EQ")) {
			} else if(!strcmp((char*)mods->value, "AND")) {
			} else if(!strcmp((char*)mods->value, "NE")) {
			} else if(!strcmp((char*)mods->value, "U32")) {
			} else if(!strcmp((char*)mods->value, "LT")) {
			} else if(!strcmp((char*)mods->value, "GT")) {
			} else if(!strcmp((char*)mods->value, "X")) {
			} else if(!strcmp((char*)mods->value, "BF")) {
			} else if(!strcmp((char*)mods->value, "OR")) {
			} else if(!strcmp((char*)mods->value, "XOR")) {
			} else if(!strcmp((char*)mods->value, "F")) {
			} else if(!strcmp((char*)mods->value, "LE")) {
			} else if(!strcmp((char*)mods->value, "GE")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISET.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_ISETP == inst->op) {
		binary[53] = true;
		binary[54] = true;
		binary[57] = true;
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 3] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 39] = ((inst->operands[4]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_register && inst->operands[3]->type == type_hex && inst->operands[4]->type == type_predicate) {
			binary[58] = true;
			binary[60] = true;
			binary[61] = true;
			binary[56] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[3]->val1 * (inst->operands[3]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
			binary[42] = inst->operands[4]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[45] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "NE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "GE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[46] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[50] = true;
					binary[51] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[0] = true;
				binary[1] = true;
				binary[2] = true;
				binary[48] = true;
				binary[58] = true;
				binary[60] = true;
				binary[61] = true;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_register && inst->operands[3]->type == type_const_mem && inst->operands[4]->type == type_predicate) {
			binary[56] = true;
			binary[59] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[3]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[3]->val2 & (0x1 << randsize));
			}
			binary[42] = inst->operands[4]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "NE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[46] = true;
					binary[48] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
					binary[50] = true;
					binary[51] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[48] = true;
				binary[56] = true;
				binary[59] = true;
				binary[62] = true;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register && inst->operands[4]->type == type_predicate) {
			binary[56] = true;
			binary[59] = true;
			binary[60] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			binary[42] = inst->operands[4]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "GE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[46] = true;
					binary[48] = true;
					binary[49] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
					binary[50] = true;
					binary[51] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[48] = true;
				binary[56] = true;
				binary[59] = true;
				binary[60] = true;
				binary[62] = true;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction ISETP.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "LT")) {
			} else if(!strcmp((char*)mods->value, "AND")) {
			} else if(!strcmp((char*)mods->value, "GT")) {
			} else if(!strcmp((char*)mods->value, "U32")) {
			} else if(!strcmp((char*)mods->value, "OR")) {
			} else if(!strcmp((char*)mods->value, "NE")) {
			} else if(!strcmp((char*)mods->value, "EQ")) {
			} else if(!strcmp((char*)mods->value, "LE")) {
			} else if(!strcmp((char*)mods->value, "GE")) {
			} else if(!strcmp((char*)mods->value, "X")) {
			} else if(!strcmp((char*)mods->value, "XOR")) {
			} else if(!strcmp((char*)mods->value, "T")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISETP.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_ICMP == inst->op) {
		binary[54] = true;
		binary[57] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_register) {
			binary[56] = true;
			binary[59] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[49] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "F") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[48] = true;
				binary[56] = true;
				binary[59] = true;
				binary[62] = true;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_register) {
			binary[58] = true;
			binary[60] = true;
			binary[61] = true;
			binary[56] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1 * (inst->operands[2]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[51] = true;
					binary[56] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
					binary[50] = true;
					binary[51] = true;
					binary[56] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[48] = true;
				binary[58] = true;
				binary[60] = true;
				binary[61] = true;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register) {
			binary[56] = true;
			binary[59] = true;
			binary[60] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
					binary[49] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "F") && !seenModType[3]) {
					seenModType[3] = true;
					binary[48] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[48] = true;
				binary[56] = true;
				binary[59] = true;
				binary[60] = true;
				binary[62] = true;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction ICMP.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "NE")) {
			} else if(!strcmp((char*)mods->value, "EQ")) {
			} else if(!strcmp((char*)mods->value, "LT")) {
			} else if(!strcmp((char*)mods->value, "U32")) {
			} else if(!strcmp((char*)mods->value, "GT")) {
			} else if(!strcmp((char*)mods->value, "T")) {
			} else if(!strcmp((char*)mods->value, "LE")) {
			} else if(!strcmp((char*)mods->value, "F")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ICMP.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_I2F == inst->op) {
		binary[51] = true;
		binary[52] = true;
		binary[53] = true;
		binary[55] = true;
		binary[58] = true;
		binary[59] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_const_mem) {
			checkrandmods = 2;
			binary[45] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[1]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[1]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "F32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "S32") && seenModType[1]) {
					binary[11] = true;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "U32") && seenModType[1]) {
					binary[9] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "F64") && !seenModType[1]) {
					seenModType[1] = true;
					binary[8] = true;
					binary[9] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDFPSRC0") && !seenModType[1]) {
					seenModType[1] = true;
					binary[11] = true;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "S64") && seenModType[1]) {
					binary[9] = true;
					binary[10] = true;
					binary[11] = true;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "S8") && seenModType[1]) {
					binary[9] = true;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[9] = true;
					binary[11] = true;
					binary[13] = true;
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
					binary[9] = true;
					binary[11] = true;
					binary[13] = true;
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "S16") && seenModType[1]) {
					binary[9] = true;
					binary[10] = true;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "U8") && seenModType[1]) {
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "U64") && seenModType[1]) {
					binary[9] = true;
					binary[10] = true;
					binary[11] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2F:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "B1")) {
					binary[41] = true;
				} else if(!strcmp((char*)mods->value, "B2")) {
					binary[42] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2F:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register) {
			checkrandmods = 2;
			binary[60] = true;
			binary[45] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "F32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "U32") && seenModType[1]) {
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
					binary[9] = true;
					binary[11] = true;
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "S32") && seenModType[1]) {
					binary[9] = true;
					binary[11] = true;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "S16") && seenModType[1]) {
					binary[9] = true;
					binary[10] = true;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "U8") && seenModType[1]) {
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "S8") && seenModType[1]) {
					binary[9] = true;
					binary[13] = true;
					binary[41] = true;
				} else if(!strcmp((char*)mods->value, "F64") && !seenModType[1]) {
					seenModType[1] = true;
					binary[8] = true;
					binary[9] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "U64") && seenModType[1]) {
					binary[9] = true;
					binary[10] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDFPSRC0") && !seenModType[1]) {
					seenModType[1] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[9] = true;
					binary[11] = true;
					binary[39] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[60] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2F:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "B1")) {
					binary[41] = true;
				} else if(!strcmp((char*)mods->value, "B2")) {
					binary[42] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2F:1.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction I2F.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "F32")) {
			} else if(!strcmp((char*)mods->value, "S32")) {
			} else if(!strcmp((char*)mods->value, "U32")) {
			} else if(!strcmp((char*)mods->value, "F64")) {
			} else if(!strcmp((char*)mods->value, "INVALIDFPSRC0")) {
			} else if(!strcmp((char*)mods->value, "S64")) {
			} else if(!strcmp((char*)mods->value, "S8")) {
			} else if(!strcmp((char*)mods->value, "RM")) {
			} else if(!strcmp((char*)mods->value, "RP")) {
			} else if(!strcmp((char*)mods->value, "S16")) {
			} else if(!strcmp((char*)mods->value, "U8")) {
			} else if(!strcmp((char*)mods->value, "U64")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2F.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_I2I == inst->op) {
		binary[53] = true;
		binary[54] = true;
		binary[55] = true;
		binary[59] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_hex) {
			checkrandmods = 2;
			binary[56] = true;
			binary[60] = true;
			binary[61] = true;
			binary[51] = binary[52] = binary[57] = binary[58] = binary[62] = binary[63] = !(inst->operands[1]->properties & prop_minus);
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[1]->val1 * (inst->operands[1]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "S32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[9] = true;
					binary[12] = true;
				} else if(!strcmp((char*)mods->value, "S32") && seenModType[1]) {
					binary[11] = true;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "???7")) {
					binary[8] = true;
				} else if(!strcmp((char*)mods->value, "S8") && !seenModType[1]) {
					seenModType[1] = true;
					binary[11] = true;
					binary[12] = true;
					binary[13] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "INVALID7") && seenModType[1]) {
					binary[9] = true;
					binary[10] = true;
					binary[11] = true;
					binary[12] = true;
					binary[13] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "S8") && seenModType[1]) {
					binary[9] = true;
					binary[12] = true;
					binary[13] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[9] = true;
					binary[11] = true;
					binary[13] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "U32") && seenModType[1]) {
					binary[9] = true;
					binary[11] = true;
					binary[12] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "S16") && seenModType[1]) {
					binary[9] = true;
					binary[10] = true;
					binary[12] = true;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "U16") && seenModType[1]) {
					binary[9] = true;
					binary[10] = true;
				} else if(!strcmp((char*)mods->value, "S16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[8] = true;
					binary[12] = true;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "U16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[8] = true;
				} else if(!strcmp((char*)mods->value, "U8") && seenModType[1]) {
					binary[8] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2I:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "B1")) {
					binary[41] = true;
				} else if(!strcmp((char*)mods->value, "B2")) {
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "ABS")) {
					binary[49] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2I:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_const_mem) {
			checkrandmods = 2;
			binary[58] = true;
			binary[62] = true;
			binary[45] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[1]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[1]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "S32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[9] = true;
					binary[12] = true;
				} else if(!strcmp((char*)mods->value, "S32") && seenModType[1]) {
					binary[11] = true;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "???7")) {
					binary[8] = true;
				} else if(!strcmp((char*)mods->value, "S8") && !seenModType[1]) {
					seenModType[1] = true;
					binary[11] = true;
					binary[12] = true;
					binary[13] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "INVALID7") && seenModType[1]) {
					binary[9] = true;
					binary[10] = true;
					binary[11] = true;
					binary[12] = true;
					binary[13] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "S8") && seenModType[1]) {
					binary[9] = true;
					binary[12] = true;
					binary[13] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[9] = true;
					binary[11] = true;
					binary[13] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "U32") && seenModType[1]) {
					binary[9] = true;
					binary[11] = true;
					binary[12] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2I:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "B1")) {
					binary[41] = true;
				} else if(!strcmp((char*)mods->value, "B2")) {
					binary[42] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2I:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register) {
			checkrandmods = 2;
			binary[58] = true;
			binary[60] = true;
			binary[62] = true;
			binary[45] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "S32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[9] = true;
					binary[12] = true;
				} else if(!strcmp((char*)mods->value, "S32") && seenModType[1]) {
					binary[11] = true;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "S16") && seenModType[1]) {
					binary[9] = true;
					binary[10] = true;
					binary[12] = true;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "U16") && seenModType[1]) {
					binary[9] = true;
					binary[10] = true;
				} else if(!strcmp((char*)mods->value, "S8") && seenModType[1]) {
					binary[12] = true;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "S16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[8] = true;
					binary[12] = true;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "U16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[8] = true;
				} else if(!strcmp((char*)mods->value, "U8") && seenModType[1]) {
					binary[8] = true;
				} else if(!strcmp((char*)mods->value, "???7")) {
					binary[8] = true;
				} else if(!strcmp((char*)mods->value, "S8") && !seenModType[1]) {
					seenModType[1] = true;
					binary[11] = true;
					binary[12] = true;
					binary[13] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "INVALID7") && seenModType[1]) {
					binary[9] = true;
					binary[10] = true;
					binary[11] = true;
					binary[12] = true;
					binary[13] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "U32") && seenModType[1]) {
					binary[9] = true;
					binary[11] = true;
					binary[12] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2I:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "B1")) {
					binary[41] = true;
				} else if(!strcmp((char*)mods->value, "B2")) {
					binary[42] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2I:1.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction I2I.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "S32")) {
			} else if(!strcmp((char*)mods->value, "S32")) {
			} else if(!strcmp((char*)mods->value, "???7")) {
			} else if(!strcmp((char*)mods->value, "S8")) {
			} else if(!strcmp((char*)mods->value, "INVALID7")) {
			} else if(!strcmp((char*)mods->value, "S8")) {
			} else if(!strcmp((char*)mods->value, "U32")) {
			} else if(!strcmp((char*)mods->value, "U32")) {
			} else if(!strcmp((char*)mods->value, "SAT")) {
			} else if(!strcmp((char*)mods->value, "S16")) {
			} else if(!strcmp((char*)mods->value, "U16")) {
			} else if(!strcmp((char*)mods->value, "S16")) {
			} else if(!strcmp((char*)mods->value, "U16")) {
			} else if(!strcmp((char*)mods->value, "U8")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2I.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_F2I == inst->op) {
		binary[52] = true;
		binary[53] = true;
		binary[55] = true;
		binary[58] = true;
		binary[59] = true;
		binary[60] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 20] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register) {
			checkrandmods = 2;
			binary[45] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "S32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[9] = true;
					binary[11] = true;
					binary[12] = true;
				} else if(!strcmp((char*)mods->value, "F32") && seenModType[1]) {
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "TRUNC") && !seenModType[4]) {
					seenModType[4] = true;
					binary[39] = true;
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "FTZ")) {
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "S64") && !seenModType[1]) {
					seenModType[1] = true;
					binary[8] = true;
					binary[9] = true;
					binary[10] = true;
					binary[11] = true;
					binary[12] = true;
					binary[39] = true;
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "F64") && seenModType[1]) {
					binary[9] = true;
					binary[10] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "U64") && !seenModType[1]) {
					seenModType[1] = true;
					binary[8] = true;
					binary[9] = true;
					binary[11] = true;
					binary[39] = true;
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "INVALID0") && !seenModType[1]) {
					seenModType[1] = true;
					binary[11] = true;
					binary[39] = true;
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDFPSRC0") && seenModType[1]) {
					binary[9] = true;
					binary[39] = true;
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "CEIL") && !seenModType[4]) {
					seenModType[4] = true;
					binary[9] = true;
					binary[11] = true;
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "FLOOR") && !seenModType[4]) {
					seenModType[4] = true;
					binary[9] = true;
					binary[11] = true;
					binary[39] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction F2I.\n";
				}

				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[3] = true;
				binary[4] = true;
				binary[9] = true;
				binary[10] = true;
				binary[11] = true;
				binary[12] = true;
				binary[22] = true;
				binary[24] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction F2I:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "H1")) {
					binary[41] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction F2I:1.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction F2I.\n";
		}
	} else if(opcode_F2F == inst->op) {
		binary[51] = true;
		binary[53] = true;
		binary[55] = true;
		binary[59] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_hex) {
			checkrandmods = 2;
			binary[60] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "F32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "F32") && seenModType[1]) {
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "TRUNC") && !seenModType[4]) {
					seenModType[4] = true;
					binary[9] = true;
					binary[11] = true;
					binary[39] = true;
					binary[40] = true;
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "F64") && !seenModType[1]) {
					seenModType[1] = true;
					binary[8] = true;
					binary[9] = true;
					binary[11] = true;
					binary[39] = true;
					binary[40] = true;
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "RZ") && !seenModType[4]) {
					seenModType[4] = true;
					binary[39] = true;
					binary[40] = true;
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDFPDEST0") && !seenModType[1]) {
					seenModType[1] = true;
					binary[11] = true;
					binary[39] = true;
					binary[40] = true;
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "F64") && seenModType[1]) {
					binary[9] = true;
					binary[10] = true;
					binary[11] = true;
					binary[39] = true;
					binary[40] = true;
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDFPSRC0") && seenModType[1]) {
					binary[9] = true;
					binary[39] = true;
					binary[40] = true;
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "CEIL") && !seenModType[4]) {
					seenModType[4] = true;
					binary[9] = true;
					binary[11] = true;
					binary[40] = true;
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "FLOOR") && !seenModType[4]) {
					seenModType[4] = true;
					binary[9] = true;
					binary[11] = true;
					binary[39] = true;
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "PASS") && !seenModType[4]) {
					seenModType[4] = true;
					binary[9] = true;
					binary[11] = true;
					binary[39] = true;
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "FTZ")) {
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "F16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[8] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[8] = true;
					binary[9] = true;
					binary[11] = true;
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
					binary[8] = true;
					binary[9] = true;
					binary[11] = true;
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "ROUND") && !seenModType[4]) {
					seenModType[4] = true;
					binary[9] = true;
					binary[11] = true;
					binary[42] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction F2F:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "ABS")) {
					binary[49] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction F2F:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_const_mem) {
			checkrandmods = 1;
			binary[58] = true;
			binary[62] = true;
			binary[45] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[1]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[1]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "F64") && !seenModType[1]) {
					seenModType[1] = true;
					binary[8] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "F32") && seenModType[1]) {
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "F32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[9] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "TRUNC") && !seenModType[4]) {
					seenModType[4] = true;
					binary[9] = true;
					binary[11] = true;
					binary[39] = true;
					binary[40] = true;
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "F16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[8] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "F64") && seenModType[1]) {
					binary[8] = true;
					binary[9] = true;
					binary[10] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDFPSRC0") && seenModType[1]) {
					binary[8] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[8] = true;
					binary[9] = true;
					binary[11] = true;
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
					binary[8] = true;
					binary[9] = true;
					binary[11] = true;
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "FTZ")) {
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[58] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction F2F:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register) {
			checkrandmods = 1;
			binary[58] = true;
			binary[60] = true;
			binary[62] = true;
			binary[45] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "F64") && !seenModType[1]) {
					seenModType[1] = true;
					binary[8] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "F32") && seenModType[1]) {
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "F32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[9] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "F64") && seenModType[1]) {
					binary[9] = true;
					binary[10] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "TRUNC") && !seenModType[4]) {
					seenModType[4] = true;
					binary[9] = true;
					binary[11] = true;
					binary[39] = true;
					binary[40] = true;
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "ROUND") && !seenModType[4]) {
					seenModType[4] = true;
					binary[9] = true;
					binary[11] = true;
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "CEIL") && !seenModType[4]) {
					seenModType[4] = true;
					binary[8] = true;
					binary[9] = true;
					binary[10] = true;
					binary[11] = true;
					binary[40] = true;
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "F16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[8] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDFPSRC0") && seenModType[1]) {
					binary[8] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "RM") && !seenModType[4]) {
					seenModType[4] = true;
					binary[8] = true;
					binary[9] = true;
					binary[11] = true;
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "RP") && !seenModType[4]) {
					seenModType[4] = true;
					binary[8] = true;
					binary[9] = true;
					binary[11] = true;
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "FTZ")) {
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[50] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[4]) {
				binary[58] = true;
				binary[60] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction F2F:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction F2F.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "F32")) {
			} else if(!strcmp((char*)mods->value, "F32")) {
			} else if(!strcmp((char*)mods->value, "TRUNC")) {
			} else if(!strcmp((char*)mods->value, "F64")) {
			} else if(!strcmp((char*)mods->value, "RZ")) {
			} else if(!strcmp((char*)mods->value, "INVALIDFPDEST0")) {
			} else if(!strcmp((char*)mods->value, "F64")) {
			} else if(!strcmp((char*)mods->value, "INVALIDFPSRC0")) {
			} else if(!strcmp((char*)mods->value, "CEIL")) {
			} else if(!strcmp((char*)mods->value, "FLOOR")) {
			} else if(!strcmp((char*)mods->value, "PASS")) {
			} else if(!strcmp((char*)mods->value, "FTZ")) {
			} else if(!strcmp((char*)mods->value, "SAT")) {
			} else if(!strcmp((char*)mods->value, "F16")) {
			} else if(!strcmp((char*)mods->value, "RM")) {
			} else if(!strcmp((char*)mods->value, "RP")) {
			} else if(!strcmp((char*)mods->value, "ROUND")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction F2F.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_LOP == inst->op) {
		binary[54] = true;
		binary[59] = true;
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 3;
			binary[48] = true;
			binary[49] = true;
			binary[50] = true;
			binary[60] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			binary[39] = inst->operands[1]->properties & prop_bitwise_complement;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[41] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "Z") && !seenModType[3]) {
					seenModType[3] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "NZ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[44] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "PASS_B") && !seenModType[2]) {
					seenModType[2] = true;
					binary[41] = true;
					binary[42] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				binary[48] = true;
				binary[49] = true;
				binary[50] = true;
				binary[60] = true;
				binary[61] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "INV")) {
					binary[40] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_const_mem) {
			checkrandmods = 2;
			binary[58] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 6; randsize++) {
				binary[randsize + 48] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			binary[39] = inst->operands[2]->properties & prop_bitwise_complement;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[40] = inst->operands[3]->properties & prop_bitwise_complement;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[3]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[3]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "Z") && !seenModType[3]) {
					seenModType[3] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[41] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "NZ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[44] = true;
					binary[45] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				binary[2] = true;
				binary[4] = true;
				binary[9] = true;
				binary[12] = true;
				binary[20] = true;
				binary[22] = true;
				binary[35] = true;
				binary[58] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_hex) {
			checkrandmods = 4;
			binary[60] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 6; randsize++) {
				binary[randsize + 48] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			binary[39] = inst->operands[2]->properties & prop_bitwise_complement;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "NZ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[44] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[41] = true;
					binary[44] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[42] = true;
					binary[44] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "Z") && !seenModType[3]) {
					seenModType[3] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[44] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				binary[0] = true;
				binary[1] = true;
				binary[2] = true;
				binary[3] = true;
				binary[8] = true;
				binary[9] = true;
				binary[10] = true;
				binary[11] = true;
				binary[20] = true;
				binary[21] = true;
				binary[22] = true;
				binary[23] = true;
				binary[24] = true;
				binary[25] = true;
				binary[26] = true;
				binary[27] = true;
				binary[60] = true;
				binary[61] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP:2.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[3]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "INV")) {
					binary[40] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP:3.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			checkrandmods = 1;
			binary[48] = true;
			binary[49] = true;
			binary[50] = true;
			binary[58] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			binary[39] = inst->operands[1]->properties & prop_bitwise_complement;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			binary[40] = inst->operands[2]->properties & prop_bitwise_complement;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "PASS_B") && !seenModType[2]) {
					seenModType[2] = true;
					binary[41] = true;
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[41] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[42] = true;
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "Z") && !seenModType[3]) {
					seenModType[3] = true;
					binary[42] = true;
					binary[45] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				binary[48] = true;
				binary[49] = true;
				binary[50] = true;
				binary[58] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register) {
			checkrandmods = 2;
			binary[58] = true;
			binary[60] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 6; randsize++) {
				binary[randsize + 48] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			binary[39] = inst->operands[2]->properties & prop_bitwise_complement;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[40] = inst->operands[3]->properties & prop_bitwise_complement;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[41] = true;
				} else if(!strcmp((char*)mods->value, "NZ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[44] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
					binary[44] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "PASS_B") && !seenModType[2]) {
					seenModType[2] = true;
					binary[41] = true;
					binary[42] = true;
					binary[44] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "Z") && !seenModType[3]) {
					seenModType[3] = true;
					binary[41] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[41] = true;
					binary[44] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				binary[0] = true;
				binary[1] = true;
				binary[4] = true;
				binary[8] = true;
				binary[10] = true;
				binary[22] = true;
				binary[41] = true;
				binary[58] = true;
				binary[60] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			checkrandmods = 1;
			binary[48] = true;
			binary[49] = true;
			binary[50] = true;
			binary[58] = true;
			binary[60] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			binary[39] = inst->operands[1]->properties & prop_bitwise_complement;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			binary[40] = inst->operands[2]->properties & prop_bitwise_complement;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "PASS_B") && !seenModType[2]) {
					seenModType[2] = true;
					binary[41] = true;
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[41] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[41] = true;
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "Z") && !seenModType[3]) {
					seenModType[3] = true;
					binary[41] = true;
					binary[45] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				binary[48] = true;
				binary[49] = true;
				binary[50] = true;
				binary[58] = true;
				binary[60] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction LOP.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "AND")) {
			} else if(!strcmp((char*)mods->value, "OR")) {
			} else if(!strcmp((char*)mods->value, "XOR")) {
			} else if(!strcmp((char*)mods->value, "X")) {
			} else if(!strcmp((char*)mods->value, "T")) {
			} else if(!strcmp((char*)mods->value, "Z")) {
			} else if(!strcmp((char*)mods->value, "NZ")) {
			} else if(!strcmp((char*)mods->value, "PASS_B")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_LOP32I == inst->op) {
		binary[58] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 32; randsize++) {
			binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[54] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP32I.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction LOP32I.\n";
		}
	} else if(opcode_SHL == inst->op) {
		binary[51] = true;
		binary[54] = true;
		binary[59] = true;
		binary[60] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			checkrandmods = 1;
			binary[58] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "W")) {
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[43] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SHL:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 1;
			binary[61] = true;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "W")) {
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[43] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SHL:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction SHL.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "W")) {
			} else if(!strcmp((char*)mods->value, "X")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SHL.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_SHR == inst->op) {
		binary[51] = true;
		binary[53] = true;
		binary[59] = true;
		binary[60] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			checkrandmods = 1;
			binary[58] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "W")) {
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "BREV")) {
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDSHRXMODE1")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[44] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[1] = true;
				binary[2] = true;
				binary[4] = true;
				binary[8] = true;
				binary[12] = true;
				binary[20] = true;
				binary[21] = true;
				binary[22] = true;
				binary[23] = true;
				binary[48] = true;
				binary[58] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SHR:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 1;
			binary[61] = true;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "W")) {
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "BREV")) {
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDSHRXMODE1")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[44] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[48] = true;
				binary[61] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SHR:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction SHR.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "U32")) {
			} else if(!strcmp((char*)mods->value, "W")) {
			} else if(!strcmp((char*)mods->value, "BREV")) {
			} else if(!strcmp((char*)mods->value, "INVALIDSHRXMODE1")) {
			} else if(!strcmp((char*)mods->value, "X")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SHR.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_BFE == inst->op) {
		binary[59] = true;
		binary[60] = true;
		binary[61] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 19; randsize++) {
			binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 1;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "BREV")) {
					binary[40] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BFE.\n";
				}

				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[31] = true;
				binary[32] = true;
				binary[48] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BFE:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction BFE.\n";
		}
	} else if(opcode_BFI == inst->op) {
		binary[52] = true;
		binary[53] = true;
		binary[54] = true;
		binary[55] = true;
		binary[57] = true;
		binary[58] = true;
		binary[60] = true;
		binary[61] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 19; randsize++) {
			binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			node * mods = inst->mods;
			while(mods) {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BFI.\n";

				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BFI:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction BFI.\n";
		}
	} else if(opcode_SEL == inst->op) {
		binary[53] = true;
		binary[55] = true;
		binary[59] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_predicate) {
			binary[58] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			binary[42] = inst->operands[3]->properties & prop_not;
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_predicate) {
			binary[60] = true;
			binary[61] = true;
			binary[56] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1 * (inst->operands[2]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
			binary[42] = inst->operands[3]->properties & prop_not;
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_predicate) {
			binary[58] = true;
			binary[60] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[42] = inst->operands[3]->properties & prop_not;
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction SEL.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SEL.\n";
			mods = mods->next;
		}
	} else if(opcode_SCHI == inst->op) {
		canGuard = false;
		for(int randsize = 0; randsize < 21; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 21; randsize++) {
			binary[randsize + 21] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 22; randsize++) {
			binary[randsize + 42] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_hex && inst->operands[1]->type == type_hex && inst->operands[2]->type == type_hex) {
			node * mods = inst->mods;
			while(mods) {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SCHI.\n";

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction SCHI.\n";
		}
	} else if(opcode_SSY == inst->op) {
		canGuard = false;
		binary[52] = true;
		binary[55] = true;
		binary[57] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_const_mem) {
			binary[5] = true;
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 20] = (inst->operands[0]->val3 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 36] = (inst->operands[0]->val2 & (0x1 << randsize));
			}
		} else if(inst->num_operands == 1 && inst->operands[0]->type == type_hex) {
			for(int randsize = 0; randsize < 24; randsize++) {
				binary[randsize + 20] = ((inst->operands[0]->val1 - (inst->size + inst->address)) & (0x1 << randsize));
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction SSY.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SSY.\n";
			mods = mods->next;
		}
	} else if(opcode_BRA == inst->op) {
		binary[54] = true;
		binary[57] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_const_mem) {
			binary[0] = true;
			binary[1] = true;
			binary[2] = true;
			binary[3] = true;
			binary[5] = true;
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 20] = (inst->operands[0]->val3 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 36] = (inst->operands[0]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LMT")) {
					binary[6] = true;
				} else if(!strcmp((char*)mods->value, "U")) {
					binary[7] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_other_operand && inst->operands[1]->type == type_const_mem) {
			checkrandmods = 1;
			binary[5] = true;
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 20] = (inst->operands[1]->val3 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 36] = (inst->operands[1]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LMT")) {
					binary[6] = true;
				} else if(!strcmp((char*)mods->value, "U")) {
					binary[7] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GEU")) {
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[0] = true;
					binary[2] = true;
					binary[3] = true;
					binary[41] = true;
					binary[42] = true;
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "LEU")) {
					binary[0] = true;
					binary[1] = true;
					binary[3] = true;
					binary[41] = true;
					binary[42] = true;
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[0] = true;
					binary[1] = true;
					binary[2] = true;
					binary[41] = true;
					binary[42] = true;
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[0] = true;
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[41] = true;
					binary[42] = true;
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "GTU")) {
					binary[2] = true;
					binary[3] = true;
					binary[41] = true;
					binary[42] = true;
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[1] = true;
					binary[3] = true;
					binary[41] = true;
					binary[42] = true;
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "GE")) {
					binary[1] = true;
					binary[2] = true;
					binary[41] = true;
					binary[42] = true;
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "RLE")) {
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[41] = true;
					binary[42] = true;
					binary[43] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BRA:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_other_operand && inst->operands[1]->type == type_hex) {
			checkrandmods = 1;
			for(int randsize = 0; randsize < 24; randsize++) {
				binary[randsize + 20] = ((inst->operands[1]->val1 - (inst->size + inst->address)) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LMT")) {
					binary[6] = true;
				} else if(!strcmp((char*)mods->value, "U")) {
					binary[7] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LE")) {
					binary[0] = true;
					binary[1] = true;
				} else if(!strcmp((char*)mods->value, "GT")) {
					binary[2] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[0] = true;
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "GEU")) {
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "GTU")) {
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[1] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "GE")) {
					binary[1] = true;
					binary[2] = true;
				} else if(!strcmp((char*)mods->value, "RLE")) {
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
				} else if(!strcmp((char*)mods->value, "LEU")) {
					binary[0] = true;
					binary[1] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[0] = true;
					binary[1] = true;
					binary[2] = true;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[0] = true;
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BRA:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 1 && inst->operands[0]->type == type_hex) {
			binary[0] = true;
			binary[1] = true;
			binary[2] = true;
			binary[3] = true;
			for(int randsize = 0; randsize < 24; randsize++) {
				binary[randsize + 20] = ((inst->operands[0]->val1 - (inst->size + inst->address)) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LMT")) {
					binary[6] = true;
				} else if(!strcmp((char*)mods->value, "U")) {
					binary[7] = true;
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction BRA.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "LMT")) {
			} else if(!strcmp((char*)mods->value, "U")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BRA.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_BRX == inst->op) {
		binary[52] = true;
		binary[54] = true;
		binary[57] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_const_mem) {
			binary[0] = true;
			binary[1] = true;
			binary[2] = true;
			binary[3] = true;
			binary[5] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 20] = (inst->operands[0]->val3 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 36] = (inst->operands[0]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LMT")) {
					binary[6] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_other_operand && inst->operands[1]->type == type_const_mem) {
			checkrandmods = 1;
			binary[5] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 20] = (inst->operands[1]->val3 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 36] = (inst->operands[1]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LMT")) {
					binary[6] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GEU")) {
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[0] = true;
					binary[2] = true;
					binary[3] = true;
					binary[41] = true;
					binary[42] = true;
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "LEU")) {
					binary[0] = true;
					binary[1] = true;
					binary[3] = true;
					binary[41] = true;
					binary[42] = true;
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[0] = true;
					binary[1] = true;
					binary[2] = true;
					binary[41] = true;
					binary[42] = true;
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[0] = true;
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[41] = true;
					binary[42] = true;
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "GTU")) {
					binary[2] = true;
					binary[3] = true;
					binary[41] = true;
					binary[42] = true;
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[1] = true;
					binary[3] = true;
					binary[41] = true;
					binary[42] = true;
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "GE")) {
					binary[1] = true;
					binary[2] = true;
					binary[41] = true;
					binary[42] = true;
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "RLE")) {
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[41] = true;
					binary[42] = true;
					binary[43] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BRX:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_hex) {
			binary[0] = true;
			binary[1] = true;
			binary[2] = true;
			binary[3] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 24; randsize++) {
				binary[randsize + 20] = ((inst->operands[1]->val1 * (inst->operands[1]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LMT")) {
					binary[6] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_other_operand && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 1;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 24; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1 * (inst->operands[2]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LMT")) {
					binary[6] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GEU")) {
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "GTU")) {
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[1] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "GE")) {
					binary[1] = true;
					binary[2] = true;
				} else if(!strcmp((char*)mods->value, "RLE")) {
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[0] = true;
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "LEU")) {
					binary[0] = true;
					binary[1] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[0] = true;
					binary[1] = true;
					binary[2] = true;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[0] = true;
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BRX:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction BRX.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "LMT")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BRX.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_PCNT == inst->op) {
		canGuard = false;
		binary[52] = true;
		binary[53] = true;
		binary[55] = true;
		binary[57] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_hex) {
			for(int randsize = 0; randsize < 24; randsize++) {
				binary[randsize + 20] = ((inst->operands[0]->val1 - (inst->size + inst->address)) & (0x1 << randsize));
			}
		} else if(inst->num_operands == 1 && inst->operands[0]->type == type_const_mem) {
			binary[5] = true;
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 20] = (inst->operands[0]->val3 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 36] = (inst->operands[0]->val2 & (0x1 << randsize));
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction PCNT.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction PCNT.\n";
			mods = mods->next;
		}
	} else if(opcode_CONT == inst->op) {
		binary[52] = true;
		binary[54] = true;
		binary[56] = true;
		binary[57] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		if(inst->num_operands == 0) {
			binary[0] = true;
			binary[1] = true;
			binary[2] = true;
			binary[3] = true;
		} else if(inst->num_operands == 1 && inst->operands[0]->type == type_other_operand) {
			checkrandmods = 1;
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GEU")) {
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "GTU")) {
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[1] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "GE")) {
					binary[1] = true;
					binary[2] = true;
				} else if(!strcmp((char*)mods->value, "RLE")) {
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[0] = true;
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "LEU")) {
					binary[0] = true;
					binary[1] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[0] = true;
					binary[1] = true;
					binary[2] = true;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[0] = true;
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction CONT:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction CONT.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction CONT.\n";
			mods = mods->next;
		}
	} else if(opcode_PBK == inst->op) {
		canGuard = false;
		binary[53] = true;
		binary[55] = true;
		binary[57] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_const_mem) {
			binary[5] = true;
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 20] = (inst->operands[0]->val3 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 36] = (inst->operands[0]->val2 & (0x1 << randsize));
			}
		} else if(inst->num_operands == 1 && inst->operands[0]->type == type_hex) {
			for(int randsize = 0; randsize < 24; randsize++) {
				binary[randsize + 20] = ((inst->operands[0]->val1 - (inst->size + inst->address)) & (0x1 << randsize));
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction PBK.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction PBK.\n";
			mods = mods->next;
		}
	} else if(opcode_BRK == inst->op) {
		binary[54] = true;
		binary[56] = true;
		binary[57] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_other_operand) {
			checkrandmods = 1;
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GEU")) {
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "GTU")) {
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[1] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "GE")) {
					binary[1] = true;
					binary[2] = true;
				} else if(!strcmp((char*)mods->value, "RLE")) {
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[0] = true;
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "LEU")) {
					binary[0] = true;
					binary[1] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[0] = true;
					binary[1] = true;
					binary[2] = true;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[0] = true;
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BRK:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 0) {
			binary[0] = true;
			binary[1] = true;
			binary[2] = true;
			binary[3] = true;
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction BRK.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BRK.\n";
			mods = mods->next;
		}
	} else if(opcode_CAL == inst->op) {
		canGuard = false;
		binary[53] = true;
		binary[54] = true;
		binary[57] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_const_mem) {
			binary[5] = true;
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 20] = (inst->operands[0]->val3 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 36] = (inst->operands[0]->val2 & (0x1 << randsize));
			}
			binary[6] = true;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NOINC")) {
					binary[6] = false;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 1 && inst->operands[0]->type == type_hex) {
			for(int randsize = 0; randsize < 24; randsize++) {
				binary[randsize + 20] = ((inst->operands[0]->val1 - (inst->size + inst->address)) & (0x1 << randsize));
			}
			binary[6] = true;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NOINC")) {
					binary[6] = false;
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction CAL.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "NOINC")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction CAL.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_RET == inst->op) {
		binary[53] = true;
		binary[56] = true;
		binary[57] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_other_operand) {
			checkrandmods = 1;
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GEU")) {
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "GTU")) {
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[1] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "GE")) {
					binary[1] = true;
					binary[2] = true;
				} else if(!strcmp((char*)mods->value, "RLE")) {
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[0] = true;
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "LEU")) {
					binary[0] = true;
					binary[1] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[0] = true;
					binary[1] = true;
					binary[2] = true;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[0] = true;
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction RET:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 0) {
			binary[0] = true;
			binary[1] = true;
			binary[2] = true;
			binary[3] = true;
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction RET.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction RET.\n";
			mods = mods->next;
		}
	} else if(opcode_EXIT == inst->op) {
		binary[56] = true;
		binary[57] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_other_operand) {
			checkrandmods = 1;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "KEEPREFCOUNT")) {
					binary[5] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GEU")) {
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "GTU")) {
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[1] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "GE")) {
					binary[1] = true;
					binary[2] = true;
				} else if(!strcmp((char*)mods->value, "RLE")) {
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[0] = true;
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "LEU")) {
					binary[0] = true;
					binary[1] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[0] = true;
					binary[1] = true;
					binary[2] = true;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[0] = true;
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction EXIT:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 0) {
			binary[0] = true;
			binary[1] = true;
			binary[2] = true;
			binary[3] = true;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "KEEPREFCOUNT")) {
					binary[5] = true;
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction EXIT.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "KEEPREFCOUNT")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction EXIT.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_NOP == inst->op) {
		binary[52] = true;
		binary[53] = true;
		binary[55] = true;
		binary[60] = true;
		binary[62] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_hex) {
			binary[8] = true;
			binary[9] = true;
			binary[10] = true;
			binary[11] = true;
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 20] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "TRIG")) {
					binary[13] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_other_operand && inst->operands[1]->type == type_hex) {
			checkrandmods = 1;
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 20] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "TRIG")) {
					binary[13] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GEU")) {
					binary[9] = true;
					binary[10] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[8] = true;
					binary[10] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "LEU")) {
					binary[8] = true;
					binary[9] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[8] = true;
					binary[9] = true;
					binary[10] = true;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[8] = true;
					binary[9] = true;
					binary[10] = true;
					binary[11] = true;
					binary[12] = true;
				} else if(!strcmp((char*)mods->value, "GTU")) {
					binary[10] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[9] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "GE")) {
					binary[9] = true;
					binary[10] = true;
				} else if(!strcmp((char*)mods->value, "RLE")) {
					binary[9] = true;
					binary[10] = true;
					binary[11] = true;
					binary[12] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction NOP:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 1 && inst->operands[0]->type == type_other_operand) {
			checkrandmods = 1;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "TRIG")) {
					binary[13] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GEU")) {
					binary[9] = true;
					binary[10] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "GTU")) {
					binary[10] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[9] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "GE")) {
					binary[9] = true;
					binary[10] = true;
				} else if(!strcmp((char*)mods->value, "RLE")) {
					binary[9] = true;
					binary[10] = true;
					binary[11] = true;
					binary[12] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[8] = true;
					binary[10] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "LEU")) {
					binary[8] = true;
					binary[9] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[8] = true;
					binary[9] = true;
					binary[10] = true;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[8] = true;
					binary[9] = true;
					binary[10] = true;
					binary[11] = true;
					binary[12] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction NOP:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 0) {
			binary[8] = true;
			binary[9] = true;
			binary[10] = true;
			binary[11] = true;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "TRIG")) {
					binary[13] = true;
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction NOP.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "TRIG")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction NOP.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_BAR == inst->op) {
		binary[39] = true;
		binary[40] = true;
		binary[41] = true;
		binary[43] = true;
		binary[44] = true;
		binary[51] = true;
		binary[53] = true;
		binary[55] = true;
		binary[60] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_hex && inst->operands[1]->type == type_hex) {
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "SYNC")) {
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 1 && inst->operands[0]->type == type_hex) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "SYNC")) {
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction BAR.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "SYNC")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BAR.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_BPT == inst->op) {
		cerr << "ERROR: unimplemented operation BPT.\n";
	} else if(opcode_B2R == inst->op) {
		cerr << "ERROR: unimplemented operation B2R.\n";
	} else if(opcode_S2R == inst->op) {
		binary[51] = true;
		binary[54] = true;
		binary[55] = true;
		binary[60] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 20] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_special_reg) {
			node * mods = inst->mods;
			while(mods) {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction S2R.\n";

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction S2R.\n";
		}
	} else if(opcode_PSETP == inst->op) {
		binary[52] = true;
		binary[55] = true;
		binary[60] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 3] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 12] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 29] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 39] = ((inst->operands[4]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_predicate && inst->operands[3]->type == type_predicate && inst->operands[4]->type == type_predicate) {
			binary[15] = inst->operands[2]->properties & prop_not;
			binary[32] = inst->operands[3]->properties & prop_not;
			binary[42] = inst->operands[4]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "AND") && seenModType[2]) {
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[24] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[25] = true;
				} else if(!strcmp((char*)mods->value, "OR") && seenModType[2]) {
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && seenModType[2]) {
					binary[46] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction PSETP.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction PSETP.\n";
		}
	} else if(opcode_PSET == inst->op) {
		binary[51] = true;
		binary[55] = true;
		binary[60] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 12] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 29] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_predicate && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[15] = inst->operands[1]->properties & prop_not;
			binary[32] = inst->operands[2]->properties & prop_not;
			binary[42] = inst->operands[3]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "AND") && seenModType[2]) {
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[24] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[25] = true;
				} else if(!strcmp((char*)mods->value, "BF")) {
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "OR") && seenModType[2]) {
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && seenModType[2]) {
					binary[46] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction PSET.\n";
				}

				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction PSET:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction PSET.\n";
		}
	} else if(opcode_FLO == inst->op) {
		binary[52] = true;
		binary[53] = true;
		binary[58] = true;
		binary[59] = true;
		binary[60] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 20] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register) {
			checkrandmods = 1;
			binary[40] = inst->operands[1]->properties & prop_bitwise_complement;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "SH")) {
					binary[41] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FLO.\n";
				}

				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[1] = true;
				binary[2] = true;
				binary[3] = true;
				binary[22] = true;
				binary[48] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FLO:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FLO.\n";
		}
	} else if(opcode_P2R == inst->op) {
		cerr << "ERROR: unimplemented operation P2R.\n";
	} else if(opcode_R2P == inst->op) {
		cerr << "ERROR: unimplemented operation R2P.\n";
	} else if(opcode_TEX == inst->op) {
		binary[51] = true;
		binary[52] = true;
		binary[53] = true;
		binary[62] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 6 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_hex && inst->operands[4]->type == type_texture_operand && inst->operands[5]->type == type_hex) {
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 13; randsize++) {
				binary[randsize + 36] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 28] = ((inst->operands[4]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 31] = ((inst->operands[5]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NDV")) {
				} else if(!strcmp((char*)mods->value, "NODEP")) {
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "DC")) {
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "P")) {
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_texture_operand && inst->operands[4]->type == type_hex) {
			binary[20] = true;
			binary[21] = true;
			binary[22] = true;
			binary[23] = true;
			binary[24] = true;
			binary[25] = true;
			binary[26] = true;
			binary[27] = true;
			for(int randsize = 0; randsize < 13; randsize++) {
				binary[randsize + 36] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 28] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 4; randsize++) {
				binary[randsize + 31] = ((inst->operands[4]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "P")) {
				} else if(!strcmp((char*)mods->value, "NDV")) {
					binary[35] = true;
				} else if(!strcmp((char*)mods->value, "NODEP")) {
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "DC")) {
					binary[50] = true;
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction TEX.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "NDV")) {
			} else if(!strcmp((char*)mods->value, "NODEP")) {
			} else if(!strcmp((char*)mods->value, "DC")) {
			} else if(!strcmp((char*)mods->value, "P")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction TEX.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_TEXDEPBAR == inst->op) {
		cerr << "ERROR: unimplemented operation TEXDEPBAR.\n";
	} else if(opcode_RRO == inst->op) {
		binary[52] = true;
		binary[55] = true;
		binary[58] = true;
		binary[59] = true;
		binary[60] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 20] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register) {
			binary[45] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			binary[39] = true;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "EX2") && !seenModType[8]) {
					seenModType[8] = true;
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "SINCOS")) {
					binary[39] = false;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction RRO.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction RRO.\n";
		}
	} else if(opcode_PRMT == inst->op) {
		binary[54] = true;
		binary[55] = true;
		binary[57] = true;
		binary[58] = true;
		binary[60] = true;
		binary[61] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 19; randsize++) {
			binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_register) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "F4E")) {
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "B4E")) {
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "ECL")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction PRMT.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction PRMT.\n";
		}
	} else if(opcode_VADD == inst->op) {
		cerr << "ERROR: unimplemented operation VADD.\n";
	} else if(opcode_DMNMX == inst->op) {
		binary[52] = true;
		binary[54] = true;
		binary[59] = true;
		binary[60] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_predicate) {
			checkrandmods = 3;
			binary[61] = true;
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[46] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[42] = inst->operands[3]->properties & prop_not;
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DMNMX:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DMNMX:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "ABS")) {
					binary[49] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DMNMX:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[58] = true;
			binary[62] = true;
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[46] = inst->operands[1]->properties & prop_absolute_value;
			binary[45] = inst->operands[2]->properties & prop_minus;
			binary[49] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[42] = inst->operands[3]->properties & prop_not;
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DMNMX:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction DMNMX.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DMNMX.\n";
			mods = mods->next;
		}
	} else if(opcode_FMNMX == inst->op) {
		binary[53] = true;
		binary[54] = true;
		binary[59] = true;
		binary[60] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_predicate) {
			checkrandmods = 3;
			binary[61] = true;
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[46] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[42] = inst->operands[3]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "FTZ")) {
					binary[44] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMNMX:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMNMX:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "ABS")) {
					binary[49] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMNMX:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[58] = true;
			binary[62] = true;
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[46] = inst->operands[1]->properties & prop_absolute_value;
			binary[45] = inst->operands[2]->properties & prop_minus;
			binary[49] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[42] = inst->operands[3]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "FTZ")) {
					binary[44] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMNMX:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FMNMX.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "FTZ")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMNMX.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_RED == inst->op) {
		binary[51] = true;
		binary[52] = true;
		binary[53] = true;
		binary[54] = true;
		binary[55] = true;
		binary[56] = true;
		binary[57] = true;
		binary[59] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 20; randsize++) {
			binary[randsize + 28] = (inst->operands[0]->val2 & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_mem && inst->operands[1]->type == type_register) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "E")) {
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "ADD")) {
				} else if(!strcmp((char*)mods->value, "F32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[20] = true;
					binary[21] = true;
				} else if(!strcmp((char*)mods->value, "FTZ")) {
				} else if(!strcmp((char*)mods->value, "RN") && !seenModType[4]) {
					seenModType[4] = true;
					binary[20] = true;
					binary[21] = true;
				} else if(!strcmp((char*)mods->value, "U64") && !seenModType[1]) {
					seenModType[1] = true;
					binary[21] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "S32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[20] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDATOMICSIZE7")) {
					binary[22] = true;
				} else if(!strcmp((char*)mods->value, "MIN")) {
					binary[23] = true;
				} else if(!strcmp((char*)mods->value, "MAX")) {
					binary[24] = true;
				} else if(!strcmp((char*)mods->value, "DEC")) {
					binary[25] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction RED.\n";
				}

				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[2] = true;
				binary[9] = true;
				binary[11] = true;
				binary[20] = true;
				binary[21] = true;
				binary[22] = true;
				binary[48] = true;
			}
			if(!seenModType[4]) {
				binary[2] = true;
				binary[9] = true;
				binary[11] = true;
				binary[48] = true;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction RED.\n";
		}
	} else if(opcode_VOTE == inst->op) {
		binary[51] = true;
		binary[52] = true;
		binary[54] = true;
		binary[55] = true;
		binary[60] = true;
		binary[62] = true;
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_predicate) {
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 45] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			binary[42] = inst->operands[2]->properties & prop_not;
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 39] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[48] = true;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "ALL")) {
					binary[48] = false;
				} else if(!strcmp((char*)mods->value, "ANY")) {
				} else if(!strcmp((char*)mods->value, "INVALIDVMODE3")) {
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate) {
			binary[0] = true;
			binary[1] = true;
			binary[2] = true;
			binary[3] = true;
			binary[4] = true;
			binary[5] = true;
			binary[6] = true;
			binary[7] = true;
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 45] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			binary[42] = inst->operands[1]->properties & prop_not;
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 39] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "ALL")) {
				} else if(!strcmp((char*)mods->value, "ANY")) {
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[49] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				binary[0] = true;
				binary[1] = true;
				binary[2] = true;
				binary[3] = true;
				binary[4] = true;
				binary[5] = true;
				binary[6] = true;
				binary[7] = true;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction VOTE.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "ALL")) {
			} else if(!strcmp((char*)mods->value, "ANY")) {
			} else if(!strcmp((char*)mods->value, "INVALIDVMODE3")) {
			} else if(!strcmp((char*)mods->value, "EQ")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction VOTE.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_POPC == inst->op) {
		binary[51] = true;
		binary[58] = true;
		binary[59] = true;
		binary[60] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 20] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register) {
			binary[40] = inst->operands[1]->properties & prop_bitwise_complement;
			node * mods = inst->mods;
			while(mods) {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction POPC.\n";

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction POPC.\n";
		}
	} else if(opcode_MEMBAR == inst->op) {
		cerr << "ERROR: unimplemented operation MEMBAR.\n";
	} else if(opcode_STSCUL == inst->op) {
		cerr << "ERROR: unimplemented operation STSCUL.\n";
	} else if(opcode_LEPC == inst->op) {
		cerr << "ERROR: unimplemented operation LEPC.\n";
	} else if(opcode_CSETP == inst->op) {
		binary[53] = true;
		binary[55] = true;
		binary[60] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 3] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_other_operand && inst->operands[3]->type == type_predicate) {
			binary[42] = inst->operands[3]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[9] = true;
					binary[10] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "NUM") && !seenModType[3]) {
					seenModType[3] = true;
					binary[8] = true;
					binary[9] = true;
					binary[10] = true;
				} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[10] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "GEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[9] = true;
					binary[10] = true;
					binary[11] = true;
				} else if(!strcmp((char*)mods->value, "HS") && !seenModType[3]) {
					seenModType[3] = true;
					binary[9] = true;
					binary[10] = true;
					binary[12] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[9] = true;
					binary[10] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[9] = true;
					binary[10] = true;
					binary[46] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction CSETP.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction CSETP.\n";
		}
	} else if(opcode_ISCADD32I == inst->op) {
		binary[58] = true;
		binary[60] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 32; randsize++) {
			binary[randsize + 20] = ((inst->operands[2]->val1 * (inst->operands[2]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 5; randsize++) {
			binary[randsize + 53] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_hex) {
			node * mods = inst->mods;
			while(mods) {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISCADD32I.\n";

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction ISCADD32I.\n";
		}
	} else if(opcode_VMNMX == inst->op) {
		cerr << "ERROR: unimplemented operation VMNMX.\n";
	} else if(opcode_TLD == inst->op) {
		cerr << "ERROR: unimplemented operation TLD.\n";
	} else if(opcode_SHF == inst->op) {
		binary[51] = true;
		binary[52] = true;
		binary[53] = true;
		binary[54] = true;
		binary[55] = true;
		binary[60] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			binary[59] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "L") && !seenModType[5]) {
					seenModType[5] = true;
					binary[56] = true;
					binary[57] = true;
				} else if(!strcmp((char*)mods->value, "U64") && !seenModType[1]) {
					seenModType[1] = true;
					binary[38] = true;
				} else if(!strcmp((char*)mods->value, "R") && !seenModType[5]) {
					seenModType[5] = true;
					binary[38] = true;
					binary[58] = true;
				} else if(!strcmp((char*)mods->value, "S64") && !seenModType[1]) {
					seenModType[1] = true;
					binary[37] = true;
					binary[38] = true;
				} else if(!strcmp((char*)mods->value, "HI") && !seenModType[3]) {
					seenModType[3] = true;
					binary[38] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "W")) {
					binary[50] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[1] = true;
				binary[2] = true;
				binary[8] = true;
				binary[9] = true;
				binary[10] = true;
				binary[11] = true;
				binary[12] = true;
				binary[13] = true;
				binary[14] = true;
				binary[15] = true;
				binary[23] = true;
				binary[40] = true;
				binary[41] = true;
				binary[42] = true;
				binary[56] = true;
				binary[57] = true;
				binary[59] = true;
				binary[62] = true;
			}
			if(!seenModType[3]) {
				binary[59] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SHF:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			binary[61] = true;
			for(int randsize = 0; randsize < 6; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "L") && !seenModType[5]) {
					seenModType[5] = true;
					binary[57] = true;
					binary[58] = true;
				} else if(!strcmp((char*)mods->value, "U64") && !seenModType[1]) {
					seenModType[1] = true;
					binary[38] = true;
				} else if(!strcmp((char*)mods->value, "R") && !seenModType[5]) {
					seenModType[5] = true;
					binary[38] = true;
					binary[59] = true;
				} else if(!strcmp((char*)mods->value, "HI") && !seenModType[3]) {
					seenModType[3] = true;
					binary[38] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "S64") && !seenModType[1]) {
					seenModType[1] = true;
					binary[37] = true;
					binary[38] = true;
					binary[57] = true;
					binary[58] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "W")) {
					binary[50] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[0] = true;
				binary[1] = true;
				binary[3] = true;
				binary[8] = true;
				binary[10] = true;
				binary[11] = true;
				binary[20] = true;
				binary[21] = true;
				binary[57] = true;
				binary[58] = true;
				binary[61] = true;
			}
			if(!seenModType[3]) {
				binary[61] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SHF:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction SHF.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "L")) {
			} else if(!strcmp((char*)mods->value, "U64")) {
			} else if(!strcmp((char*)mods->value, "R")) {
			} else if(!strcmp((char*)mods->value, "S64")) {
			} else if(!strcmp((char*)mods->value, "HI")) {
			} else if(!strcmp((char*)mods->value, "X")) {
			} else if(!strcmp((char*)mods->value, "W")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SHF.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_FCHK == inst->op) {
		binary[51] = true;
		binary[55] = true;
		binary[58] = true;
		binary[59] = true;
		binary[60] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 3] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[46] = inst->operands[1]->properties & prop_absolute_value;
			binary[45] = inst->operands[2]->properties & prop_minus;
			binary[49] = inst->operands[2]->properties & prop_absolute_value;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "DIVIDE")) {
				} else if(!strcmp((char*)mods->value, "INVALIDCHKMODE1")) {
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDCHKMODE2")) {
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDCHKMODE4")) {
					binary[41] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDCHKMODE8")) {
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDCHKMODE16")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDCHKMODE32")) {
					binary[44] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FCHK.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FCHK.\n";
		}
	} else if(opcode_ISUB == inst->op) {
		cerr << "ERROR: unimplemented operation ISUB.\n";
	} else if(opcode_JCAL == inst->op) {
		canGuard = false;
		binary[53] = true;
		binary[57] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_const_mem) {
			binary[5] = true;
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 20] = (inst->operands[0]->val3 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 36] = (inst->operands[0]->val2 & (0x1 << randsize));
			}
			binary[6] = true;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NOINC")) {
					binary[6] = false;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 1 && inst->operands[0]->type == type_hex) {
			for(int randsize = 0; randsize < 32; randsize++) {
				binary[randsize + 20] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			binary[6] = true;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NOINC")) {
					binary[6] = false;
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction JCAL.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "NOINC")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction JCAL.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_SHFL == inst->op) {
		cerr << "ERROR: unimplemented operation SHFL.\n";
	} else if(opcode_LDG == inst->op) {
		binary[52] = true;
		binary[54] = true;
		binary[55] = true;
		binary[57] = true;
		binary[58] = true;
		binary[59] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 24; randsize++) {
			binary[randsize + 20] = (inst->operands[1]->val2 & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_mem) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "E")) {
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "U8") && !seenModType[1]) {
					seenModType[1] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "64")) {
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "U16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[45] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "CG")) {
					binary[46] = true;
				} else if(!strcmp((char*)mods->value, "CI")) {
					binary[47] = true;
				} else if(!strcmp((char*)mods->value, "CV")) {
					binary[46] = true;
					binary[47] = true;
				} else if(!strcmp((char*)mods->value, "128")) {
					binary[49] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LDG.\n";
				}

				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[50] = true;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction LDG.\n";
		}
	} else if(opcode_LD_LDU == inst->op) {
		cerr << "ERROR: unimplemented operation LD_LDU.\n";
	} else if(opcode_ATOM == inst->op) {
		binary[52] = true;
		binary[53] = true;
		binary[54] = true;
		binary[55] = true;
		binary[57] = true;
		binary[58] = true;
		binary[59] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		
		int lastOp = 2;
		if(inst->operands[3]->reg == -1) {
			binary[50] = true;
			lastOp = 3;
		} else if(inst->operands[2]->reg == -1) {
			binary[51] = true;
		} else if(inst->operands[3]->reg == inst->operands[2]->reg + 1) {
			binary[49] = false;
		} else if(inst->operands[3]->reg == inst->operands[2]->reg + 2) {
			binary[49] = true;
		} else {
			cerr << "SANITY CHECK ERROR b50~6073; invalid ATOM?\n";
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 20] = ((inst->operands[lastOp]->val1) & (0x1 << randsize));
		}
		
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 20; randsize++) {
			binary[randsize + 28] = (inst->operands[1]->val2 & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_mem && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "E")) {
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "CAS")) {
				} else if(!strcmp((char*)mods->value, "64")) {
					binary[49] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ATOM.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction ATOM.\n";
		}
	} else if(opcode_CCTL == inst->op) {
		cerr << "ERROR: unimplemented operation CCTL.\n";
	} else if(opcode_XMAD == inst->op) {
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_const_mem) {
			checkrandmods = 3;
			binary[56] = true;
			binary[60] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[3]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[3]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "S16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[48] = true;
					binary[52] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "U16") && seenModType[1]) {
					binary[48] = true;
					binary[52] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "U16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[49] = true;
					binary[52] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "S16") && seenModType[1]) {
					binary[49] = true;
					binary[52] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "CLO") && !seenModType[9]) {
					seenModType[9] = true;
					binary[50] = true;
					binary[52] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "PSL") && !seenModType[9]) {
					seenModType[9] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "CHI") && !seenModType[9]) {
					seenModType[9] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "MRG") && !seenModType[9]) {
					seenModType[9] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
				} else if(!strcmp((char*)mods->value, "CBCC")) {
				} else if(!strcmp((char*)mods->value, "CSFU") && !seenModType[9]) {
					seenModType[9] = true;
					binary[48] = true;
					binary[49] = true;
					binary[50] = true;
					binary[53] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[56] = true;
				binary[60] = true;
				binary[62] = true;
			}
			if(!seenModType[9]) {
				binary[56] = true;
				binary[60] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction XMAD:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "H1")) {
					binary[52] = true;
					binary[53] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction XMAD:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "H1")) {
					binary[52] = true;
					binary[53] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction XMAD:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_register) {
			checkrandmods = 2;
			binary[57] = true;
			binary[58] = true;
			binary[60] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "PSL") && !seenModType[9]) {
					seenModType[9] = true;
					binary[36] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "CHI") && !seenModType[9]) {
					seenModType[9] = true;
					binary[51] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "S16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[48] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "U16") && seenModType[1]) {
					binary[48] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "MRG") && !seenModType[9]) {
					seenModType[9] = true;
					binary[37] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[38] = true;
				} else if(!strcmp((char*)mods->value, "U16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[49] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "S16") && seenModType[1]) {
					binary[49] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "CLO") && !seenModType[9]) {
					seenModType[9] = true;
					binary[50] = true;
					binary[53] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[57] = true;
				binary[58] = true;
				binary[60] = true;
				binary[61] = true;
			}
			if(!seenModType[9]) {
				binary[57] = true;
				binary[58] = true;
				binary[60] = true;
				binary[61] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction XMAD:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "H1")) {
					binary[53] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction XMAD:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_register) {
			checkrandmods = 3;
			binary[57] = true;
			binary[58] = true;
			binary[59] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "MRG") && !seenModType[9]) {
					seenModType[9] = true;
					binary[52] = true;
					binary[56] = true;
				} else if(!strcmp((char*)mods->value, "CHI") && !seenModType[9]) {
					seenModType[9] = true;
					binary[51] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "S16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "U16") && seenModType[1]) {
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "U16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "S16") && seenModType[1]) {
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "CLO") && !seenModType[9]) {
					seenModType[9] = true;
					binary[50] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[57] = true;
				binary[58] = true;
				binary[59] = true;
				binary[62] = true;
			}
			if(!seenModType[9]) {
				binary[57] = true;
				binary[58] = true;
				binary[59] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction XMAD:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "H1")) {
					binary[53] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction XMAD:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "H1")) {
					binary[52] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction XMAD:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register) {
			checkrandmods = 3;
			binary[56] = true;
			binary[57] = true;
			binary[59] = true;
			binary[60] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "MRG") && !seenModType[9]) {
					seenModType[9] = true;
					binary[37] = true;
				} else if(!strcmp((char*)mods->value, "PSL") && !seenModType[9]) {
					seenModType[9] = true;
					binary[36] = true;
				} else if(!strcmp((char*)mods->value, "CBCC")) {
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "CHI") && !seenModType[9]) {
					seenModType[9] = true;
					binary[51] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "U16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "S16") && seenModType[1]) {
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "S16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "CSFU") && !seenModType[9]) {
					seenModType[9] = true;
					binary[35] = true;
					binary[48] = true;
					binary[49] = true;
					binary[50] = true;
					binary[51] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "U16") && seenModType[1]) {
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[38] = true;
				} else if(!strcmp((char*)mods->value, "CLO") && !seenModType[9]) {
					seenModType[9] = true;
					binary[50] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[56] = true;
				binary[57] = true;
				binary[59] = true;
				binary[60] = true;
				binary[62] = true;
			}
			if(!seenModType[9]) {
				binary[56] = true;
				binary[57] = true;
				binary[59] = true;
				binary[60] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction XMAD:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "H1")) {
					binary[53] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction XMAD:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "H1")) {
					binary[35] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction XMAD:2.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction XMAD.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "S16")) {
			} else if(!strcmp((char*)mods->value, "U16")) {
			} else if(!strcmp((char*)mods->value, "U16")) {
			} else if(!strcmp((char*)mods->value, "S16")) {
			} else if(!strcmp((char*)mods->value, "CLO")) {
			} else if(!strcmp((char*)mods->value, "PSL")) {
			} else if(!strcmp((char*)mods->value, "CHI")) {
			} else if(!strcmp((char*)mods->value, "MRG")) {
			} else if(!strcmp((char*)mods->value, "X")) {
			} else if(!strcmp((char*)mods->value, "CBCC")) {
			} else if(!strcmp((char*)mods->value, "CSFU")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction XMAD.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_SYNC == inst->op) {
		binary[51] = true;
		binary[52] = true;
		binary[53] = true;
		binary[54] = true;
		binary[55] = true;
		binary[60] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_other_operand) {
			checkrandmods = 1;
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GEU")) {
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "GTU")) {
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[1] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "GE")) {
					binary[1] = true;
					binary[2] = true;
				} else if(!strcmp((char*)mods->value, "RLE")) {
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[0] = true;
					binary[2] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "LEU")) {
					binary[0] = true;
					binary[1] = true;
					binary[3] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[0] = true;
					binary[1] = true;
					binary[2] = true;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[0] = true;
					binary[1] = true;
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SYNC:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 0) {
			binary[0] = true;
			binary[1] = true;
			binary[2] = true;
			binary[3] = true;
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction SYNC.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SYNC.\n";
			mods = mods->next;
		}
	} else if(opcode_STG == inst->op) {
		binary[51] = true;
		binary[52] = true;
		binary[54] = true;
		binary[55] = true;
		binary[57] = true;
		binary[58] = true;
		binary[59] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 24; randsize++) {
			binary[randsize + 20] = (inst->operands[0]->val2 & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_mem && inst->operands[1]->type == type_register) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "E")) {
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "U8") && !seenModType[1]) {
					seenModType[1] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "64")) {
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "U16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[45] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "CG")) {
					binary[46] = true;
				} else if(!strcmp((char*)mods->value, "CS")) {
					binary[47] = true;
				} else if(!strcmp((char*)mods->value, "WT")) {
					binary[46] = true;
					binary[47] = true;
				} else if(!strcmp((char*)mods->value, "128")) {
					binary[49] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction STG.\n";
				}

				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[50] = true;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction STG.\n";
		}
	} else if(opcode_IADD3 == inst->op) {
		binary[54] = true;
		binary[55] = true;
		binary[59] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			binary[58] = true;
			binary[62] = true;
			binary[50] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			binary[49] = inst->operands[3]->properties & prop_minus;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "X")) {
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "RS") && !seenModType[7]) {
					seenModType[7] = true;
				} else if(!strcmp((char*)mods->value, "???3") && !seenModType[7]) {
					seenModType[7] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[7]) {
				binary[58] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD3:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_register) {
			checkrandmods = 3;
			binary[60] = true;
			binary[61] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			
			//TODO double check this is correct:
			if(inst->operands[2]->val1 & 0x80000) {
				binary[56] = true;
			}
			
			binary[49] = inst->operands[3]->properties & prop_minus;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "X")) {
					binary[48] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[7]) {
				binary[60] = true;
				binary[61] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD3:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD3:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[50] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD3:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register) {
			checkrandmods = 4;
			binary[58] = true;
			binary[60] = true;
			binary[62] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			binary[50] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[49] = inst->operands[3]->properties & prop_minus;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "RS") && !seenModType[7]) {
					seenModType[7] = true;
					binary[37] = true;
				} else if(!strcmp((char*)mods->value, "???3") && !seenModType[7]) {
					seenModType[7] = true;
					binary[37] = true;
					binary[38] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[48] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[7]) {
				binary[58] = true;
				binary[60] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD3:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "H0")) {
					binary[35] = true;
				} else if(!strcmp((char*)mods->value, "H1")) {
					binary[36] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD3:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "H0")) {
					binary[33] = true;
				} else if(!strcmp((char*)mods->value, "H1")) {
					binary[34] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD3:2.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[3]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "H0")) {
					binary[31] = true;
				} else if(!strcmp((char*)mods->value, "H1")) {
					binary[32] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD3:3.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction IADD3.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "X")) {
			} else if(!strcmp((char*)mods->value, "RS")) {
			} else if(!strcmp((char*)mods->value, "???3")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD3.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_VABSDIFF == inst->op) {
		binary[53] = true;
		binary[58] = true;
		binary[60] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_register) {
			checkrandmods = 2;
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1 * (inst->operands[2]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "ACC")) {
				} else if(!strcmp((char*)mods->value, "INVALIDSEL7") && !seenModType[1]) {
					seenModType[1] = true;
					binary[36] = true;
					binary[37] = true;
					binary[38] = true;
					binary[47] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "S16") && seenModType[1]) {
					binary[47] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "S16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[38] = true;
					binary[47] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "S8") && !seenModType[1]) {
					seenModType[1] = true;
					binary[37] = true;
					binary[47] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
					//binary[37] = true;
					//binary[38] = true;
					binary[47] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "S32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[37] = true;
					binary[38] = true;
					binary[47] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "U16") && seenModType[1]) {
					binary[37] = true;
					binary[38] = true;
					//binary[47] = true;
					//binary[48] = true;
					binary[49] = false;//not sure about this
				} else if(!strcmp((char*)mods->value, "U32") && seenModType[1]) {
					binary[37] = true;
					binary[38] = true;
					binary[47] = true;
				} else if(!strcmp((char*)mods->value, "S8") && seenModType[1]) {
					binary[37] = true;
					binary[38] = true;
					binary[47] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDSEL7") && seenModType[1]) {
					binary[37] = true;
					binary[38] = true;
					binary[47] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "S32") && seenModType[1]) {
					binary[47] = true;
					binary[49] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[37] = true;
				binary[38] = true;
				binary[48] = true;
				binary[49] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction VABSDIFF:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "B2")) {
					binary[38] = false;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction VABSDIFF:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register) {
			checkrandmods = 3;
			binary[50] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "ACC")) {
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[29] = true;
					binary[30] = true;
					binary[37] = true;
					binary[38] = true;
					binary[47] = true;
				} else if(!strcmp((char*)mods->value, "U32") && seenModType[1]) {
					binary[29] = true;
					binary[30] = true;
					binary[37] = true;
					binary[38] = true;
					binary[47] = true;
				} else if(!strcmp((char*)mods->value, "S32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[37] = true;
					binary[38] = true;
					binary[47] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "S8") && seenModType[1]) {
					binary[37] = true;
					binary[38] = true;
					binary[47] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDSEL7") && seenModType[1]) {
					binary[28] = true;
					binary[29] = true;
					binary[30] = true;
					binary[37] = true;
					binary[38] = true;
					binary[47] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "S16") && seenModType[1]) {
					binary[30] = true;
					binary[37] = true;
					binary[38] = true;
					binary[47] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDSEL7") && !seenModType[1]) {
					seenModType[1] = true;
					binary[29] = true;
					binary[30] = true;
					binary[36] = true;
					binary[37] = true;
					binary[38] = true;
					binary[47] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "S32") && seenModType[1]) {
					binary[29] = true;
					binary[30] = true;
					binary[47] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "S16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[29] = true;
					binary[30] = true;
					binary[38] = true;
					binary[47] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "S8") && !seenModType[1]) {
					seenModType[1] = true;
					binary[29] = true;
					binary[30] = true;
					binary[37] = true;
					binary[47] = true;
					binary[48] = true;
					binary[49] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[1]) {
				binary[29] = true;
				binary[30] = true;
				binary[37] = true;
				binary[38] = true;
				binary[48] = true;
				binary[49] = true;
				binary[50] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction VABSDIFF:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "B2")) {
					binary[38] = false;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction VABSDIFF:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "B2")) {
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction VABSDIFF:2.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction VABSDIFF.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "ACC")) {
			} else if(!strcmp((char*)mods->value, "INVALIDSEL7")) {
			} else if(!strcmp((char*)mods->value, "S16")) {
			} else if(!strcmp((char*)mods->value, "S16")) {
			} else if(!strcmp((char*)mods->value, "S8")) {
			} else if(!strcmp((char*)mods->value, "U32")) {
			} else if(!strcmp((char*)mods->value, "S32")) {
			} else if(!strcmp((char*)mods->value, "U16")) {
			} else if(!strcmp((char*)mods->value, "U32")) {
			} else if(!strcmp((char*)mods->value, "S8")) {
			} else if(!strcmp((char*)mods->value, "INVALIDSEL7")) {
			} else if(!strcmp((char*)mods->value, "S32")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction VABSDIFF.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_DEPBAR == inst->op) {
		binary[52] = true;
		binary[53] = true;
		binary[54] = true;
		binary[55] = true;
		binary[60] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_bit_list) {
			for(int randsize = 0; randsize < 6; randsize++) {
				binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 0) {
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_sb && inst->operands[1]->type == type_hex && inst->operands[2]->type == type_bit_list) {
			binary[29] = true;
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 26] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 6; randsize++) {
				binary[randsize + 20] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 6; randsize++) {
				binary[randsize + 0] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_sb && inst->operands[1]->type == type_hex) {
			binary[29] = true;
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 26] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 6; randsize++) {
				binary[randsize + 20] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction DEPBAR.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "LE")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DEPBAR.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_LOP3 == inst->op) {
		if(inst->num_operands == 6 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register && inst->operands[4]->type == type_register && inst->operands[5]->type == type_hex) {
			checkrandmods = 2;
			binary[53] = true;
			binary[54] = true;
			binary[55] = true;
			binary[56] = true;
			binary[57] = true;
			binary[59] = true;
			binary[60] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 48] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[4]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 28] = ((inst->operands[5]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LUT")) {
				} else if(!strcmp((char*)mods->value, "NZ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[36] = true;
					binary[37] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[36] = true;
				} else if(!strcmp((char*)mods->value, "Z") && !seenModType[3]) {
					seenModType[3] = true;
					binary[37] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[38] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				binary[53] = true;
				binary[54] = true;
				binary[55] = true;
				binary[56] = true;
				binary[57] = true;
				binary[59] = true;
				binary[60] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP3:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP3:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register && inst->operands[4]->type == type_hex) {
			checkrandmods = 1;
			binary[48] = true;
			binary[49] = true;
			binary[50] = true;
			binary[53] = true;
			binary[54] = true;
			binary[55] = true;
			binary[56] = true;
			binary[57] = true;
			binary[59] = true;
			binary[60] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 28] = ((inst->operands[4]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LUT")) {
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[36] = true;
				} else if(!strcmp((char*)mods->value, "Z") && !seenModType[3]) {
					seenModType[3] = true;
					binary[37] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[38] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				binary[48] = true;
				binary[49] = true;
				binary[50] = true;
				binary[53] = true;
				binary[54] = true;
				binary[55] = true;
				binary[56] = true;
				binary[57] = true;
				binary[59] = true;
				binary[60] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP3:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_register && inst->operands[4]->type == type_hex) {
			checkrandmods = 1;
			binary[58] = true;
			binary[59] = true;
			binary[60] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			binary[56] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1 * (inst->operands[2]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 48] = ((inst->operands[4]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LUT")) {
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				//binary[55] = true;
				binary[58] = true;
				binary[59] = true;
				binary[60] = true;
				binary[61] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP3:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_register && inst->operands[4]->type == type_hex) {
			checkrandmods = 1;
			binary[57] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 9; randsize++) {
				binary[randsize + 48] = ((inst->operands[4]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LUT")) {
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				binary[51] = true;
				binary[54] = true;
				binary[57] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP3:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction LOP3.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "LUT")) {
			} else if(!strcmp((char*)mods->value, "NZ")) {
			} else if(!strcmp((char*)mods->value, "T")) {
			} else if(!strcmp((char*)mods->value, "Z")) {
			} else if(!strcmp((char*)mods->value, "X")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP3.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_TLDS == inst->op) {
		binary[57] = true;
		binary[59] = true;
		binary[60] = true;
		binary[62] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 28] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 7 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register && inst->operands[4]->type == type_hex && inst->operands[5]->type == type_texture_operand && inst->operands[6]->type == type_channel) {
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 13; randsize++) {
				binary[randsize + 36] = ((inst->operands[4]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 1; randsize++) {
				binary[randsize + 56] = ((inst->operands[5]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 50] = ((inst->operands[6]->val1) & (0x1 << randsize));
			}
			binary[53] = true;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LZ")) {
					binary[53] = false;
				} else if(!strcmp((char*)mods->value, "NODEP")) {
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "LL")) {
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "AOFFI")) {
					binary[55] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
				} else if(!strcmp((char*)mods->value, "P")) {
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 6 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_hex && inst->operands[4]->type == type_texture_operand && inst->operands[5]->type == type_channel) {
			binary[20] = true;
			binary[21] = true;
			binary[22] = true;
			binary[23] = true;
			binary[24] = true;
			binary[25] = true;
			binary[26] = true;
			binary[27] = true;
			for(int randsize = 0; randsize < 13; randsize++) {
				binary[randsize + 36] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 1; randsize++) {
				binary[randsize + 56] = ((inst->operands[4]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 50] = ((inst->operands[5]->val1) & (0x1 << randsize));
			}
			binary[53] = true;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LZ")) {
					binary[53] = false;
				} else if(!strcmp((char*)mods->value, "NODEP")) {
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
				} else if(!strcmp((char*)mods->value, "P")) {
				} else if(!strcmp((char*)mods->value, "LL")) {
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "AOFFI")) {
					binary[55] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				binary[20] = true;
				binary[21] = true;
				binary[22] = true;
				binary[23] = true;
				binary[24] = true;
				binary[25] = true;
				binary[26] = true;
				binary[27] = true;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction TLDS.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "LZ")) {
			} else if(!strcmp((char*)mods->value, "NODEP")) {
			} else if(!strcmp((char*)mods->value, "LL")) {
			} else if(!strcmp((char*)mods->value, "AOFFI")) {
			} else if(!strcmp((char*)mods->value, "T")) {
			} else if(!strcmp((char*)mods->value, "P")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction TLDS.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_TEXS == inst->op) {
		binary[59] = true;
		binary[60] = true;
		binary[62] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 28] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 6 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_hex && inst->operands[4]->type == type_texture_operand && inst->operands[5]->type == type_channel) {
			binary[20] = true;
			binary[21] = true;
			binary[22] = true;
			binary[23] = true;
			binary[24] = true;
			binary[25] = true;
			binary[26] = true;
			binary[27] = true;
			for(int randsize = 0; randsize < 13; randsize++) {
				binary[randsize + 36] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 1; randsize++) {
				binary[randsize + 56] = ((inst->operands[4]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 50] = ((inst->operands[5]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LZ")) {
				} else if(!strcmp((char*)mods->value, "NODEP")) {
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "DC")) {
					binary[55] = true;
				} else if(!strcmp((char*)mods->value, "P")) {
				} else if(!strcmp((char*)mods->value, "LL")) {
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 7 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register && inst->operands[4]->type == type_hex && inst->operands[5]->type == type_texture_operand && inst->operands[6]->type == type_channel) {
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 13; randsize++) {
				binary[randsize + 36] = ((inst->operands[4]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 1; randsize++) {
				binary[randsize + 56] = ((inst->operands[5]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 50] = ((inst->operands[6]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NODEP")) {
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "P")) {
				} else if(!strcmp((char*)mods->value, "LZ")) {
				} else if(!strcmp((char*)mods->value, "LL")) {
				} else if(!strcmp((char*)mods->value, "DC")) {
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction TEXS.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "LZ")) {
			} else if(!strcmp((char*)mods->value, "NODEP")) {
			} else if(!strcmp((char*)mods->value, "DC")) {
			} else if(!strcmp((char*)mods->value, "P")) {
			} else if(!strcmp((char*)mods->value, "LL")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction TEXS.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_LEA == inst->op) {
		binary[52] = true;
		binary[54] = true;
		binary[55] = true;
		binary[57] = true;
		binary[60] = true;
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_hex) {
			checkrandmods = 1;
			binary[48] = true;
			binary[49] = true;
			binary[50] = true;
			binary[56] = true;
			binary[59] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 6; randsize++) {
				binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "X")) {
					binary[46] = true;
				} else if(!strcmp((char*)mods->value, "HI") && !seenModType[3]) {
					seenModType[3] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				binary[48] = true;
				binary[49] = true;
				binary[50] = true;
				binary[56] = true;
				binary[59] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LEA:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register && inst->operands[4]->type == type_hex) {
			checkrandmods = 2;
			binary[56] = true;
			binary[59] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 4; randsize++) {
				binary[randsize + 48] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 6; randsize++) {
				binary[randsize + 39] = ((inst->operands[4]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "X")) {
					binary[46] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				binary[56] = true;
				binary[59] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LEA:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LEA:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register) {
			checkrandmods = 2;
			binary[56] = true;
			binary[59] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 4; randsize++) {
				binary[randsize + 48] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "X")) {
					binary[46] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				binary[56] = true;
				binary[59] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LEA:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LEA:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register && inst->operands[4]->type == type_hex) {
			checkrandmods = 1;
			binary[48] = true;
			binary[49] = true;
			binary[50] = true;
			binary[51] = true;
			binary[56] = true;
			binary[59] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			binary[37] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 6; randsize++) {
				binary[randsize + 28] = ((inst->operands[4]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "HI") && !seenModType[3]) {
					seenModType[3] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[38] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LEA:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			binary[48] = true;
			binary[49] = true;
			binary[50] = true;
			binary[51] = true;
			binary[56] = true;
			binary[59] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			binary[37] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 13; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "HI") && !seenModType[3]) {
					seenModType[3] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[38] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LEA:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 6 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register && inst->operands[4]->type == type_register && inst->operands[5]->type == type_hex) {
			checkrandmods = 2;
			binary[51] = true;
			binary[56] = true;
			binary[59] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 48] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			binary[37] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[4]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 28] = ((inst->operands[5]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "HI") && !seenModType[3]) {
					seenModType[3] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[38] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LEA:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LEA:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_hex) {
			checkrandmods = 1;
			binary[48] = true;
			binary[49] = true;
			binary[50] = true;
			binary[58] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "X")) {
					binary[46] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				binary[48] = true;
				binary[49] = true;
				binary[50] = true;
				binary[58] = true;
				binary[61] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LEA:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_hex) {
			checkrandmods = 2;
			binary[58] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 4; randsize++) {
				binary[randsize + 48] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 24; randsize++) {
				binary[randsize + 20] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "X")) {
					binary[46] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				binary[58] = true;
				binary[61] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LEA:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LEA:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_hex && inst->operands[4]->type == type_hex) {
			checkrandmods = 2;
			binary[58] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 4; randsize++) {
				binary[randsize + 48] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 39] = ((inst->operands[4]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "X")) {
					binary[46] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				binary[58] = true;
				binary[61] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LEA:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LEA:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 1;
			binary[48] = true;
			binary[49] = true;
			binary[50] = true;
			binary[58] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 24; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "X")) {
					binary[46] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				binary[48] = true;
				binary[49] = true;
				binary[50] = true;
				binary[58] = true;
				binary[61] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LEA:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register && inst->operands[4]->type == type_register) {
			checkrandmods = 2;
			binary[51] = true;
			binary[56] = true;
			binary[59] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 48] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			binary[37] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 13; randsize++) {
				binary[randsize + 20] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 39] = ((inst->operands[4]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "HI") && !seenModType[3]) {
					seenModType[3] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[38] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LEA:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LEA:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			checkrandmods = 1;
			binary[48] = true;
			binary[49] = true;
			binary[50] = true;
			binary[56] = true;
			binary[59] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "X")) {
					binary[46] = true;
				}
				mods = mods->next;
			}
			if(!seenModType[3]) {
				binary[48] = true;
				binary[49] = true;
				binary[50] = true;
				binary[56] = true;
				binary[59] = true;
				binary[62] = true;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LEA:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction LEA.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "X")) {
			} else if(!strcmp((char*)mods->value, "HI")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LEA.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_DSET == inst->op) {
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 0] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 8] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 39] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_predicate) {
			checkrandmods = 3;
			binary[54] = true;
			binary[57] = true;
			binary[60] = true;
			binary[61] = true;
			binary[43] = inst->operands[1]->properties & prop_minus;
			binary[52] = binary[53] = binary[55] = binary[56] = binary[58] = binary[59] = binary[62] = binary[63] = !(inst->operands[1]->properties & prop_absolute_value);
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[42] = inst->operands[3]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "EQ")) {
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "AND")) {
				} else if(!strcmp((char*)mods->value, "GTU")) {
					binary[50] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "OR")) {
					binary[45] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "XOR")) {
					binary[46] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "LE")) {
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "F")) {
				} else if(!strcmp((char*)mods->value, "GE")) {
					binary[49] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "LT")) {
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[48] = true;
					binary[49] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "NAN")) {
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "GT")) {
					binary[47] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "NE")) {
					binary[48] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "LTU")) {
					binary[48] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[49] = true;
					binary[51] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DSET:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DSET:1.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "ABS")) {
					binary[44] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DSET:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[56] = true;
			binary[59] = true;
			binary[62] = true;
			binary[43] = inst->operands[1]->properties & prop_minus;
			binary[44] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 20] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			binary[42] = inst->operands[3]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LE")) {
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "AND")) {
				} else if(!strcmp((char*)mods->value, "OR")) {
					binary[45] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "XOR")) {
					binary[46] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "EQ")) {
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "LT")) {
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[48] = true;
					binary[49] = true;
					binary[50] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DSET:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[56] = true;
			binary[59] = true;
			binary[60] = true;
			binary[62] = true;
			binary[43] = inst->operands[1]->properties & prop_minus;
			binary[54] = inst->operands[1]->properties & prop_absolute_value;
			binary[44] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 20] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[42] = inst->operands[3]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NAN")) {
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "AND")) {
				} else if(!strcmp((char*)mods->value, "LE")) {
					binary[47] = true;
					binary[48] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "EQ")) {
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "GT")) {
					binary[47] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "NE")) {
					binary[48] = true;
					binary[50] = true;
				} else if(!strcmp((char*)mods->value, "OR")) {
					binary[45] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "XOR")) {
					binary[46] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "LTU")) {
					binary[48] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[49] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "GTU")) {
					binary[50] = true;
					binary[51] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[47] = true;
				} else if(strcmp((char*)mods->value, "reuse")) {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DSET:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction DSET.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "EQ")) {
			} else if(!strcmp((char*)mods->value, "AND")) {
			} else if(!strcmp((char*)mods->value, "GTU")) {
			} else if(!strcmp((char*)mods->value, "OR")) {
			} else if(!strcmp((char*)mods->value, "XOR")) {
			} else if(!strcmp((char*)mods->value, "LE")) {
			} else if(!strcmp((char*)mods->value, "F")) {
			} else if(!strcmp((char*)mods->value, "GE")) {
			} else if(!strcmp((char*)mods->value, "LT")) {
			} else if(!strcmp((char*)mods->value, "NUM")) {
			} else if(!strcmp((char*)mods->value, "NAN")) {
			} else if(!strcmp((char*)mods->value, "GT")) {
			} else if(!strcmp((char*)mods->value, "NE")) {
			} else if(!strcmp((char*)mods->value, "LTU")) {
			} else if(!strcmp((char*)mods->value, "EQU")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DSET.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_BINCODE != inst->op) {
		cerr << "ERROR: Unimplemented instruction: " << opcodes[inst->op] << ".\n";
	}


	if(canGuard) {
		if(inst->guard) {
			for(int x = 0; x < 3; x++) {
				binary[x+guardLoc] = inst->guard->val1 & (0x1 << x);
			}
			if(inst->guard->properties & prop_not) {
				binary[guardLoc + 3] = 1;
			}
		} else {
			binary[guardLoc] = binary[guardLoc + 1] = binary[guardLoc + 2] = 1;
			binary[guardLoc + 3] = 0;
		}
	}

	for(int x = checkrandmods; x < inst->num_operands; x++) {
		node * mods = inst->operands[x]->mods;
		while(mods) {
			if(strcmp((char*)mods->value, "reuse")) {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction " << opcodes[inst->op] << ".\n";
			}
			mods = mods->next;
		}
	}
	
	if(inst->op == opcode_BINCODE) {
		char * binstr = (char*) inst->mods->value;
		if(binstr[0] == 'x') {
			bool * hex = hextobool(binstr + 1);
			for(int x = 0; x < 64; x++) {
				binary[x] = hex[x];
			}
			free(hex);
		} else {
			for(int x = 0; x < 64; x++) {
				binary[x] = (binstr[x] == '1');
			}
		}
	}
	
	//Convert binary to correct format:
	char * answer = (char*) malloc(17);
	answer[16] = 0;
	for(int x = 0; x < 64; x+=4) {
		int blah = 0;
		if(binary[60-x]) {
			blah++;
		}
		if(binary[61-x]) {
			blah+=2;
		}
		if(binary[62-x]) {
			blah+=4;
		}
		if(binary[63-x]) {
			blah+=8;
		}
		if(blah < 10) {
			int y = x + 32;
			if(x >= 32) {
				y = x - 32;
			}
			answer[y/4] = blah + '0';
		} else {
			int y = x + 32;
			if(x >= 32) {
				y = x - 32;
			}
			answer[y/4] = blah + 'a' - 10;
		}
	}
	return answer;
}

