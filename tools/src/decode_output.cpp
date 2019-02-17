#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode_common.hpp"
#include "decode_output.hpp"
#include "decode.hpp"
using namespace std;

#ifdef WINDOWS
	#include <inttypes.h>
	#define LLx "I64x"
#else
	#define LLx "llx"
#endif

extern node * _blocks;
extern char* _kernel_name;
extern int _arch;
extern int _cuobjdump_version;
extern int _shared_memory;
extern int _min_stack_size;
extern int _frame_size;
extern char** _function_names;
extern int _num_functions;
extern node * _functions;
extern std::string _specialNames35[];

void printfMetadata() {
	if(_kernel_name) {
		cout << "//Name: " << _kernel_name << "\n";
	}
	cout << "//Arch: sm_" << _arch << "\n";
	cout << "//cuobjdump: " << _cuobjdump_version << "\n";
	cout << "//Shared memory usage: " << _shared_memory << "\n";
	cout << "//Min Stack Size: " << _min_stack_size << "\n";
	cout << "//Frame Size: " << _frame_size << "\n";
	if(_function_names) {
		cout << "//Function count: " << _num_functions << "\n";
		for(int x = 0; x < _num_functions; x++) {
			cout << "//Function: " << _function_names[x] << "\n";
		}
	}
}

char * operandToString(instruction * inst, operand * op) {
	char * answer = (char*) malloc(sizeof(char) * 256);
	char * pointer = answer;
	bool mustShowHex = false; //used for mem operand
	
	int counter;//because g++ gets mad if anything is defined inside a case
	
	switch (op->type) {
		case type_register:
			if(op->properties & prop_not) {
				sprintf(pointer, "!");
				pointer++;
			} else if(op->properties & prop_minus) {
				sprintf(pointer, "-");
				pointer++;
			} else if(op->properties & prop_bitwise_complement) {
				sprintf(pointer, "~");
				pointer++;
			}
			if(op->properties & prop_absolute_value) {
				sprintf(pointer, "|");
				pointer++;
			}
			
			if(op->reg < 0) {
				sprintf(pointer, "RZ");
				pointer+=2;
			} else {
				sprintf(pointer, "R%d",(int) op->val1);
				pointer += strlen(pointer);
			}
			
			if(op->properties & prop_absolute_value) {
				sprintf(pointer, "|");
				pointer++;
			}
			
			break;
			
		case type_special_reg:
			if(op->properties & prop_not) {
				sprintf(pointer, "!");
				pointer++;
			} else if(op->properties & prop_minus) {
				sprintf(pointer, "-");
				pointer++;
			} else if(op->properties & prop_bitwise_complement) {
				sprintf(pointer, "~");
				pointer++;
			}
			if(op->properties & prop_absolute_value) {
				sprintf(pointer, "|");
				pointer++;
			}
			
			sprintf(pointer, "SR");
			pointer += 2;
			if(_arch >= 35 && _cuobjdump_version < 55) {
				sprintf(pointer, "%d", (int) op->val1);
				pointer += strlen(pointer);
			} else if(_arch < 35) {
				sprintf(pointer, "_");
				pointer++;
				for(int x = 0; x < _numSpecials20; x++) {
					if(_specialIDs20[x] == (int) op->val1) {
						const char * name = _specialNames20[x].c_str();
						sprintf(pointer, "%s", name);
						pointer += strlen(pointer);
						break;
					}
				}
			} else if(_arch <= 50) {
				if(_specialNames35[op->val1].length() > 0) {
					sprintf(pointer, "_");
					pointer++;
					sprintf(pointer, "%s", _specialNames35[op->val1].c_str());
					pointer += strlen(pointer);
				} else {
					sprintf(pointer, "%d", (int) op->val1);
					pointer += strlen(pointer);
				}
			} else {
				cerr << "WARNING o~120: unimplemented arch for special operands.\n";
				sprintf(pointer, "%d", (int) op->val1);
				pointer += strlen(pointer);
			}
			
			if(op->properties & prop_absolute_value) {
				sprintf(pointer, "|");
				pointer++;
			}
			
			break;
			
		case type_predicate:
			if(op->properties & prop_not) {
				sprintf(pointer, "!");
				pointer++;
			}
			
			if(op->val1 == 7) {
				if(_cuobjdump_version <= 50 && _arch < 35) {
					sprintf(pointer, "pt");
					pointer += 2;
				} else {
					sprintf(pointer, "PT");
					pointer += 2;
				}
			} else {
				sprintf(pointer, "P%d", (int) op->val1);
				pointer += strlen(pointer);
			}
			
			break;
		
		case type_label:
			sprintf(pointer, "label");
			pointer += 5;
			sprintf(pointer, "%s", op->strval);
			pointer += strlen(pointer);
			break;
		
		case type_sb:
			sprintf(pointer, "SB");
			pointer += 2;
			sprintf(pointer, "%d", (int) op->val1);
			pointer += strlen(pointer);
			break;
		
		case type_hex:
			if(op->properties & prop_not) {
				sprintf(pointer, "!");
				pointer++;
			} else if(op->properties & prop_minus && !(op->properties & prop_float) && !(op->properties & prop_double)) {
				sprintf(pointer, "-");
				pointer++;
			} else if(op->properties & prop_bitwise_complement) {
				sprintf(pointer, "~");
				pointer++;
			}
			if(op->properties & prop_absolute_value) {
				sprintf(pointer, "|");
				pointer++;
			}
			
			if((op->properties & prop_float) || (op->properties & prop_float32i)) {
				if(op->val1 == 0x7f80000000000000LL >> 44) {
					sprintf(pointer, "+INF ");
					pointer += 5;
				} else if(op->val1 == 0xff80000000000000LL >> 44) {
					sprintf(pointer, "-INF ");
					pointer += 5;
				} else {
					union {unsigned int i; float f;} u;
					u.i = (int)(op->val1 & 0xffffffff);
					if(!(op->properties & prop_float32i)) {
						u.i = u.i << 12;
					}
					if(op->properties & prop_exp) {
						sprintf(pointer, "%.*e", op->precision, (double)u.f);
						pointer += strlen(pointer);
					} else {
						sprintf(pointer, "%.*f", op->precision, u.f);
						pointer += strlen(pointer);
					}
				}
			}
			else if(op->properties & prop_double) {
				if(op->val1 == 0x7ff0000000000000LL >> 44) {
					sprintf(pointer, "+INF ");
					pointer += 5;
				} else if(op->val1 == 0xfff0000000000000LL >> 44) {
					sprintf(pointer, "-INF ");
					pointer += 5;
				} else {
					union {unsigned long long int i; double d;} u;
					u.i = op->val1 << 32;
					if(!(op->properties & prop_float32i)) {
						u.i = u.i << 12;
					}
					if(op->properties & prop_exp) {
						sprintf(pointer, "%.*e", op->precision, u.d);
						pointer += strlen(pointer);
					} else {
						sprintf(pointer, "%.*f", op->precision, u.d);
						pointer += strlen(pointer);
					}
				}
			} else {
				sprintf(pointer, "0x%" LLx, op->val1);
				pointer += strlen(pointer);
			}
			
			if(op->properties & prop_absolute_value) {
				sprintf(pointer, "|");
				pointer++;
			}
			
			break;
			
		case type_bit_list:
			sprintf(pointer, "{");
			pointer += 1;
			
			counter = 0;
			for(unsigned int x = 0; x < 10; x++) {
				if(op->val1 & (0x1 << x)) {
					counter++;
				}
			}
			
			for(unsigned int x = 9; x >= 0; x--) {
				if(op->val1 & (0x1 << x)) {
					counter--;
					sprintf(pointer, "%d", x);
					pointer += strlen(pointer);
					if(counter) {
						sprintf(pointer, ",");
						pointer++;
					} else {
						break;
					}
				}
			}
			
			sprintf(pointer, "}");
			pointer += 1;
			
			break;
			
		case type_const_mem:
			if(op->properties & prop_not) {
				sprintf(pointer, "!");
				pointer++;
			} else if(op->properties & prop_minus) {
				sprintf(pointer, "-");
				pointer++;
			} else if(op->properties & prop_bitwise_complement) {
				sprintf(pointer, "~");
				pointer++;
			}
			if(op->properties & prop_absolute_value) {
				sprintf(pointer, "|");
				pointer++;
			}
			
			sprintf(pointer, "c");
			pointer++;
			
			if(_cuobjdump_version <= 50) {
				if(op->properties & prop_absolute_value) {
					sprintf(pointer, "|");
					pointer++;
				}
			}
			
			if(_cuobjdump_version <= 50) {
				sprintf(pointer, " [0x%x] [", op->val2);
				pointer += strlen(pointer);
			}
			else {
				if((op->converted && inst->op != opcode_F2F) || (inst->op == opcode_XMAD && op != inst->operands[inst->num_operands - 1])) {
					sprintf(pointer, "[0x%x] [", op->val2);
					pointer += strlen(pointer);
				} else {
					sprintf(pointer, "[0x%x][", op->val2);
					pointer += strlen(pointer);
				}
			}
			
			if(op->reg >= 0) {
				sprintf(pointer, "R%d", (int) op->val1);
				pointer += strlen(pointer);
				if(op->val3) {
					sprintf(pointer, "+");
					pointer++;
				}
			}
			if(op->val3 || op->reg < 0) {
				if(op->val3 < 0) {
					sprintf(pointer, "-0x%" LLx, (~((unsigned long long) op->val3)) + 1);
					pointer += strlen(pointer);
				} else {
					sprintf(pointer, "0x%" LLx, (unsigned long long) op->val3);
					pointer += strlen(pointer);
				}
			}
			sprintf(pointer, "]");
			pointer++;
			
			if(_cuobjdump_version >= 55) {
				if(op->properties & prop_absolute_value) {
					sprintf(pointer, "|");
					pointer++;
				}
			}
			
			break;
			
		case type_mem:
			if(inst->op == opcode_LD_LDU) {
				mustShowHex = true;
			}
			
			if(op->properties & prop_not) {
				sprintf(pointer, "!");
				pointer++;
			} else if(op->properties & prop_minus) {
				sprintf(pointer, "-");
				pointer++;
			} else if(op->properties & prop_bitwise_complement) {
				sprintf(pointer, "~");
				pointer++;
			} else if(op->properties & prop_absolute_value) {
				sprintf(pointer, "|");
				pointer++;
			}
			
			sprintf(pointer, "[");
			pointer++;
			if(op->reg >= 0) {
				sprintf(pointer, "R%d", (int) op->val1);
				pointer += strlen(pointer);
				if(op->val2 || mustShowHex) {
					sprintf(pointer, "+");
					pointer++;
				}
			}
			if(mustShowHex || op->val2 || op->reg < 0) {
				if(op->val2 == 0) {
					if(_cuobjdump_version <= 50) {
						sprintf(pointer, "0x0");
						pointer += 3;
					} else {
						sprintf(pointer, "RZ");
						pointer += 2;
					}
				} else if(op->val2 < 0) {
					sprintf(pointer, "-0x%" LLx, (~((unsigned long long) op->val2)) + 1);
					pointer += strlen(pointer);
				} else {
					sprintf(pointer, "0x%" LLx, (unsigned long long) op->val2);
					pointer += strlen(pointer);
				}
			}
			sprintf(pointer, "]");
			pointer++;
			
			if(op->properties & prop_absolute_value) {
				sprintf(pointer, "|");
				pointer++;
			}
			
			break;
			
		case type_texture_operand:
			for(int x = 0; x < _numTextures; x++) {
				if(_textureIDs[x] == (int) op->val1) {
					if(_cuobjdump_version >= 55 && _textureIDs[x+1] == _textureIDs[x]) {
						sprintf(pointer, "%s", _textureNames[x+1]);
						pointer += strlen(pointer);
					} else {
						sprintf(pointer, "%s", _textureNames[x]);
						pointer += strlen(pointer);
					}
					break;
				}
			}
			
			break;
			
		case type_channel:
			for(int x = 0; x < _numChannels; x++) {
				if(_channelIDs[x] == (int) op->val1) {
					sprintf(pointer, "%s", _channelNames[x]);
					pointer += strlen(pointer);
					break;
				}
			}
			
			break;
			
		case type_other_operand:
			sprintf(pointer, "%s", _otherNames[op->val1]);
			pointer += strlen(pointer);
			
			break;
			
		default:
			cerr << "SANITY CHECK ERROR o~1871: operand is of non-operand type.\n";
	}
	return answer;
}

void printfOperand(instruction * inst, operand * op) {
	char * operand = operandToString(inst, op);
	printf(operand);
	free(operand);
}

void fprintfInstruction(ostream & out, instruction * inst) {
	//printf predicate guard if it exists
	if(inst->guard) {
		out << "@";
		char * operand = operandToString(inst, inst->guard);
		out << operand << " ";
		if(!(inst->guard->properties & prop_not)) {
			out << " ";
		}
		free(operand);
	} else {
		out << "     ";
	}
	
	//printf opcode + mods
	out << opcodes[inst->op];
	node * iter3 = inst->mods;
	while(iter3 != 0) {
		out << "." << (char*)iter3->value;
		iter3 = iter3->next;
	}
	
	//printf operands + mods
	int x;
	for(x = 0; x < inst->num_operands; x++) {
		if(x > 0) {
			out << ",";
		}
		out << " ";
		char * operand = operandToString(inst, inst->operands[x]);
		out << operand;
		free(operand);
		
		iter3 = inst->operands[x]->mods;
		while(iter3 != 0) {
			out << "." << (char*)iter3->value;
			iter3 = iter3->next;
		}
	}
	
	out << ";";
}
