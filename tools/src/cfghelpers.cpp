#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <assert.h>
#include <algorithm>
#include "cfghelpers.hpp"
#include "common.hpp"
#include "cudacommon.hpp"
#include "output.hpp"
using namespace std;

extern node * _blocks;
extern char* _kernel_name;
extern int _arch;
extern int _shared_memory;
extern char** _function_names;
extern int _num_functions;
extern node * _functions;
extern int _tag;
extern int _num_registers;
extern int _counter;
extern bool hasMod(instruction* inst, char* mod);

bool addBranchToLine(blockNode * a, instruction * line) {
	blockNode * b;
	node * j = _blocks;
	bool added = false;
	while(j) {
		b = (blockNode*) j->value;
		if(line == b->instructions->value) {
			if(a) {
				added = added || addNode(&a->branches, (void*)b, &blockorder);
				addNode(&b->predecessors, (void*)a, &blockorder);
			} else {
				return false;
			}
			break;
		} else {
			node * insts = b->instructions;
			while(insts->next) {
				if(insts->next->value == line) {
					break;
				}
				insts = insts->next;
			}
			
			if(insts->next) {
				added = true;
				
				//split block
				blockNode * newBlock;
				initBlock(&newBlock);
				newBlock->first = line->line;
				newBlock->forward = b->forward;
				newBlock->branches = b->branches;
				newBlock->last = b->last;
				newBlock->guarded = b->guarded;
				newBlock->target = b->target;
				newBlock->end = b->end;
				b->end = end_generic;
				b->last = line->line - 1;
				b->branches = 0;
				b->forward = 0;
				
				//split instruction lists
				newBlock->instructions = insts->next;
				insts->next = 0;
				
				//Fix addresses:
				newBlock->address = ((instruction*)newBlock->instructions->value)->address;
				int orgSize = b->size;
				b->size = newBlock->address - b->address;
				newBlock->size = orgSize - b->size;
				
				//Add appropriate successors:
				addNode(&b->forward, (void*)newBlock, &blockorder);
				addNode(&newBlock->predecessors, (void*)b, &blockorder);
				if(a) {
					addNode(&a->branches, (void*)newBlock, &blockorder);
					addNode(&newBlock->predecessors, (void*)a, &blockorder);
				}
				
				//Change predecessors for other blocks appropriately
				node * blocks = _blocks;
				while(blocks) {
					blockNode * block = (blockNode*) blocks->value;
					if(containsValue(block->predecessors, (int*)b)) {
						if(!newBlock->guarded) {
							deleteNode(&block->predecessors, b, &blockorder);
						}
						addNode(&block->predecessors, newBlock, &blockorder);
					}
					blocks = blocks->next;
				}
				
				//Add newBlock to blocks list:
				addNode(&_blocks, (void*)newBlock, &blockorder);
				break;
			}
		}
		j = j->next;
	}
	
	return added;
}

bool addBranchToLine(blockNode * a, int line) {
	blockNode * b;
	node * k;
	node * j = _blocks;
	bool added = false;
	while(j) {
		b = (blockNode*) j->value;
		if(line == b->first && a) {
			added = added || addNode(&a->branches, (void*)b, &blockorder);
			
			addNode(&b->predecessors, (void*)a, &blockorder);
			break;
		} else if(line > b->first && line <= b->last) {
			added = true;
			
			//split block
			blockNode * newBlock;
			initBlock(&newBlock);
			newBlock->first = line;
			newBlock->forward = b->forward;
			newBlock->branches = b->branches;
			newBlock->last = b->last;
			newBlock->guarded = b->guarded;
			newBlock->target = b->target;
			newBlock->end = b->end;
			b->end = end_generic;
			b->last = line - 1;
			b->branches = 0;
			b->forward = 0;
			
			//split instruction lists
			k = b->instructions;
			int kline = b->first + 1;
			while(kline < line) {
				k = k->next;
				kline++;
			}
			newBlock->instructions = k->next;
			k->next = 0;
			
			//Fix addresses:
			newBlock->address = ((instruction*)newBlock->instructions->value)->address;
			int orgSize = b->size;
			b->size = newBlock->address - b->address;
			newBlock->size = orgSize - b->size;
			
			//Add appropriate successors:
			addNode(&b->forward, (void*)newBlock, &blockorder);
			addNode(&newBlock->predecessors, (void*)b, &blockorder);
			if(a) {
				addNode(&a->branches, (void*)newBlock, &blockorder);
				addNode(&newBlock->predecessors, (void*)a, &blockorder);
			}
			
			//Change predecessors for other blocks appropriately
			node * blocks = _blocks;
			while(blocks) {
				blockNode * block = (blockNode*) blocks->value;
				if(containsValue(block->predecessors, (int*)b)) {
					if(!newBlock->guarded) {
						deleteNode(&block->predecessors, b, &blockorder);
					}
					addNode(&block->predecessors, newBlock, &blockorder);
				}
				blocks = blocks->next;
			}
			
			//Add newBlock to blocks list:
			addNode(&_blocks, (void*)newBlock, &blockorder);
			break;
		}
		j = j->next;
	}
	
	return added;
}

bool addBranchToAddress(blockNode * a, int address) {
	blockNode * b;
	node * j = _blocks;
	while(j) {
		b = (blockNode*) j->value;
		if(address >= b->address && address < b->address + b->size) {
			node * insts = b->instructions;
			while(insts) {
				instruction * inst = (instruction*) insts->value;
				if(inst->address == address) {
					int lineNumber = inst->line;
					return addBranchToLine(a, lineNumber);
				}
		
				insts = insts->next;
			}
		}
		
		j = j->next;
	}
	
	cerr << "ERROR ch~132: can't find branch target at address 0x" << std::hex << address << ".\n";
	cerr << "\tThis error may mean that a branch jumps to a nonexistent instruction.\n";
	cerr << "\tSince we use pointers in our IR, this will cause problems.\n";
	return false;
}

/**
 * Gets the address corresponding to a given label.
 * //TODO: this would be better with some preprocessing
 * @param label The label to look for
 * @return The address of an instruction with matching label, or -1.
 */
int labelToAddress(char * label) {
	node * blocks = _blocks;
	while(blocks) {
		blockNode * block = (blockNode*) blocks->value;
		node * instructions = block->instructions;
		while(instructions) {
			instruction * inst = (instruction*) instructions->value;
			if(inst->label) {
				if(!strcmp(inst->label, label)) {
					return inst->address;
				}
			}
			instructions = instructions->next;
		}
		blocks = blocks->next;
	}
	return -1;
}

void labelsToAddresses() {
	node * blocks = _blocks;
	while(blocks) {
		blockNode * block = (blockNode*) blocks->value;
		node * instructions = block->instructions;
		while(instructions) {
			instruction * inst = (instruction*) instructions->value;
			for(int x = 0; x < inst->num_operands; x++) {
				if(inst->operands[x]->type == type_label) {
					inst->operands[x]->val1 = labelToAddress(inst->operands[x]->strval);
					if(inst->operands[x]->val1 >= 0) {
						inst->operands[x]->type = type_hex;
						if(!instructions->next) {
							if(block->end == end_call || block->end == end_branch) {
								block->target = inst->operands[x]->val1;
							}
						}
					} else {
						cerr << "ERROR ch~261: can't find label " << inst->operands[x]->strval << ".\n";
						cerr << "\tThis error may mean that a branch jumps to a nonexistent instruction.\n";
						inst->operands[x]->val1 = 0x0;
						inst->operands[x]->type = type_hex;
						block->target = 0x0;
					}
				}
			}
			instructions = instructions->next;
		}
		blocks = blocks->next;
	}
}

void addressesToLabels() {
	//Apply labels to blocks, overwriting existing labels:
	int counter = 0;
	node * blocks = _blocks;
	while(blocks) {
		blockNode * block = (blockNode*) blocks->value;
		node * instructions = block->instructions;
		instruction * firstInst = (instruction*) instructions->value;
		if(firstInst->label) {
			free(firstInst->label);
		}
		firstInst->label = (char*) malloc(sizeof(char) * sizeof(int) * 4);
		sprintf(firstInst->label, "%d", counter);
		blocks = blocks->next;
		counter++;
	}
	
	//Convert addresses to labels for branches, synchronizations instructions, etcetera:
	blocks = _blocks;
	while(blocks) {
		blockNode * block = (blockNode*) blocks->value;
		node * instructions = block->instructions;
		while(instructions) {
			instruction * inst = (instruction*) instructions->value;
			if(inst->ptr) {
				for(int x = 0; x < inst->num_operands; x++) {
					if(inst->operands[x]->type == type_hex && ((blockNode*)inst->ptr)->address == (long long)inst->operands[x]->val1) {
						blockNode * ptr =  ((blockNode*)inst->ptr);
						instruction * target = (instruction*) ptr->instructions->value;
						inst->operands[x]->type = type_label;
						if(inst->operands[x]->strval) {
							free(inst->operands[x]->strval);
						}
						inst->operands[x]->strval = (char*) malloc(sizeof(char) * (strlen(target->label) + 1));
						strcpy(inst->operands[x]->strval, target->label);
					}
				}
			}
			instructions = instructions->next;
		}
		blocks = blocks->next;
	}
}

//Helper for combine blocks
void combineBlocksHelper(node * blocks, blockNode * deleted, blockNode * combined) {
	while(blocks) {
		blockNode * block = (blockNode *) blocks->value;
		deleteNode(&block->forward, deleted, &intorder);
		deleteNode(&block->branches, deleted, &intorder);
		deleteNode(&block->predecessors, deleted, &intorder);
		blocks = blocks->next;
	}
}

//This function combines adjacent blocks that have different predicate guards.
//In other words, this redefines basic blocks in terms of control flow, instead of execution.
void combineBlocks() {
	node * blocks = _blocks;
	while(blocks->next) {
		bool combined = false;
		blockNode * block1 = (blockNode*) blocks->value;
		blockNode * block2 = (blockNode*) blocks->next->value;
		
		if(block1->end == end_generic && !block2->isBranchTarget) {
			combined = true;
			while(block2->instructions) {
				addLast(&block1->instructions, block2->instructions->value);
				node * temp = block2->instructions;
				block2->instructions = block2->instructions->next;
				free(temp);
			}
			
			block1->end = block2->end;
			cleanNodes(block1->forward);
			block1->forward = block2->forward;
			block2->forward = 0;
			cleanNodes(block1->branches);
			block1->branches = block2->branches;
			block2->branches = 0;
			
			combineBlocksHelper(_blocks, block2, block1);
			
			node * sucs = block1->forward;
			while(sucs) {
				blockNode * suc = (blockNode *) sucs->value;
				addNode(&suc->predecessors, block1, &blockorder);
				sucs = sucs->next;
			}
			sucs = block1->branches;
			while(sucs) {
				blockNode * suc = (blockNode *) sucs->value;
				addNode(&suc->predecessors, block1, &blockorder);
				sucs = sucs->next;
			}
			
			cleanBlock(block2);
			node * temp = blocks->next;
			blocks->next = blocks->next->next;
			free(temp);
		}
		
		if(!combined) {
			blocks = blocks->next;
		}
	}
	
	blocks = _blocks;
	while(blocks->next) {
		blockNode * block = (blockNode *) blocks->value;
		if(!block->forward) {
			addFirst(&block->forward, blocks->next->value);
		}
		if(block->forward->value != blocks->next->value) {
			cerr << "SANITY CHECK ERROR ch~294.\n";
			cerr << "\tThis error absolutely should not ever happen.\n";
			cerr << "\tThis error means the assembly code is now non-linear, even without branches.\n";
		}
		blocks = blocks->next;
	}
}

void changePtrs(blockNode* oldVal, blockNode* newVal) {
	blockNode * a;
	instruction * inst;
	node * i = _blocks;
	while(i) {
		a = (blockNode*) i->value;
		node * j = a->instructions;
		while(j) {
			inst = (instruction*) j->value;
			
			if(inst->op == opcode_SSY ||
				inst->op == opcode_BRA ||
				inst->op == opcode_CAL ||
				inst->op == opcode_PBK ||
				inst->op == opcode_PCNT ||
				inst->op == opcode_BRX) {//found relevant instruction
				if(inst->ptr == oldVal) {
					inst->ptr = newVal;
				}
			}
			
			j = j->next;
		}
		
		i = i->next;
	}
}

void deleteBlock(blockNode* deleted) {
	//Fix up successors:
	node * iter = _blocks;
	while(iter) {
		blockNode* block = (blockNode*) iter->value;
		
		if(block == deleted) {
			if(iter->next) {
				//Change target block for appropriate SSYS, BRAs, CALs, etcetera
				changePtrs(deleted, (blockNode*) iter->next->value);
				
				//If deleted block is start of function, fix function list:
				node * iter2 = _functions;
				while(iter2) {
					blockNode* func = (blockNode*) iter2->value;
					if(func == deleted) {
						if(func->end == end_return) {
							cerr << "SANITY CHECK ERROR ~5432 deleted subroutine\n";
							cerr << "\tThis error suggests we (accidentally?) deleted a subroutine in the assembly.\n";
						}
						else if(func->end == end_generic || func->guarded) {
							iter2->value = iter->next->value;
						}
					}
		
					iter2 = iter2->next;
				}
			}
		}
		else {
			//If deleted block is a successor, adds its successor(s?) to this block's lists:
			if((block->end == end_call || block->end == end_branch)) {
				node * i = block->branches;
				while(i != 0) {
					if(i->value == deleted) {
						node * j = deleted->forward;
						if(listSize(deleted->forward)) {
							while(j) {
								addNode(&block->branches, j->value, &blockorder);
								addNode(&((blockNode*)j->value)->predecessors, block, &blockorder);
								j = j->next;
							}
						}
						else {
							node * temp = iter;
							blockNode * b;
							do {
								b = (blockNode*) temp->value;
								addNode(&block->branches, b, &blockorder);
								addNode(&b->predecessors, block, &blockorder);
								temp = temp->next;
							} while(b->guarded);
						}
					}
					i = i->next;
				}
			}
			if(block->end == end_generic || block->end == end_call) {
				node * i = block->forward;
				while(i != 0) {
					if(i->value == deleted) {
						node * j = deleted->forward;
						while(j) {
							addNode(&block->forward, j->value, &blockorder);
							addNode(&((blockNode*)j->value)->predecessors, block, &blockorder);
							j = j->next;
						}
					}
					i = i->next;
				}
			}
			
			//Remove deleted block from this block's successor & predecessor lists:
			deleteNode(&block->forward, deleted, &intorder);
			deleteNode(&block->branches, deleted, &intorder);
			deleteNode(&block->predecessors, deleted, &intorder);
			//deleteNode(&block->pseudoSuccessors, deleted, &intorder);
		}
		
		iter = iter->next;
	}
	
	//Remove deleted block from block list:
	deleteNode(&_blocks, deleted, &intorder);
	
	//Cleanup:
	cleanBlock(deleted);
}

void fixAssembly() {
	//Delete empty blocks:
	node * iter = _blocks;
	while(iter) {
		blockNode* block = (blockNode*) iter->value;
		if(block->instructions == 0) {
			deleteBlock(block);
		}
		else {
			break;
		}
		iter = _blocks;
	}
	iter = _blocks;
	while(iter && iter->next) {
		blockNode* block = (blockNode*) iter->next->value;
		if(block->instructions == 0) {
			deleteBlock(block);
			iter = _blocks;
			continue;
		}
		iter = iter->next;
	}
	
	//Fix line numbers and addresses:
	iter = _blocks;
	int trueLine = 0;
	int blockid = -1;
	int addr = 0;
	_num_registers = 0;
	while(iter) {
		blockNode* block = (blockNode*) iter->value;
		block->isBranchTarget = false;
		block->address = addr;
		block->first = trueLine;
		block->id = ++blockid;
		block->size = 0;
		
		if(!block->instructions) {
			cerr << "SANITY CHECK ERROR ~2951 empty block\n";
			cerr << "\tThis error means the assembly has a basic block with no instructions.\n";
		}
		
		node * iter2 = block->instructions;
		while(iter2) {
			instruction * inst = (instruction*) iter2->value;
			
			inst->address = addr;
			addr += inst->size;
			block->size += inst->size;
			
			inst->line = trueLine++;
			inst->blockID = blockid;
			iter2 = iter2->next;
			
			if(inst->size != 8) {
				cerr << "WARNING: non-8byte instruction code for: " << opcodes[inst->op] << "\n";
				cerr << "\t(Currently, this tool is not equipped to handle instructions of other sizes.)\n";
			}
			
			for(int x = 0; x < inst->num_operands; x++) {
				int reg = inst->operands[x]->reg;
				if(reg >= 0) {
					inst->operands[x]->reg = inst->operands[x]->val1;
					reg = inst->operands[x]->reg;
					int align = inst->operands[x]->width;
					if(align == 3) {
						align = 4;
					}
					int tempreg = reg - (reg % align);
					if(tempreg != reg) {
						cerr << "WARNING ch~481: a register has incorrect alignment: R" << reg << " in operand[" << x << "] of ";
						fprintfInstruction(cerr, inst);
						cerr << ".\n";
						static bool seenError = false;
						if(!seenError) {
							cerr << "\tNote that instructions with 64-bit or larger operands expect registers to be aligned to locations divisible by 2 or 4.\n";
							seenError = true;
						}
					}
					if(tempreg + inst->operands[x]->width > _num_registers) {
						_num_registers = tempreg + inst->operands[x]->width;
					}
				}
			}
		}
		
		block->last = trueLine - 1;
		iter = iter->next;
	}
	
	//Fix operands for BRA, CAL, SSY, etcetera
	iter = _blocks;
	while(iter) {
		blockNode* block = (blockNode*) iter->value;
		
		node * iter2 = block->instructions;
		while(iter2) {
			instruction* inst = (instruction*) iter2->value;
			if(inst->ptr) {
				if(inst->op != opcode_BRX) {
					blockNode* targetBlock = (blockNode*) inst->ptr;
					for(int x = 0; x < inst->num_operands; x++) {
						if(inst->operands[x]->type == type_hex) {
							inst->operands[x]->val1 = targetBlock->address;
						}
					}
					targetBlock->isBranchTarget = true;
				}
				else {
					static bool seenError = false;
					if(!seenError) {
						cerr << "ERROR ch~620: This tool is not able to handle addressing for the BRX instruction.\n";
						seenError = true;
					}
				}
			}
			
			iter2 = iter2->next;
		}
		
		_counter = block->last;
		iter = iter->next;
	}
	
	//Fix function IDs:
	int funcID = 0;
	blockNode * previousBlock = 0;
	iter = _blocks;
	while(iter) {
		blockNode* block = (blockNode*) iter->value;
		if(previousBlock) {
			if(previousBlock->end != end_generic) {
				node * preds = block->predecessors;
				while(preds) {
					blockNode * pred = (blockNode*) preds->value;
					if(pred->end == end_call) {
						if(!containsValue(pred->forward, block)) {
							funcID++;
							break;
						}
					}
					preds = preds->next;
				}
			}
		}
		
		block->funcID = funcID;
		
		node * iter2 = block->instructions;
		while(iter2) {
			instruction* inst = (instruction*) iter2->value;
			inst->funcID = funcID;
			iter2 = iter2->next;
		}
		
		previousBlock = block;
		iter = iter->next;
	}
}

void propogateBranches(blockEnd branch_type) {
	blockNode * a;
	node * i = _blocks;
	while(i) {
		a = (blockNode*) i->value;
		if(a->end == branch_type && a->target) {
			addBranchToAddress(a, a->target);
		}
		i = i->next;
	}
}

bool propogateReturns(blockNode * search, int target, int tag) {
	search->tag = tag;
	bool changed = false;
	
	if(search->guarded) {
		blockNode * next = getBlock(search->last + 1);
		if(next->tag != tag) {
			changed = changed || propogateReturns(next, target, tag);
		}
	}
	
	//Check if this block ends in a return:
	if(search->end == end_return) {
		changed = changed || addBranchToAddress(search, target);
		if(!search->guarded) {//no guard; end of path
			return changed;
		}
	}
	
	node * n;
	
	//If appropriate, search following blocks for returns:
	if(search->end == end_generic || search->end == end_call) {
		n = search->forward;
		while(n) {
			if(((blockNode*)n->value)->tag != tag) {
				changed = changed || propogateReturns((blockNode*)n->value, target, tag);
			}
			n = n->next;
		}
	}
	
	//Search branching successors for returns:
	if(search->end == end_branch) {
		n = search->branches;
		while(n) {
			if(((blockNode*)n->value)->tag != tag) {
				changed = changed || propogateReturns((blockNode*)n->value, target, tag);
			}
			n = n->next;
		}
	}
	
	return changed;
}

bool propogateCallReturns() {
	blockNode * a;
	blockNode * b;
	node * j;
	node * i = _blocks;
	bool changed = false;
	while(i) {
		a = (blockNode*) i->value;//current block being checked
		
		if(a->end == end_call) {//block ends with a call
			//Search successors for returns:
			j = a->branches;
			while(j) {
				b = (blockNode*) j->value;
				if(a->target == b->address) {//b is target of call
					node * insts = a->instructions;
					while(insts->next) insts = insts->next;
					instruction * inst = (instruction*) insts->value;
					changed = changed || propogateReturns(b, inst->address + inst->size, ++_tag);
					break;
				}
				j = j->next;
			}
			
			if(!j) {//call's target is not among the successors
				printf("\nSANITY-CHECK FAILED ch~493: cannot find called address\n");
			}
		}
		i = i->next;
	}
	
	return changed;
}

bool propogatePointersHelper(blockNode * search, stack<pair<opcode, long long> > ptrs, int tag, bool firstCall) {
	if(search->tag == tag) {
		return false;
	}
	if(!firstCall) {
		search->tag = tag;
	}
	
	bool changed = false;
	if(search->guarded) {
		blockNode * next = getBlock(search->last + 1);
		changed = changed || propogatePointersHelper(next, ptrs, tag, false);
	}
	
	int addedPtrs = 0;
	
	node * iter = search->instructions;
	while(iter) {
		instruction * inst = (instruction*) iter->value;//current instruction being checked
		if(inst->op == opcode_SSY || inst->op == opcode_PBK || inst->op == opcode_PCNT) {
			long long target = inst->operands[0]->val1;
			ptrs.push(make_pair(inst->op, target));
			addedPtrs++;
		} else if(!ptrs.empty()) {
			opcode mustMatch = opcode_NOP;
			if(hasMod(inst, "S") || inst->op == opcode_SYNC) {//mod S is used prior to arch 50
				mustMatch = opcode_SSY;
			}
			else if(inst->op == opcode_CONT) {
				mustMatch = opcode_PCNT;
			}
			else if(inst->op == opcode_BRK) {
				mustMatch = opcode_PBK;
			}
			if(mustMatch != opcode_NOP) {
				//look for desired instruction in stack:
				pair<opcode, long long> targetPair = ptrs.top();
				ptrs.pop();
				while(targetPair.first != mustMatch && !ptrs.empty()) {
					targetPair = ptrs.top();
					ptrs.pop();
				}
				//if found desired instruction, match it to this jump
				if(targetPair.first == mustMatch) {
					long long target = targetPair.second;
					if(addedPtrs) {
						addedPtrs--;
					}
					changed = changed || addBranchToAddress(search, target);
				}
			}
		}
		iter = iter->next;
	}
	
	if(!ptrs.empty()) {
		node * children = 0;
		if(search->end == end_generic || search->end == end_call) {
			children = search->forward;
		} else if(search->end == end_branch) {
			children = search->branches;
		} else if(search->end == end_return) {
			static bool subroutineWarning = false;
			if(!subroutineWarning) {
				cerr << "Warning ch~976; subroutine may modify the pointer stack\n";
				subroutineWarning = true;
			}
		}
		while(children) {
			changed = changed || propogatePointersHelper((blockNode*)children->value, ptrs, tag, false);
			children = children->next;
		}
	}
	
	//Garbage removal:
	while(addedPtrs) {
		addedPtrs--;
		ptrs.pop();
	}
	
	return changed;
}

bool propogatePointers() {
	node * blocks = _blocks;
	bool changed = false;
	stack<pair<opcode, long long> > ptrs;
	while(blocks) {
		blockNode * block = (blockNode*) blocks->value;//current block being checked
		changed = changed || propogatePointersHelper(block, ptrs, ++_tag, true);
		blocks = blocks->next;
	}
	
	return changed;
}

void setPtr() {
	_functions = 0;
	blockNode * a;
	instruction * inst;
	node * i = _blocks;
	while(i) {
		a = (blockNode*) i->value;
		node * j = a->instructions;
		while(j) {
			inst = (instruction*) j->value;
			
			if(inst->op == opcode_SSY ||
				inst->op == opcode_BRA ||
				inst->op == opcode_CAL ||
				inst->op == opcode_PBK ||
				inst->op == opcode_PCNT) {//found relevant instruction
				int target = -1;
				for(int x = 0; x < inst->num_operands; x++) {
					if(inst->operands[x]->type == type_hex) {
						target = inst->operands[x]->val1;
						break;
					}
				}
				
				//Search for target at start of each block
				node * k = _blocks;
				while(k) {
					a = (blockNode*) k->value;
					if(a->address == target) {
						inst->ptr = a;
						if(inst->op == opcode_CAL) {
							addNode(&_functions, a, &blockorder);
						}
						break;
					}
					
					k = k->next;
				}
				
				if(!k) {//target is not at the start of a block
					addBranchToAddress(NULL, target);
					
					//Search for target
					k = _blocks;
					while(k) {
						a = (blockNode*) k->value;
						if(a->address == target) {
							inst->ptr = a;
							if(inst->op == opcode_CAL) {
								addNode(&_functions, a, &blockorder);
							}
							break;
						}
						
						k = k->next;
					}
					
					//Sanity check:
					if(!k) {
						fprintf(stderr, "SANITY CHECK ERROR ch~1203; pointer to non-existant address 0x%x\n", target);
					}
				}
			}
			else if(inst->op == opcode_BRX) {
				//TODO handle BRX
				inst->ptr = 0;
			}
			else {
				inst->ptr = 0;
			}
			
			j = j->next;
		}
		
		i = i->next;
	}
	
	if(_num_functions && listSize(_functions) < _num_functions) {
		cerr << "SANITY CHECK FAILED ~1783; found " << listSize(_functions) << "/" << _num_functions << " functions.\n";
		cerr << "\tMismatch between actual and expected subroutine count.\n";
	}
	else if(_num_functions && listSize(_functions) > _num_functions) {
		cerr << "WARNING: found " << listSize(_functions) << "/" << _num_functions << " functions.\n";
		cerr << "\tMismatch between actual and expected subroutine count.\n";
	}
}
