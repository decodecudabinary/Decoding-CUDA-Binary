#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.hpp"
#include "elfmanip.hpp"
using namespace std;

int main(int argc, char ** argv) {
	int argElf = 0;
	int argKernel = 0;
	char * arch = 0;
	string sys;
	FILE* sass;
	char line[512+1];
	
	bool flag_includeHex = false;
	bool flag_fallbackelf = false;
	
	//Compatbility check:
	#ifndef HAS_CUOBJDUMP
		cerr << "FATAL ERROR e2a~24: elf2asm requires cuobjdump in PATH.\n";
		cerr << " Please install the CUDA SDK/Toolkit, fix your PATH variable, and then recompile.\n";
		exit(0);
	#endif
	
	//Read arguments:
	for(int x = 1; x < argc; x++) {
		if(!strcmp(argv[x],"-h")) {
			argElf = 0;
			argKernel = 0;
			break;
		} else if(!strcasecmp(argv[x], "-sm") && x + 1 != argc) {
			arch = argv[++x];
		} else if(!strcasecmp(argv[x], "-hex")) {
			flag_includeHex = true;
		} else if(!strcasecmp(argv[x], "-fallbackelf")) {
			flag_fallbackelf = true;
		} else if(!argElf) {
			argElf = x;
		} else if(!argKernel) {
			argKernel = x;
		} else {
			argElf = 0;
			argKernel = 0;
			break;
		}
	}
	bool flag_list = (argElf && !argKernel);
	
	if(!argElf || (!argKernel && !flag_list)) {
		cout << "Usage:\n";
		cout << argv[0] << " [-sm <architecture>] <executable>\n";
		cout << "\tLists the kernel functions in the specified file.\n";
		cout << argv[0] << " <executable> <function index>\n";
		cout << "\tTransform the specified kernel function to assembly.\n";
		cout << argv[0] << " -hex <executable> <function index>\n";
		cout << "\tTransform the specified kernel function to assembly & binary.\n";
		cout << argv[0] << " -h\n";
		cout << "\tDisplays this usage information.\n";
		return 0;
	}
	
	if(flag_list) {
		node * names = getKernelNames(argv[argElf], flag_fallbackelf);
		if(!names) {
			cerr << "FATAL ERROR e2a ~51; unable to find kernel functions in specified file.\n";
		}
		node * iter = names;
		int index = 0;
		while(iter) {
			if(!arch) {
				cout << (index+1) << ": " << (char*) iter->value << "\n";
			}
			else if(!strncmp(((char*)iter->value)+3, arch, strlen(arch))) {
				cout << (index+1) << ": " << (((char*)iter->value)+6) << "\n";
			}
			index++;
			iter = iter->next;
		}
		cleanNodesFully(names, &free);
		return 0;
	}
	
	//Get kernel data from ELF
	kernelData * kern = getKernelFunction(argv[argElf], 0, atoi(argv[argKernel]) - 1, flag_fallbackelf);
	if(!kern) {
		cerr << "Fatal error e2a~46; could not retrieve specified kernel function.\n";
		return 1;
	}
	
	//Prepare target strings used to find start of assembly code
	string strTarget = "Function : ";
	strTarget += kern->name;
	const char * target = strTarget.c_str();
	
	ostringstream oss;
	oss << "sm_" << kern->arch;
	string strTargetArch = oss.str();
	const char * targetArch = strTargetArch.c_str();
	
	//Get cuobjdump version:
	char * vers;
	#ifdef WINDOWS
		sys = "cuobjdump --version";
		sass = popen(sys.c_str(), "r");
		while(fgets(line, 512, sass)) {
			vers = strstr(line, "release ");
			if(vers) {
				vers = vers + strlen("release ");
				break;
			}
		}
	#else
		sys = "cuobjdump --version | grep -E \"release [0-9]\\.[0-9]\" | sed -r \"s/.*release ([0-9]\\.[0-9]).*/\\1/\"";
		sass = popen(sys.c_str(), "r");
		fgets(line, 512, sass);
		vers = line;
	#endif
	if(!vers || vers[0] < '0' || vers[0] > '9') {
		cerr << "FATAL ERROR e2a~151: Unable to determine cuobjdump version.\n";
		exit(0);
	}
	vers[1] = vers[2];
	vers[2] = 0;
	pclose(sass);
	
	//Prepare cuobjdump output for reading:
	sys = "cuobjdump -sass -fun ";
	sys += kern->name;
	sys += " ";
	sys += argv[argElf];
	sass = popen(sys.c_str(), "r");
	
	cout << "//Name: " << (kern->name) << "\n";
	cout << "//Arch: sm_" << kern->arch << "\n";
	cout << "//cuobjdump: " << vers << "\n";
	cout << "//Shared memory usage: " << kern->sharedMemory << "\n";
	cout << "//Min Stack Size: " << kern->min_stack_size << "\n";
	cout << "//Max Stack Size: " << kern->max_stack_size << "\n";
	cout << "//Frame Size: " << kern->frame_size << "\n";
	cout << "//Function count: " << listSize(kern->functionNames) << "\n";
	node * iter = kern->functionNames;
	while(iter) {
		cout << "//Function: " << (char*) iter->value << "\n";
		iter = iter->next;
	}
	
	int seenArch = 9999999;
	
	//Find function code inside cuobjdump output:
	while(fgets(line, 512, sass)) {
		if(strstr(line, targetArch)) {
			seenArch = 0;
		} else {
			seenArch++;
		}
		
		if(strstr(line, target) && seenArch < 10) {
			int nextaddr = 0;//address of next instruction
			
			//Write function's assembly to stdout:
			while(fgets(line, 512, sass)) {
				if(!nextaddr) {
					char * address = strstr(line,"/*");
					if(!address) {
						continue;
					}
				}
				char* address = strstr(line,"/*");
				if(!address) {//done with function
					break;//note: skips any SCHIs at end, but there shouldn't ever be any
				} else if(!strstr(address+1,"/*")) {
					if(kern->arch >= 50 && nextaddr % (8 * 4)) {
						//we use 'continue' here to stop CUDA 7.x's reordering of SCHI from tricking us about addresses
						continue;
					}
					
					unsigned long long hexval;
					if(kern->bytes) {
						hexval = *((unsigned int*)&kern->bytes[nextaddr+4]);
						hexval = hexval << 32;
						hexval += *((unsigned int*)&kern->bytes[nextaddr]);
						if(flag_includeHex) {
							cout << std::hex << setfill('0') << setw(16) << hexval << std::dec << ": ";
						}
					} else {
						cerr << "SANITY CHECK ERROR e2a~187\n";
						cerr << "\tThere is no assembly associated with the kernel function.\n";
						exit(0);
					}
					
					if(kern->arch == 30) {
						hexval = hexval >> 4;
					} else if(kern->arch == 32 || kern->arch == 35 || kern->arch == 37) {
						hexval = hexval >> 2;
					} else if(kern->arch == 50 || kern->arch == 52 || kern->arch == 53) {
						//no opcode; SCHI identified by address, and (almost?) all bits are used
					} else if(kern->arch == 60 || kern->arch == 61 || kern->arch == 62) {
						//no opcode; SCHI identified by address, and (almost?) all bits are used
					} else {
						cerr << "ERROR: Don't know how to handle scheduling instructions in this architecture.\n";
					}
					cout << "SCHI";
					if(kern->arch < 50) {
						for(int x = 0; x < 7; x++) {
							if(x) cout << ",";
							cout << " 0x";
							cout << std::hex << (hexval & 0xff);
							hexval = hexval >> 8;
						}
					} else if(kern->arch == 50 || kern->arch == 52 || kern->arch == 53) {
						for(int x = 0; x < 3; x++) {
							if(x) cout << ",";
							cout << " 0x";
							cout << std::hex << (hexval & 0x1fffff);
							hexval = hexval >> 21;
						}
					} else if(kern->arch == 60 || kern->arch == 61 || kern->arch == 62) {
						for(int x = 0; x < 3; x++) {
							if(x) cout << ",";
							cout << " 0x";
							cout << std::hex << (hexval & 0x1fffff);
							hexval = hexval >> 21;
						}
					} else {
						//unrecognized arch, but already printed an error
						for(int x = 0; x < 3; x++) {
							if(x) cout << ",";
							cout << " 0x";
							cout << std::hex << (hexval & 0x1fffff);
							hexval = hexval >> 21;
						}
					}
					cout << std::dec << ";\n";
					nextaddr += 8;
					continue;
				} else if(kern->bytes && nextaddr >= kern->numBytes) {
					cerr << "FATAL ERROR e2a~77: out-of-bounds assembly code (larger than expected size)\n";
					return 1;
				}
				
				address += 2;
				int currentaddr = strtoul(address, 0, 16);
				
				//Deal with SCHI being skipped in cuobjdump (cuobjdump version 5.0) or reordered (cuobjdump version 7.x):
				while(currentaddr > nextaddr) {
					unsigned long long hexval;
					if(kern->bytes) {
						hexval = *((unsigned int*)&kern->bytes[nextaddr+4]);
						hexval = hexval << 32;
						hexval += *((unsigned int*)&kern->bytes[nextaddr]);
						if(flag_includeHex) {
							cout << std::hex << setfill('0') << setw(16) << hexval << std::dec << ": ";
						}
					} else {
						cerr << "SANITY CHECK ERROR e2a~248\n";
						cerr << "\tNo assembly code is associated with this kernel function.\n";
						exit(0);
					}
					
					if(kern->arch == 30) {
						hexval = hexval >> 4;
					} else if(kern->arch == 32 || kern->arch == 35 || kern->arch == 37) {
						hexval = hexval >> 2;
					} else if(kern->arch == 50 || kern->arch == 52 || kern->arch == 53) {
						//no opcode; SCHI identified by address, and (almost?) all bits are used
					} else if(kern->arch == 60 || kern->arch == 61 || kern->arch == 62) {
						//no opcode; SCHI identified by address, and (almost?) all bits are used
					} else {
						cerr << "ERROR: Don't know how to handle scheduling meta-instructions on this architecture.\n";
					}
					cout << "SCHI";
					if(kern->arch < 50) {
						for(int x = 0; x < 7; x++) {
							if(x) cout << ",";
							cout << " 0x";
							cout << std::hex << (hexval & 0xff);
							hexval = hexval >> 8;
						}
					} else if(kern->arch == 50 || kern->arch == 52 || kern->arch == 53) {
						for(int x = 0; x < 3; x++) {
							if(x) cout << ",";
							cout << " 0x";
							cout << std::hex << (hexval & 0x1fffff);
							hexval = hexval >> 21;
						}
					} else if(kern->arch == 60 || kern->arch == 61 || kern->arch == 62) {
						for(int x = 0; x < 3; x++) {
							if(x) cout << ",";
							cout << " 0x";
							cout << std::hex << (hexval & 0x1fffff);
							hexval = hexval >> 21;
						}
					} else {
						//unrecognized arch, but already printed an error
						for(int x = 0; x < 3; x++) {
							if(x) cout << ",";
							cout << " 0x";
							cout << std::hex << (hexval & 0x1fffff);
							hexval = hexval >> 21;
						}
					}
					cout << std::dec << ";\n";
					
					nextaddr += 8;
				}
				if(currentaddr < nextaddr) {
					continue;
				}
				
				if(flag_includeHex) {
					unsigned long long hexval;
					if(kern->bytes) {
						hexval = *((unsigned int*)&kern->bytes[nextaddr+4]);
						hexval = hexval << 32;
						hexval += *((unsigned int*)&kern->bytes[nextaddr]);
						if(flag_includeHex) {
							cout << std::hex << setfill('0') << setw(16) << hexval << std::dec << ": ";
						}
					} else {
						cerr << "SANITY CHECK ERROR e2a~295: no assembly code associated with kernel function\n";
						exit(0);
					}
				}
				
				//Write assembly code for instruction:
				int x = address - line;
				while(line[x] != '*' || line[x+1] != '/') {
					x++;
				}
				x += 2;
				while(line[x] == ' ' || line[x] == '\t' || line[x] == '{' || line[x] == '}') {
					x++;
				}
				if(line[x] == '/') {
					while(line[x] != '*' || line[x+1] != '/') {
						x++;
					}
					x += 2;
					while(line[x] == ' ' || line[x] == '\t' || line[x] == '{' || line[x] == '}') {
						x++;
					}
				}
				while(line[x] != ';') {
					cout << line[x++];
				}
				cout << ";\n";
				
				//Increment instruction address:
				int increment = 0;
				char * bytes = strstr(address, "/*");
				if(!bytes) {
					cerr << "FATAL ERROR e2a~206: invalid instruction address\n";
					return 1;
				}
				bytes += 2;//skip "/*"
				while((*bytes) != '*' && (*bytes) != '/') {
					if((*bytes) == ' ') {
					} else if((*bytes) == 'x') {
						increment--;
					} else {
						increment++;
					}
					bytes++;
				}
				if(increment % 2) {
					cerr << "FATAL ERROR e2a~215: fractional instruction size.\n";
				}
				increment = increment/2;
				nextaddr += increment;
			}
			
			//Get out of loop:
			break;
		}
	}
	
	//Cleanup & return:
	pclose(sass);
	free(kern->name);
	if(kern->bytes) {
		free(kern->bytes);
	}
	cleanNodesFully(kern->functionNames, &free);
	free(kern);
	return 0;
}
