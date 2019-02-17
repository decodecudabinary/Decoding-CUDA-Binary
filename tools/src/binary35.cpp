#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binary.hpp"
#include "binary35.hpp"
#include "common.hpp"
using namespace std;

char* instructionToHexString35(instruction * inst, int cuobjdump_version) {
	int checkrandmods = 0;
	int guardLoc = 18;
	bool binary[64];
	for(int x = 0; x < 64; x++) {
		binary[x] = 0;
	}
	bool canGuard = true;
	bool seenModType[7];
	for(int x = 0; x < 7; x++) {
		seenModType[x] = false;
	}

	if(opcode_MOV == inst->op) {
		binary[1] = true;
		binary[54] = true;
		binary[55] = true;
		binary[58] = true;
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_const_mem && inst->operands[2]->type == type_hex) {
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[1]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[1]->val2 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 4; randsize++) {
				binary[randsize + 42] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			binary[63] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 42] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register) {
			binary[42] = true;
			binary[43] = true;
			binary[44] = true;
			binary[45] = true;
			binary[63] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_const_mem) {
			binary[42] = true;
			binary[43] = true;
			binary[44] = true;
			binary[45] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[1]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[1]->val2 & (0x1 << randsize));
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction MOV.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction MOV.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_MOV32I == inst->op) {
		binary[1] = true;
		binary[58] = true;
		binary[60] = true;
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 32; randsize++) {
			binary[randsize + 23] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_hex && inst->operands[2]->type == type_hex) {
			for(int randsize = 0; randsize < 4; randsize++) {
				binary[randsize + 14] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_hex) {
			binary[14] = true;
			binary[15] = true;
			binary[16] = true;
			binary[17] = true;
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction MOV32I.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction MOV32I.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_LD == inst->op) {
		binary[55] = true;
		binary[62] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 32; randsize++) {
			binary[randsize + 23] = (inst->operands[1]->val2 & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_mem) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "E")) {
				} else if(!strcmp((char*)mods->value, "U8") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "64")) {
					binary[56] = true;
					binary[58] = true;
				} else if(!strcmp((char*)mods->value, "U16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[57] = true;
				} else if(!strcmp((char*)mods->value, "S16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[56] = true;
					binary[57] = true;
				} else if(!strcmp((char*)mods->value, "128")) {
					binary[57] = true;
					binary[58] = true;
				} else if(!strcmp((char*)mods->value, "CV")) {
					binary[57] = true;
					binary[58] = true;
					binary[59] = true;
					binary[60] = true;
				} else if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LD.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction LD.\n";
		}
		if(!seenModType[1]) {
			binary[58] = true;
		}
	} else if(opcode_LDU == inst->op) {
		cerr << "ERROR: unimplemented operation LDU.\n";
	} else if(opcode_LDL == inst->op) {
		binary[1] = true;
		binary[57] = true;
		binary[59] = true;
		binary[60] = true;
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 24; randsize++) {
			binary[randsize + 23] = (inst->operands[1]->val2 & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_mem) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else if(!strcmp((char*)mods->value, "64")) {
					binary[51] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "LU")) {
					binary[48] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "128")) {
					binary[52] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "CG")) {
					binary[22] = true;
					binary[47] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "U8") && !seenModType[1]) {
					seenModType[1] = true;
					binary[22] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LDL.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction LDL.\n";
		}
		if(!seenModType[1]) {
			binary[53] = true;
		}
	} else if(opcode_LDS == inst->op) {
		binary[1] = true;
		binary[54] = true;
		binary[57] = true;
		binary[59] = true;
		binary[60] = true;
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 24; randsize++) {
			binary[randsize + 23] = (inst->operands[1]->val2 & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_mem) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "128")) {
					binary[52] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "S16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[51] = true;
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "64")) {
					binary[51] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "U8") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LDS.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction LDS.\n";
		}
		if(!seenModType[1]) {
			binary[53] = true;
		}
	} else if(opcode_LDC == inst->op) {
		binary[1] = true;
		binary[55] = true;
		binary[58] = true;
		binary[59] = true;
		binary[60] = true;
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 16; randsize++) {
			binary[randsize + 23] = (inst->operands[1]->val3 & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 5; randsize++) {
			binary[randsize + 39] = (inst->operands[1]->val2 & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_const_mem) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "U16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "64")) {
					binary[51] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "S16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[51] = true;
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else if(!strcmp((char*)mods->value, "IL")) {
					binary[47] = true;
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "IS")) {
					binary[48] = true;
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "U8") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "128")) {
					binary[52] = true;
					binary[53] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LDC.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction LDC.\n";
		}
		if(!seenModType[1]) {
			binary[40] = true;
			binary[53] = true;
		}
	} else if(opcode_ST == inst->op) {
		binary[55] = true;
		binary[61] = true;
		binary[62] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 32; randsize++) {
			binary[randsize + 23] = (inst->operands[0]->val2 & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_mem && inst->operands[1]->type == type_register) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "E")) {
				} else if(!strcmp((char*)mods->value, "U8") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "64")) {
					binary[56] = true;
					binary[58] = true;
				} else if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else if(!strcmp((char*)mods->value, "U16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[57] = true;
				} else if(!strcmp((char*)mods->value, "128")) {
					binary[57] = true;
					binary[58] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ST.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction ST.\n";
		}
		if(!seenModType[1]) {
			binary[58] = true;
		}
	} else if(opcode_STL == inst->op) {
		binary[1] = true;
		binary[55] = true;
		binary[57] = true;
		binary[59] = true;
		binary[60] = true;
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 24; randsize++) {
			binary[randsize + 23] = (inst->operands[0]->val2 & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_mem && inst->operands[1]->type == type_register) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "64")) {
					binary[51] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else if(!strcmp((char*)mods->value, "128")) {
					binary[52] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "CG")) {
					binary[47] = true;
					binary[51] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "CS")) {
					binary[48] = true;
					binary[51] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDSIZE7")) {
					binary[51] = true;
					binary[52] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "S8") && !seenModType[1]) {
					seenModType[1] = true;
					binary[51] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction STL.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction STL.\n";
		}
		if(!seenModType[1]) {
			binary[53] = true;
		}
	} else if(opcode_STS == inst->op) {
		binary[1] = true;
		binary[54] = true;
		binary[55] = true;
		binary[57] = true;
		binary[59] = true;
		binary[60] = true;
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 24; randsize++) {
			binary[randsize + 23] = (inst->operands[0]->val2 & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_mem && inst->operands[1]->type == type_register) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "U16") && !seenModType[1]) {
					seenModType[1] = true;
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "64")) {
					binary[51] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else if(!strcmp((char*)mods->value, "128")) {
					binary[52] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "U8") && !seenModType[1]) {
					seenModType[1] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction STS.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction STS.\n";
		}
		if(!seenModType[1]) {
			binary[53] = true;
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
		binary[54] = true;
		binary[55] = true;
		binary[57] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 3;
			binary[0] = true;
			binary[63] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			binary[59] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "RM")) {
					binary[42] = true;
					binary[59] = true;
				} else if(!strcmp((char*)mods->value, "RP")) {
					binary[43] = true;
					binary[59] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[53] = true;
					binary[59] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
					binary[59] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FADD:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FADD:1.\n";
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[48] = true;
					binary[59] = true;
				} else if(!strcmp((char*)mods->value, "ABS")) {
					binary[23] = false;
					binary[52] = true;
					binary[53] = false;
					binary[59] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FADD:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			binary[48] = inst->operands[2]->properties & prop_minus;
			binary[52] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "RM")) {
					binary[42] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "RP")) {
					binary[43] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[48] = true;
					binary[53] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[28] = false;
					binary[48] = true;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FADD:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			binary[63] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			binary[48] = inst->operands[2]->properties & prop_minus;
			binary[52] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "RM")) {
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "RP")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[53] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FADD:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FADD.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "FTZ")) {
				binary[47] = true;
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "RM")) {
			} else if(!strcmp((char*)mods->value, "RP")) {
			} else if(!strcmp((char*)mods->value, "SAT")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FADD.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_FADD32I == inst->op) {
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 32; randsize++) {
			binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			binary[59] = inst->operands[1]->properties & prop_minus;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FADD32I.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FADD32I.\n";
		}
	} else if(opcode_FMUL == inst->op) {
		binary[54] = true;
		binary[56] = true;
		binary[57] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMUL:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 1;
			binary[0] = true;
			binary[63] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			binary[59] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[26] = false;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMUL:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			binary[63] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMUL:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FMUL.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "RM")) {
				binary[42] = true;
			} else if(!strcmp((char*)mods->value, "RP")) {
				binary[43] = true;
			} else if(!strcmp((char*)mods->value, "D2")) {
				binary[44] = true;
			} else if(!strcmp((char*)mods->value, "D4")) {
				binary[45] = true;
			} else if(!strcmp((char*)mods->value, "M8")) {
				binary[46] = true;
			} else if(!strcmp((char*)mods->value, "FTZ")) {
				binary[47] = true;
			} else if(!strcmp((char*)mods->value, "FMZ")) {
				binary[48] = true;
			} else if(!strcmp((char*)mods->value, "SAT")) {
				binary[53] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMUL.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_FMUL32I == inst->op) {
		binary[1] = true;
		binary[61] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 32; randsize++) {
			binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMUL32I.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FMUL32I.\n";
		}
	} else if(opcode_FFMA == inst->op) {
		binary[58] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			binary[1] = true;
			binary[59] = true;
			binary[62] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			binary[52] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FFMA:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			binary[1] = true;
			binary[59] = true;
			binary[62] = true;
			binary[63] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[52] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
					binary[52] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FFMA:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			binary[0] = true;
			binary[60] = true;
			binary[63] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			binary[59] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[52] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
					binary[52] = true;
					binary[59] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FFMA:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_const_mem) {
			checkrandmods = 1;
			binary[1] = true;
			binary[59] = true;
			binary[63] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 42] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[52] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[3]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[3]->val2 & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[26] = false;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FFMA:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FFMA.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "SAT")) {
				binary[53] = true;
			} else if(!strcmp((char*)mods->value, "RM")) {
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "RP")) {
				binary[55] = true;
			} else if(!strcmp((char*)mods->value, "RZ")) {
				binary[54] = true;
				binary[55] = true;
			} else if(!strcmp((char*)mods->value, "FTZ")) {
				binary[56] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FFMA.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_FSET == inst->op) {
		cerr << "ERROR: unimplemented operation FSET.\n";
	} else if(opcode_FSETP == inst->op) {
		binary[55] = true;
		binary[56] = true;
		binary[58] = true;
		binary[60] = true;
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 5] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 2] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 42] = ((inst->operands[4]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_register && inst->operands[3]->type == type_const_mem && inst->operands[4]->type == type_predicate) {
			binary[1] = true;
			binary[59] = true;
			binary[62] = true;
			binary[46] = inst->operands[2]->properties & prop_minus;
			binary[9] = inst->operands[2]->properties & prop_absolute_value;
			binary[8] = inst->operands[3]->properties & prop_minus;
			binary[47] = inst->operands[3]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[3]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[3]->val2 & (0x1 << randsize));
			}
			binary[45] = inst->operands[4]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "GEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[52] = true;
					binary[53] = true;
					binary[54] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[48] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[49] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "GTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[53] = true;
					binary[54] = true;
				} else if(!strcmp((char*)mods->value, "NEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[53] = true;
					binary[54] = true;
				} else if(!strcmp((char*)mods->value, "LEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[52] = true;
					binary[54] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "LTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[54] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register && inst->operands[4]->type == type_predicate) {
			binary[1] = true;
			binary[59] = true;
			binary[62] = true;
			binary[63] = true;
			binary[46] = inst->operands[2]->properties & prop_minus;
			binary[9] = inst->operands[2]->properties & prop_absolute_value;
			binary[8] = inst->operands[3]->properties & prop_minus;
			binary[47] = inst->operands[3]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[4]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[53] = true;
					binary[54] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "GEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[52] = true;
					binary[53] = true;
					binary[54] = true;
				} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "LEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[52] = true;
					binary[54] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "LTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[54] = true;
				} else if(!strcmp((char*)mods->value, "GTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[53] = true;
					binary[54] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_register && inst->operands[3]->type == type_hex && inst->operands[4]->type == type_predicate) {
			checkrandmods = 4;
			binary[0] = true;
			binary[61] = true;
			binary[63] = true;
			binary[46] = inst->operands[2]->properties & prop_minus;
			binary[9] = inst->operands[2]->properties & prop_absolute_value;
			binary[59] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[4]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[53] = true;
					binary[54] = true;
				} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "GEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[52] = true;
					binary[53] = true;
					binary[54] = true;
				} else if(!strcmp((char*)mods->value, "LEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[9] = true;
					binary[51] = true;
					binary[52] = true;
					binary[54] = true;
				} else if(!strcmp((char*)mods->value, "LTU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[54] = true;
					binary[59] = true;
				} else if(!strcmp((char*)mods->value, "NEU") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[53] = true;
					binary[54] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[9] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[9] = true;
					binary[49] = true;
					binary[50] = true;
					binary[53] = true;
					binary[54] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FSETP:0.\n";
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FSETP:1.\n";
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FSETP:2.\n";
				mods = mods->next;
			}
			mods = inst->operands[3]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[8] = true;
					binary[9] = true;
					binary[50] = true;
					binary[53] = true;
					binary[54] = true;
				} else if(!strcmp((char*)mods->value, "ABS")) {
					binary[9] = true;
					binary[47] = true;
					binary[50] = true;
					binary[53] = true;
					binary[54] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FSETP:3.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FSETP.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "GT")) {
			} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
				seenModType[2] = true;
			} else if(!strcmp((char*)mods->value, "GEU")) {
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "OR")) {
			} else if(!strcmp((char*)mods->value, "XOR")) {
			} else if(!strcmp((char*)mods->value, "FTZ")) {
				binary[50] = true;
			} else if(!strcmp((char*)mods->value, "NE") && !seenModType[3]) {
				seenModType[3] = true;
				binary[51] = true;
				binary[53] = true;
			} else if(!strcmp((char*)mods->value, "GE") && !seenModType[3]) {
				seenModType[3] = true;
				binary[52] = true;
				binary[53] = true;
			} else if(!strcmp((char*)mods->value, "F") && !seenModType[3]) {
				seenModType[3] = true;
			} else if(!strcmp((char*)mods->value, "GTU")) {
			} else if(!strcmp((char*)mods->value, "NEU")) {
			} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
				seenModType[3] = true;
				binary[51] = true;
			} else if(!strcmp((char*)mods->value, "LEU")) {
			} else if(!strcmp((char*)mods->value, "EQ")) {
			} else if(!strcmp((char*)mods->value, "LTU")) {
			} else if(!strcmp((char*)mods->value, "INVALIDBOP3")) {
				binary[48] = true;
				binary[49] = true;
				binary[50] = true;
				binary[51] = true;
				binary[53] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
				seenModType[3] = true;
				binary[48] = true;
				binary[50] = true;
				binary[51] = true;
				binary[52] = true;
				binary[53] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
				seenModType[3] = true;
				binary[9] = true;
				binary[50] = true;
				binary[51] = true;
				binary[52] = true;
				binary[59] = false;
			} else if(!strcmp((char*)mods->value, "NAN") && !seenModType[3]) {
				seenModType[3] = true;
				binary[9] = true;
				binary[50] = true;
				binary[54] = true;
				binary[59] = false;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FSETP.\n";
			}
			mods = mods->next;
		}
		if(!seenModType[2]) {
			binary[1] = true;
			binary[2] = true;
			binary[3] = true;
			binary[4] = true;
			binary[10] = true;
			binary[12] = true;
			binary[13] = true;
			binary[23] = true;
			binary[24] = true;
			binary[25] = true;
			binary[26] = true;
			binary[28] = true;
			binary[30] = true;
			binary[31] = true;
			binary[33] = true;
			binary[48] = true;
			binary[49] = true;
			binary[50] = true;
			binary[51] = true;
			binary[53] = true;
			binary[54] = true;
			binary[59] = true;
			binary[62] = true;
			binary[63] = true;
		}
	} else if(opcode_DSETP == inst->op) {
		binary[58] = true;
		binary[60] = true;
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 5] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 2] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 42] = ((inst->operands[4]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_register && inst->operands[3]->type == type_hex && inst->operands[4]->type == type_predicate) {
			checkrandmods = 4;
			binary[0] = true;
			binary[61] = true;
			binary[63] = true;
			binary[46] = inst->operands[2]->properties & prop_minus;
			binary[9] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[4]->properties & prop_not;
			node * mods = inst->operands[0]->mods;
			while(mods) {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DSETP:0.\n";
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DSETP:1.\n";
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DSETP:2.\n";
				mods = mods->next;
			}
			mods = inst->operands[3]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[8] = true;
					binary[9] = true;
					binary[51] = true;
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "ABS")) {
					binary[9] = true;
					binary[47] = true;
					binary[51] = true;
					binary[52] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DSETP:3.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_register && inst->operands[3]->type == type_const_mem && inst->operands[4]->type == type_predicate) {
			binary[1] = true;
			binary[59] = true;
			binary[62] = true;
			binary[46] = inst->operands[2]->properties & prop_minus;
			binary[9] = inst->operands[2]->properties & prop_absolute_value;
			binary[8] = inst->operands[3]->properties & prop_minus;
			binary[47] = inst->operands[3]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[3]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[3]->val2 & (0x1 << randsize));
			}
			binary[45] = inst->operands[4]->properties & prop_not;
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register && inst->operands[4]->type == type_predicate) {
			binary[1] = true;
			binary[59] = true;
			binary[62] = true;
			binary[63] = true;
			binary[46] = inst->operands[2]->properties & prop_minus;
			binary[9] = inst->operands[2]->properties & prop_absolute_value;
			binary[8] = inst->operands[3]->properties & prop_minus;
			binary[47] = inst->operands[3]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[4]->properties & prop_not;
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction DSETP.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
				seenModType[3] = true;
				binary[51] = true;
				binary[52] = true;
			} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
				seenModType[2] = true;
			} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
				seenModType[3] = true;
				binary[52] = true;
			} else if(!strcmp((char*)mods->value, "GTU") && !seenModType[3]) {
				seenModType[3] = true;
				binary[53] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
				seenModType[3] = true;
				binary[51] = true;
			} else if(!strcmp((char*)mods->value, "NEU") && !seenModType[3]) {
				seenModType[3] = true;
				binary[51] = true;
				binary[53] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "LEU") && !seenModType[3]) {
				seenModType[3] = true;
				binary[51] = true;
				binary[52] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
				seenModType[2] = true;
				binary[48] = true;
			} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
				seenModType[2] = true;
				binary[49] = true;
			} else if(!strcmp((char*)mods->value, "NUM") && !seenModType[3]) {
				seenModType[3] = true;
				binary[51] = true;
				binary[52] = true;
				binary[53] = true;
			} else if(!strcmp((char*)mods->value, "LTU") && !seenModType[3]) {
				seenModType[3] = true;
				binary[51] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "GEU") && !seenModType[3]) {
				seenModType[3] = true;
				binary[52] = true;
				binary[53] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "NAN") && !seenModType[3]) {
				seenModType[3] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
				seenModType[3] = true;
				binary[53] = true;
			} else if(!strcmp((char*)mods->value, "GE") && !seenModType[3]) {
				seenModType[3] = true;
				binary[52] = true;
				binary[53] = true;
			} else if(!strcmp((char*)mods->value, "NE") && !seenModType[3]) {
				seenModType[3] = true;
				binary[51] = true;
				binary[53] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DSETP.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_FCMP == inst->op) {
		binary[1] = true;
		binary[56] = true;
		binary[58] = true;
		binary[59] = true;
		binary[60] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_register) {
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register) {
			binary[63] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FCMP.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "FTZ")) {
				binary[50] = true;
			} else if(!strcmp((char*)mods->value, "NE") && !seenModType[3]) {
				seenModType[3] = true;
				binary[51] = true;
				binary[53] = true;
			} else if(!strcmp((char*)mods->value, "GE") && !seenModType[3]) {
				seenModType[3] = true;
				binary[52] = true;
				binary[53] = true;
			} else if(!strcmp((char*)mods->value, "F") && !seenModType[3]) {
				seenModType[3] = true;
			} else if(!strcmp((char*)mods->value, "GTU") && !seenModType[3]) {
				seenModType[3] = true;
				binary[53] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
				seenModType[3] = true;
				binary[51] = true;
			} else if(!strcmp((char*)mods->value, "LEU") && !seenModType[3]) {
				seenModType[3] = true;
				binary[51] = true;
				binary[52] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
				seenModType[3] = true;
				binary[51] = true;
				binary[52] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FCMP.\n";
			}
			mods = mods->next;
		}
		if(!seenModType[3]) {
			binary[27] = true;
			binary[53] = true;
		}
	} else if(opcode_MUFU == inst->op) {
		binary[1] = true;
		binary[58] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register) {
			binary[51] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "RCP")) {
					binary[25] = true;
				} else if(!strcmp((char*)mods->value, "RSQ")) {
					binary[23] = true;
					binary[25] = true;
				} else if(!strcmp((char*)mods->value, "EX2")) {
					binary[24] = true;
				} else if(!strcmp((char*)mods->value, "RCP64H")) {
					binary[24] = true;
					binary[25] = true;
				} else if(!strcmp((char*)mods->value, "LG2")) {
					binary[23] = true;
					binary[24] = true;
				} else if(!strcmp((char*)mods->value, "SIN")) {
					binary[23] = true;
				} else if(!strcmp((char*)mods->value, "COS")) {
				} else if(!strcmp((char*)mods->value, "INVALIDMUFUOPCODEE")) {
					binary[24] = true;
					binary[25] = true;
					binary[26] = true;
					binary[32] = true;
					binary[33] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDMUFUOPCODEF")) {
					binary[23] = true;
					binary[24] = true;
					binary[25] = true;
					binary[26] = true;
					binary[32] = true;
					binary[33] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDMUFUOPCODEC")) {
					binary[25] = true;
					binary[26] = true;
					binary[32] = true;
					binary[33] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDMUFUOPCODED")) {
					binary[23] = true;
					binary[25] = true;
					binary[26] = true;
					binary[32] = true;
					binary[33] = true;
				} else if(!strcmp((char*)mods->value, "RSQ64H")) {
					binary[23] = true;
					binary[24] = true;
					binary[25] = true;
					binary[32] = true;
					binary[33] = true;
				} else if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else if(!strcmp((char*)mods->value, "SAT")) {
					binary[24] = true;
					binary[25] = true;
					binary[26] = true;
					binary[27] = true;
					binary[31] = true;
					binary[32] = true;
					binary[33] = true;
					binary[53] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction MUFU.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction MUFU.\n";
		}
	} else if(opcode_DADD == inst->op) {
		binary[55] = true;
		binary[56] = true;
		binary[57] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			binary[48] = inst->operands[2]->properties & prop_minus;
			binary[52] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DADD:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 3;
			binary[0] = true;
			binary[63] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			binary[59] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[26] = false;
					binary[50] = true;
					binary[51] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DADD:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DADD:1.\n";
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[48] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "ABS")) {
					binary[24] = false;
					binary[51] = true;
					binary[52] = true;
					binary[53] = false;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DADD:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			binary[63] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			binary[48] = inst->operands[2]->properties & prop_minus;
			binary[52] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DADD:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction DADD.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "RM")) {
				binary[42] = true;
			} else if(!strcmp((char*)mods->value, "RP")) {
				binary[43] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DADD.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_DMUL == inst->op) {
		binary[58] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			binary[63] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[31] = true;
					binary[32] = true;
					binary[33] = true;
					binary[42] = true;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DMUL:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 1;
			binary[0] = true;
			binary[63] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			binary[59] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
					binary[59] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DMUL:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[25] = false;
					binary[50] = true;
				} else {
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
				binary[42] = true;
			} else if(!strcmp((char*)mods->value, "RP")) {
				binary[43] = true;
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "RZ")) {
				binary[42] = true;
				binary[43] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DMUL.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_DFMA == inst->op) {
		binary[55] = true;
		binary[56] = true;
		binary[57] = true;
		binary[60] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			binary[0] = true;
			binary[61] = true;
			binary[63] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[52] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DFMA:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_const_mem) {
			checkrandmods = 1;
			binary[1] = true;
			binary[59] = true;
			binary[63] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 42] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[52] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[3]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[3]->val2 & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[26] = false;
					binary[50] = true;
					binary[51] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DFMA:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			binary[1] = true;
			binary[59] = true;
			binary[62] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			binary[52] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DFMA:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			binary[1] = true;
			binary[59] = true;
			binary[62] = true;
			binary[63] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[52] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DFMA:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction DFMA.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "RM")) {
				binary[53] = true;
			} else if(!strcmp((char*)mods->value, "RZ")) {
				binary[53] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "RP")) {
				binary[54] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DFMA.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_IADD == inst->op) {
		binary[55] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			binary[63] = true;
			binary[52] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 1;
			binary[0] = true;
			binary[63] = true;
			binary[59] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1 * (inst->operands[2]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
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
				binary[46] = true;
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "SAT")) {
				binary[53] = true;
			} else if(!strcmp((char*)mods->value, "PO")) {
				binary[51] = true;
				binary[52] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_IADD32I == inst->op) {
		binary[0] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 32; randsize++) {
			binary[randsize + 23] = ((inst->operands[2]->val1 * (inst->operands[2]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IADD32I.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction IADD32I.\n";
		}
	} else if(opcode_IMNMX == inst->op) {
		binary[56] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[0] = true;
			binary[63] = true;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[3]->properties & prop_not;
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[29] = false;
					binary[50] = true;
					binary[51] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IMNMX:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			binary[63] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[3]->properties & prop_not;
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[45] = true;
					binary[50] = true;
					binary[51] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IMNMX:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			binary[45] = inst->operands[3]->properties & prop_not;
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
					binary[51] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IMNMX:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction IMNMX.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "XLO")) {
				binary[46] = true;
				binary[51] = true;
			} else if(!strcmp((char*)mods->value, "XMED")) {
				binary[47] = true;
				binary[51] = true;
			} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
				seenModType[1] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IMNMX.\n";
			}
			mods = mods->next;
		}
		if(!seenModType[1]) {
			binary[51] = true;
		}
	} else if(opcode_IMUL == inst->op) {
		binary[54] = true;
		binary[55] = true;
		binary[56] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[42] = true;
					binary[43] = true;
					binary[44] = true;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IMUL:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 1;
			binary[0] = true;
			binary[63] = true;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IMUL:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			binary[63] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[42] = true;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IMUL:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction IMUL.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "HI")) {
				binary[42] = true;
			} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
				seenModType[1] = true;
			} else if(!strcmp((char*)mods->value, "U32") && seenModType[1]) {
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "S32") && seenModType[1]) {
				binary[44] = true;
			} else if(!strcmp((char*)mods->value, "S32") && !seenModType[1]) {
				seenModType[1] = true;
				binary[43] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IMUL.\n";
			}
			mods = mods->next;
		}
		if(!seenModType[1]) {
			binary[43] = true;
			binary[44] = true;
		}
	} else if(opcode_IMUL32I == inst->op) {
		binary[1] = true;
		binary[59] = true;
		binary[61] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 32; randsize++) {
			binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "U32") && seenModType[1]) {
				} else if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IMUL32I.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction IMUL32I.\n";
		}
		if(!seenModType[1]) {
			binary[57] = true;
			binary[58] = true;
		}
	} else if(opcode_IMAD == inst->op) {
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			binary[0] = true;
			binary[61] = true;
			binary[63] = true;
			binary[55] = inst->operands[1]->properties & prop_minus;
			binary[59] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1 * (inst->operands[2]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
			binary[54] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IMAD:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			binary[1] = true;
			binary[60] = true;
			binary[62] = true;
			binary[55] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			binary[54] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IMAD:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			binary[1] = true;
			binary[60] = true;
			binary[62] = true;
			binary[63] = true;
			binary[55] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IMAD:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_const_mem) {
			checkrandmods = 1;
			binary[1] = true;
			binary[60] = true;
			binary[63] = true;
			binary[55] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 42] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[54] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[3]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[3]->val2 & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IMAD:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction IMAD.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
				seenModType[1] = true;
			} else if(!strcmp((char*)mods->value, "U32") && seenModType[1]) {
			} else if(!strcmp((char*)mods->value, "HI")) {
				binary[57] = true;
			} else if(!strcmp((char*)mods->value, "X")) {
				binary[52] = true;
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "S32") && seenModType[1]) {
				binary[56] = true;
			} else if(!strcmp((char*)mods->value, "SAT")) {
				binary[51] = true;
				binary[53] = true;
				binary[56] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction IMAD.\n";
			}
			mods = mods->next;
		}
		if(!seenModType[1]) {
			binary[51] = true;
			binary[56] = true;
		}
	} else if(opcode_ISCADD == inst->op) {
		binary[54] = true;
		binary[55] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 5; randsize++) {
			binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_hex) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			binary[52] = inst->operands[1]->properties & prop_minus;
			binary[51] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISCADD:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_hex) {
			checkrandmods = 3;
			binary[0] = true;
			binary[63] = true;
			binary[52] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISCADD:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISCADD:1.\n";
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[24] = false;
					binary[51] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISCADD:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_hex) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			binary[63] = true;
			binary[52] = inst->operands[1]->properties & prop_minus;
			binary[51] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISCADD:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction ISCADD.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISCADD.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_ISET == inst->op) {
		binary[1] = true;
		binary[55] = true;
		binary[57] = true;
		binary[59] = true;
		binary[60] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			binary[45] = inst->operands[3]->properties & prop_not;
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[46] = true;
					binary[51] = true;
					binary[52] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[46] = true;
					binary[50] = true;
					binary[51] = true;
					binary[52] = true;
					binary[54] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISET:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_predicate) {
			binary[51] = true;
			binary[52] = true;
			binary[63] = true;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 23] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 24] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 25] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 26] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 27] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 28] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 9; randsize++) {
				binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction ISET.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
				seenModType[3] = true;
				binary[51] = true;
				binary[52] = false;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "AND")) {
			} else if(!strcmp((char*)mods->value, "NE") && !seenModType[3]) {
				seenModType[3] = true;
				binary[52] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "X")) {
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "BF")) {
				binary[51] = true;
				binary[52] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
				seenModType[2] = true;
				binary[51] = true;
				binary[52] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
				seenModType[2] = true;
				binary[51] = true;
				binary[52] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
				seenModType[1] = true;
				binary[51] = false;
				binary[52] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
				seenModType[3] = true;
				binary[51] = true;
				binary[52] = true;
				binary[53] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "LT")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISET.\n";
			}
			mods = mods->next;
		}
		if(!seenModType[1]) {
			binary[25] = true;
			binary[27] = true;
			binary[51] = true;
		}
	} else if(opcode_ISETP == inst->op) {
		binary[56] = true;
		binary[57] = true;
		binary[60] = true;
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 5] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 2] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 42] = ((inst->operands[4]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_register && inst->operands[3]->type == type_const_mem && inst->operands[4]->type == type_predicate) {
			binary[1] = true;
			binary[59] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[3]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[3]->val2 & (0x1 << randsize));
			}
			binary[45] = inst->operands[4]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[52] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[49] = true;
					binary[51] = true;
					binary[53] = true;
					binary[54] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[52] = true;
					binary[53] = true;
					binary[54] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_register && inst->operands[3]->type == type_hex && inst->operands[4]->type == type_predicate) {
			binary[0] = true;
			binary[61] = true;
			binary[63] = true;
			binary[59] = inst->operands[3]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[3]->val1 * (inst->operands[3]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
			binary[45] = inst->operands[4]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[52] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[49] = true;
					binary[53] = true;
					binary[54] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[52] = true;
					binary[53] = true;
					binary[54] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register && inst->operands[4]->type == type_predicate) {
			binary[1] = true;
			binary[59] = true;
			binary[62] = true;
			binary[63] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[4]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[52] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[49] = true;
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[52] = true;
					binary[53] = true;
					binary[54] = true;
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction ISETP.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "GE") && !seenModType[3]) {
				seenModType[3] = true;
				binary[53] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
				seenModType[2] = true;
			} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
				seenModType[3] = true;
				binary[52] = true;
			} else if(!strcmp((char*)mods->value, "U32")) {
			} else if(!strcmp((char*)mods->value, "X")) {
				binary[46] = true;
			} else if(!strcmp((char*)mods->value, "LE")) {
			} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
				seenModType[3] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "OR")) {
			} else if(!strcmp((char*)mods->value, "NE") && !seenModType[3]) {
				seenModType[3] = true;
				binary[52] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
				seenModType[3] = true;
				binary[53] = true;
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "F") && !seenModType[3]) {
				seenModType[3] = true;
				binary[45] = true;
			} else if(!strcmp((char*)mods->value, "XOR")) {
			} else if(!strcmp((char*)mods->value, "T")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISETP.\n";
			}
			mods = mods->next;
		}
		if(!seenModType[1]) {
			binary[51] = true;
		}
	} else if(opcode_ICMP == inst->op) {
		binary[57] = true;
		binary[60] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_register) {
			binary[1] = true;
			binary[59] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[52] = true;
					binary[54] = true;
				} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[52] = true;
					binary[53] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_register) {
			binary[0] = true;
			binary[61] = true;
			binary[63] = true;
			binary[59] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1 * (inst->operands[2]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[52] = true;
					binary[54] = true;
				} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[52] = true;
					binary[53] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register) {
			binary[1] = true;
			binary[59] = true;
			binary[62] = true;
			binary[63] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "EQ") && !seenModType[3]) {
					seenModType[3] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
					seenModType[1] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "LT") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "LE") && !seenModType[3]) {
					seenModType[3] = true;
					binary[51] = true;
					binary[52] = true;
					binary[53] = true;
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction ICMP.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "NE") && !seenModType[3]) {
				seenModType[3] = true;
				binary[52] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "EQ")) {
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "LT")) {
			} else if(!strcmp((char*)mods->value, "U32")) {
			} else if(!strcmp((char*)mods->value, "GT") && !seenModType[3]) {
				seenModType[3] = true;
				binary[51] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
				seenModType[3] = true;
				binary[51] = true;
				binary[52] = true;
				binary[53] = true;
				binary[54] = true;
			} else if(!strcmp((char*)mods->value, "LE")) {
			} else if(!strcmp((char*)mods->value, "F") && !seenModType[3]) {
				seenModType[3] = true;
				binary[51] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ICMP.\n";
			}
			mods = mods->next;
		}
		if(!seenModType[1]) {
			binary[51] = true;
		}
	} else if(opcode_I2F == inst->op) {
		binary[1] = true;
		binary[54] = true;
		binary[55] = true;
		binary[56] = true;
		binary[58] = true;
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_const_mem) {
			checkrandmods = 2;
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[52] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[1]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[1]->val2 & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[11] = true;
					binary[13] = true;
					binary[15] = true;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2F:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "B1")) {
					binary[11] = true;
					binary[13] = true;
					binary[15] = true;
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "B2")) {
					binary[11] = true;
					binary[13] = true;
					binary[15] = true;
					binary[45] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2F:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register) {
			checkrandmods = 2;
			binary[63] = true;
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[52] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[11] = true;
					binary[13] = true;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2F:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "B1")) {
					binary[11] = true;
					binary[13] = true;
					binary[44] = true;
				} else if(!strcmp((char*)mods->value, "B2")) {
					binary[11] = true;
					binary[13] = true;
					binary[45] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2F:1.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction I2F.\n";
		}
		binary[11] = true;
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "F32") && !seenModType[1]) {
				seenModType[1] = true;
				binary[11] = true;
			} else if(!strcmp((char*)mods->value, "S32") && seenModType[1]) {
				binary[11] = true;
				binary[13] = true;
				binary[15] = true;
			} else if(!strcmp((char*)mods->value, "U32") && seenModType[1]) {
				binary[11] = true;
				binary[13] = true;
			} else if(!strcmp((char*)mods->value, "F64") && !seenModType[1]) {
				seenModType[1] = true;
				binary[10] = true;
				binary[11] = true;
				binary[13] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDFPSRC0")) {
				binary[11] = false;
				binary[13] = true;
			} else if(!strcmp((char*)mods->value, "S32") && !seenModType[1]) {
				seenModType[1] = true;
				binary[13] = true;
				binary[15] = true;
			} else if(!strcmp((char*)mods->value, "S64") && seenModType[1]) {
				binary[11] = true;
				binary[12] = true;
				binary[13] = true;
				binary[15] = true;
			} else if(!strcmp((char*)mods->value, "S8") && seenModType[1]) {
				binary[11] = true;
				binary[15] = true;
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "RM")) {
				binary[11] = true;
				binary[13] = true;
				binary[42] = true;
			} else if(!strcmp((char*)mods->value, "RP")) {
				binary[11] = true;
				binary[13] = true;
				binary[43] = true;
			} else if(!strcmp((char*)mods->value, "S16") && seenModType[1]) {
				binary[11] = true;
				binary[12] = true;
				binary[15] = true;
			} else if(!strcmp((char*)mods->value, "U64") && seenModType[1]) {
				binary[11] = true;
				binary[12] = true;
				binary[13] = true;
			} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
				seenModType[1] = true;
				binary[13] = true;
			} else if(!strcmp((char*)mods->value, "U8") && seenModType[1]) {
				binary[11] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2F.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_I2I == inst->op) {
		binary[57] = true;
		binary[58] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_const_mem) {
			checkrandmods = 2;
			binary[1] = true;
			binary[61] = true;
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[52] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[1]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[1]->val2 & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[11] = true;
					binary[13] = true;
					binary[14] = true;
					binary[15] = true;
					binary[50] = true;
					binary[52] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2I:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "B1")) {
					binary[11] = true;
					binary[13] = true;
					binary[14] = true;
					binary[15] = true;
					binary[44] = true;
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "B2")) {
					binary[11] = true;
					binary[13] = true;
					binary[14] = true;
					binary[15] = true;
					binary[45] = true;
					binary[52] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2I:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register) {
			checkrandmods = 2;
			binary[1] = true;
			binary[61] = true;
			binary[63] = true;
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[52] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[11] = true;
					binary[13] = true;
					binary[14] = true;
					binary[15] = true;
					binary[50] = true;
					binary[52] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2I:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "B1")) {
					binary[11] = true;
					binary[13] = true;
					binary[14] = true;
					binary[15] = true;
					binary[44] = true;
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "B2")) {
					binary[11] = true;
					binary[13] = true;
					binary[14] = true;
					binary[15] = true;
					binary[45] = true;
					binary[52] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2I:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_hex) {
			checkrandmods = 2;
			binary[0] = true;
			binary[59] = true;
			binary[63] = true;
			binary[1] = binary[54] = binary[55] = binary[56] = binary[60] = binary[61] = !(inst->operands[1]->properties & prop_minus);
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[1]->val1 * (inst->operands[1]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[11] = true;
					binary[13] = true;
					binary[14] = true;
					binary[15] = true;
					binary[30] = false;
					binary[48] = true;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2I:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "B1")) {
					binary[11] = true;
					binary[13] = true;
					binary[14] = true;
					binary[15] = true;
					binary[44] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "B2")) {
					binary[11] = true;
					binary[13] = true;
					binary[14] = true;
					binary[15] = true;
					binary[45] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "ABS")) {
					binary[11] = true;
					binary[13] = true;
					binary[14] = true;
					binary[15] = true;
					binary[24] = false;
					binary[48] = true;
					binary[52] = true;
					binary[53] = false;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2I:1.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction I2I.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "S32") && !seenModType[1]) {
				seenModType[1] = true;
				binary[11] = true;
				binary[14] = true;
			} else if(!strcmp((char*)mods->value, "S32") && seenModType[1]) {
				binary[13] = true;
				binary[15] = true;
			} else if(!strcmp((char*)mods->value, "S64") && !seenModType[1]) {
				seenModType[1] = true;
				binary[10] = true;
				binary[11] = true;
				binary[13] = true;
				binary[14] = true;
				binary[15] = true;
			} else if(!strcmp((char*)mods->value, "S8") && !seenModType[1]) {
				seenModType[1] = true;
				binary[13] = true;
				binary[14] = true;
				binary[15] = true;
			} else if(!strcmp((char*)mods->value, "S64") && seenModType[1]) {
				binary[11] = true;
				binary[12] = true;
				binary[13] = true;
				binary[14] = true;
				binary[15] = true;
			} else if(!strcmp((char*)mods->value, "S8") && seenModType[1]) {
				binary[11] = true;
				binary[14] = true;
				binary[15] = true;
			} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
				seenModType[1] = true;
				binary[11] = true;
			} else if(!strcmp((char*)mods->value, "U32") && seenModType[1]) {
				binary[11] = true;
				binary[13] = true;
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "SAT")) {
				binary[11] = true;
				binary[13] = true;
				binary[14] = true;
				binary[15] = true;
				binary[53] = true;
			} else if(!strcmp((char*)mods->value, "U8") && !seenModType[1]) {
				seenModType[1] = true;
				binary[12] = true;
			} else if(!strcmp((char*)mods->value, "U16") && seenModType[1]) {
				binary[12] = true;
			} else if(!strcmp((char*)mods->value, "S16") && seenModType[1]) {
				binary[11] = true;
				binary[12] = true;
				binary[14] = true;
				binary[15] = true;
			} else if(!strcmp((char*)mods->value, "U16") && !seenModType[1]) {
				seenModType[1] = true;
				binary[10] = true;
				binary[12] = true;
			} else if(!strcmp((char*)mods->value, "S16") && !seenModType[1]) {
				seenModType[1] = true;
				binary[10] = true;
				binary[12] = true;
				binary[14] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction I2I.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_F2I == inst->op) {
		binary[1] = true;
		binary[55] = true;
		binary[56] = true;
		binary[58] = true;
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_const_mem) {
			checkrandmods = 2;
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[52] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[1]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[1]->val2 & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[11] = true;
					binary[13] = true;
					binary[15] = true;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction F2I:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "H1")) {
					binary[11] = true;
					binary[13] = true;
					binary[15] = true;
					binary[44] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction F2I:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register) {
			checkrandmods = 2;
			binary[63] = true;
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[52] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[11] = true;
					binary[13] = true;
					binary[25] = false;
					binary[42] = true;
					binary[43] = true;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction F2I:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "H1")) {
					binary[11] = true;
					binary[13] = true;
					binary[42] = true;
					binary[43] = true;
					binary[44] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction F2I:1.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction F2I.\n";
		}
		binary[13] = true;
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
				seenModType[1] = true;
				binary[11] = true;
			} else if(!strcmp((char*)mods->value, "F32") && seenModType[1]) {
				binary[13] = true;
			} else if(!strcmp((char*)mods->value, "U64") && !seenModType[1]) {
				seenModType[1] = true;
				binary[10] = true;
				binary[11] = true;
				binary[13] = true;
			} else if(!strcmp((char*)mods->value, "U8") && !seenModType[1]) {
				seenModType[1] = true;
				binary[13] = true;
			} else if(!strcmp((char*)mods->value, "F64") && seenModType[1]) {
				binary[11] = true;
				binary[12] = true;
				binary[13] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDFPSRC0")) {
				binary[11] = true;
				binary[13] = false;
			} else if(!strcmp((char*)mods->value, "S32") && !seenModType[1]) {
				seenModType[1] = true;
				binary[11] = true;
				binary[13] = true;
				binary[14] = true;
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "FLOOR")) {
				binary[11] = true;
				binary[13] = true;
				binary[42] = true;
			} else if(!strcmp((char*)mods->value, "CEIL")) {
				binary[11] = true;
				binary[13] = true;
				binary[43] = true;
			} else if(!strcmp((char*)mods->value, "FTZ")) {
				binary[11] = true;
				binary[13] = true;
				binary[47] = true;
			} else if(!strcmp((char*)mods->value, "S64") && !seenModType[1]) {
				seenModType[1] = true;
				binary[10] = true;
				binary[11] = true;
				binary[12] = true;
				binary[13] = true;
				binary[14] = true;
				binary[42] = true;
			} else if(!strcmp((char*)mods->value, "TRUNC")) {
				binary[42] = true;
				binary[43] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction F2I.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_F2F == inst->op) {
		binary[1] = true;
		binary[54] = true;
		binary[56] = true;
		binary[58] = true;
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_const_mem) {
			checkrandmods = 2;
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[52] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[1]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[1]->val2 & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[10] = true;
					binary[11] = true;
					binary[13] = true;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction F2F:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "H1")) {
					binary[10] = true;
					binary[11] = true;
					binary[13] = true;
					binary[44] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction F2F:1.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register) {
			checkrandmods = 2;
			binary[63] = true;
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[52] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[10] = true;
					binary[11] = true;
					binary[13] = true;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction F2F:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "H1")) {
					binary[10] = true;
					binary[11] = true;
					binary[13] = true;
					binary[44] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction F2F:1.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction F2F.\n";
		}
		binary[13] = true;
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "F64") && !seenModType[1]) {
				seenModType[1] = true;
				binary[10] = true;
				binary[11] = true;
			} else if(!strcmp((char*)mods->value, "F32") && seenModType[1]) {
				binary[13] = true;
			} else if(!strcmp((char*)mods->value, "F32") && !seenModType[1]) {
				seenModType[1] = true;
				binary[11] = true;
				binary[13] = true;
			} else if(!strcmp((char*)mods->value, "F16") && !seenModType[1]) {
				seenModType[1] = true;
				binary[10] = true;
				binary[13] = true;
			} else if(!strcmp((char*)mods->value, "F64") && seenModType[1]) {
				binary[11] = true;
				binary[12] = true;
				binary[13] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDFPSRC0")) {
				binary[10] = true;
				binary[11] = true;
				binary[13] = false;
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "RM")) {
				binary[10] = true;
				binary[11] = true;
				binary[13] = true;
				binary[42] = true;
			} else if(!strcmp((char*)mods->value, "RP")) {
				binary[10] = true;
				binary[11] = true;
				binary[13] = true;
				binary[43] = true;
			} else if(!strcmp((char*)mods->value, "FTZ")) {
				binary[11] = true;
				binary[13] = true;
				binary[47] = true;
			} else if(!strcmp((char*)mods->value, "SAT")) {
				binary[11] = true;
				binary[13] = true;
				binary[53] = true;
			} else if(!strcmp((char*)mods->value, "FLOOR")) {
				binary[11] = true;
				binary[13] = true;
				binary[42] = true;
				binary[45] = true;
			} else if(!strcmp((char*)mods->value, "TRUNC")) {
				binary[11] = true;
				binary[13] = true;
				binary[42] = true;
				binary[43] = true;
				binary[45] = true;
			} else if(!strcmp((char*)mods->value, "ROUND")) {
				binary[11] = true;
				binary[13] = true;
				binary[45] = true;
			} else if(!strcmp((char*)mods->value, "RZ")) {
				binary[11] = true;
				binary[12] = true;
				binary[13] = true;
				binary[42] = true;
				binary[43] = true;
			} else if(!strcmp((char*)mods->value, "CEIL")) {
				binary[10] = true;
				binary[11] = true;
				binary[12] = true;
				binary[13] = true;
				binary[43] = true;
				binary[45] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction F2F.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_LOP == inst->op) {
		binary[57] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			binary[42] = inst->operands[1]->properties & prop_bitwise_complement;
			binary[43] = inst->operands[2]->properties & prop_bitwise_complement;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[43] = true;
					binary[44] = true;
					binary[45] = true;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 3;
			binary[0] = true;
			binary[63] = true;
			binary[42] = inst->operands[1]->properties & prop_bitwise_complement;
			binary[59] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1 * (inst->operands[2]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP:1.\n";
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "INV")) {
					binary[43] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			binary[63] = true;
			binary[42] = inst->operands[1]->properties & prop_bitwise_complement;
			binary[43] = inst->operands[2]->properties & prop_bitwise_complement;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[44] = true;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction LOP.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "PASS_B")) {
				binary[44] = true;
				binary[45] = true;
			} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
				seenModType[2] = true;
				binary[45] = true;
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
				seenModType[2] = true;
				binary[44] = true;
			} else if(!strcmp((char*)mods->value, "X")) {
				binary[46] = true;
			} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
				seenModType[2] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP.\n";
			}
			mods = mods->next;
		}
		if(!seenModType[2]) {
			binary[1] = true;
			binary[44] = true;
			binary[45] = true;
			binary[61] = true;
		}
	} else if(opcode_LOP32I == inst->op) {
		binary[61] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 32; randsize++) {
			binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[56] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[57] = true;
				} else if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LOP32I.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction LOP32I.\n";
		}
	} else if(opcode_SHL == inst->op) {
		binary[1] = true;
		binary[54] = true;
		binary[57] = true;
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 14; randsize++) {
			binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
		}
		for(int randsize = 0; randsize < 5; randsize++) {
			binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			checkrandmods = 1;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else if(!strcmp((char*)mods->value, "W")) {
					binary[42] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[46] = true;
					binary[48] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SHL.\n";
				}

				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[48] = true;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SHL:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction SHL.\n";
		}
	} else if(opcode_SHR == inst->op) {
		binary[54] = true;
		binary[56] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[42] = true;
					binary[43] = true;
					binary[44] = true;
					binary[50] = true;
					binary[51] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SHR:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 1;
			binary[0] = true;
			binary[63] = true;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
					binary[51] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SHR:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction SHR.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "W")) {
				binary[42] = true;
			} else if(!strcmp((char*)mods->value, "BREV")) {
				binary[43] = true;
			} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
				seenModType[1] = true;
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDSHRXMODE1")) {
				binary[46] = true;
				binary[51] = true;
			} else if(!strcmp((char*)mods->value, "X")) {
				binary[47] = true;
				binary[51] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SHR.\n";
			}
			mods = mods->next;
		}
		if(!seenModType[1]) {
			binary[51] = true;
		}
	} else if(opcode_BFE == inst->op) {
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BFE:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 1;
			binary[0] = true;
			binary[63] = true;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
					binary[51] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BFE:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction BFE.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
				seenModType[1] = true;
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "BREV")) {
				binary[43] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BFE.\n";
			}
			mods = mods->next;
		}
		if(!seenModType[1]) {
			binary[51] = true;
		}
	} else if(opcode_BFI == inst->op) {
		binary[0] = true;
		binary[55] = true;
		binary[56] = true;
		binary[57] = true;
		binary[58] = true;
		binary[60] = true;
		binary[61] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 19; randsize++) {
			binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BFI.\n";
				}

				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BFI:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction BFI.\n";
		}
	} else if(opcode_SEL == inst->op) {
		binary[56] = true;
		binary[58] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_predicate) {
			binary[1] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			binary[45] = inst->operands[3]->properties & prop_not;
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_predicate) {
			binary[1] = true;
			binary[61] = true;
			binary[63] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[3]->properties & prop_not;
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_predicate) {
			binary[0] = true;
			binary[63] = true;
			binary[59] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1 * (inst->operands[2]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
			binary[45] = inst->operands[3]->properties & prop_not;
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction SEL.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SEL.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_SCHI == inst->op) {
		canGuard = false;
		binary[59] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 18] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 26] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 34] = ((inst->operands[4]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 42] = ((inst->operands[5]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 9; randsize++) {
			binary[randsize + 50] = ((inst->operands[6]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 7 && inst->operands[0]->type == type_hex && inst->operands[1]->type == type_hex && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_hex && inst->operands[4]->type == type_hex && inst->operands[5]->type == type_hex && inst->operands[6]->type == type_hex) {
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
		binary[55] = true;
		binary[58] = true;
		binary[60] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_const_mem) {
			binary[7] = true;
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 23] = (inst->operands[0]->val3 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 39] = (inst->operands[0]->val2 & (0x1 << randsize));
			}
		} else if(inst->num_operands == 1 && inst->operands[0]->type == type_hex) {
			for(int randsize = 0; randsize < 24; randsize++) {
				binary[randsize + 23] = ((inst->operands[0]->val1 - (inst->size + inst->address)) & (0x1 << randsize));
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
		binary[57] = true;
		binary[60] = true;
		if(inst->num_operands == 2 && inst->operands[0]->type == type_other_operand && inst->operands[1]->type == type_hex) {
			checkrandmods = 1;
			for(int randsize = 0; randsize < 24; randsize++) {
				binary[randsize + 23] = ((inst->operands[1]->val1 - (inst->size + inst->address)) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LMT")) {
					binary[2] = true;
					binary[3] = true;
					binary[5] = true;
					binary[8] = true;
					binary[9] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LEU")) {
					binary[2] = true;
					binary[3] = true;
					binary[5] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[3] = true;
					binary[5] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "LTU")) {
					binary[2] = true;
					binary[5] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "LE")) {
					binary[2] = true;
					binary[3] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "FCSM_TA")) {
					binary[2] = true;
					binary[3] = true;
					binary[5] = true;
					binary[6] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "GEU")) {
					binary[2] = false;
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[2] = true;
					binary[3] = false;
					binary[4] = true;
					binary[5] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[5] = false;
					binary[6] = false;
					binary[9] = true;
					binary[23] = false;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
					binary[6] = true;
					binary[9] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BRA:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_other_operand && inst->operands[1]->type == type_const_mem) {
			checkrandmods = 1;
			binary[7] = true;
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 23] = (inst->operands[1]->val3 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 39] = (inst->operands[1]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LMT")) {
					binary[2] = true;
					binary[3] = true;
					binary[5] = true;
					binary[8] = true;
					binary[9] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GEU")) {
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[2] = true;
					binary[4] = true;
					binary[5] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "LEU")) {
					binary[2] = true;
					binary[3] = true;
					binary[5] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
					binary[6] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[2] = false;
					binary[3] = true;
					binary[5] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "LTU")) {
					binary[2] = true;
					binary[3] = false;
					binary[5] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "LE")) {
					binary[2] = true;
					binary[3] = true;
					binary[5] = false;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "FCSM_TA")) {
					binary[2] = true;
					binary[3] = true;
					binary[5] = true;
					binary[6] = true;
					binary[9] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BRA:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 1 && inst->operands[0]->type == type_const_mem) {
			binary[2] = true;
			binary[3] = true;
			binary[4] = true;
			binary[5] = true;
			binary[7] = true;
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 23] = (inst->operands[0]->val3 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 39] = (inst->operands[0]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LMT")) {
					binary[8] = true;
					binary[9] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 1 && inst->operands[0]->type == type_hex) {
			binary[2] = true;
			binary[3] = true;
			binary[4] = true;
			binary[5] = true;
			for(int randsize = 0; randsize < 24; randsize++) {
				binary[randsize + 23] = ((inst->operands[0]->val1 - (inst->size + inst->address)) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LMT")) {
					binary[8] = true;
					binary[9] = true;
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction BRA.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "U")) {
				binary[9] = true;
			} else if(!strcmp((char*)mods->value, "LMT")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BRA.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_BRX == inst->op) {
		binary[55] = true;
		binary[57] = true;
		binary[60] = true;
		if(inst->num_operands == 3 && inst->operands[0]->type == type_other_operand && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 1;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 24; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1 * (inst->operands[2]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GEU")) {
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
				} else if(!strcmp((char*)mods->value, "GTU")) {
					binary[4] = true;
					binary[5] = true;
					binary[8] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[3] = true;
					binary[5] = true;
				} else if(!strcmp((char*)mods->value, "GE")) {
					binary[3] = true;
					binary[4] = true;
					binary[8] = true;
				} else if(!strcmp((char*)mods->value, "RLE")) {
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
					binary[6] = true;
					binary[8] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[2] = true;
					binary[3] = false;
					binary[4] = true;
					binary[5] = true;
				} else if(!strcmp((char*)mods->value, "LEU")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = false;
					binary[5] = true;
					binary[25] = false;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[5] = false;
					binary[6] = false;
					binary[23] = false;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
					binary[6] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BRX:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_other_operand && inst->operands[1]->type == type_const_mem) {
			checkrandmods = 1;
			binary[7] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 23] = (inst->operands[1]->val3 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 39] = (inst->operands[1]->val2 & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "LEU")) {
					binary[2] = true;
					binary[3] = true;
					binary[5] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "GEU")) {
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[3] = true;
					binary[5] = true;
				} else if(!strcmp((char*)mods->value, "NAN")) {
					binary[5] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "EQ")) {
					binary[3] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "CSM_MX")) {
					binary[3] = true;
					binary[5] = true;
					binary[6] = true;
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[2] = true;
					binary[3] = false;
					binary[4] = true;
					binary[5] = true;
					binary[9] = true;
					binary[25] = false;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[5] = false;
					binary[6] = false;
					binary[9] = true;
					binary[24] = false;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
					binary[6] = true;
					binary[9] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BRX:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 1 && inst->operands[0]->type == type_const_mem) {
			binary[2] = true;
			binary[3] = true;
			binary[4] = true;
			binary[5] = true;
			binary[7] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 10] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 23] = (inst->operands[0]->val3 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 39] = (inst->operands[0]->val2 & (0x1 << randsize));
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_hex) {
			binary[2] = true;
			binary[3] = true;
			binary[4] = true;
			binary[5] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 10] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 24; randsize++) {
				binary[randsize + 23] = ((inst->operands[1]->val1 * (inst->operands[1]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction BRX.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "LMT")) {
				binary[8] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BRX.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_PCNT == inst->op) {
		canGuard = false;
		binary[55] = true;
		binary[56] = true;
		binary[58] = true;
		binary[60] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_const_mem) {
			binary[7] = true;
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 23] = (inst->operands[0]->val3 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 39] = (inst->operands[0]->val2 & (0x1 << randsize));
			}
		} else if(inst->num_operands == 1 && inst->operands[0]->type == type_hex) {
			for(int randsize = 0; randsize < 24; randsize++) {
				binary[randsize + 23] = ((inst->operands[0]->val1 - (inst->size + inst->address)) & (0x1 << randsize));
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
		binary[55] = true;
		binary[57] = true;
		binary[59] = true;
		binary[60] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_other_operand) {
			checkrandmods = 1;
			binary[29] = true;
			binary[31] = true;
			binary[32] = true;
			binary[33] = true;
			binary[35] = true;
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GEU")) {
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
					binary[23] = true;
				} else if(!strcmp((char*)mods->value, "GTU")) {
					binary[4] = true;
					binary[5] = true;
					binary[23] = true;
					binary[24] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[3] = true;
					binary[5] = true;
					binary[23] = true;
					binary[25] = true;
				} else if(!strcmp((char*)mods->value, "GE")) {
					binary[3] = true;
					binary[4] = true;
					binary[23] = true;
					binary[25] = true;
				} else if(!strcmp((char*)mods->value, "RLE")) {
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
					binary[6] = true;
					binary[23] = true;
					binary[24] = true;
					binary[25] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[2] = true;
					binary[3] = false;
					binary[4] = true;
					binary[5] = true;
					binary[26] = true;
					binary[27] = true;
					binary[28] = true;
				} else if(!strcmp((char*)mods->value, "LEU")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = false;
					binary[5] = true;
					binary[23] = true;
					binary[24] = false;
					binary[26] = true;
					binary[27] = true;
					binary[28] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[5] = false;
					binary[6] = false;
					binary[23] = false;
					binary[24] = true;
					binary[26] = true;
					binary[27] = true;
					binary[28] = true;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
					binary[6] = true;
					binary[23] = true;
					binary[24] = true;
					binary[26] = true;
					binary[27] = true;
					binary[28] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction CONT:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 0) {
			binary[2] = true;
			binary[3] = true;
			binary[4] = true;
			binary[5] = true;
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
		binary[56] = true;
		binary[58] = true;
		binary[60] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_const_mem) {
			binary[7] = true;
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 23] = (inst->operands[0]->val3 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 39] = (inst->operands[0]->val2 & (0x1 << randsize));
			}
		} else if(inst->num_operands == 1 && inst->operands[0]->type == type_hex) {
			for(int randsize = 0; randsize < 24; randsize++) {
				binary[randsize + 23] = ((inst->operands[0]->val1 - (inst->size + inst->address)) & (0x1 << randsize));
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
		binary[57] = true;
		binary[59] = true;
		binary[60] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_other_operand) {
			checkrandmods = 1;
			binary[34] = true;
			binary[35] = true;
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GEU")) {
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
				} else if(!strcmp((char*)mods->value, "GTU")) {
					binary[4] = true;
					binary[5] = true;
					binary[7] = true;
					binary[23] = true;
					binary[26] = true;
					binary[27] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[3] = true;
					binary[5] = true;
					binary[7] = true;
					binary[24] = true;
					binary[26] = true;
					binary[27] = true;
				} else if(!strcmp((char*)mods->value, "GE")) {
					binary[3] = true;
					binary[4] = true;
					binary[7] = true;
					binary[23] = true;
					binary[24] = true;
					binary[26] = true;
					binary[27] = true;
				} else if(!strcmp((char*)mods->value, "RLE")) {
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
					binary[6] = true;
					binary[7] = true;
					binary[25] = true;
					binary[26] = true;
					binary[27] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[2] = true;
					binary[3] = false;
					binary[4] = true;
					binary[5] = true;
					binary[24] = true;
					binary[25] = true;
					binary[26] = true;
					binary[29] = true;
				} else if(!strcmp((char*)mods->value, "LEU")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = false;
					binary[5] = true;
					binary[23] = true;
					binary[24] = true;
					binary[25] = true;
					binary[26] = true;
					binary[27] = false;
					binary[29] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[5] = false;
					binary[6] = false;
					binary[23] = false;
					binary[27] = true;
					binary[29] = true;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
					binary[6] = true;
					binary[23] = true;
					binary[27] = true;
					binary[29] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BRK:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 0) {
			binary[2] = true;
			binary[3] = true;
			binary[4] = true;
			binary[5] = true;
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
		binary[56] = true;
		binary[57] = true;
		binary[60] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_const_mem) {
			binary[7] = true;
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 23] = (inst->operands[0]->val3 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 39] = (inst->operands[0]->val2 & (0x1 << randsize));
			}
		} else if(inst->num_operands == 1 && inst->operands[0]->type == type_hex) {
			for(int randsize = 0; randsize < 24; randsize++) {
				binary[randsize + 23] = ((inst->operands[0]->val1 - (inst->size + inst->address)) & (0x1 << randsize));
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction CAL.\n";
		}
		binary[8] = true;
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "NOINC")) {
				binary[8] = false;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction CAL.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_RET == inst->op) {
		binary[56] = true;
		binary[59] = true;
		binary[60] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_other_operand) {
			checkrandmods = 1;
			binary[34] = true;
			binary[35] = true;
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GEU")) {
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
					binary[23] = true;
				} else if(!strcmp((char*)mods->value, "GTU")) {
					binary[4] = true;
					binary[5] = true;
					binary[23] = true;
					binary[24] = true;
					binary[25] = true;
					binary[26] = true;
					binary[28] = true;
					binary[29] = true;
					binary[30] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[3] = true;
					binary[5] = true;
					binary[23] = true;
					binary[25] = true;
					binary[26] = true;
					binary[27] = true;
					binary[28] = true;
					binary[29] = true;
					binary[30] = true;
				} else if(!strcmp((char*)mods->value, "GE")) {
					binary[3] = true;
					binary[4] = true;
					binary[23] = true;
					binary[25] = true;
					binary[26] = true;
					binary[27] = true;
					binary[28] = true;
					binary[29] = true;
					binary[30] = true;
				} else if(!strcmp((char*)mods->value, "RLE")) {
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
					binary[6] = true;
					binary[23] = true;
					binary[24] = true;
					binary[25] = true;
					binary[26] = true;
					binary[27] = true;
					binary[28] = true;
					binary[29] = true;
					binary[30] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[2] = true;
					binary[3] = false;
					binary[4] = true;
					binary[5] = true;
					binary[25] = true;
					binary[28] = true;
					binary[31] = true;
				} else if(!strcmp((char*)mods->value, "LEU")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = false;
					binary[5] = true;
					binary[23] = true;
					binary[24] = false;
					binary[25] = true;
					binary[28] = true;
					binary[31] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[5] = false;
					binary[6] = false;
					binary[23] = false;
					binary[24] = true;
					binary[25] = true;
					binary[28] = true;
					binary[31] = true;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
					binary[6] = true;
					binary[23] = true;
					binary[24] = true;
					binary[25] = true;
					binary[28] = true;
					binary[31] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction RET:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 0) {
			binary[2] = true;
			binary[3] = true;
			binary[4] = true;
			binary[5] = true;
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction RET.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction RET.\n";
			mods = mods->next;
		}
	} else if(opcode_EXIT == inst->op) {
		binary[59] = true;
		binary[60] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_other_operand) {
			checkrandmods = 1;
			binary[31] = true;
			binary[34] = true;
			binary[35] = true;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "KEEPREFCOUNT")) {
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
					binary[7] = true;
					binary[23] = true;
					binary[24] = true;
					binary[25] = true;
					binary[26] = true;
					binary[27] = true;
					binary[29] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GEU")) {
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
					binary[23] = true;
					binary[25] = true;
				} else if(!strcmp((char*)mods->value, "GTU")) {
					binary[4] = true;
					binary[5] = true;
					binary[23] = true;
					binary[24] = true;
					binary[25] = true;
					binary[26] = true;
					binary[27] = true;
					binary[29] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[3] = true;
					binary[5] = true;
					binary[23] = true;
					binary[24] = true;
					binary[25] = true;
					binary[26] = true;
					binary[27] = true;
					binary[29] = true;
				} else if(!strcmp((char*)mods->value, "GE")) {
					binary[3] = true;
					binary[4] = true;
					binary[23] = true;
					binary[24] = true;
					binary[25] = true;
					binary[26] = true;
					binary[27] = true;
					binary[29] = true;
				} else if(!strcmp((char*)mods->value, "RLE")) {
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
					binary[6] = true;
					binary[23] = true;
					binary[24] = true;
					binary[25] = true;
					binary[26] = true;
					binary[27] = true;
					binary[29] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[2] = true;
					binary[3] = false;
					binary[4] = true;
					binary[5] = true;
					binary[24] = true;
					binary[25] = true;
					binary[26] = true;
					binary[30] = true;
				} else if(!strcmp((char*)mods->value, "LEU")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = false;
					binary[5] = true;
					binary[23] = true;
					binary[24] = true;
					binary[25] = true;
					binary[26] = true;
					binary[27] = false;
					binary[30] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[5] = false;
					binary[6] = false;
					binary[23] = false;
					binary[27] = true;
					binary[30] = true;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[2] = true;
					binary[3] = true;
					binary[4] = true;
					binary[5] = true;
					binary[6] = true;
					binary[23] = true;
					binary[27] = true;
					binary[30] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction EXIT:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 0) {
			binary[2] = true;
			binary[3] = true;
			binary[4] = true;
			binary[5] = true;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "KEEPREFCOUNT")) {
					binary[7] = true;
					binary[24] = true;
					binary[27] = true;
					binary[30] = true;
					binary[31] = true;
					binary[34] = true;
					binary[35] = true;
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
		binary[1] = true;
		binary[55] = true;
		binary[56] = true;
		binary[58] = true;
		binary[63] = true;
		if(inst->num_operands == 1 && inst->operands[0]->type == type_hex) {
			binary[10] = true;
			binary[11] = true;
			binary[12] = true;
			binary[13] = true;
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 23] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_other_operand && inst->operands[1]->type == type_hex) {
			checkrandmods = 1;
			for(int randsize = 0; randsize < 16; randsize++) {
				binary[randsize + 23] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "GEU")) {
					binary[11] = true;
					binary[12] = true;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "GTU")) {
					binary[12] = true;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "EQU")) {
					binary[11] = true;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "GE")) {
					binary[11] = true;
					binary[12] = true;
				} else if(!strcmp((char*)mods->value, "RLE")) {
					binary[11] = true;
					binary[12] = true;
					binary[13] = true;
					binary[14] = true;
				} else if(!strcmp((char*)mods->value, "NEU")) {
					binary[10] = true;
					binary[11] = false;
					binary[12] = true;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "LEU")) {
					binary[10] = true;
					binary[11] = true;
					binary[12] = false;
					binary[13] = true;
				} else if(!strcmp((char*)mods->value, "NUM")) {
					binary[10] = true;
					binary[11] = true;
					binary[12] = true;
					binary[13] = false;
				} else if(!strcmp((char*)mods->value, "RGT")) {
					binary[10] = true;
					binary[11] = true;
					binary[12] = true;
					binary[13] = true;
					binary[14] = true;
					binary[25] = false;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction NOP:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 0) {
			binary[10] = true;
			binary[11] = true;
			binary[12] = true;
			binary[13] = true;
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction NOP.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "TRIG")) {
				binary[15] = true;
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction NOP.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_BAR == inst->op) {
		canGuard = false;
		binary[1] = true;
		binary[18] = true;
		binary[19] = true;
		binary[20] = true;
		binary[42] = true;
		binary[43] = true;
		binary[44] = true;
		binary[46] = true;
		binary[47] = true;
		binary[54] = true;
		binary[56] = true;
		binary[58] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 16; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 15; randsize++) {
			binary[randsize + 3] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 14; randsize++) {
			binary[randsize + 4] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 13; randsize++) {
			binary[randsize + 5] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 12; randsize++) {
			binary[randsize + 6] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 11; randsize++) {
			binary[randsize + 7] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 10; randsize++) {
			binary[randsize + 8] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 9; randsize++) {
			binary[randsize + 9] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 7; randsize++) {
			binary[randsize + 11] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 6; randsize++) {
			binary[randsize + 12] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 5; randsize++) {
			binary[randsize + 13] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 4; randsize++) {
			binary[randsize + 14] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 21; randsize++) {
			binary[randsize + 21] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 20; randsize++) {
			binary[randsize + 22] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 19; randsize++) {
			binary[randsize + 23] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 18; randsize++) {
			binary[randsize + 24] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 17; randsize++) {
			binary[randsize + 25] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 16; randsize++) {
			binary[randsize + 26] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 15; randsize++) {
			binary[randsize + 27] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 14; randsize++) {
			binary[randsize + 28] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 13; randsize++) {
			binary[randsize + 29] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 12; randsize++) {
			binary[randsize + 30] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 11; randsize++) {
			binary[randsize + 31] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 10; randsize++) {
			binary[randsize + 32] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 9; randsize++) {
			binary[randsize + 33] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 34] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 7; randsize++) {
			binary[randsize + 35] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 6; randsize++) {
			binary[randsize + 36] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 5; randsize++) {
			binary[randsize + 37] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 4; randsize++) {
			binary[randsize + 38] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 6; randsize++) {
			binary[randsize + 48] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 5; randsize++) {
			binary[randsize + 49] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 4; randsize++) {
			binary[randsize + 50] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 4; randsize++) {
			binary[randsize + 59] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 1 && inst->operands[0]->type == type_hex) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "SYNC")) {
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BAR.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction BAR.\n";
		}
	} else if(opcode_BPT == inst->op) {
		canGuard = false;
		for(int randsize = 0; randsize < 20; randsize++) {
			binary[randsize + 23] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 1 && inst->operands[0]->type == type_hex) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "DRAIN_ILLEGAL")) {
				} else if(!strcmp((char*)mods->value, "CAL")) {
					binary[8] = true;
				} else if(!strcmp((char*)mods->value, "PAUSE")) {
					binary[9] = true;
				} else if(!strcmp((char*)mods->value, "INT")) {
					binary[10] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction BPT.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction BPT.\n";
		}
	} else if(opcode_B2R == inst->op) {
		cerr << "ERROR: unimplemented operation B2R.\n";
	} else if(opcode_S2R == inst->op) {
		binary[1] = true;
		binary[54] = true;
		binary[57] = true;
		binary[58] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 23] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_special_reg) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction S2R.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction S2R.\n";
		}
	} else if(opcode_PSETP == inst->op) {
		binary[1] = true;
		binary[55] = true;
		binary[58] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 5] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 2] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 14] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 32] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 42] = ((inst->operands[4]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 5 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_predicate && inst->operands[3]->type == type_predicate && inst->operands[4]->type == type_predicate) {
			binary[17] = inst->operands[2]->properties & prop_not;
			binary[29] = binary[30] = binary[35] = inst->operands[3]->properties & prop_not;
			binary[45] = inst->operands[4]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "AND") && seenModType[2]) {
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[28] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[27] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && seenModType[2]) {
					binary[27] = true;
					binary[49] = true;
				} else if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else if(!strcmp((char*)mods->value, "OR") && seenModType[2]) {
					binary[27] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDBOP3")) {
					binary[27] = true;
					binary[28] = true;
					binary[29] = true;
					binary[30] = true;
					binary[35] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction PSETP.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction PSETP.\n";
		}
	} else if(opcode_PSET == inst->op) {
		binary[1] = true;
		binary[54] = true;
		binary[58] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 14] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 32] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_predicate && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[17] = inst->operands[1]->properties & prop_not;
			binary[35] = inst->operands[2]->properties & prop_not;
			binary[45] = inst->operands[3]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "AND") && seenModType[2]) {
				} else if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDBOP3")) {
					binary[27] = true;
					binary[28] = true;
					binary[35] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[27] = true;
					binary[31] = true;
					binary[35] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[28] = true;
					binary[31] = true;
					binary[35] = true;
				} else if(!strcmp((char*)mods->value, "BF")) {
					binary[23] = true;
					binary[25] = true;
					binary[26] = true;
					binary[28] = true;
					binary[31] = true;
					binary[35] = true;
					binary[47] = true;
				} else if(!strcmp((char*)mods->value, "OR") && seenModType[2]) {
					binary[24] = true;
					binary[25] = true;
					binary[26] = true;
					binary[28] = true;
					binary[31] = true;
					binary[35] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && seenModType[2]) {
					binary[23] = true;
					binary[24] = true;
					binary[25] = true;
					binary[26] = true;
					binary[28] = true;
					binary[31] = true;
					binary[35] = true;
					binary[49] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction PSET.\n";
				}

				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[27] = true;
					binary[28] = true;
					binary[31] = true;
					binary[35] = true;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction PSET:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction PSET.\n";
		}
	} else if(opcode_FLO == inst->op) {
		binary[1] = true;
		binary[55] = true;
		binary[56] = true;
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_const_mem) {
			checkrandmods = 1;
			binary[43] = inst->operands[1]->properties & prop_bitwise_complement;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[1]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[1]->val2 & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[10] = true;
					binary[11] = true;
					binary[14] = true;
					binary[42] = true;
					binary[43] = true;
					binary[44] = true;
					binary[50] = true;
					binary[51] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FLO:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register) {
			checkrandmods = 1;
			binary[63] = true;
			binary[43] = inst->operands[1]->properties & prop_bitwise_complement;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FLO:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FLO.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "SH")) {
				binary[44] = true;
			} else if(!strcmp((char*)mods->value, "U32") && !seenModType[1]) {
				seenModType[1] = true;
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FLO.\n";
			}
			mods = mods->next;
		}
		if(!seenModType[1]) {
			binary[25] = true;
			binary[26] = true;
			binary[32] = true;
			binary[34] = true;
			binary[51] = true;
		}
	} else if(opcode_P2R == inst->op) {
		binary[54] = true;
		binary[57] = true;
		binary[58] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_other_operand && inst->operands[2]->type == type_register && inst->operands[3]->type == type_const_mem) {
			binary[1] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[3]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[3]->val2 & (0x1 << randsize));
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_other_operand && inst->operands[2]->type == type_register && inst->operands[3]->type == type_hex) {
			binary[0] = true;
			binary[63] = true;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction P2R.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "H1")) {
				binary[42] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction P2R.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_R2P == inst->op) {
		binary[55] = true;
		binary[57] = true;
		binary[58] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_other_operand && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			binary[1] = true;
			binary[52] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_other_operand && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			binary[0] = true;
			binary[63] = true;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction R2P.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "H1")) {
				binary[42] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction R2P.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_TEX == inst->op) {
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 6 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_hex && inst->operands[4]->type == type_texture_operand && inst->operands[5]->type == type_hex) {
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 47] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 38] = ((inst->operands[4]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 4; randsize++) {
				binary[randsize + 34] = ((inst->operands[5]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NODEP")) {
					binary[1] = true;
					binary[31] = true;
					binary[32] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDPHASE3")) {
					binary[1] = true;
					binary[32] = true;
					binary[33] = true;
				} else if(!strcmp((char*)mods->value, "NDV")) {
					binary[1] = true;
					binary[32] = true;
					binary[33] = true;
					binary[41] = true;
				} else if(!strcmp((char*)mods->value, "DC")) {
					binary[1] = true;
					binary[32] = true;
					binary[33] = true;
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "AOFFI")) {
					binary[1] = true;
					binary[32] = true;
					binary[33] = true;
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "LB")) {
					binary[1] = true;
					binary[32] = true;
					binary[33] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDBLOD4")) {
					binary[1] = true;
					binary[32] = true;
					binary[33] = true;
					binary[46] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_texture_operand && inst->operands[4]->type == type_hex) {
			binary[0] = true;
			binary[23] = true;
			binary[24] = true;
			binary[25] = true;
			binary[26] = true;
			binary[27] = true;
			binary[28] = true;
			binary[29] = true;
			binary[30] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 47] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 38] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 4; randsize++) {
				binary[randsize + 34] = ((inst->operands[4]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NODEP")) {
					binary[31] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDPHASE3")) {
					binary[32] = true;
					binary[33] = true;
				} else if(!strcmp((char*)mods->value, "NDV")) {
					binary[41] = true;
				} else if(!strcmp((char*)mods->value, "DC")) {
					binary[42] = true;
				} else if(!strcmp((char*)mods->value, "AOFFI")) {
					binary[43] = true;
				} else if(!strcmp((char*)mods->value, "LB")) {
					binary[33] = true;
					binary[45] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDBLOD4")) {
					binary[33] = true;
					binary[46] = true;
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction TEX.\n";
		}
		binary[0] = true;
		binary[32] = true;
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "B")) {
				binary[0] = false;
				binary[1] = true;
			} else if(!strcmp((char*)mods->value, "NODEP")) {
			} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
				seenModType[3] = true;
				binary[32] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDPHASE3")) {
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "P")) {
				binary[32] = false;
				binary[33] = true;
			} else if(!strcmp((char*)mods->value, "NDV")) {
			} else if(!strcmp((char*)mods->value, "DC")) {
			} else if(!strcmp((char*)mods->value, "AOFFI")) {
			} else if(!strcmp((char*)mods->value, "LZ")) {
				binary[44] = true;
			} else if(!strcmp((char*)mods->value, "LB")) {
			} else if(!strcmp((char*)mods->value, "INVALIDBLOD4")) {
			} else if(!strcmp((char*)mods->value, "LL")) {
				binary[32] = true;
				binary[44] = true;
				binary[45] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDBLOD5")) {
				binary[32] = true;
				binary[44] = true;
				binary[46] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction TEX.\n";
			}
			mods = mods->next;
		}
		if(!seenModType[3]) {
			binary[26] = true;
			binary[33] = true;
		}
	} else if(opcode_TEXDEPBAR == inst->op) {
		binary[1] = true;
		binary[56] = true;
		binary[57] = true;
		binary[58] = true;
		binary[60] = true;
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 6; randsize++) {
			binary[randsize + 23] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 1 && inst->operands[0]->type == type_hex) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction TEXDEPBAR.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction TEXDEPBAR.\n";
		}
	} else if(opcode_RRO == inst->op) {
		binary[1] = true;
		binary[55] = true;
		binary[58] = true;
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_const_mem) {
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[52] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[1]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[1]->val2 & (0x1 << randsize));
			}
		} else if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register) {
			binary[63] = true;
			binary[48] = inst->operands[1]->properties & prop_minus;
			binary[52] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction RRO.\n";
		}
		binary[42] = true;
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "SINCOS")) {
				binary[42] = false;
			} else if(!strcmp((char*)mods->value, "EX2")) {
				binary[42] = true;
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction RRO.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_PRMT == inst->op) {
		binary[0] = true;
		binary[57] = true;
		binary[58] = true;
		binary[60] = true;
		binary[61] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 19; randsize++) {
			binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_register) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else if(!strcmp((char*)mods->value, "F4E")) {
					binary[51] = true;
				} else if(!strcmp((char*)mods->value, "B4E")) {
					binary[52] = true;
				} else if(!strcmp((char*)mods->value, "ECL")) {
					binary[53] = true;
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
		binary[1] = true;
		binary[55] = true;
		binary[57] = true;
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[51] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			binary[48] = inst->operands[2]->properties & prop_minus;
			binary[52] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			binary[45] = inst->operands[3]->properties & prop_not;
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[48] = true;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DMNMX:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[63] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			binary[48] = inst->operands[2]->properties & prop_minus;
			binary[52] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[3]->properties & prop_not;
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[45] = true;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DMNMX:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction DMNMX.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction DMNMX.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_FMNMX == inst->op) {
		binary[56] = true;
		binary[57] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			binary[48] = inst->operands[2]->properties & prop_minus;
			binary[52] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			binary[45] = inst->operands[3]->properties & prop_not;
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMNMX:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_predicate) {
			checkrandmods = 3;
			binary[0] = true;
			binary[63] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[3]->properties & prop_not;
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMNMX:0.\n";
				}
				mods = mods->next;
			}
			mods = inst->operands[1]->mods;
			while(mods) {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMNMX:1.\n";
				mods = mods->next;
			}
			mods = inst->operands[2]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "NEG")) {
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "ABS")) {
					binary[26] = false;
					binary[52] = true;
					binary[53] = false;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMNMX:2.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_predicate) {
			checkrandmods = 1;
			binary[1] = true;
			binary[61] = true;
			binary[63] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			binary[48] = inst->operands[2]->properties & prop_minus;
			binary[52] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[3]->properties & prop_not;
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[45] = true;
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMNMX:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FMNMX.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "FTZ")) {
				binary[47] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FMNMX.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_RED == inst->op) {
		cerr << "ERROR: unimplemented operation RED.\n";
	} else if(opcode_VOTE == inst->op) {
		binary[1] = true;
		binary[54] = true;
		binary[55] = true;
		binary[57] = true;
		binary[58] = true;
		binary[63] = true;
		if(inst->num_operands == 2 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate) {
			binary[2] = true;
			binary[3] = true;
			binary[4] = true;
			binary[5] = true;
			binary[6] = true;
			binary[7] = true;
			binary[8] = true;
			binary[9] = true;
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 48] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[1]->properties & prop_not;
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 42] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "ALL")) {
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_predicate) {
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 48] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			binary[45] = inst->operands[2]->properties & prop_not;
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 42] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			binary[51] = true;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "ALL")) {
					binary[32] = true;
					binary[33] = true;
					binary[34] = true;
					binary[35] = true;
					binary[51] = false;
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction VOTE.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "ALL")) {
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "ANY")) {
				binary[51] = true;
			} else if(!strcmp((char*)mods->value, "EQ")) {
				binary[23] = true;
				binary[27] = true;
				binary[28] = true;
				binary[29] = true;
				binary[30] = true;
				binary[32] = true;
				binary[33] = true;
				binary[34] = true;
				binary[35] = true;
				binary[52] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDVMODE3")) {
				binary[24] = true;
				binary[25] = true;
				binary[28] = true;
				binary[31] = true;
				binary[32] = true;
				binary[33] = true;
				binary[34] = true;
				binary[35] = true;
				binary[51] = true;
				binary[52] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction VOTE.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_POPC == inst->op) {
		binary[1] = true;
		binary[54] = true;
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			binary[42] = inst->operands[1]->properties & prop_bitwise_complement;
			binary[43] = inst->operands[2]->properties & prop_bitwise_complement;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			binary[63] = true;
			binary[42] = inst->operands[1]->properties & prop_bitwise_complement;
			binary[43] = inst->operands[2]->properties & prop_bitwise_complement;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction POPC.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction POPC.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_MEMBAR == inst->op) {
		binary[1] = true;
		binary[24] = true;
		binary[25] = true;
		binary[31] = true;
		binary[32] = true;
		binary[54] = true;
		binary[55] = true;
		binary[58] = true;
		binary[59] = true;
		binary[60] = true;
		binary[61] = true;
		binary[62] = true;
		if(inst->num_operands == 0) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CTA")) {
					binary[11] = false;
					binary[27] = true;
					binary[39] = true;
					binary[40] = true;
				} else if(!strcmp((char*)mods->value, "SYS")) {
					binary[11] = true;
					binary[26] = true;
					binary[28] = true;
					binary[30] = true;
					binary[33] = true;
					binary[34] = true;
					binary[35] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDMEMBAR3")) {
					binary[4] = true;
					binary[5] = true;
					binary[10] = true;
					binary[11] = true;
					binary[12] = true;
					binary[23] = true;
					binary[26] = true;
					binary[27] = true;
					binary[28] = true;
					binary[30] = true;
					binary[33] = true;
					binary[34] = true;
					binary[35] = true;
					binary[39] = true;
					binary[40] = true;
					binary[53] = true;
				} else if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction MEMBAR.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction MEMBAR.\n";
		}
	} else if(opcode_STSCUL == inst->op) {
		cerr << "ERROR: unimplemented operation STSCUL.\n";
	} else if(opcode_LEPC == inst->op) {
		cerr << "ERROR: unimplemented operation LEPC.\n";
	} else if(opcode_CSETP == inst->op) {
		binary[1] = true;
		binary[56] = true;
		binary[58] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 5] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 2] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_predicate && inst->operands[2]->type == type_other_operand && inst->operands[3]->type == type_predicate) {
			binary[45] = inst->operands[3]->properties & prop_not;
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "OFF") && !seenModType[3]) {
					seenModType[3] = true;
					binary[14] = true;
				} else if(!strcmp((char*)mods->value, "AND") && !seenModType[2]) {
					seenModType[2] = true;
				} else if(!strcmp((char*)mods->value, "LO") && !seenModType[3]) {
					seenModType[3] = true;
					binary[10] = true;
					binary[14] = true;
				} else if(!strcmp((char*)mods->value, "SFF") && !seenModType[3]) {
					seenModType[3] = true;
					binary[11] = true;
					binary[14] = true;
					binary[23] = true;
					binary[24] = true;
					binary[27] = true;
					binary[29] = true;
					binary[30] = true;
					binary[31] = true;
					binary[32] = true;
					binary[33] = true;
					binary[34] = true;
					binary[35] = true;
				} else if(!strcmp((char*)mods->value, "HI") && !seenModType[3]) {
					seenModType[3] = true;
					binary[12] = true;
					binary[14] = true;
					binary[25] = true;
					binary[27] = true;
					binary[29] = true;
					binary[30] = true;
					binary[31] = true;
					binary[32] = true;
					binary[33] = true;
					binary[34] = true;
					binary[35] = true;
				} else if(!strcmp((char*)mods->value, "CSM_TA") && !seenModType[3]) {
					seenModType[3] = true;
					binary[13] = true;
					binary[14] = true;
					binary[23] = true;
					binary[25] = true;
					binary[27] = true;
					binary[29] = true;
					binary[30] = true;
					binary[31] = true;
					binary[32] = true;
					binary[33] = true;
					binary[34] = true;
					binary[35] = true;
				} else if(!strcmp((char*)mods->value, "F") && !seenModType[3]) {
					seenModType[3] = true;
					binary[24] = true;
					binary[25] = true;
					binary[27] = true;
					binary[29] = true;
					binary[30] = true;
					binary[31] = true;
					binary[32] = true;
					binary[33] = true;
					binary[34] = true;
					binary[35] = true;
				} else if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else if(!strcmp((char*)mods->value, "OR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[14] = true;
					binary[26] = true;
					binary[27] = true;
					binary[28] = true;
					binary[29] = true;
					binary[30] = true;
					binary[31] = true;
					binary[32] = true;
					binary[33] = true;
					binary[34] = true;
					binary[35] = true;
					binary[48] = true;
				} else if(!strcmp((char*)mods->value, "XOR") && !seenModType[2]) {
					seenModType[2] = true;
					binary[14] = true;
					binary[23] = true;
					binary[26] = true;
					binary[27] = true;
					binary[28] = true;
					binary[29] = true;
					binary[30] = true;
					binary[31] = true;
					binary[32] = true;
					binary[33] = true;
					binary[34] = true;
					binary[35] = true;
					binary[49] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction CSETP.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction CSETP.\n";
		}
	} else if(opcode_ISCADD32I == inst->op) {
		binary[61] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 32; randsize++) {
			binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 5; randsize++) {
			binary[randsize + 56] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_hex) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISCADD32I.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction ISCADD32I.\n";
		}
	} else if(opcode_VMNMX == inst->op) {
		cerr << "ERROR: unimplemented operation VMNMX.\n";
	} else if(opcode_TLD == inst->op) {
		binary[1] = true;
		binary[60] = true;
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 6 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_hex && inst->operands[4]->type == type_texture_operand && inst->operands[5]->type == type_hex) {
			for(int randsize = 0; randsize < 9; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 15; randsize++) {
				binary[randsize + 45] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 4; randsize++) {
				binary[randsize + 38] = ((inst->operands[4]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 4; randsize++) {
				binary[randsize + 34] = ((inst->operands[5]->val1) & (0x1 << randsize));
			}
		} else if(inst->num_operands == 5 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_texture_operand && inst->operands[4]->type == type_hex) {
			binary[23] = true;
			binary[24] = true;
			binary[25] = true;
			binary[26] = true;
			binary[27] = true;
			binary[28] = true;
			binary[29] = true;
			binary[30] = true;
			for(int randsize = 0; randsize < 15; randsize++) {
				binary[randsize + 45] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 3; randsize++) {
				binary[randsize + 38] = ((inst->operands[3]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 34] = ((inst->operands[4]->val1) & (0x1 << randsize));
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction TLD.\n";
		}
		binary[44] = true;
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "LZ")) {
				binary[44] = false;
			} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
				seenModType[3] = true;
				binary[32] = true;
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "CL")) {
				binary[32] = true;
				binary[42] = true;
			} else if(!strcmp((char*)mods->value, "LL")) {
				binary[32] = true;
				binary[44] = true;
			} else if(!strcmp((char*)mods->value, "P")) {
				binary[33] = true;
			} else if(!strcmp((char*)mods->value, "NODEP")) {
				binary[32] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDPHASE3")) {
				binary[32] = true;
				binary[33] = true;
			} else if(!strcmp((char*)mods->value, "AOFFI")) {
				binary[32] = true;
			} else if(!strcmp((char*)mods->value, "MS")) {
				binary[32] = true;
				binary[43] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction TLD.\n";
			}
			mods = mods->next;
		}
		if(!seenModType[3]) {
			binary[24] = true;
			binary[25] = true;
			binary[26] = true;
			binary[27] = true;
			binary[28] = true;
			binary[29] = true;
			binary[30] = true;
			binary[34] = true;
			binary[47] = true;
			binary[49] = true;
			binary[51] = true;
		}
	} else if(opcode_SHF == inst->op) {
		binary[54] = true;
		binary[55] = true;
		binary[56] = true;
		binary[57] = true;
		binary[58] = true;
		binary[63] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 42] = ((inst->operands[3]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			binary[1] = true;
			binary[62] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "L") && !seenModType[5]) {
					seenModType[5] = true;
					binary[59] = true;
					binary[60] = true;
				} else if(!strcmp((char*)mods->value, "R") && !seenModType[5]) {
					seenModType[5] = true;
					binary[61] = true;
				} else if(!strcmp((char*)mods->value, "S64")) {
					binary[40] = true;
					binary[41] = true;
					binary[61] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDMAXSHIFT3")) {
					binary[36] = true;
					binary[40] = true;
					binary[59] = true;
					binary[60] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[36] = true;
					binary[52] = true;
					binary[59] = true;
					binary[60] = true;
				} else if(!strcmp((char*)mods->value, "W")) {
					binary[36] = true;
					binary[53] = true;
					binary[59] = true;
					binary[60] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[36] = true;
					binary[50] = true;
					binary[59] = true;
					binary[60] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SHF:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 4 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex && inst->operands[3]->type == type_register) {
			checkrandmods = 1;
			binary[0] = true;
			for(int randsize = 0; randsize < 6; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "L") && !seenModType[5]) {
					seenModType[5] = true;
					binary[60] = true;
					binary[61] = true;
				} else if(!strcmp((char*)mods->value, "W")) {
					binary[53] = true;
					binary[60] = true;
					binary[61] = true;
				} else if(!strcmp((char*)mods->value, "R") && !seenModType[5]) {
					seenModType[5] = true;
					binary[62] = true;
				} else if(!strcmp((char*)mods->value, "S64")) {
					binary[40] = true;
					binary[41] = true;
					binary[62] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDMAXSHIFT3")) {
					binary[36] = true;
					binary[40] = true;
					binary[60] = true;
					binary[61] = true;
				} else if(!strcmp((char*)mods->value, "X")) {
					binary[31] = true;
					binary[36] = true;
					binary[52] = true;
					binary[60] = true;
					binary[61] = true;
				}
				mods = mods->next;
			}
			mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[31] = true;
					binary[36] = true;
					binary[50] = true;
					binary[60] = true;
					binary[61] = true;
				} else {
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
			} else if(!strcmp((char*)mods->value, "R")) {
			} else if(!strcmp((char*)mods->value, "U64")) {
				binary[41] = true;
			} else if(!strcmp((char*)mods->value, "S64")) {
			} else if(!strcmp((char*)mods->value, "HI")) {
				binary[51] = true;
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDMAXSHIFT3")) {
			} else if(!strcmp((char*)mods->value, "X")) {
			} else if(!strcmp((char*)mods->value, "W")) {
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction SHF.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_FCHK == inst->op) {
		binary[1] = true;
		binary[54] = true;
		binary[58] = true;
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 3; randsize++) {
			binary[randsize + 5] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			binary[51] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			binary[48] = inst->operands[2]->properties & prop_minus;
			binary[52] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "DIVIDE")) {
					binary[3] = true;
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_predicate && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			binary[63] = true;
			binary[51] = inst->operands[1]->properties & prop_minus;
			binary[49] = inst->operands[1]->properties & prop_absolute_value;
			binary[48] = inst->operands[2]->properties & prop_minus;
			binary[52] = inst->operands[2]->properties & prop_absolute_value;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "DIVIDE")) {
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction FCHK.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "INVALIDCHKMODE14") && !seenModType[6]) {
				seenModType[6] = true;
				binary[43] = true;
				binary[44] = true;
				binary[45] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDCHKMODE15") && !seenModType[6]) {
				seenModType[6] = true;
				binary[2] = true;
				binary[42] = true;
				binary[43] = true;
				binary[44] = true;
				binary[45] = true;
			} else if(!strcmp((char*)mods->value, "DIVIDE")) {
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDCHKMODE12") && !seenModType[6]) {
				seenModType[6] = true;
				binary[2] = true;
				binary[44] = true;
				binary[45] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDCHKMODE10") && !seenModType[6]) {
				seenModType[6] = true;
				binary[2] = true;
				binary[43] = true;
				binary[45] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDCHKMODE6") && !seenModType[6]) {
				seenModType[6] = true;
				binary[2] = true;
				binary[43] = true;
				binary[44] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDCHKMODE30") && !seenModType[6]) {
				seenModType[6] = true;
				binary[2] = true;
				binary[43] = true;
				binary[44] = true;
				binary[45] = true;
				binary[46] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDCHKMODE46") && !seenModType[6]) {
				seenModType[6] = true;
				binary[2] = true;
				binary[43] = true;
				binary[44] = true;
				binary[45] = true;
				binary[47] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDCHKMODE1") && !seenModType[6]) {
				seenModType[6] = true;
				binary[42] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDCHKMODE2") && !seenModType[6]) {
				seenModType[6] = true;
				binary[43] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDCHKMODE4") && !seenModType[6]) {
				seenModType[6] = true;
				binary[44] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDCHKMODE8") && !seenModType[6]) {
				seenModType[6] = true;
				binary[45] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDCHKMODE16") && !seenModType[6]) {
				seenModType[6] = true;
				binary[46] = true;
			} else if(!strcmp((char*)mods->value, "INVALIDCHKMODE32") && !seenModType[6]) {
				seenModType[6] = true;
				binary[47] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction FCHK.\n";
			}
			mods = mods->next;
		}
		if(!seenModType[6]) {
			//binary[25] = true;
		}
	} else if(opcode_ISUB == inst->op) {
		binary[55] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_const_mem && inst->operands[2]->type == type_register) {
			checkrandmods = 1;
			binary[1] = true;
			binary[52] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[1]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[1]->val2 & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 10] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISUB:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_register) {
			checkrandmods = 1;
			binary[1] = true;
			binary[51] = true;
			binary[61] = true;
			binary[63] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISUB:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_const_mem) {
			checkrandmods = 1;
			binary[1] = true;
			binary[51] = true;
			binary[61] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 14; randsize++) {
				binary[randsize + 23] = (inst->operands[2]->val3 & (0x1 << (randsize + 2)));
			}
			for(int randsize = 0; randsize < 5; randsize++) {
				binary[randsize + 37] = (inst->operands[2]->val2 & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISUB:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_register && inst->operands[2]->type == type_hex) {
			checkrandmods = 1;
			binary[0] = true;
			binary[51] = true;
			binary[63] = true;
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
			}
			binary[59] = inst->operands[2]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[2]->val1 * (inst->operands[2]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISUB:0.\n";
				}
				mods = mods->next;
			}
		} else if(inst->num_operands == 3 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_hex && inst->operands[2]->type == type_register) {
			checkrandmods = 1;
			binary[0] = true;
			binary[52] = true;
			binary[63] = true;
			binary[59] = inst->operands[1]->properties & prop_minus;
			for(int randsize = 0; randsize < 19; randsize++) {
				binary[randsize + 23] = ((inst->operands[1]->val1 * (inst->operands[1]->properties & prop_minus ? -1 : 1)) & (0x1 << randsize));
			}
			for(int randsize = 0; randsize < 8; randsize++) {
				binary[randsize + 10] = ((inst->operands[2]->val1) & (0x1 << randsize));
			}
			node * mods = inst->operands[0]->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "CC")) {
					binary[50] = true;
					binary[53] = false;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISUB:0.\n";
				}
				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction ISUB.\n";
		}
		node * mods = inst->mods;
		while(mods) {
			if(!strcmp((char*)mods->value, "X")) {
				binary[46] = true;
			} else if(!strcmp((char*)mods->value, "S")) {
				binary[22] = true;
			} else if(!strcmp((char*)mods->value, "SAT")) {
				binary[53] = true;
			} else {
				cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction ISUB.\n";
			}
			mods = mods->next;
		}
	} else if(opcode_JCAL == inst->op) {
		cerr << "ERROR: unimplemented operation JCAL.\n";
	} else if(opcode_SHFL == inst->op) {
		cerr << "ERROR: unimplemented operation SHFL.\n";
	} else if(opcode_LDG == inst->op) {
		binary[0] = true;
		binary[23] = true;
		binary[24] = true;
		binary[25] = true;
		binary[26] = true;
		binary[27] = true;
		binary[28] = true;
		binary[29] = true;
		binary[30] = true;
		binary[34] = true;
		binary[35] = true;
		binary[36] = true;
		binary[37] = true;
		binary[44] = true;
		binary[48] = true;
		binary[49] = true;
		binary[61] = true;
		binary[62] = true;
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 2] = ((inst->operands[0]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 10] = ((inst->operands[1]->val1) & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 11; randsize++) {
			binary[randsize + 50] = (inst->operands[1]->val2 & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 10; randsize++) {
			binary[randsize + 51] = (inst->operands[1]->val2 & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 9; randsize++) {
			binary[randsize + 52] = (inst->operands[1]->val2 & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 8; randsize++) {
			binary[randsize + 53] = (inst->operands[1]->val2 & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 7; randsize++) {
			binary[randsize + 54] = (inst->operands[1]->val2 & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 6; randsize++) {
			binary[randsize + 55] = (inst->operands[1]->val2 & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 5; randsize++) {
			binary[randsize + 56] = (inst->operands[1]->val2 & (0x1 << randsize));
		}
		for(int randsize = 0; randsize < 4; randsize++) {
			binary[randsize + 57] = (inst->operands[1]->val2 & (0x1 << randsize));
		}
		if(inst->num_operands == 2 && inst->operands[0]->type == type_register && inst->operands[1]->type == type_mem) {
			node * mods = inst->mods;
			while(mods) {
				if(!strcmp((char*)mods->value, "E")) {
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "128")) {
				} else if(!strcmp((char*)mods->value, "T") && !seenModType[3]) {
					seenModType[3] = true;
					binary[32] = true;
				} else if(!strcmp((char*)mods->value, "S")) {
					binary[22] = true;
				} else if(!strcmp((char*)mods->value, "INVALIDPHASE3")) {
					binary[32] = true;
					binary[33] = true;
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "???3")) {
					binary[32] = true;
					binary[38] = true;
					binary[39] = true;
				} else if(!strcmp((char*)mods->value, "???6")) {
					binary[32] = true;
					binary[39] = true;
					binary[40] = true;
				} else {
					cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction LDG.\n";
				}

				mods = mods->next;
			}
		} else {
			cerr << "ERROR: Unrecognized operand list for instruction LDG.\n";
		}
		if(!seenModType[3]) {
			binary[39] = true;
		}
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
			cerr << "ERROR: Unrecognized mod " << (char*) mods->value << " for instruction " << opcodes[inst->op] << ".\n";
			mods = mods->next;
		}
	}
	
	 if(inst->op == opcode_BINCODE) {
		char * binstr = (char*) inst->mods->value;
		for(int x = 0; x < 64; x++) {
			binary[x] = (binstr[x] == '1');
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
