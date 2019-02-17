#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfghelpers.hpp"
#include "cudacommon.hpp"
using namespace std;

extern node* _blocks;
extern node* _functions;
extern int _tag;
extern bool _verbose;

bool hasMod(instruction* inst, const char* mod) {
	node * n = inst->mods;
	while(n) {
		if(!strcmp((char*)n->value, mod)) {
			return true;
		}
		n = n->next;
	}
	
	return false;
}

bool hasMod(operand* op, const char* mod) {
	node * n = op->mods;
	while(n) {
		if(!strcmp((char*)n->value, mod)) {
			return true;
		}
		n = n->next;
	}
	
	return false;
}

bool hasTypeMod(instruction* inst, const char* mod, char later) {
	node * n = inst->mods;
	bool seenType = 0;
	while(n) {
		if(!strcmp((char*)n->value, mod)) {
			if(seenType ^ !later) {
				return true;
			}
		}
		
		if(!strcmp((char*)n->value, "U16")) {
			seenType = true;
		} else if(!strcmp((char*)n->value, "U32")) {
			seenType = true;
		} else if(!strcmp((char*)n->value, "U64")) {
			seenType = true;
		} else if(!strcmp((char*)n->value, "S16")) {
			seenType = true;
		} else if(!strcmp((char*)n->value, "S32")) {
			seenType = true;
		} else if(!strcmp((char*)n->value, "S64")) {
			seenType = true;
		} else if(!strcmp((char*)n->value, "F16")) {
			seenType = true;
		} else if(!strcmp((char*)n->value, "F32")) {
			seenType = true;
		} else if(!strcmp((char*)n->value, "F64")) {
			seenType = true;
		} else if(!strcmp((char*)n->value, "32")) {
			seenType = true;
		} else if(!strcmp((char*)n->value, "64")) {
			seenType = true;
		} else if(!strcmp((char*)n->value, "128")) {
			seenType = true;
		}
		
		n = n->next;
	}
	
	return false;
}

blockNode * getBlock(int line) {
	blockNode * b;
	node * n = _blocks;
	
	while(n) {
		b = (blockNode*) n->value;
		if(b->first <= line && b->last >= line) {
			return b;
		}
		n = n->next;
	}
	
	return 0;
}

blockNode * getBlock(instruction * inst) {
	blockNode * b;
	node * n = _blocks;
	
	while(n) {
		b = (blockNode*) n->value;
		if(inst->blockID == b->id) {
			return b;
		}
		n = n->next;
	}
	
	return 0;
}

instruction * getLine(int line) {
	blockNode * b = getBlock(line);
	node * n = b->instructions;
	int counter = b->first;
	while(counter < line) {
		n = n->next;
		counter++;
	}
	
	return (instruction*) n->value;
}
