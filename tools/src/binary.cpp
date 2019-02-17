#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binary.hpp"
#include "binary20.hpp"
#include "binary35.hpp"
#include "binary50.hpp"
#include "common.hpp"
using namespace std;

long long getBinaryDifference(unsigned long long val1, int val2, char sizeLimit) {
	long long answer = 0;
	if(val1 > (unsigned long long)val2) {
		answer = val1 - val2;
	}
	else {
		answer = val2 - val1;
		unsigned long long temp = 0xffffffffffffffffLL >> (64 - sizeLimit);
		temp ^= answer;
		temp++;
		temp &= 0xffffffffffffffffLL >> (64 - sizeLimit);
		answer = temp;
	}
	return answer;
}

void setBinary(unsigned long long * binary, int location, long long value, int size, char overwrite) {
	int x;
	if(overwrite) {
		for(x = 0; x < size; x++) {
			if(value & (0x1LL << x)) {
				*binary |= 0x1LL << location;
			}
			else if(*binary & (0x1LL << location)) {
				*binary ^= 0x1LL << location;
			}
			location--;
		}
	}
	else {
		for(x = 0; x < size; x++) {
			if(value & (0x1LL << x)) {
				*binary |= 0x1LL << location;
			}
			location--;
		}
	}
}

bool printedStatusMessage = false;
char* instructionToHexString(instruction * inst, int arch, int cuobjdump_version) {
	if(arch < 20) {
		cerr << "FATAL ERROR: The sm_1x architecture is not supported.\n";
		exit(0);
	} else if(arch == 20 || arch == 21 || arch == 30) {
		return instructionToHexString20(inst, arch, cuobjdump_version);
	} else if(arch == 32 || arch == 35 || arch == 37) {
		return instructionToHexString35(inst, cuobjdump_version);
	} else if(arch == 50 || arch == 52 || arch == 53 || arch == 60 || arch == 61 || arch == 62) {
		return instructionToHexString50(inst, cuobjdump_version);
	} else {
		cerr << "FATAL ERROR: No assembler found for architecture " << arch << ".\n";
		exit(0);
	}
}

