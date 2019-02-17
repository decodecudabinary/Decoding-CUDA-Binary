#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binary.hpp"
#include "binary20.hpp"
#include "common.hpp"
using namespace std;

long long getImme20(int arch, operand * op, char sizeLimit, char special) {
	long long answer = 0;
	if(special & 2) {
		answer = 1LL << (sizeLimit + 1);
		answer += (op->val2 & 0xf) << (sizeLimit - 4);
		answer += op->val3 & 0xffff;
		
		if(op->val2 >= 0x10) {
			answer |= 1;
		}
	} else if(op->type == type_const_mem) {
		answer = 1LL << sizeLimit;
		answer += (op->val2 & 0xf) << (sizeLimit - 4);
		answer += op->val3 & 0xffff;
		
		if(op->val2 >= 0x10) {
			answer |= 1;
		}
	} else if(op->type == type_hex) {
		if(sizeLimit < 32) {
			answer = 3LL << sizeLimit;
		}
		
		if(!(special & 0x1) && (op->properties & prop_minus)) {
			if(op->properties & (prop_float | prop_double)) {
				answer += (op->val1 | (0x1 << (sizeLimit-1)));
			} else {
				unsigned long long temp = 0xffffffffffffffffLL >> (64 - sizeLimit);
				temp ^= op->val1;
				temp++;
				temp &= 0xffffffffffffffffLL >> (64 - sizeLimit);
				answer += temp;
			}
		} else {
			answer += (op->val1 & (0xffffffffffffffffLL >> (64 - sizeLimit)));
		}
		
	} else if(op->type == type_register) {
		if(arch == 35) {
			answer += op->val1 & 0xff;
		} else if(arch == 30 || arch == 20 || arch == 21) {
			answer += op->val1 & 0x3f;
		} else {
			answer += op->val1 & 0x3f;
			fprintf(stderr, "ERROR bin~58: unrecognized architecture.\n");
		}
	}
	
	return answer;
}

char* instructionToHexString20(instruction * inst, int arch, int cuobjdump_version) {
	unsigned long long value;
	
	char first;//used for instructions which use same mod twice (ie IMUL can take two integer types as mods)
	//char first2;
	node * n;
	
	int regOp;
	int compOp;
	
	int noMods = 0;//indicates where to start scanning for unprocessed & unrecognized mods after doing encoding
	
	switch(inst->op) {
		case opcode_SCHI:
			value = 0xe000000000000004LL;
			//NOTE: asfermi wiki claims values should be 6 bits, but that doesn't seem to work right
			setBinary(&value, 63-4, inst->operands[0]->val1, 8, 0);
			setBinary(&value, 63-12, inst->operands[1]->val1, 8, 0);
			setBinary(&value, 63-20, inst->operands[2]->val1, 8, 0);
			setBinary(&value, 63-28, inst->operands[3]->val1, 8, 0);
			setBinary(&value, 63-36, inst->operands[4]->val1, 8, 0);
			setBinary(&value, 63-44, inst->operands[5]->val1, 8, 0);
			setBinary(&value, 63-52, inst->operands[6]->val1, 8, 0);
			break;
		case opcode_MOV:
			value = 0x27b8000000000014LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[1], 20, 0), 22, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: %s.\"%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_MOV32I:
			value = 0x47b8000000000018LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[1], 32, 0), 32, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: %s.\"%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_LD:
			value = 0xa138000000000001LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, inst->operands[1]->val2, 32, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "E")) {
					setBinary(&value, 63-58, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "CA")) {//default cache policy
					//setBinary(&value, 63-8, 0, 2, 0);
				} else if(!strcmp((char*)n->value, "CG")) {
					setBinary(&value, 63-8, 1, 2, 0);
				} else if(!strcmp((char*)n->value, "CS") || !strcmp((char*)n->value, "LU")) {
					setBinary(&value, 63-8, 2, 2, 0);
				} else if(!strcmp((char*)n->value, "CV")) {
					setBinary(&value, 63-8, 3, 2, 0);
				} else if(!strcmp((char*)n->value, "U8")) {
					setBinary(&value, 63-5, 0, 3, 1);
				} else if(!strcmp((char*)n->value, "S8")) {
					setBinary(&value, 63-5, 1, 3, 1);
				} else if(!strcmp((char*)n->value, "U16")) {
					setBinary(&value, 63-5, 2, 3, 1);
				} else if(!strcmp((char*)n->value, "S16")) {
					setBinary(&value, 63-5, 3, 3, 1);
				} else if(!strcmp((char*)n->value, "64")) {
					setBinary(&value, 63-5, 5, 3, 1);
				} else if(!strcmp((char*)n->value, "128")) {
					setBinary(&value, 63-5, 6, 3, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_LD_LDU:
			value = 0xa0380000000000fdLL;
			setBinary(&value, 63-32, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-14, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[2]->val1, 6, 0);
			setBinary(&value, 63-41, inst->operands[2]->val2, 8, 0);//TODO: double check this
			setBinary(&value, 63-26, inst->operands[3]->val1, 6, 0);
			setBinary(&value, 63-49, inst->operands[3]->val2, 6, 0);//TODO: double check this
			
			first = 0;
			//first2 = 0;
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "E")) {
					first = 1;
				} else if(!strcmp((char*)n->value, "32")) {
					//first2 = 1;
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_LDU:
			value = 0xa138000000000011LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, inst->operands[1]->val2, 32, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "E")) {
					setBinary(&value, 63-58, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "U8")) {
					setBinary(&value, 63-5, 0, 3, 1);
				} else if(!strcmp((char*)n->value, "S8")) {
					setBinary(&value, 63-5, 1, 3, 1);
				} else if(!strcmp((char*)n->value, "U16")) {
					setBinary(&value, 63-5, 2, 3, 1);
				} else if(!strcmp((char*)n->value, "S16")) {
					setBinary(&value, 63-5, 3, 3, 1);
				} else if(!strcmp((char*)n->value, "64")) {
					setBinary(&value, 63-5, 5, 3, 1);
				} else if(!strcmp((char*)n->value, "128")) {
					setBinary(&value, 63-5, 6, 3, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_LDL:
			value = 0xa138000000000003LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, inst->operands[1]->val2, 24, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 1);
				} else if(!strcmp((char*)n->value, "CA")) {//default cache policy
					//setBinary(&value, 63-8, 0, 2, 0);
				} else if(!strcmp((char*)n->value, "CG")) {
					setBinary(&value, 63-8, 1, 2, 0);
				} else if(!strcmp((char*)n->value, "CS") || !strcmp((char*)n->value, "LU")) {
					setBinary(&value, 63-8, 2, 2, 0);
				} else if(!strcmp((char*)n->value, "CV")) {
					setBinary(&value, 63-8, 3, 2, 0);
				} else if(!strcmp((char*)n->value, "U8")) {
					setBinary(&value, 63-5, 0, 3, 1);
				} else if(!strcmp((char*)n->value, "S8")) {
					setBinary(&value, 63-5, 1, 3, 1);
				} else if(!strcmp((char*)n->value, "U16")) {
					setBinary(&value, 63-5, 2, 3, 1);
				} else if(!strcmp((char*)n->value, "S16")) {
					setBinary(&value, 63-5, 3, 3, 1);
				} else if(!strcmp((char*)n->value, "64")) {
					setBinary(&value, 63-5, 5, 3, 1);
				} else if(!strcmp((char*)n->value, "128")) {
					setBinary(&value, 63-5, 6, 3, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_LDS:
			value = 0xa138000000000083LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, inst->operands[1]->val2, 24, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "U8")) {
					setBinary(&value, 63-5, 0, 3, 1);
				} else if(!strcmp((char*)n->value, "S8")) {
					setBinary(&value, 63-5, 1, 3, 1);
				} else if(!strcmp((char*)n->value, "U16")) {
					setBinary(&value, 63-5, 2, 3, 1);
				} else if(!strcmp((char*)n->value, "S16")) {
					setBinary(&value, 63-5, 3, 3, 1);
				} else if(!strcmp((char*)n->value, "64")) {
					setBinary(&value, 63-5, 5, 3, 1);
				} else if(!strcmp((char*)n->value, "128")) {
					setBinary(&value, 63-5, 6, 3, 1);
				} else if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_LDC:
			value = 0x6138000000000028LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, inst->operands[1]->val3, 16, 0);
			setBinary(&value, 63-42, inst->operands[1]->val2, 5, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "U8")) {
					setBinary(&value, 63-5, 0, 3, 1);
				} else if(!strcmp((char*)n->value, "S8")) {
					setBinary(&value, 63-5, 1, 3, 1);
				} else if(!strcmp((char*)n->value, "U16")) {
					setBinary(&value, 63-5, 2, 3, 1);
				} else if(!strcmp((char*)n->value, "S16")) {
					setBinary(&value, 63-5, 3, 3, 1);
				} else if(!strcmp((char*)n->value, "64")) {
					setBinary(&value, 63-5, 5, 3, 1);
				} else if(!strcmp((char*)n->value, "128")) {
					setBinary(&value, 63-5, 6, 3, 1);
				} else if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_ST:
			value = 0xa138000000000009LL;
			setBinary(&value, 63-14, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-26, inst->operands[0]->val2, 32, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "E")) {
					setBinary(&value, 63-58, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "WB")) {//default cache policy
					//setBinary(&value, 63-8, 0, 2, 0);
				} else if(!strcmp((char*)n->value, "CG")) {
					setBinary(&value, 63-8, 1, 2, 0);
				} else if(!strcmp((char*)n->value, "CS")) {
					setBinary(&value, 63-8, 2, 2, 0);
				} else if(!strcmp((char*)n->value, "WT")) {
					setBinary(&value, 63-8, 3, 2, 0);
				} else if(!strcmp((char*)n->value, "U8")) {
					setBinary(&value, 63-5, 0, 3, 1);
				} else if(!strcmp((char*)n->value, "S8")) {
					setBinary(&value, 63-5, 1, 3, 1);
				} else if(!strcmp((char*)n->value, "U16")) {
					setBinary(&value, 63-5, 2, 3, 1);
				} else if(!strcmp((char*)n->value, "S16")) {
					setBinary(&value, 63-5, 3, 3, 1);
				} else if(!strcmp((char*)n->value, "64")) {
					setBinary(&value, 63-5, 5, 3, 1);
				} else if(!strcmp((char*)n->value, "128")) {
					setBinary(&value, 63-5, 6, 3, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_STL:
			value = 0xa138000000000013LL;
			setBinary(&value, 63-14, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-26, inst->operands[0]->val2, 24, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "WB")) {//default cache policy
					//setBinary(&value, 63-8, 0, 2, 0);
				} else if(!strcmp((char*)n->value, "CG")) {
					setBinary(&value, 63-8, 1, 2, 0);
				} else if(!strcmp((char*)n->value, "CS")) {
					setBinary(&value, 63-8, 2, 2, 0);
				} else if(!strcmp((char*)n->value, "WT")) {
					setBinary(&value, 63-8, 3, 2, 0);
				} else if(!strcmp((char*)n->value, "U8")) {
					setBinary(&value, 63-5, 0, 3, 1);
				} else if(!strcmp((char*)n->value, "S8")) {
					setBinary(&value, 63-5, 1, 3, 1);
				} else if(!strcmp((char*)n->value, "U16")) {
					setBinary(&value, 63-5, 2, 3, 1);
				} else if(!strcmp((char*)n->value, "S16")) {
					setBinary(&value, 63-5, 3, 3, 1);
				} else if(!strcmp((char*)n->value, "64")) {
					setBinary(&value, 63-5, 5, 3, 1);
				} else if(!strcmp((char*)n->value, "128")) {
					setBinary(&value, 63-5, 6, 3, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_STS:
			value = 0xa138000000000093LL;
			setBinary(&value, 63-14, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-26, inst->operands[0]->val2, 24, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "U8")) {
					setBinary(&value, 63-5, 0, 3, 1);
				} else if(!strcmp((char*)n->value, "S8")) {
					setBinary(&value, 63-5, 1, 3, 1);
				} else if(!strcmp((char*)n->value, "U16")) {
					setBinary(&value, 63-5, 2, 3, 1);
				} else if(!strcmp((char*)n->value, "S16")) {
					setBinary(&value, 63-5, 3, 3, 1);
				} else if(!strcmp((char*)n->value, "64")) {
					setBinary(&value, 63-5, 5, 3, 1);
				} else if(!strcmp((char*)n->value, "128")) {
					setBinary(&value, 63-5, 6, 3, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_LDLK:
			value = 0xa138000000000005LL;
			//TODO (medium-low priority): deal with first operand (predicate)
			setBinary(&value, 63-16, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-22, inst->operands[2]->val1, 6, 0);
			setBinary(&value, 63-28, inst->operands[2]->val2, 32, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "U8")) {
					setBinary(&value, 63-5, 0, 3, 1);
				} else if(!strcmp((char*)n->value, "S8")) {
					setBinary(&value, 63-5, 1, 3, 1);
				} else if(!strcmp((char*)n->value, "U16")) {
					setBinary(&value, 63-5, 2, 3, 1);
				} else if(!strcmp((char*)n->value, "S16")) {
					setBinary(&value, 63-5, 3, 3, 1);
				} else if(!strcmp((char*)n->value, "64")) {
					setBinary(&value, 63-5, 5, 3, 1);
				} else if(!strcmp((char*)n->value, "128")) {
					setBinary(&value, 63-5, 6, 3, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_LDSLK:
			value = 0xa138000000000015LL;
			setBinary(&value, 63-58, inst->operands[0]->val1, 3, 0);
			setBinary(&value, 63-14, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[2]->val1, 6, 0);
			setBinary(&value, 63-26, inst->operands[2]->val2, 24, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "U8")) {
					setBinary(&value, 63-5, 0, 3, 1);
				} else if(!strcmp((char*)n->value, "S8")) {
					setBinary(&value, 63-5, 1, 3, 1);
				} else if(!strcmp((char*)n->value, "U16")) {
					setBinary(&value, 63-5, 2, 3, 1);
				} else if(!strcmp((char*)n->value, "S16")) {
					setBinary(&value, 63-5, 3, 3, 1);
				} else if(!strcmp((char*)n->value, "64")) {
					setBinary(&value, 63-5, 5, 3, 1);
				} else if(!strcmp((char*)n->value, "128")) {
					setBinary(&value, 63-5, 6, 3, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_STUL:
			value = 0xa138000000000017LL;
			setBinary(&value, 63-14, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-26, inst->operands[0]->val2, 32, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "U8")) {
					setBinary(&value, 63-5, 0, 3, 1);
				} else if(!strcmp((char*)n->value, "S8")) {
					setBinary(&value, 63-5, 1, 3, 1);
				} else if(!strcmp((char*)n->value, "U16")) {
					setBinary(&value, 63-5, 2, 3, 1);
				} else if(!strcmp((char*)n->value, "S16")) {
					setBinary(&value, 63-5, 3, 3, 1);
				} else if(!strcmp((char*)n->value, "64")) {
					setBinary(&value, 63-5, 5, 3, 1);
				} else if(!strcmp((char*)n->value, "128")) {
					setBinary(&value, 63-5, 6, 3, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_STSUL:
			value = 0xa138000000000033LL;
			setBinary(&value, 63-14, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-26, inst->operands[0]->val2, 24, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "U8")) {
					setBinary(&value, 63-5, 0, 3, 1);
				} else if(!strcmp((char*)n->value, "S8")) {
					setBinary(&value, 63-5, 1, 3, 1);
				} else if(!strcmp((char*)n->value, "U16")) {
					setBinary(&value, 63-5, 2, 3, 1);
				} else if(!strcmp((char*)n->value, "S16")) {
					setBinary(&value, 63-5, 3, 3, 1);
				} else if(!strcmp((char*)n->value, "64")) {
					setBinary(&value, 63-5, 5, 3, 1);
				} else if(!strcmp((char*)n->value, "128")) {
					setBinary(&value, 63-5, 6, 3, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_FADD:
			value = 0x003800000000000aLL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 1), 22, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "FTZ")) {
					setBinary(&value, 63-5, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			if(inst->operands[1]->properties & prop_minus) {
				setBinary(&value, 63-9, 1, 1, 0);
			}
			if(inst->operands[1]->properties & prop_absolute_value) {
				setBinary(&value, 63-7, 1, 1, 0);
			}
			if(!(inst->operands[2]->properties & prop_float) && (inst->operands[2]->properties & prop_minus)) {
				setBinary(&value, 63-8, 1, 1, 0);
			}
			if(inst->operands[2]->properties & prop_absolute_value) {
				setBinary(&value, 63-6, 1, 1, 0);
			}
			break;
		case opcode_FADD32I:
			value = 0x4038000000000014LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 32, 0), 32, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "FTZ")) {
					setBinary(&value, 63-5, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			if(inst->operands[1]->properties & prop_minus) {
				setBinary(&value, 63-9, 1, 1, 0);
			}
			if(inst->operands[1]->properties & prop_absolute_value) {
				setBinary(&value, 63-7, 1, 1, 0);
			}
			break;
		case opcode_FMUL:
			value = 0x003800000000001aLL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 1), 22, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "FTZ")) {
					setBinary(&value, 63-6, 1, 2, 0);
				} else if(!strcmp((char*)n->value, "FMZ")) {
					setBinary(&value, 63-6, 2, 2, 0);
				} else if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			if(inst->operands[1]->properties & prop_minus) {//TODO (medium priority) double check this
				setBinary(&value, 63-57, 1, 1, 0);
			}
			if(!(inst->operands[2]->properties & prop_float) && (inst->operands[2]->properties & prop_minus)) {//TODO (medium priority) double check this
				setBinary(&value, 63-57, 1, 1, 0);
			}
			break;
		case opcode_FMUL32I:
			value = 0x403800000000000cLL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 32, 0), 32, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "FTZ")) {
					setBinary(&value, 63-6, 1, 2, 0);
				} else if(!strcmp((char*)n->value, "FMZ")) {
					setBinary(&value, 63-6, 2, 2, 0);
				} else if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			if(inst->operands[1]->properties & prop_minus) {
				setBinary(&value, 63-9, 1, 1, 0);
			}
			break;
		case opcode_FFMA:
			value = 0x003800000000000cLL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			if(inst->operands[2]->type == type_register && inst->operands[3]->type == type_const_mem) {
				setBinary(&value, 63-49, inst->operands[2]->val1, 6, 0);
				setBinary(&value, 63-26, getImme20(arch, inst->operands[3], 20, 3), 22, 0);
			} else if(inst->operands[3]->type == type_register) {
				setBinary(&value, 63-49, inst->operands[3]->val1, 6, 0);
				setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 1), 22, 0);
			} else {
				fprintf(stderr,"Error: unrecognized FFMA.");
			}
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "RM")) {
					setBinary(&value, 63-55, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "RP")) {
					setBinary(&value, 63-56, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "RZ")) {
					setBinary(&value, 63-55, 3, 2, 0);
				} else if(!strcmp((char*)n->value, "FTZ")) {
					setBinary(&value, 63-6, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "FMZ")) {
					setBinary(&value, 63-7, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "SAT")) {
					setBinary(&value, 63-5, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			if(inst->operands[1]->properties & prop_minus) {//TODO (medium priority) verify this
				setBinary(&value, 63-9, 1, 1, 0);
			}
			if(!(inst->operands[2]->properties & prop_float) && (inst->operands[2]->properties & prop_minus)) {//TODO (medium priority) verify this
				setBinary(&value, 63-9, 1, 1, 0);
			}
			if(inst->operands[3]->properties & prop_minus) {
				setBinary(&value, 63-8, 1, 1, 0);
			}
			break;
		case opcode_FSET:
			value = 0x0038000000007008LL;//NOTE: not in asfermi wiki
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 0), 22, 0);
			setBinary(&value, 63-49, inst->operands[3]->val1, 3, 0);
			if(inst->operands[3]->properties & prop_not) {
				setBinary(&value, 63-52, 1, 1, 1);
			} else {
				setBinary(&value, 63-52, 0, 1, 1);
			}
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "LT")) {
					setBinary(&value, 63-55, 1, 4, 0);
				} else if(!strcmp((char*)n->value, "EQ")) {
					setBinary(&value, 63-55, 2, 4, 0);
				} else if(!strcmp((char*)n->value, "LE")) {
					setBinary(&value, 63-55, 3, 4, 0);
				} else if(!strcmp((char*)n->value, "GT")) {
					setBinary(&value, 63-55, 4, 4, 0);
				} else if(!strcmp((char*)n->value, "NE")) {
					setBinary(&value, 63-55, 5, 4, 0);
				} else if(!strcmp((char*)n->value, "GE")) {
					setBinary(&value, 63-55, 6, 4, 0);
				} else if(!strcmp((char*)n->value, "NUM")) {
					setBinary(&value, 63-55, 7, 4, 0);
				} else if(!strcmp((char*)n->value, "NAN")) {
					setBinary(&value, 63-55, 8, 4, 0);
				} else if(!strcmp((char*)n->value, "LTU")) {
					setBinary(&value, 63-55, 9, 4, 0);
				} else if(!strcmp((char*)n->value, "EQU")) {
					setBinary(&value, 63-55, 10, 4, 0);
				} else if(!strcmp((char*)n->value, "LEU")) {
					setBinary(&value, 63-55, 11, 4, 0);
				} else if(!strcmp((char*)n->value, "GTU")) {
					setBinary(&value, 63-55, 12, 4, 0);
				} else if(!strcmp((char*)n->value, "NEU")) {
					setBinary(&value, 63-55, 13, 4, 0);
				} else if(!strcmp((char*)n->value, "GEU")) {
					setBinary(&value, 63-55, 14, 4, 0);
				} else if(!strcmp((char*)n->value, "AND")) {
					setBinary(&value, 63-53, 0, 2, 0);
				} else if(!strcmp((char*)n->value, "OR")) {
					setBinary(&value, 63-53, 1, 2, 0);
				} else if(!strcmp((char*)n->value, "XOR")) {
					setBinary(&value, 63-53, 2, 2, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_FSETP:
		case opcode_DSETP:
		case opcode_ISETP:
			if(inst->op == opcode_FSETP) {
				value = 0x003b800000007004LL;
			} else if(inst->op == opcode_DSETP) {
				value = 0x803b800000007018LL;
			} else {
				value = 0xc43b800000007018LL;
			}
			setBinary(&value, 63-17, inst->operands[0]->val1, 3, 1);
			setBinary(&value, 63-14, inst->operands[1]->val1, 3, 1);
			setBinary(&value, 63-20, inst->operands[2]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[3], 20, 0), 22, 0);
			setBinary(&value, 63-49, inst->operands[4]->val1, 3, 1);
			if(inst->operands[4]->properties & prop_not) {
				setBinary(&value, 63-52, 1, 1, 1);
			} else {
				setBinary(&value, 63-52, 0, 1, 1);
			}
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "LT")) {
					setBinary(&value, 63-55, 1, 4, 0);
				} else if(!strcmp((char*)n->value, "EQ")) {
					setBinary(&value, 63-55, 2, 4, 0);
				} else if(!strcmp((char*)n->value, "LE")) {
					setBinary(&value, 63-55, 3, 4, 0);
				} else if(!strcmp((char*)n->value, "GT")) {
					setBinary(&value, 63-55, 4, 4, 0);
				} else if(!strcmp((char*)n->value, "NE")) {
					setBinary(&value, 63-55, 5, 4, 0);
				} else if(!strcmp((char*)n->value, "GE")) {
					setBinary(&value, 63-55, 6, 4, 0);
				} else if(!strcmp((char*)n->value, "NUM")) {
					setBinary(&value, 63-55, 7, 4, 0);
				} else if(!strcmp((char*)n->value, "NAN")) {
					setBinary(&value, 63-55, 8, 4, 0);
				} else if(!strcmp((char*)n->value, "LTU")) {
					setBinary(&value, 63-55, 9, 4, 0);
				} else if(!strcmp((char*)n->value, "EQU")) {
					setBinary(&value, 63-55, 10, 4, 0);
				} else if(!strcmp((char*)n->value, "LEU")) {
					setBinary(&value, 63-55, 11, 4, 0);
				} else if(!strcmp((char*)n->value, "GTU")) {
					setBinary(&value, 63-55, 12, 4, 0);
				} else if(!strcmp((char*)n->value, "NEU")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-55, 13, 4, 0);
				} else if(!strcmp((char*)n->value, "GEU")) {
					setBinary(&value, 63-55, 14, 4, 0);
				} else if(!strcmp((char*)n->value, "FTZ")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-59, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "AND")) {
					setBinary(&value, 63-53, 0, 2, 0);
				} else if(!strcmp((char*)n->value, "OR")) {
					setBinary(&value, 63-53, 1, 2, 0);
				} else if(!strcmp((char*)n->value, "XOR")) {
					setBinary(&value, 63-53, 2, 2, 0);
				} else if(!strcmp((char*)n->value, "XLU")) {
					setBinary(&value, 63-8, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "U32")) {//Only used for ISETP?
					setBinary(&value, 63-5, 0, 1, 1);
				} else if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki (Only used for ISETP?)
					setBinary(&value, 63-4, 1, 1, 1);
				} else if(!strcmp((char*)n->value, "X")) {//NOTE: not in asfermi wiki (Only used for ISETP?)
					setBinary(&value, 63-6, 1, 1, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			if(!(inst->operands[2]->properties & (prop_float | prop_double)) && (inst->operands[2]->properties & prop_minus)) {
				setBinary(&value, 63-9, 1, 1, 0);
			}
			if(inst->operands[2]->properties & prop_absolute_value) {
				setBinary(&value, 63-7, 1, 1, 0);
			}
			if(inst->operands[3]->properties & prop_minus && inst->operands[3]->type != type_hex) {//TODO (medium priority) double check this
				setBinary(&value, 63-8, 1, 1, 0);
			}
			if(inst->operands[3]->properties & prop_absolute_value) {
				setBinary(&value, 63-6, 1, 1, 0);
			}
			break;
		case opcode_FCMP:
			value = 0x003800000000005cLL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 1), 22, 0);
			setBinary(&value, 63-49, inst->operands[3]->val1, 6, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "F")) {
					setBinary(&value, 63-55, 0, 4, 1);
				} else if(!strcmp((char*)n->value, "LT")) {
					setBinary(&value, 63-55, 1, 4, 1);
				} else if(!strcmp((char*)n->value, "EQ")) {
					setBinary(&value, 63-55, 2, 4, 1);
				} else if(!strcmp((char*)n->value, "LE")) {
					setBinary(&value, 63-55, 3, 4, 1);
				} else if(!strcmp((char*)n->value, "GT")) {
					setBinary(&value, 63-55, 4, 4, 1);
				} else if(!strcmp((char*)n->value, "NE")) {
					setBinary(&value, 63-55, 5, 4, 1);
				} else if(!strcmp((char*)n->value, "GE")) {
					setBinary(&value, 63-55, 6, 4, 1);
				} else if(!strcmp((char*)n->value, "NUM")) {
					setBinary(&value, 63-55, 7, 4, 1);
				} else if(!strcmp((char*)n->value, "NAN")) {
					setBinary(&value, 63-55, 8, 4, 1);
				} else if(!strcmp((char*)n->value, "LTU")) {
					setBinary(&value, 63-55, 9, 4, 1);
				} else if(!strcmp((char*)n->value, "EQU")) {
					setBinary(&value, 63-55, 10, 4, 1);
				} else if(!strcmp((char*)n->value, "LEU")) {
					setBinary(&value, 63-55, 11, 4, 1);
				} else if(!strcmp((char*)n->value, "GTU")) {
					setBinary(&value, 63-55, 12, 4, 1);
				} else if(!strcmp((char*)n->value, "NEU")) {
					setBinary(&value, 63-55, 13, 4, 1);
				} else if(!strcmp((char*)n->value, "GEU")) {
					setBinary(&value, 63-55, 14, 4, 1);
				} else if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_MUFU:
			value = 0x0038000000000013LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "SAT")) {
					setBinary(&value, 63-5, 1, 4, 0);
				} else if(!strcmp((char*)n->value, "COS")) {
					setBinary(&value, 63-26, 0, 4, 0);
				} else if(!strcmp((char*)n->value, "SIN")) {
					setBinary(&value, 63-26, 1, 4, 0);
				} else if(!strcmp((char*)n->value, "EX2")) {
					setBinary(&value, 63-26, 2, 4, 0);
				} else if(!strcmp((char*)n->value, "LG2")) {
					setBinary(&value, 63-26, 3, 4, 0);
				} else if(!strcmp((char*)n->value, "RCP")) {
					setBinary(&value, 63-26, 4, 4, 0);
				} else if(!strcmp((char*)n->value, "RSQ")) {
					setBinary(&value, 63-26, 5, 4, 0);
				} else if(!strcmp((char*)n->value, "RCP64H")) {
					setBinary(&value, 63-26, 6, 4, 0);
				} else if(!strcmp((char*)n->value, "RSQ64H")) {
					setBinary(&value, 63-26, 7, 4, 0);
				} else if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			if(inst->operands[1]->properties & prop_minus) {
				setBinary(&value, 63-9, 1, 1, 0);
			}
			if(inst->operands[1]->properties & prop_absolute_value) {
				setBinary(&value, 63-7, 1, 1, 0);
			}
			break;
		case opcode_DADD:
			value = 0x8038000000000012LL;//Note: different value than on asfermi wiki
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 1), 22, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "RM")) {
					setBinary(&value, 63-55, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "RP")) {
					setBinary(&value, 63-56, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "RZ")) {
					setBinary(&value, 63-55, 3, 2, 0);
				} else if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			if(inst->operands[1]->properties & prop_minus) {
				setBinary(&value, 63-9, 1, 1, 0);
			}
			if(inst->operands[1]->properties & prop_absolute_value) {
				setBinary(&value, 63-7, 1, 1, 0);
			}
			if(!(inst->operands[2]->properties & (prop_float | prop_double)) && (inst->operands[2]->properties & prop_minus)) {
				setBinary(&value, 63-8, 1, 1, 0);
			}
			if(inst->operands[2]->properties & prop_absolute_value) {
				setBinary(&value, 63-6, 1, 1, 0);
			}
			break;
		case opcode_DMUL:
			value = 0x803800000000000aLL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 1), 22, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "RM")) {
					setBinary(&value, 63-55, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "RP")) {
					setBinary(&value, 63-56, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "RZ")) {
					setBinary(&value, 63-55, 3, 2, 0);
				} else if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			if(!(inst->operands[2]->properties & prop_double) && (inst->operands[2]->properties & prop_minus)) {
				setBinary(&value, 63-9, 1, 1, 0);
			}
			break;
		case opcode_DFMA:
			value = 0x8038000000000004LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			if(inst->operands[2]->type == type_register && inst->operands[3]->type == type_const_mem) {
				setBinary(&value, 63-49, inst->operands[2]->val1, 6, 0);
				setBinary(&value, 63-26, getImme20(arch, inst->operands[3], 20, 3), 22, 0);
			} else if(inst->operands[3]->type == type_register) {
				setBinary(&value, 63-49, inst->operands[3]->val1, 6, 0);
				setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 1), 22, 0);
			} else {
				fprintf(stderr,"Error: unrecognized DFMA.");
			}
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "RM")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-55, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "RP")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-56, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "RZ")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-55, 3, 2, 0);
				} else if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			if(inst->operands[1]->properties & prop_minus) {//TODO (medium priority) fix condition
				setBinary(&value, 63-9, 1, 1, 0);
			}
			if(!(inst->operands[2]->properties & (prop_float | prop_double)) && (inst->operands[2]->properties & prop_minus)) {//TODO (medium priority) fix condition
				setBinary(&value, 63-9, 1, 1, 0);
			}
			if(inst->operands[3]->properties & prop_minus) {
				setBinary(&value, 63-8, 1, 1, 0);
			}
			break;
		case opcode_IADD:
			value = 0xc038000000000012LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			compOp = 2;
			regOp = 1;
			if(inst->operands[1]->type != type_register) {
				regOp = 2;
				compOp = 1;
			}
			setBinary(&value, 63-20, inst->operands[regOp]->val1, 6, 0);
			if(inst->operands[regOp]->properties & prop_minus) {
				setBinary(&value, 63-9, 1, 1, 0);
			}
			if(cuobjdump_version >= 55 && inst->operands[compOp]->type == type_hex && (inst->operands[compOp]->properties & prop_minus) && (inst->operands[compOp]->val1 & 0x80000) == 0) {
				inst->operands[compOp]->val1 *= -1;
				setBinary(&value, 63-26, getImme20(arch, inst->operands[compOp], 20, 1), 22, 0);
				inst->operands[compOp]->val1 *= -1;
			} else if(cuobjdump_version >= 55 && inst->operands[compOp]->type == type_hex && !(inst->operands[compOp]->properties & prop_minus) && (inst->operands[compOp]->val1 & 0x80000) != 0) {
				setBinary(&value, 63-8, 1, 1, 0);
				inst->operands[compOp]->val1 *= -1;
				setBinary(&value, 63-26, getImme20(arch, inst->operands[compOp], 20, 0), 22, 0);
				inst->operands[compOp]->val1 *= -1;
			} else if(inst->operands[compOp]->properties & prop_minus) {
				setBinary(&value, 63-8, 1, 1, 0);
				setBinary(&value, 63-26, getImme20(arch, inst->operands[compOp], 20, 1), 22, 0);
			} else {
				setBinary(&value, 63-26, getImme20(arch, inst->operands[compOp], 20, 0), 22, 0);
			}
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "X")) {
					setBinary(&value, 63-6, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "SAT")) {
					setBinary(&value, 63-5, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			n = inst->operands[0]->mods;
			while(n) {
				if(!strcmp((char*)n->value, "CC")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-48, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s[0].%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			noMods = 1;
			break;
		case opcode_ISUB:
			value = 0xc038000000000012LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			compOp = 2;
			regOp = 1;
			if(inst->operands[1]->type != type_register) {
				regOp = 2;
				compOp = 1;
				setBinary(&value, 63-9, 1, 1, 0);//third operand is subtracted
			} else {
				setBinary(&value, 63-8, 1, 1, 0);//third operand is subtracted
			}
			setBinary(&value, 63-20, inst->operands[regOp]->val1, 6, 0);
			
			if(inst->operands[regOp]->properties & prop_minus) {
				if(regOp == 2) {
					setBinary(&value, 63-9, 0, 1, 0);
				} else {
					setBinary(&value, 63-9, 1, 1, 0);
				}
			}
			if(inst->operands[compOp]->properties & prop_minus) {
				setBinary(&value, 63-8, 0, 1, 0);
				setBinary(&value, 63-26, getImme20(arch, inst->operands[compOp], 20, 0), 22, 0);
			} else {
				if(compOp == 2) {
					setBinary(&value, 63-8, 1, 1, 0);
					setBinary(&value, 63-26, getImme20(arch, inst->operands[compOp], 20, 1), 22, 0);
				} else {
					setBinary(&value, 63-26, getImme20(arch, inst->operands[compOp], 20, 0), 22, 0);
				}
			}
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "X")) {
					setBinary(&value, 63-6, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "SAT")) {
					setBinary(&value, 63-5, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			n = inst->operands[0]->mods;
			while(n) {
				if(!strcmp((char*)n->value, "CC")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-48, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s[0].%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			noMods = 1;
			break;
		case opcode_IADD32I:
			value = 0x4038000000000010LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 32, 0), 32, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "X")) {
					setBinary(&value, 63-6, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "SAT")) {
					setBinary(&value, 63-5, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			n = inst->operands[0]->mods;
			while(n) {
				if(!strcmp((char*)n->value, "CC")) {
					setBinary(&value, 63-58, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s[0].%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			noMods = 1;
			break;
		case opcode_IMNMX:
			value = 0xC438000000007010LL;//NOTE: not in asfermi wiki
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 0), 22, 0);
			setBinary(&value, 63-49, inst->operands[3]->val1, 3, 0);
			if(inst->operands[3]->properties & prop_not) {
				setBinary(&value, 63-52, 1, 1, 1);
			} else {
				setBinary(&value, 63-52, 0, 1, 1);
			}
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "U32")) {
					setBinary(&value, 63-5, 0, 1, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_IMUL:
			value = 0xc53800000000000aLL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 0), 22, 0);
			n = inst->mods;
			first = 1;
			while(n) {
				if(!strcmp((char*)n->value, "HI")) {
					setBinary(&value, 63-6, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "U32")) {
					if(first) {
						setBinary(&value, 63-7, 0, 1, 1);
					} else {
						setBinary(&value, 63-5, 0, 1, 1);
					}
					first = 0;
				} else if(!strcmp((char*)n->value, "S32")) {
					if(first) {
						setBinary(&value, 63-7, 1, 1, 1);
					} else {
						setBinary(&value, 63-5, 1, 1, 1);
					}
					first = 0;
				} else if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			n = inst->operands[0]->mods;
			while(n) {
				if(!strcmp((char*)n->value, "CC")) {
					setBinary(&value, 63-48, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s[0].%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			noMods = 1;
			break;
		case opcode_IMUL32I:
			value = 0x4538000000000008LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 32, 0), 32, 0);
			n = inst->mods;
			first = 1;
			while(n) {
				if(!strcmp((char*)n->value, "HI")) {
					setBinary(&value, 63-6, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "U32")) {
					if(first) {
						setBinary(&value, 63-7, 0, 1, 1);
					} else {
						setBinary(&value, 63-5, 0, 1, 1);
					}
					first = 0;
				} else if(!strcmp((char*)n->value, "S32")) {
					if(first) {
						setBinary(&value, 63-7, 1, 1, 1);
					} else {
						setBinary(&value, 63-5, 1, 1, 1);
					}
					first = 0;
				} else if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			n = inst->operands[0]->mods;
			while(n) {
				if(!strcmp((char*)n->value, "CC")) {
					setBinary(&value, 63-58, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s[0].%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			noMods = 1;
			break;
		case opcode_IMAD:
			value = 0xc538000000000004LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			if(inst->operands[2]->type == type_register && inst->operands[3]->type == type_const_mem) {
				setBinary(&value, 63-49, inst->operands[2]->val1, 6, 0);
				setBinary(&value, 63-26, getImme20(arch, inst->operands[3], 20, 3), 22, 0);
			} else if(inst->operands[3]->type == type_register) {
				setBinary(&value, 63-49, inst->operands[3]->val1, 6, 0);
				setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 0), 22, 0);
			} else {
				fprintf(stderr,"Error: unrecognized IMAD.");
			}
			n = inst->mods;
			first = 1;
			while(n) {
				if(!strcmp((char*)n->value, "HI")) {
					setBinary(&value, 63-6, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "U32")) {
					if(first) {
						setBinary(&value, 63-7, 0, 1, 1);
					} else {
						setBinary(&value, 63-5, 0, 1, 1);
					}
					first = 0;
				} else if(!strcmp((char*)n->value, "S32")) {
					if(first) {
						setBinary(&value, 63-7, 1, 1, 1);
					} else {
						setBinary(&value, 63-5, 1, 1, 1);
					}
					first = 0;
				} else if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "X")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-55, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			n = inst->operands[0]->mods;
			while(n) {
				if(!strcmp((char*)n->value, "CC")) {
					setBinary(&value, 63-48, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s[0].%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			noMods = 1;
			if(inst->operands[1]->properties & prop_minus) {
				setBinary(&value, 63-9, 1, 1, 0);
			}
			if(inst->operands[2]->properties & prop_minus) {
				if(inst->operands[2]->type != type_hex) {
					setBinary(&value, 63-8, 1, 1, 0);
				}
			} else if(inst->operands[3]->properties & prop_minus) {
				setBinary(&value, 63-8, 1, 1, 0);
			}
			break;
		case opcode_ISCADD:
			value = 0xc038000000000002LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 1), 22, 0);
			setBinary(&value, 63-5, getImme20(arch, inst->operands[3], 5, 1), 5, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			n = inst->operands[0]->mods;
			while(n) {
				if(!strcmp((char*)n->value, "CC")) {
					setBinary(&value, 63-48, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s[0].%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			noMods = 1;
			if(inst->operands[1]->properties & prop_minus) {
				setBinary(&value, 63-56, 1, 1, 0);//Note: asfermi wiki's bit location is off by one for this
			}
			if(inst->operands[2]->properties & prop_minus) {
				setBinary(&value, 63-55, 1, 1, 0);//Note: asfermi wiki's bit location is off by one for this
			}
			break;
		case opcode_ISET:
			value = 0xC438000000007008LL;//NOTE: not in asfermi wiki
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 0), 22, 0);
			setBinary(&value, 63-49, inst->operands[3]->val1, 3, 0);
			if(inst->operands[3]->properties & prop_not) {
				setBinary(&value, 63-52, 1, 1, 1);
			} else {
				setBinary(&value, 63-52, 0, 1, 1);
			}
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "LT")) {
					setBinary(&value, 63-55, 1, 4, 0);
				} else if(!strcmp((char*)n->value, "EQ")) {
					setBinary(&value, 63-55, 2, 4, 0);
				} else if(!strcmp((char*)n->value, "LE")) {
					setBinary(&value, 63-55, 3, 4, 0);
				} else if(!strcmp((char*)n->value, "GT")) {
					setBinary(&value, 63-55, 4, 4, 0);
				} else if(!strcmp((char*)n->value, "NE")) {
					setBinary(&value, 63-55, 5, 4, 0);
				} else if(!strcmp((char*)n->value, "GE")) {
					setBinary(&value, 63-55, 6, 4, 0);
				} else if(!strcmp((char*)n->value, "NUM")) {
					setBinary(&value, 63-55, 7, 4, 0);
				} else if(!strcmp((char*)n->value, "NAN")) {
					setBinary(&value, 63-55, 8, 4, 0);
				} else if(!strcmp((char*)n->value, "LTU")) {
					setBinary(&value, 63-55, 9, 4, 0);
				} else if(!strcmp((char*)n->value, "EQU")) {
					setBinary(&value, 63-55, 10, 4, 0);
				} else if(!strcmp((char*)n->value, "LEU")) {
					setBinary(&value, 63-55, 11, 4, 0);
				} else if(!strcmp((char*)n->value, "GTU")) {
					setBinary(&value, 63-55, 12, 4, 0);
				} else if(!strcmp((char*)n->value, "NEU")) {
					setBinary(&value, 63-55, 13, 4, 0);
				} else if(!strcmp((char*)n->value, "GEU")) {
					setBinary(&value, 63-55, 14, 4, 0);
				} else if(!strcmp((char*)n->value, "AND")) {
					setBinary(&value, 63-53, 0, 2, 0);
				} else if(!strcmp((char*)n->value, "OR")) {
					setBinary(&value, 63-53, 1, 2, 0);
				} else if(!strcmp((char*)n->value, "XOR")) {
					setBinary(&value, 63-53, 2, 2, 0);
				} else if(!strcmp((char*)n->value, "BF")) {
					setBinary(&value, 63-7, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "U32")) {
					setBinary(&value, 63-5, 0, 1, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_ICMP:
			value = 0xc43800000000000cLL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			if(inst->operands[3]->type != type_register) {
				setBinary(&value, 63-26, getImme20(arch, inst->operands[3], 20, 3), 22, 0);
				setBinary(&value, 63-49, inst->operands[2]->val1, 6, 0);
			} else {
				setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 0), 22, 0);
				setBinary(&value, 63-49, inst->operands[3]->val1, 6, 0);
			}
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 1);
				} else if(!strcmp((char*)n->value, "U32")) {
					setBinary(&value, 63-5, 0, 1, 1);
				} else if(!strcmp((char*)n->value, "LT")) {
					setBinary(&value, 63-55, 1, 3, 0);
				} else if(!strcmp((char*)n->value, "EQ")) {
					setBinary(&value, 63-55, 2, 3, 0);
				} else if(!strcmp((char*)n->value, "LE")) {
					setBinary(&value, 63-55, 3, 3, 0);
				} else if(!strcmp((char*)n->value, "GT")) {
					setBinary(&value, 63-55, 4, 3, 0);
				} else if(!strcmp((char*)n->value, "NE")) {
					setBinary(&value, 63-55, 5, 3, 0);
				} else if(!strcmp((char*)n->value, "GE")) {
					setBinary(&value, 63-55, 6, 3, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_I2F:
			value = 0x2078048000000018LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[1], 20, 1), 22, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 1);
				} else if(!strcmp((char*)n->value, "U16")) {
					setBinary(&value, 63-9, 0, 1, 1);
					setBinary(&value, 63-23, 1, 2, 1);
				} else if(!strcmp((char*)n->value, "U32")) {
					setBinary(&value, 63-9, 0, 1, 1);
					setBinary(&value, 63-23, 2, 2, 1);
				} else if(!strcmp((char*)n->value, "U64")) {
					setBinary(&value, 63-9, 0, 1, 1);
					setBinary(&value, 63-23, 3, 2, 1);
				} else if(!strcmp((char*)n->value, "S16")) {
					setBinary(&value, 63-9, 1, 1, 1);
					setBinary(&value, 63-23, 1, 2, 1);
				} else if(!strcmp((char*)n->value, "S32")) {
					setBinary(&value, 63-9, 1, 1, 1);
					setBinary(&value, 63-23, 2, 2, 1);
				} else if(!strcmp((char*)n->value, "S64")) {
					setBinary(&value, 63-9, 1, 1, 1);
					setBinary(&value, 63-23, 3, 2, 1);
				} else if(!strcmp((char*)n->value, "F16")) {
					setBinary(&value, 63-20, 1, 2, 1);
				} else if(!strcmp((char*)n->value, "F32")) {
					setBinary(&value, 63-20, 2, 2, 1);
				} else if(!strcmp((char*)n->value, "F64")) {
					setBinary(&value, 63-20, 3, 2, 1);
				} else if(!strcmp((char*)n->value, "RM")) {
					setBinary(&value, 63-49, 1, 2, 1);
				} else if(!strcmp((char*)n->value, "RP")) {
					setBinary(&value, 63-49, 2, 2, 1);
				} else if(!strcmp((char*)n->value, "RZ")) {
					setBinary(&value, 63-49, 3, 2, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			if(inst->operands[1]->properties & prop_minus) {
				setBinary(&value, 63-8, 1, 1, 0);
			}
			if(inst->operands[1]->properties & prop_absolute_value) {
				setBinary(&value, 63-6, 1, 1, 0);
			}
			break;
		case opcode_I2I:
			value = 0x2038048000000038LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			if(inst->operands[1]->mods) {
				setBinary(&value, 63-26, getImme20(arch, inst->operands[1], 20, 0), 22, 0);
			} else {
				setBinary(&value, 63-26, getImme20(arch, inst->operands[1], 20, 1), 22, 0);
			}
			first = 1;
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 1);
				} else if(!strcmp((char*)n->value, "U8")) {
					if(!first) {
						setBinary(&value, 63-9, 0, 1, 1);
						setBinary(&value, 63-23, 0, 2, 1);
					} else {
						setBinary(&value, 63-7, 0, 1, 1);
						setBinary(&value, 63-20, 0, 2, 1);
					}
				} else if(!strcmp((char*)n->value, "U16")) {
					if(!first) {
						setBinary(&value, 63-9, 0, 1, 1);
						setBinary(&value, 63-23, 1, 2, 1);
					} else {
						setBinary(&value, 63-7, 0, 1, 1);
						setBinary(&value, 63-20, 1, 2, 1);
					}
				} else if(!strcmp((char*)n->value, "U32")) {
					if(!first) {
						setBinary(&value, 63-9, 0, 1, 1);
						setBinary(&value, 63-23, 2, 2, 1);
					} else {
						setBinary(&value, 63-7, 0, 1, 1);
						setBinary(&value, 63-20, 2, 2, 1);
					}
				} else if(!strcmp((char*)n->value, "U64")) {
					if(!first) {
						setBinary(&value, 63-9, 0, 1, 1);
						setBinary(&value, 63-23, 3, 2, 1);
					} else {
						setBinary(&value, 63-7, 0, 1, 1);
						setBinary(&value, 63-20, 3, 2, 1);
					}
				} else if(!strcmp((char*)n->value, "S8")) {
					if(!first) {
						setBinary(&value, 63-9, 1, 1, 1);
						setBinary(&value, 63-23, 0, 2, 1);
					} else {
						setBinary(&value, 63-7, 1, 1, 1);
						setBinary(&value, 63-20, 0, 2, 1);
					}
				} else if(!strcmp((char*)n->value, "S16")) {
					if(!first) {
						setBinary(&value, 63-9, 1, 1, 1);
						setBinary(&value, 63-23, 1, 2, 1);
					} else {
						setBinary(&value, 63-7, 1, 1, 1);
						setBinary(&value, 63-20, 1, 2, 1);
					}
				} else if(!strcmp((char*)n->value, "S32")) {
					if(!first) {
						setBinary(&value, 63-9, 1, 1, 1);
						setBinary(&value, 63-23, 2, 2, 1);
					} else {
						setBinary(&value, 63-7, 1, 1, 1);
						setBinary(&value, 63-20, 2, 2, 1);
					}
				} else if(!strcmp((char*)n->value, "S64")) {
					if(!first) {
						setBinary(&value, 63-9, 1, 1, 1);
						setBinary(&value, 63-23, 3, 3, 1);
					} else {
						setBinary(&value, 63-7, 1, 1, 1);
						setBinary(&value, 63-20, 3, 2, 1);
					}
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				first = 0;
				n = n->next;
			}
			
			n = inst->operands[0]->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s[0].%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			n = inst->operands[1]->mods;
			while(n) {
				if(!strcmp((char*)n->value, "NEG")) {
					setBinary(&value, 63-8, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s[1].%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			noMods = 2;
			
			if(inst->operands[1]->properties & prop_minus) {
				setBinary(&value, 63-8, 1, 1, 0);
			}
			if(inst->operands[1]->properties & prop_absolute_value) {
				setBinary(&value, 63-6, 1, 1, 0);
			}
			break;
		case opcode_F2I:
			value = 0x2138048000000028LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[1], 20, 1), 22, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 1);
				} else if(!strcmp((char*)n->value, "U16")) {
					setBinary(&value, 63-7, 0, 1, 1);
					setBinary(&value, 63-20, 1, 2, 1);
				} else if(!strcmp((char*)n->value, "U32")) {
					setBinary(&value, 63-7, 0, 1, 1);
					setBinary(&value, 63-20, 2, 2, 1);
				} else if(!strcmp((char*)n->value, "U64")) {
					setBinary(&value, 63-7, 0, 1, 1);
					setBinary(&value, 63-20, 3, 2, 1);
				} else if(!strcmp((char*)n->value, "S16")) {
					setBinary(&value, 63-7, 1, 1, 1);
					setBinary(&value, 63-20, 1, 2, 1);
				} else if(!strcmp((char*)n->value, "S32")) {
					setBinary(&value, 63-7, 1, 1, 1);
					setBinary(&value, 63-20, 2, 2, 1);
				} else if(!strcmp((char*)n->value, "S64")) {
					setBinary(&value, 63-7, 1, 1, 1);
					setBinary(&value, 63-20, 3, 2, 1);
				} else if(!strcmp((char*)n->value, "F16")) {
					setBinary(&value, 63-23, 1, 2, 1);
				} else if(!strcmp((char*)n->value, "F32")) {
					setBinary(&value, 63-23, 2, 2, 1);
				} else if(!strcmp((char*)n->value, "F64")) {
					setBinary(&value, 63-23, 3, 2, 1);
				} else if(!strcmp((char*)n->value, "FLOOR")) {
					setBinary(&value, 63-49, 1, 2, 1);
				} else if(!strcmp((char*)n->value, "CEIL")) {
					setBinary(&value, 63-49, 2, 2, 1);
				} else if(!strcmp((char*)n->value, "TRUNC")) {
					setBinary(&value, 63-49, 3, 2, 1);
				} else if(!strcmp((char*)n->value, "FTZ")) {
					setBinary(&value, 63-55, 1, 1, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			if(inst->operands[1]->properties & prop_minus) {
				setBinary(&value, 63-8, 1, 1, 0);
			}
			if(inst->operands[1]->properties & prop_absolute_value) {
				setBinary(&value, 63-6, 1, 1, 0);
			}
			break;
		case opcode_F2F:
			value = 0x2038048000000008LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[1], 20, 1), 22, 0);
			first = 1;
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 1);
				} else if(!strcmp((char*)n->value, "F16")) {
					if(!first) {
						setBinary(&value, 63-23, 1, 2, 1);
					} else {
						setBinary(&value, 63-20, 1, 2, 1);
					}
					first = 0;
				} else if(!strcmp((char*)n->value, "F32")) {
					if(!first) {
						setBinary(&value, 63-23, 2, 2, 1);
					} else {
						setBinary(&value, 63-20, 2, 2, 1);
					}
					first = 0;
				} else if(!strcmp((char*)n->value, "F64")) {
					if(!first) {
						setBinary(&value, 63-23, 3, 2, 1);
					} else {
						setBinary(&value, 63-20, 3, 2, 1);
					}
					first = 0;
				} else if(!strcmp((char*)n->value, "FTZ")) {
					setBinary(&value, 63-55, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "SAT")) {
					setBinary(&value, 63-5, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "PASS")) {
					setBinary(&value, 63-7, 0, 1, 1);
				} else if(!strcmp((char*)n->value, "ROUND")) {
					setBinary(&value, 63-7, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "RM")) {
					setBinary(&value, 63-49, 1, 2, 1);
				} else if(!strcmp((char*)n->value, "RP")) {
					setBinary(&value, 63-49, 2, 2, 1);
				} else if(!strcmp((char*)n->value, "RZ")) {
					setBinary(&value, 63-49, 3, 2, 1);
				} else if(!strcmp((char*)n->value, "CEIL")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-7, 1, 1, 0);
					setBinary(&value, 63-49, 2, 2, 1);
				} else if(!strcmp((char*)n->value, "FLOOR")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-7, 1, 1, 0);
					setBinary(&value, 63-49, 1, 2, 1);
				} else if(!strcmp((char*)n->value, "TRUNC")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-7, 1, 1, 0);
					setBinary(&value, 63-49, 3, 2, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			
			if(inst->operands[1]->properties & prop_minus) {
				setBinary(&value, 63-8, 1, 1, 0);
			}
			if(inst->operands[1]->properties & prop_absolute_value) {
				setBinary(&value, 63-6, 1, 1, 0);
			}
			break;
		case opcode_LOP:
			value = 0xc038000000000016LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 0), 22, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "AND")) {
					setBinary(&value, 63-6, 0, 2, 1);
				} else if(!strcmp((char*)n->value, "OR")) {
					setBinary(&value, 63-6, 1, 2, 0);
				} else if(!strcmp((char*)n->value, "XOR")) {
					setBinary(&value, 63-6, 2, 2, 0);
				} else if(!strcmp((char*)n->value, "PASS_B")) {
					setBinary(&value, 63-6, 3, 2, 0);
				} else if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			if(inst->operands[1]->properties & prop_bitwise_complement) {
				setBinary(&value, 63-9, 1, 1, 0);
			}
			if(inst->operands[2]->properties & prop_bitwise_complement) {
				setBinary(&value, 63-8, 1, 1, 0);
			}
			break;
		case opcode_LOP32I:
			value = 0x403800000000001cLL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 32, 0), 32, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "AND")) {
					setBinary(&value, 63-6, 0, 2, 1);
				} else if(!strcmp((char*)n->value, "OR")) {
					setBinary(&value, 63-6, 1, 2, 0);
				} else if(!strcmp((char*)n->value, "XOR")) {
					setBinary(&value, 63-6, 2, 2, 0);
				} else if(!strcmp((char*)n->value, "PASS_B")) {
					setBinary(&value, 63-6, 3, 2, 0);
				} else if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			if(inst->operands[1]->properties & prop_bitwise_complement) {
				setBinary(&value, 63-9, 1, 1, 0);
			}
			if(inst->operands[2]->properties & prop_bitwise_complement) {
				setBinary(&value, 63-8, 1, 1, 0);
			}
			break;
		case opcode_SHL:
			value = 0xc038000000000006LL;//Note: different than value on asfermi wiki
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 1), 22, 0);
			if(cuobjdump_version < 50) {
				setBinary(&value, 63-9, 1, 1, 0);
			}
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "U32")) {
					setBinary(&value, 63-5, 0, 1, 1);
				} else if(!strcmp((char*)n->value, "W")) {
					if(cuobjdump_version < 50) {
						setBinary(&value, 63-9, 2, 2, 1);
					} else {
						setBinary(&value, 63-9, 1, 1, 0);
					}
				} else if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			n = inst->operands[0]->mods;
			while(n) {
				if(!strcmp((char*)n->value, "CC")) {
					setBinary(&value, 63-48, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s[0].%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			noMods = 1;
			break;
		case opcode_SHR:
			value = 0xc43800000000001aLL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 1), 22, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "U32")) {
					setBinary(&value, 63-5, 0, 1, 1);
				} else if(!strcmp((char*)n->value, "W")) {
					setBinary(&value, 63-10, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			n = inst->operands[0]->mods;
			while(n) {
				if(!strcmp((char*)n->value, "CC")) {
					setBinary(&value, 63-48, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s[0].%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			noMods = 1;
			break;
		case opcode_BFE:
			value = 0xc43800000000000eLL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 1), 22, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "U32")) {
					setBinary(&value, 63-5, 0, 1, 1);
				} else if(!strcmp((char*)n->value, "BREV")) {
					setBinary(&value, 63-8, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			n = inst->operands[0]->mods;
			while(n) {
				if(!strcmp((char*)n->value, "CC")) {
					setBinary(&value, 63-48, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s[0].%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			noMods = 1;
			break;
		case opcode_BFI:
			value = 0xc038000000000014LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 1), 22, 0);
			setBinary(&value, 63-49, inst->operands[3]->val1, 6, 0);
			n = inst->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			break;
		case opcode_SEL:
			value = 0x2038000000000004LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 0), 22, 0);
			setBinary(&value, 63-49, inst->operands[3]->val1, 3, 1);
			if(inst->operands[3]->properties & prop_not) {
				setBinary(&value, 63-52, 1, 1, 1);
			} else {
				setBinary(&value, 63-52, 0, 1, 1);
			}
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_SSY:
			value = 0xe000000000000006LL;
			setBinary(&value, 63-26, getBinaryDifference(inst->operands[0]->val1, inst->address + inst->size, 24), 24, 0);
			n = inst->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			break;
		case opcode_BRA:
			value = 0xe7b8000000000002LL;
			setBinary(&value, 63-26, getBinaryDifference(inst->operands[0]->val1, inst->address + inst->size, 24), 24, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "LMT")) {
					setBinary(&value, 63-16, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "U")) {
					setBinary(&value, 63-15, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_BRX:
			value = 0xe7b8000000000012LL;//NOTE: not in asfermi wiki
			setBinary(&value, 63-20, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-26, inst->operands[1]->val1 * (inst->operands[1]->properties & prop_minus ? -1 : 1), 24, 0);
			n = inst->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			break;
		case opcode_PCNT:
			value = 0xe00000000000000eLL;//NOTE: contradicts value in asfermi wiki; PCNT cannot have predicate guard?
			setBinary(&value, 63-26, getBinaryDifference(inst->operands[0]->val1, inst->address + inst->size, 24), 24, 0);
			n = inst->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			break;
		case opcode_CONT:
			value = 0xe7b800000000000dLL;
			n = inst->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			break;
		case opcode_PBK:
			value = 0xe000000000000016LL;//NOTE: contradicts value in asfermi wiki; PBK cannot have predicate guard?
			setBinary(&value, 63-26, getBinaryDifference(inst->operands[0]->val1, inst->address + inst->size, 24), 24, 0);
			n = inst->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			break;
		case opcode_BRK:
			value = 0xe7b8000000000015LL;//NOTE: contradicts value in asfermi wiki; was missing a bit inside mod
			n = inst->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			break;
		case opcode_CAL:
			value = 0xe00080000000000aLL;
			setBinary(&value, 63-26, getBinaryDifference(inst->operands[0]->val1, inst->address + inst->size, 24), 24, 0);
			n = inst->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			break;
		case opcode_RET:
			value = 0xe7b8000000000009LL;
			n = inst->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			break;
		case opcode_EXIT:
			value = 0xe7b8000000000001LL;
			n = inst->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			break;
		case opcode_NOP:
			value = 0x27B8000000000002LL;
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "TRIG")) {
					setBinary(&value, 63-50, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			if(inst->num_operands > 0) {
				n = inst->operands[0]->mods;
				while(n) {
					if(!strcmp((char*)n->value, "F")) {
						setBinary(&value, 63-5, 0, 5, 1);
					} else if(!strcmp((char*)n->value, "LT")) {
						setBinary(&value, 63-5, 1, 5, 1);
					} else if(!strcmp((char*)n->value, "EQ")) {
						setBinary(&value, 63-5, 2, 5, 1);
					} else if(!strcmp((char*)n->value, "LE")) {
						setBinary(&value, 63-5, 3, 5, 1);
					} else if(!strcmp((char*)n->value, "GT")) {
						setBinary(&value, 63-5, 4, 5, 1);
					} else if(!strcmp((char*)n->value, "NE")) {
						setBinary(&value, 63-5, 5, 5, 1);
					} else if(!strcmp((char*)n->value, "GE")) {
						setBinary(&value, 63-5, 6, 5, 1);
					} else if(!strcmp((char*)n->value, "NUM")) {
						setBinary(&value, 63-5, 7, 5, 1);
					} else if(!strcmp((char*)n->value, "NAN")) {
						setBinary(&value, 63-5, 8, 5, 1);
					} else if(!strcmp((char*)n->value, "LTU")) {
						setBinary(&value, 63-5, 9, 5, 1);
					} else if(!strcmp((char*)n->value, "EQU")) {
						setBinary(&value, 63-5, 10, 5, 1);
					} else if(!strcmp((char*)n->value, "LEU")) {
						setBinary(&value, 63-5, 11, 5, 1);
					} else if(!strcmp((char*)n->value, "GTU")) {
						setBinary(&value, 63-5, 12, 5, 1);
					} else if(!strcmp((char*)n->value, "NEU")) {
						setBinary(&value, 63-5, 13, 5, 1);
					} else if(!strcmp((char*)n->value, "GEU")) {
						setBinary(&value, 63-5, 14, 5, 1);
					} else if(!strcmp((char*)n->value, "T")) {
						setBinary(&value, 63-5, 15, 5, 1);
					} else if(!strcmp((char*)n->value, "OFF")) {
						setBinary(&value, 63-5, 16, 5, 1);
					} else if(!strcmp((char*)n->value, "LO")) {
						setBinary(&value, 63-5, 17, 5, 1);
					} else if(!strcmp((char*)n->value, "SFF")) {
						setBinary(&value, 63-5, 18, 5, 1);
					} else if(!strcmp((char*)n->value, "LS")) {
						setBinary(&value, 63-5, 19, 5, 1);
					} else if(!strcmp((char*)n->value, "HI")) {
						setBinary(&value, 63-5, 20, 5, 1);
					} else if(!strcmp((char*)n->value, "SFT")) {
						setBinary(&value, 63-5, 21, 5, 1);
					} else if(!strcmp((char*)n->value, "HS")) {
						setBinary(&value, 63-5, 22, 5, 1);
					} else if(!strcmp((char*)n->value, "OFT")) {
						setBinary(&value, 63-5, 23, 5, 1);
					} else if(!strcmp((char*)n->value, "CSM_TA")) {
						setBinary(&value, 63-5, 24, 5, 1);
					} else if(!strcmp((char*)n->value, "CSM_TR")) {
						setBinary(&value, 63-5, 25, 5, 1);
					} else if(!strcmp((char*)n->value, "CSM_MX")) {
						setBinary(&value, 63-5, 26, 5, 1);
					} else if(!strcmp((char*)n->value, "FCSM_TA")) {
						setBinary(&value, 63-5, 27, 5, 1);
					} else if(!strcmp((char*)n->value, "FCSM_TR")) {
						setBinary(&value, 63-5, 28, 5, 1);
					} else if(!strcmp((char*)n->value, "FCSM_MX")) {
						setBinary(&value, 63-5, 29, 5, 1);
					} else if(!strcmp((char*)n->value, "RLE")) {
						setBinary(&value, 63-5, 30, 5, 1);
					} else if(!strcmp((char*)n->value, "RGT")) {
						setBinary(&value, 63-5, 31, 5, 1);
					} else {
						fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
					}
					n = n->next;
				}
				noMods = 1;
			}
			break;
		case opcode_BAR://TODO (medium-low priority) verify correctness, with brute force testing if necessary
			value = 0x203800000000700ALL;
			if(inst->num_operands == 1 && inst->operands[0]->type == type_register) {
				setBinary(&value, 63-20, inst->operands[0]->val1, 6, 0);
			} else {
				setBinary(&value, 63-14, getImme20(arch, inst->operands[0], 6, 0), 6, 0);
				if(inst->num_operands < 2 || inst->operands[1]->type != type_register) {
					setBinary(&value, 63-47, 1, 1, 0);
				}
			}
			if(inst->num_operands > 1) {
				setBinary(&value, 63-20, getImme20(arch, inst->operands[0], 6, 0), 6, 0);
			}
			if(inst->num_operands <= 1 || inst->operands[1]->type != type_register) {
				setBinary(&value, 63-46, 1, 1, 0);
			}
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "RED")) {
					setBinary(&value, 63-7, 0, 1, 1);
					setBinary(&value, 63-26, 63, 6, 0);//tcount default
					setBinary(&value, 63-53, 7, 3, 0);//p default
				} else if(!strcmp((char*)n->value, "ARV")) {
					setBinary(&value, 63-7, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "POPC")) {
					setBinary(&value, 63-8, 0, 2, 1);
				} else if(!strcmp((char*)n->value, "AND")) {
					setBinary(&value, 63-8, 1, 2, 1);
				} else if(!strcmp((char*)n->value, "OR")) {
					setBinary(&value, 63-8, 2, 2, 1);
				} else if(!strcmp((char*)n->value, "SYNC")) {//Note: not in asfermi wiki
					//TODO (medium-low priority): does this really do nothing to the binary?
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_BPT:
			value = 0xE00000000000000BLL;//Note: not in asfermi wiki
			fprintf(stderr,"Warning: BPT operation not fully implemented.\n");//TODO (medium-low priority)
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "DRAIN")) {
					//TODO (medium-low priority)
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_B2R:
			value = 0x203800000000001cLL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			if(inst->operands[1]->type == type_hex) {
				setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			} else if(inst->operands[1]->type == type_predicate) {
				setBinary(&value, 63-26, 0x3f, 6, 0);
			} else {
				fprintf(stderr, "ERROR: Unexpected operand for B2R.\n");
			}
			
			fprintf(stderr,"Warning: B2R operation not fully implemented.\n");//TODO (medium-low priority)
			break;
		case opcode_S2R:
			value = 0x2038000000000034LL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-26, inst->operands[1]->val1, 8, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_PSETP:
			value = 0x20380e0000000030LL;
			setBinary(&value, 63-17, inst->operands[0]->val1, 3, 1);
			setBinary(&value, 63-14, inst->operands[1]->val1, 3, 1);
			setBinary(&value, 63-26, inst->operands[3]->val1, 3, 1);
			if(cuobjdump_version >= 55) {
				setBinary(&value, 63-20, inst->operands[2]->val1, 3, 1);
				setBinary(&value, 63-49, inst->operands[4]->val1, 3, 1);
				if(inst->operands[4]->properties & prop_not) {//TODO (low priority) make sure this wasn't mixed up with operand 2's bits
					setBinary(&value, 63-52, 1, 1, 1);
				} else {
					setBinary(&value, 63-52, 0, 1, 1);
				}
				if(inst->operands[2]->properties & prop_not) {
					setBinary(&value, 63-23, 1, 1, 1);
				} else {
					setBinary(&value, 63-23, 0, 1, 1);
				}
			} else if(cuobjdump_version <= 50) {
				setBinary(&value, 63-49, inst->operands[2]->val1, 3, 1);
				setBinary(&value, 63-20, inst->operands[4]->val1, 3, 1);
				if(inst->operands[2]->properties & prop_not) {//TODO (low priority) make sure this wasn't mixed up with operand 4's bits
					setBinary(&value, 63-52, 1, 1, 1);
				} else {
					setBinary(&value, 63-52, 0, 1, 1);
				}
				if(inst->operands[4]->properties & prop_not) {
					setBinary(&value, 63-23, 1, 1, 1);
				} else {
					setBinary(&value, 63-23, 0, 1, 1);
				}
			}
			
			if(inst->operands[3]->properties & prop_not) {
				setBinary(&value, 63-29, 1, 1, 1);
			} else {
				setBinary(&value, 63-29, 0, 1, 1);
			}
			
			n = inst->mods;
			first = 1;
			while(n) {
				if(!strcmp((char*)n->value, "AND")) {
					if(first) {
						setBinary(&value, 63-30, 0, 2, 1);
					} else {
						setBinary(&value, 63-53, 0, 2, 1);
					}
				} else if(!strcmp((char*)n->value, "OR")) {
					if(first) {
						setBinary(&value, 63-30, 1, 2, 1);
					} else {
						setBinary(&value, 63-53, 1, 2, 1);
					}
				} else if(!strcmp((char*)n->value, "XOR")) {
					if(first) {
						setBinary(&value, 63-30, 2, 2, 1);
					} else {
						setBinary(&value, 63-53, 2, 2, 1);
					}
				} else if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				first = 0;
				n = n->next;
			}
			break;
		case opcode_PSET:
			value = 0x2038000000000010LL;//NOTE: not in asfermi wiki
			if(inst->operands[2]->properties & prop_not || inst->operands[3]->properties & prop_not) {
				fprintf(stderr,"ERROR: PSET binary will be incorrect.\n");//TODO (medium-low priority)
			}
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-26, inst->operands[2]->val1, 3, 0);
			
			if(cuobjdump_version >= 55) {
				setBinary(&value, 63-20, inst->operands[1]->val1, 3, 0);
				setBinary(&value, 63-49, inst->operands[3]->val1, 3, 0);
				if(inst->operands[1]->properties & prop_not) {//TODO (low priority) make sure this wasn't mixed up with operand 3's bits
					setBinary(&value, 63-52, 1, 1, 1);
				} else {
					setBinary(&value, 63-52, 0, 1, 1);
				}
			} else if(cuobjdump_version <= 50) {
				setBinary(&value, 63-20, inst->operands[3]->val1, 3, 0);
				setBinary(&value, 63-49, inst->operands[1]->val1, 3, 0);
				if(inst->operands[3]->properties & prop_not) {//TODO (low priority) make sure this wasn't mixed up with operand 1's bits
					setBinary(&value, 63-52, 1, 1, 1);
				} else {
					setBinary(&value, 63-52, 0, 1, 1);
				}
			}
			
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 1);
				} else if(!strcmp((char*)n->value, "LT")) {
					setBinary(&value, 63-55, 1, 4, 0);
				} else if(!strcmp((char*)n->value, "EQ")) {
					setBinary(&value, 63-55, 2, 4, 0);
				} else if(!strcmp((char*)n->value, "LE")) {
					setBinary(&value, 63-55, 3, 4, 0);
				} else if(!strcmp((char*)n->value, "GT")) {
					setBinary(&value, 63-55, 4, 4, 0);
				} else if(!strcmp((char*)n->value, "NE")) {
					setBinary(&value, 63-55, 5, 4, 0);
				} else if(!strcmp((char*)n->value, "GE")) {
					setBinary(&value, 63-55, 6, 4, 0);
				} else if(!strcmp((char*)n->value, "NUM")) {
					setBinary(&value, 63-55, 7, 4, 0);
				} else if(!strcmp((char*)n->value, "NAN")) {
					setBinary(&value, 63-55, 8, 4, 0);
				} else if(!strcmp((char*)n->value, "LTU")) {
					setBinary(&value, 63-55, 9, 4, 0);
				} else if(!strcmp((char*)n->value, "EQU")) {
					setBinary(&value, 63-55, 10, 4, 0);
				} else if(!strcmp((char*)n->value, "LEU")) {
					setBinary(&value, 63-55, 11, 4, 0);
				} else if(!strcmp((char*)n->value, "GTU")) {
					setBinary(&value, 63-55, 12, 4, 0);
				} else if(!strcmp((char*)n->value, "NEU")) {
					setBinary(&value, 63-55, 13, 4, 0);
				} else if(!strcmp((char*)n->value, "GEU")) {
					setBinary(&value, 63-55, 14, 4, 0);
				} else if(!strcmp((char*)n->value, "AND")) {
					setBinary(&value, 63-53, 0, 2, 0);
				} else if(!strcmp((char*)n->value, "OR")) {
					setBinary(&value, 63-53, 1, 2, 0);
				} else if(!strcmp((char*)n->value, "XOR")) {
					setBinary(&value, 63-53, 2, 2, 0);
				} else if(!strcmp((char*)n->value, "BF")) {
					setBinary(&value, 63-7, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_FLO:
			value = 0xc43800000000001eLL;//NOTE: not in asfermi wiki
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[1], 20, 0), 22, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "U32")) {
					setBinary(&value, 63-5, 0, 1, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_P2R:
			value = 0x203800000000000cLL;//NOTE: not in asfermi wiki
			fprintf(stderr,"Warning: P2R operation not fully implemented.\n");//TODO (medium-low priority) - what is PR (operand[1])
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[2]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[3], 20, 0), 22, 0);
			n = inst->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			break;
		case opcode_R2P:
			value = 0x203800000000002cLL;//NOTE: not in asfermi wiki
			fprintf(stderr,"Warning: R2P operation not fully implemented.\n");//TODO (medium-low priority) - what is PR (operand[0])
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 0), 22, 0);
			n = inst->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			break;
		case opcode_TEX:
			value = 0x6038000000000001LL;//NOTE: not in asfermi wiki
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			if(inst->operands[2]->type == type_register) {
				setBinary(&value, 63-26, inst->operands[2]->val1, 6, 0);
				if(inst->num_operands == 7) {
					setBinary(&value, 63-32, inst->operands[3]->val1, 8, 0);
					setBinary(&value, 63-40, inst->operands[4]->val1, 5, 0);
					setBinary(&value, 63-51, inst->operands[5]->val1, 3, 0);
					setBinary(&value, 63-46, inst->operands[6]->val1, 4, 0);
				} else {
					setBinary(&value, 63-40, inst->operands[3]->val1, 5, 0);
					setBinary(&value, 63-51, inst->operands[4]->val1, 3, 0);
					setBinary(&value, 63-46, inst->operands[5]->val1, 4, 0);
				}
			} else {
				setBinary(&value, 63-26, 63, 6, 0);
				setBinary(&value, 63-32, inst->operands[2]->val1, 8, 0);
				if(inst->num_operands == 6) {
					setBinary(&value, 63-40, inst->operands[3]->val1, 5, 0);
					setBinary(&value, 63-51, inst->operands[4]->val1, 3, 0);
					setBinary(&value, 63-46, inst->operands[5]->val1, 4, 0);
				} else {
					setBinary(&value, 63-51, inst->operands[3]->val1, 3, 0);
					setBinary(&value, 63-46, inst->operands[4]->val1, 4, 0);
				}
			}
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "T")) {
					setBinary(&value, 63-7, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "P")) {
					setBinary(&value, 63-8, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "NODEP")) {
					setBinary(&value, 63-9, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "NDV")) {
					setBinary(&value, 63-45, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "I")) {
					setBinary(&value, 63-50, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_TEXDEPBAR:
			value = 0x67b800000000000fLL;//NOTE: not in asfermi wiki
			setBinary(&value, 63-26, getImme20(arch, inst->operands[0], 22, 0), 22, 0);
			n = inst->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			break;
		case opcode_RRO:
			value = 0x0038000000000006LL;//NOTE: not in asfermi wiki
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[1], 20, 0), 22, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "EX2")) {
					setBinary(&value, 63-5, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_PRMT:
			value = 0x2038000000000024;//NOTE: not in asfermi wiki
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 0), 22, 0);
			setBinary(&value, 63-49, inst->operands[3]->val1, 6, 0);
			n = inst->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			break;
		case opcode_VADD:
			value = 0x2638000000200003;//NOTE: not in asfermi wiki
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 16, 0), 16, 0);
			setBinary(&value, 63-49, inst->operands[3]->val1, 6, 0);
			setBinary(&value, 63-45, 3, 2, 1);
			if(inst->operands[2]->type == type_register) {
				setBinary(&value, 63-47, 1, 1, 1);
			}
			first = 1;
			
			if(inst->operands[2]->properties & prop_minus) {
				setBinary(&value, 63-7, 1, 1, 1);
			}
			if(inst->operands[1]->properties & prop_minus) {
				setBinary(&value, 63-8, 1, 1, 1);
			}
			
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 1);
				} else if(!strcmp((char*)n->value, "SAT")) {
					setBinary(&value, 63-9, 1, 1, 1);
				} else if(!strcmp((char*)n->value, "PO")) {
					setBinary(&value, 63-7, 3, 2, 1);
				} else if(!strcmp((char*)n->value, "UD")) {
					setBinary(&value, 63-42, 0, 1, 1);
				} else if(!strcmp((char*)n->value, "MRG_16H")) {
					setBinary(&value, 63-55, 0, 3, 1);
				} else if(!strcmp((char*)n->value, "MRG_16L")) {
					setBinary(&value, 63-55, 1, 3, 1);
				} else if(!strcmp((char*)n->value, "MRG_8B0")) {
					setBinary(&value, 63-55, 2, 3, 1);
				} else if(!strcmp((char*)n->value, "MRG_8B2")) {
					setBinary(&value, 63-55, 3, 3, 1);
				} else if(!strcmp((char*)n->value, "ACC")) {
					setBinary(&value, 63-55, 4, 3, 1);
				} else if(!strcmp((char*)n->value, "MIN")) {
					setBinary(&value, 63-55, 5, 3, 1);
				} else if(!strcmp((char*)n->value, "MAX")) {
					setBinary(&value, 63-55, 6, 3, 1);
				} else if(!strcmp((char*)n->value, "S8")) {
					first = 0;
				} else if(!strcmp((char*)n->value, "S16")) {
					if(!first) {
						//empty?
					} else {
						setBinary(&value, 63-45, 1, 1, 1);
					}
					first = 0;
				} else if(!strcmp((char*)n->value, "S32")) {
					if(!first) {
						setBinary(&value, 63-33, 3, 2, 0);
					} else {
						setBinary(&value, 63-45, 3, 2, 1);
					}
					first = 0;
				} else if(!strcmp((char*)n->value, "U8")) {
					if(!first) {
						setBinary(&value, 63-5, 0, 1, 1);
					} else {
						setBinary(&value, 63-6, 0, 1, 1);
					}
					first = 0;
				} else if(!strcmp((char*)n->value, "U16")) {
					if(!first) {
						setBinary(&value, 63-5, 0, 1, 1);
						//?
					} else {
						setBinary(&value, 63-6, 0, 1, 1);
						setBinary(&value, 63-45, 1, 1, 1);
					}
					first = 0;
				} else if(!strcmp((char*)n->value, "U32")) {
					if(!first) {
						setBinary(&value, 63-5, 0, 1, 1);
						setBinary(&value, 63-33, 3, 2, 0);
					} else {
						setBinary(&value, 63-6, 0, 1, 1);
						setBinary(&value, 63-45, 3, 2, 1);
					}
					first = 0;
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			
			if(first) {
				setBinary(&value, 63-33, 3, 2, 1);
				setBinary(&value, 63-47, 1, 1, 1);
			}
			
			n = inst->operands[0]->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s[0].%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			n = inst->operands[1]->mods;
			while(n) {
				if(!strcmp((char*)n->value, "B1")) {
					setBinary(&value, 63-44, 1, 2, 1);
				} else if(!strcmp((char*)n->value, "B2")) {
					setBinary(&value, 63-44, 2, 2, 0);
				} else if(!strcmp((char*)n->value, "B3")) {
					setBinary(&value, 63-44, 3, 2, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s[1].%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			n = inst->operands[2]->mods;
			while(n) {
				if(!strcmp((char*)n->value, "B1")) {
					setBinary(&value, 63-32, 1, 2, 1);
				} else if(!strcmp((char*)n->value, "B2")) {
					setBinary(&value, 63-32, 2, 2, 0);
				} else if(!strcmp((char*)n->value, "B3")) {
					setBinary(&value, 63-32, 3, 2, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s[1].%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			noMods = 3;
			break;
		case opcode_DMNMX:
			value = 0x8038000000007010LL;//NOTE: not in wiki
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 0), 22, 0);
			setBinary(&value, 63-49, inst->operands[3]->val1, 3, 0);
			if(inst->operands[3]->properties & prop_not) {
				setBinary(&value, 63-52, 1, 1, 1);
			} else {
				setBinary(&value, 63-52, 0, 1, 1);
			}
			n = inst->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			break;
		case opcode_FMNMX:
			value = 0x0038000000007010LL;//NOTE: not in wiki
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 20, 0), 22, 0);
			setBinary(&value, 63-49, inst->operands[3]->val1, 3, 0);
			if(inst->operands[3]->properties & prop_not) {
				setBinary(&value, 63-52, 1, 1, 1);
			} else {
				setBinary(&value, 63-52, 0, 1, 1);
			}
			n = inst->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			break;
		case opcode_RED:
			value = 0xA038000000000008LL;
			setBinary(&value, 63-20, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-32, inst->operands[0]->val2, 32, 0);//TODO (medium priority) verify correctness
			setBinary(&value, 63-14, inst->operands[1]->val1, 6, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "ADD")) {
					setBinary(&value, 63-5, 0, 4, 0);
				} else if(!strcmp((char*)n->value, "MIN")) {
					setBinary(&value, 63-5, 1, 4, 0);
				} else if(!strcmp((char*)n->value, "MAX")) {
					setBinary(&value, 63-5, 2, 4, 0);
				} else if(!strcmp((char*)n->value, "INC")) {
					setBinary(&value, 63-5, 3, 4, 0);
				} else if(!strcmp((char*)n->value, "DEC")) {
					setBinary(&value, 63-5, 4, 4, 0);
				} else if(!strcmp((char*)n->value, "AND")) {
					setBinary(&value, 63-5, 5, 4, 0);
				} else if(!strcmp((char*)n->value, "OR")) {
					setBinary(&value, 63-5, 6, 4, 0);
				} else if(!strcmp((char*)n->value, "XOR")) {
					setBinary(&value, 63-5, 7, 4, 0);
				} else if(!strcmp((char*)n->value, "EXCH")) {
					setBinary(&value, 63-5, 8, 4, 0);
				} else if(!strcmp((char*)n->value, "CAS")) {
					setBinary(&value, 63-5, 9, 4, 0);
				} else if(!strcmp((char*)n->value, "E")) {
					setBinary(&value, 63-58, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "U64")) {//TODO (medium-high priority) add this to list of 64bit instructions
					setBinary(&value, 63-9, 1, 1, 0);
					setBinary(&value, 63-59, 2, 2, 1);
				} else if(!strcmp((char*)n->value, "S32")) {
					setBinary(&value, 63-9, 1, 1, 0);
					setBinary(&value, 63-59, 3, 3, 1);
				} else if(!strcmp((char*)n->value, "F32") && n->next && n->next->next && !strcmp((char*)n->next->value, "FTZ") && !strcmp((char*)n->next->next->value, "RN")) {
					setBinary(&value, 63-9, 1, 1, 0);
					setBinary(&value, 63-59, 5, 3, 1);
					n = n->next;
					n = n->next;
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_VOTE://TODO (medium priority) test conversion for correctness; probe for details
			value = 0x2038000000000012LL;
			if(inst->operands[0]->type == type_register) {
				setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
				setBinary(&value, 63-54, inst->operands[1]->val1, 3, 0);
				setBinary(&value, 63-20, inst->operands[2]->val1, 3, 0);
				if(inst->operands[2]->properties & prop_not) {
					setBinary(&value, 63-23, 1, 1, 1);
				}
			} else {
				setBinary(&value, 63-14, -1, 6, 0);
				setBinary(&value, 63-54, inst->operands[0]->val1, 3, 0);
				setBinary(&value, 63-20, inst->operands[1]->val1, 3, 0);
				if(inst->operands[1]->properties & prop_not) {
					setBinary(&value, 63-23, 1, 1, 1);
				}
			}
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "ALL")) {
					setBinary(&value, 63-5, 0, 3, 0);
				} else if(!strcmp((char*)n->value, "ANY")) {
					setBinary(&value, 63-5, 1, 3, 0);
				} else if(!strcmp((char*)n->value, "EQ")) {
					setBinary(&value, 63-5, 2, 3, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_POPC://TODO (medium priority) test conversion for correctness
			value = 0x203800000000002ALL;
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, inst->operands[2]->val1, 6, 0);
			if(inst->operands[0]->type != type_register || inst->operands[1]->type != type_register || inst->operands[2]->type != type_register) {
				fprintf(stderr,"SANITY CHECK FAILED ~3050\n");
			}
			n = inst->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			break;
		case opcode_MEMBAR:
			value = 0xA038000000000007LL;
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "CTA")) {
					setBinary(&value, 63-5, 0, 2, 0);
				} else if(!strcmp((char*)n->value, "GL")) {
					setBinary(&value, 63-5, 1, 2, 0);
				} else if(!strcmp((char*)n->value, "SYS")) {
					setBinary(&value, 63-5, 2, 2, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_STSCUL://TODO (medium priority) test conversion for correctness; probe for details
			value = 0xA03800000000001DLL;
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-14, inst->operands[2]->val1, 6, 0);
			n = inst->mods;
			while(n) {
				fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				n = n->next;
			}
			break;
		case opcode_LEPC:
			value = 0x2038000000000022LL;//NOTE: value taken from asfermi wiki; not yet confirmed by myself as nvcc value
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_CSETP:
			value = 0x2038000000000020LL;//Note: not in asfermi wiki
			setBinary(&value, 63-17, inst->operands[0]->val1, 4, 0);
			setBinary(&value, 63-14, inst->operands[1]->val1, 4, 0);
			setBinary(&value, 63-49, inst->operands[3]->val1, 4, 0);
			setBinary(&value, 63-24, 1, 1, 0); //shrug
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "AND")) {
					
				} else if(!strcmp((char*)n->value, "F")) {
					
				} else if(!strcmp((char*)n->value, "OFF")) {
					
				} else if(!strcmp((char*)n->value, "LO")) {
					setBinary(&value, 63-20, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_ISCADD32I:
			value = 0x4038000000000002LL;//Note: not in asfermi wiki
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, getImme20(arch, inst->operands[2], 32, 0), 32, 0);
			setBinary(&value, 63-5, getImme20(arch, inst->operands[3], 5, 1), 5, 0);
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {//NOTE: not in asfermi wiki
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_VMNMX:
			value = 0x20380000602700D3LL;//Note: not in asfermi wiki
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, inst->operands[2]->val1, 6, 0);
			setBinary(&value, 63-49, inst->operands[3]->val1, 6, 0);
			setBinary(&value, 63-5, 3, 2, 0);
			n = inst->mods;
			//TODO (medium-low priority) add missing mods
			//setBinary(&value, 63-5, 1, 2, 0);//.u32.s32.max
			//setBinary(&value, 63-5, 2, 2, 0);//.s32.u32.max
			//setBinary(&value, 63-5, 3, 2, 0);//.max
			//setBinary(&value, 63-5, 4, 3, 0);//.u32.u32.mx.max
			//setBinary(&value, 63-5, 5, 3, 0);//.u32.s32.mx.max
			while(n) {
				if(!strcmp((char*)n->value, "S")) {//untested
					setBinary(&value, 63-4, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "MX")) {
					setBinary(&value, 63-7, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "MAX")) {
					//nothing?
				} else if(!strcmp((char*)n->value, "ACC")) {
					setBinary(&value, 63-56, 0, 1, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_TLD:
			value = 0x6038003F00000009LL;//Note: not in asfermi wiki
			setBinary(&value, 63-14, inst->operands[0]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-32, inst->operands[2]->val1, 8, 0);
			setBinary(&value, 63-51, inst->operands[3]->val1, 3, 0);
			setBinary(&value, 63-46, inst->operands[4]->val1, 4, 0);
			
			if(arch < 30) {//TODO: not sure what the deal is with this
				setBinary(&value, 63-40, inst->operands[2]->val1, 6, 0);
			}
			
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {//Untested, but seems safe to assume
					setBinary(&value, 63-4, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "LZ")) {
					//nothing?
				} else if(!strcmp((char*)n->value, "T")) {
					setBinary(&value, 63-7, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "P")) {
					setBinary(&value, 63-8, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_SHF:
			value = 0x0LL;//Note: not in asfermi wiki
			fprintf(stderr,"ERROR: unsupported opcode: \"%s\".\n", opcodes[inst->op]);//TODO (medium-low priority)
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {//Untested, but seems safe to assume
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_FCHK:
			value = 0x0LL;//Note: not in asfermi wiki
			fprintf(stderr,"ERROR: unsupported opcode: \"%s\".\n", opcodes[inst->op]);//TODO (medium-low priority)
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {//Untested, but seems safe to assume
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_JCAL:
			value = 0xE000800000000008LL;
			if(inst->operands[0]->type != type_hex || inst->num_operands != 1) {
				cerr << "ERROR: Unexpected operands for opcode " << opcodes[inst->op] << ".\n";
			} else {
				setBinary(&value, 63-26, inst->operands[0]->val1, 24, 0);
			}
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {//Untested, but seems safe to assume
					setBinary(&value, 63-4, 1, 1, 0);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_SHFL:
			value = 0xA0F8000000000031LL;
			setBinary(&value, 63-8, inst->operands[0]->val1, 2, 1);
			setBinary(&value, 63-58, inst->operands[0]->val1 >> 2, 1, 1);
			setBinary(&value, 63-14, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-20, inst->operands[2]->val1, 6, 0);
			
			setBinary(&value, 63-5, inst->operands[3]->type == type_hex, 1, 1);
			setBinary(&value, 63-6, inst->operands[4]->type == type_hex, 1, 1);
			
			if(inst->operands[3]->type == type_hex) {
				setBinary(&value, 63-26, inst->operands[3]->val1, 5, 0);
				if(inst->operands[3]->val1 > 31) {
					cerr << "ERROR: invalid value for SHFL[3]\n";
				}
			} else {
				setBinary(&value, 63-26, inst->operands[3]->val1, 6, 0);
			}
			
			if(inst->operands[4]->type == type_hex) {
				setBinary(&value, 63-42, inst->operands[4]->val1, 5, 0);
				if(inst->operands[4]->val1 > 31) {
					cerr << "ERROR: invalid value for SHFL[4]\n";
				}
			} else {
				setBinary(&value, 63-49, inst->operands[4]->val1, 6, 0);
			}
			
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "IDX")) {
					setBinary(&value, 63-56, 0, 1, 1);
				} else if(!strcmp((char*)n->value, "DOWN")) {
					setBinary(&value, 63-56, 1, 1, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_ATOM:
			value = 0xa03800000000000aLL;
			setBinary(&value, 63-43, inst->operands[0]->val1, 6, 1);
			setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
			setBinary(&value, 63-26, inst->operands[1]->val2 >> 3, 17, 1);
			setBinary(&value, 63-56, inst->operands[1]->val2 & 0x7, 3, 1);
			setBinary(&value, 63-14, inst->operands[2]->val1, 6, 0);
			if(inst->num_operands > 3) {
				setBinary(&value, 63-49, inst->operands[3]->val1, 6, 0);
			} else {
				setBinary(&value, 63-49, -1, 6, 0);
			}
			
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "E")) {
					setBinary(&value, 63-58, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "U32")) {
					setBinary(&value, 63-9, 1, 1, 1);
					setBinary(&value, 63-59, 2, 3, 1);
				} else if(!strcmp((char*)n->value, "S32")) {
					setBinary(&value, 63-9, 1, 1, 1);
					setBinary(&value, 63-59, 3, 3, 1);
				} else if(!strcmp((char*)n->value, "F32")) {
					setBinary(&value, 63-9, 1, 1, 1);
					setBinary(&value, 63-59, 5, 3, 1);
				} else if(!strcmp((char*)n->value, "FTZ")) {
					//empty; always paired with F32
				} else if(!strcmp((char*)n->value, "RN")) {
					//empty; always paired with F32
				} else if(!strcmp((char*)n->value, "ADD")) {
					setBinary(&value, 63-5, 0, 4, 1);
				} else if(!strcmp((char*)n->value, "MIN")) {
					setBinary(&value, 63-5, 1, 4, 1);
				} else if(!strcmp((char*)n->value, "MAX")) {
					setBinary(&value, 63-5, 2, 4, 1);
				} else if(!strcmp((char*)n->value, "INC")) {
					setBinary(&value, 63-5, 3, 4, 1);
				} else if(!strcmp((char*)n->value, "DEC")) {
					setBinary(&value, 63-5, 4, 4, 1);
				} else if(!strcmp((char*)n->value, "AND")) {
					setBinary(&value, 63-5, 5, 4, 1);
				} else if(!strcmp((char*)n->value, "OR")) {
					setBinary(&value, 63-5, 6, 4, 1);
				} else if(!strcmp((char*)n->value, "XOR")) {
					setBinary(&value, 63-5, 7, 4, 1);
				} else if(!strcmp((char*)n->value, "EXCH")) {
					setBinary(&value, 63-5, 8, 4, 1);
				} else if(!strcmp((char*)n->value, "CAS")) {
					setBinary(&value, 63-5, 9, 4, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_CCTL:
			value = 0xa038000000000019LL;
			if(inst->num_operands == 2) {
				setBinary(&value, 63-14, inst->operands[0]->val1, 6, 1);
				setBinary(&value, 63-20, inst->operands[1]->val1, 6, 0);
				setBinary(&value, 63-28, inst->operands[1]->val2, 30, 0);
			} else if(inst->num_operands == 1) {
				setBinary(&value, 63-20, inst->operands[0]->val1, 6, 0);
				setBinary(&value, 63-28, inst->operands[0]->val2, 30, 0);
			} else {
				fprintf(stderr,"SANITY CHECK ERROR b20~2892: unrecognized operand config for \"%s\".\n", opcodes[inst->op]);
			}
			
			n = inst->mods;
			while(n) {
				if(!strcmp((char*)n->value, "S")) {
					setBinary(&value, 63-4, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "E")) {
					setBinary(&value, 63-58, 1, 1, 0);
				} else if(!strcmp((char*)n->value, "U")) {
					setBinary(&value, 63-26, 1, 2, 1);
				} else if(!strcmp((char*)n->value, "C")) {
					setBinary(&value, 63-26, 2, 2, 1);
				} else if(!strcmp((char*)n->value, "I")) {
					setBinary(&value, 63-26, 3, 2, 1);
				} else if(!strcmp((char*)n->value, "QRY1")) {
					setBinary(&value, 63-5, 0, 3, 1);
				} else if(!strcmp((char*)n->value, "PF1")) {
					setBinary(&value, 63-5, 1, 3, 1);
				} else if(!strcmp((char*)n->value, "PF1_5")) {
					setBinary(&value, 63-5, 2, 3, 1);
				} else if(!strcmp((char*)n->value, "PR2")) {
					setBinary(&value, 63-5, 3, 3, 1);
				} else if(!strcmp((char*)n->value, "WB")) {
					setBinary(&value, 63-5, 4, 3, 1);
				} else if(!strcmp((char*)n->value, "IV")) {
					setBinary(&value, 63-5, 5, 3, 1);
				} else if(!strcmp((char*)n->value, "IVALL")) {
					setBinary(&value, 63-5, 6, 3, 1);
				} else if(!strcmp((char*)n->value, "RS")) {
					setBinary(&value, 63-5, 7, 3, 1);
				} else {
					fprintf(stderr,"Error: unrecognized mod: \"%s.%s\".\n", opcodes[inst->op], (char*)n->value);
				}
				n = n->next;
			}
			break;
		case opcode_PHI:
			cerr << "FATAL ERROR b20~3163: Trying to write PHI function to binary.\n";
			exit(0);
		default:
			fprintf(stderr,"SANITY CHECK FAILED ~2969: unrecognized opcode: \"%s\".\n", opcodes[inst->op]);
			break;
	}
	if(inst->guard) {
		setBinary(&value, 63-10, inst->guard->val1, 3, 1);
		if(inst->guard->properties & prop_not) {
			setBinary(&value, 63-13, 1, 1, 0);
		} else {
			if(inst->guard->properties & prop_not) {
				setBinary(&value, 63-13, 0, 1, 1);
			}
		}
	}
	
	//Check operands for unprocessed mods
	for(int x = noMods; x < inst->num_operands; x++) {
		n = inst->operands[x]->mods;
		while(n) {
			fprintf(stderr,"Error: unrecognized mod: \"%s[%d].%s\".\n", opcodes[inst->op], x, (char*)n->value);
			n = n->next;
		}
	}
	
	//A special mod to help debug operands:
	n = inst->mods;
	while(n) {
		if(!strcmp((char*)n->value, "FORCEBITS")) {
			n = n->next;
			int index = atoi((char*)n->value);
			n = n->next;
			int val = atoi((char*)n->value);
			n = n->next;
			int size = atoi((char*)n->value);
			setBinary(&value, 63-index, val, size, 1);
		}
		n = n->next;
	}
	
	//These next three loops convert the binary to the expected format:
	int x;
	char * hex = (char*) malloc(17);
	hex[16] = 0;
	for(x = 0; x < 8; x++) {
		int y;
		int temp = 0;
		for(y = 0; y < 4; y++) {
			if((value >> (8*x+y)) & 0x1) {
				temp += 0x1 << (3 - y);
			}
		}
		if(temp < 10) {
			hex[2*x+1] = '0' + temp;
		} else {
			hex[2*x+1] = 'a' + temp - 10;
		}
		
		temp = 0;
		for(y = 4; y < 8; y++) {
			if((value >> (8*x+y)) & 0x1) {
				temp += 0x1 << (7 - y);
			}
		}
		if(temp < 10) {
			hex[2*x] = '0' + temp;
		} else {
			hex[2*x] = 'a' + temp - 10;
		}
	}
	for(x = 0; x < 16; x += 8) {
		char temp = hex[x];
		hex[x] = hex[x + 6];
		hex[x + 6] = temp;
		temp = hex[x + 1];
		hex[x + 1] = hex[x + 7];
		hex[x + 7] = temp;
		
		temp = hex[x + 2];
		hex[x + 2] = hex[x + 4];
		hex[x + 4] = temp;
		temp = hex[x + 3];
		hex[x + 3] = hex[x + 5];
		hex[x + 5] = temp;
	}
	for(x = 0; x < 8; x++) {
		char temp = hex[x];
		hex[x] = hex[15 - x];
		hex[15 - x] = temp;
	}
	
	return hex;
}

