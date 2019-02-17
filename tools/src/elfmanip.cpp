#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "elf.hpp"
#include "elfmanip.hpp"
using namespace std;

/**
 * Alternate overwriteKernel for cases where CPU executable is not an ELF.
 * This is more limited in capability than the non-fallback version; it cannot insert additional code.
 * @param file The name of the executable ELF
 * @param kernel Contains the name of the function and the new bytecode
 * @param calls A list of function offsets
 * @return true iff the kernel is successfully overwritten
 */
bool fallbackOverwriteKernel(char * file, kernelData * kernel, node * calls) {
	//Turn file into convenient char array:
	ifstream inelf(file, ifstream::binary);
	if(!inelf) {
		cerr << "ERROR: unable to read file.\n";
		return false;
	}
	inelf.seekg(0, std::ios::end);
	unsigned int exeSize = inelf.tellg();
	inelf.seekg(0, std::ios::beg);
	char * data = (char *) malloc(exeSize);
	inelf.read(data, exeSize);
	inelf.close();
	
	for(unsigned int x = 0; x < exeSize - sizeof(fatHeader); x += 0x8) {
		fatHeader * head = (fatHeader*) &data[x];
		if(head->magic == 0xba55ed50 && head->unknown == 0x00100001) {
			int totalsize = head->size + 0x10;
			while(true) {
				int padding = 0;
				while(padding < 16 && !data[x + totalsize + padding]) {
					padding++;
				}
				
				fatHeader * head = (fatHeader*) &data[x + totalsize + padding];
				if(head->magic == 0xba55ed50 && head->unknown == 0x00100001) {
					totalsize += head->size + 0x10 + padding;
				} else {
					break;
				}
			}
			
			char * newbytes;
			unsigned int garbage = 0;
			
			int tempvalue = overwriteKernelHelper(data + x, newbytes, totalsize, kernel, calls, garbage, garbage);
			if(tempvalue >= 0) {
				if(tempvalue != totalsize) {
					cerr << "ERROR em~43: fallback can't handle extra " << (tempvalue - totalsize) << " bytes for ELF.\n";
					free(newbytes);
					free(data);
					return false;
				}
				
				for(int y = 0; y < tempvalue; y++) {
					data[x + y] = newbytes[y];
				}
				
				ofstream out(file, ofstream::binary);
				out.write(data, exeSize);
				out.close();
				
				free(newbytes);
				free(data);
				return true;
			} else {
				return false;
			}
			
			break;
		}
	}
	
	free(data);
	cerr << "ERROR em~51: unable to find fatbin. Is this really a CUDA program?\n";
	return false;
}

bool overwriteKernel(char * file, kernelData * kernel, node * calls, bool forceFallback) {
	//Turn ELF file into an ELF struct:
	std::ifstream inelf(file, ifstream::binary);
	if(!inelf) {
		cerr << "ERROR: unable to read file.\n";
		return false;
	}
	std::string contents((std::istreambuf_iterator<char>(inelf)), std::istreambuf_iterator<char>());
	const char * filedata = contents.c_str();
	ELF * elf = bytes2ELF(filedata);
	inelf.close();
	if(forceFallback || !elf) {
		if(!forceFallback) {
			cerr << "WARNING: not an ELF. Attempting fallback kernel identification.\n";
			cerr << "\tNote: if kernel is found via fallback, we can't increase its size.\n";
		}
		return fallbackOverwriteKernel(file, kernel, calls);
	}
	
	//Locate fatbin section
	string target = ".nv_fatbin";
	int found = -1;//whether the fatbin section was actually found
	int sizeChange = 0;//the byte-size increase to fatbin
	unsigned int addr = 0;//address of fatbin
	unsigned int maxaddr = 0;//highest address to change offset for after the size increase
	unsigned int oldsize = 0;//original size of fatbin
	unsigned int newsize = 0;//new size of fatbin
	unsigned int oldELFEnd;
	int numSections = getNumSections(elf);
	for(int x = 0; x < numSections; x++) {
		ELF_Section section = getSection(elf, x);
		const ELF_SHeader shdr = getHeader(elf, section);
		
		if(found < 0) {//still looking for .nv_fatbin
			const char * name = getName(elf, section);
			if(name && !target.compare(name)) {//found .nv_fatbin
				found = x;
				
				//Set variables for later usage
				oldsize = shdr.sh_size;
				const char * bytes = getSectionData(elf, section);
				char * newbytes;
				
				//Get new nv_fatbin data:
				int tempvalue = overwriteKernelHelper(bytes, newbytes, oldsize, kernel, calls, addr, oldELFEnd);
				if(tempvalue < 0) {
					fprintf(stderr, "FATAL ERROR ~119; problem modifying ELF.\n");
					cleanELF(elf);
					return false;
				}
				addr += shdr.sh_addr;
				oldELFEnd += shdr.sh_addr;
				newsize = tempvalue;
				
				//Fix up sizes:
				sizeChange = newsize - oldsize;
				if(sizeChange % 0x40 != 0) {//avoid the hassle of analyzing alignment of later sections
					//TODO what does this error message even mean? Did I make it so the sizechange is guaranteed to be divisible by 0x40?
					fprintf(stderr,"SANITY CHECK ERROR ~130: the ELF might be corrupt now.\n");
					int extra = 0x40 - (sizeChange % 0x40);
					sizeChange += extra;
					newsize += extra;
				}
				
				replaceSectionData(elf, found, newbytes, newsize, oldELFEnd);
				free(newbytes);
			}
		}
		
		//We want maxaddr to be greater than this section's address:
		if(shdr.sh_addr + shdr.sh_size > maxaddr) {
			maxaddr = shdr.sh_addr + shdr.sh_size;
		}
	}
	
	//Alternate attempt to locate fatbin section (for CUDA version <= 4.0)
	if(found < 0) {
		target = ".rodata";
		sizeChange = 0;
		addr = 0;
		maxaddr = 0;
		oldsize = 0;
		newsize = 0;
		int numSections = getNumSections(elf);
		for(int x = 0; x < numSections; x++) {
			ELF_Section section = getSection(elf, x);
			const ELF_SHeader shdr = getHeader(elf, section);
		
			if(found < 0) {//still looking for .rodata
				const char * name = getName(elf, section);
				if(name && !target.compare(name)) {//found .rodata
					found = x;
				
					//Set variables for later usage
					oldsize = shdr.sh_size;
					const char * bytes = getSectionData(elf, section);
					char * newbytes;
					
					//Get new rodata data:
					int tempvalue = overwriteKernelHelper(bytes, newbytes, oldsize, kernel, calls, addr, oldELFEnd);
					if(tempvalue < 0) {
						fprintf(stderr, "FATAL ERROR ~119; problem modifying ELF.\n");
						cleanELF(elf);
						return false;
					}
					addr += shdr.sh_addr;
					oldELFEnd += shdr.sh_addr;
					newsize = tempvalue;
				
					//Fix up sizes:
					sizeChange = newsize - oldsize;
					if(sizeChange % 0x40 != 0) {//avoid the hassle of analyzing alignment of later sections
						fprintf(stderr,"SANITY CHECK ERROR ~130: the ELF might be corrupt now.\n");
						int extra = 0x40 - (sizeChange % 0x40);
						sizeChange += extra;
						newsize += extra;
					}
				
					replaceSectionData(elf, found, newbytes, newsize, oldELFEnd);
					free(newbytes);
				}
			}
		
			//We want maxaddr to be greater than this section's address:
			if(shdr.sh_addr + shdr.sh_size > maxaddr) {
				maxaddr = shdr.sh_addr + shdr.sh_size;
			}
		}
	}
	
	if(found < 0) {
		fprintf(stderr, "FATAL ERROR ~150; no CUDA code found in file.\n");
		cleanELF(elf);
		return false;
	}
	
	//We want maxaddr to include an arbitrary amount of stuff after last addressed section
	// Why? That's just the way it is. Without this you're likely to get segfaults.
	//TODO decide if there's a better way of picking maxaddr's value
	unsigned int temp = maxaddr / 0x100000;
	temp = (temp + 1) * 0x100000;
	temp--;
	maxaddr += 0x8000;
	if(maxaddr < temp) {
		maxaddr = temp;
	}
	
	//Convert ELF to bytes:
	uint64_t numBytes;
	char * bytes = ELF2bytes(elf, numBytes);
	cleanELF(elf);
	
	//Write ELF to file:
	std::ofstream outelf(file, ofstream::binary);
	if(!outelf) {
		fprintf(stderr, "FATAL ERROR ~329: unable to write ELF to file.\n");
		free(bytes);
		return false;
	}
	outelf.write(bytes, numBytes);
	outelf.close();
	free(bytes);
	
	if(oldsize == newsize) {
		return true;
	} else {
		#ifdef LINUX
			cerr << "WARNING: modifying CPU code to account for increased kernel size.\n";
			cerr << "\tThese modifications might theoretically cause problems.\n";
		#else
			cerr << "ERROR: cannot increase binary size on non-linux systems.";
			cerr << "ERROR: ELF modification failed.\n";
			return false;
		#endif
	}
	
	//NOTE: Everything after this comment is devoted to fixing addresses inside the ELF's executable code.
	//	this is done by using objdump to get the hex code and equivalent assembly, then searching the
	//	assembly code for addresses which need to be changed.
	
	//Prepare temporary copy of file:
	string sys = "cp ";
	sys += file;
	sys += " .temp";
	system(sys.c_str());
	
	//Write ELF's executable code to file using objdump
	sys = "objdump -S --insn-width=16 ";
	sys += ".temp > ";
	sys += ".temp.assembly";
	system(sys.c_str());
	
	//Prepare to read objdump's output
	ifstream fasm;
	fasm.open(".temp.assembly", ifstream::binary);
	if (!fasm.good()) {
		fprintf(stderr, "Fatal error ~346 couldn't open temporary file for writing");
		return false;
	}
	
	//Prepare .temp and .out files for reading and writing, respectively
	FILE* in;
	if(!(in = fopen(".temp", "rb"))) {
		fprintf(stderr,"Fatal error. ~353 couldn't open temporary file for writing");
		return false;
	}
	FILE* out;
	if(!(out = fopen(file, "wb"))) {
		fprintf(stderr,"Fatal error ~358; output file may be in use.\n");
		return false;
	}
	int currentOffset = 0;
	
	int toWriteAddr = -1;
	unsigned long long toWriteVal = 0;
	int toWriteSize = -1;
	
	//Loop to parse objdump's results and write the final output accordingly
	while(!fasm.eof()) {
		char line[2048];
		fasm.getline(line, 2048);
		
		//Skip ahead to instruction's address:
		int x = 0;
		while(line[x] == ' ' || line[x] == '\t') {
			x++;
		}
		if(!((line[x] >= '0' && line[x] <= '9') || (line[x] >= 'a' && line[x] <= 'f'))) {//there's no code on this line
			continue;
		}
		
		//address of current instruction
		unsigned long long current_addr;
		string hexval = "";
		while((line[x] >= '0' && line[x] <= '9') || (line[x] >= 'a' && line[x] <= 'f')) {
			hexval += line[x];
			x++;
		}
		stringstream ss;
		ss << std::hex << hexval;
		ss >> current_addr;
		
		//Skip ahead to instruction's hex code:
		if(line[x] != ':') {//there's no code on this line
			continue;
		}
		x++;
		while(line[x] == ' ' || line[x] == '\t') {
			x++;
		}
		
		//hex code for current instruction
		int index_hex = x;
		int numbytes = 0;
		while(line[x] != 0 && ((line[x] >= '0' && line[x] <= '9') || (line[x] >= 'a' && line[x] <= 'f') || line[x] == ' ')) {
			if((line[x] >= '0' && line[x] <= '9') || (line[x] >= 'a' && line[x] <= 'f')) {
				numbytes++;
			}
			x++;
		}
		numbytes = numbytes / 2;
		while(line[x] == ' ' || line[x] == '\t') {
			x++;
		}
		
		//Go to next line if done with this one:
		if(line[x] == 0) {//no assembly-style code for this line
			continue;
		}
		
		int index_posthex = x;
		
		//This loop looks for addresses in current instruction's assembly; changes addresses in binary if appropriate:
		while(line[x] != 0) {
			if(line[x] == '0' && line[x-1] != '-') {
				x++;
				if(line[x] == 'x') {//Found a hex value (thus a possible address)
					x++;
					int index_val = x;
					string hexval = "";
					int hexsize = 0;
					while((line[x] >= '0' && line[x] <= '9') || (line[x] >= 'a' && line[x] <= 'f')) {
						hexval += line[x];
						hexsize++;
						x++;
					}
					bool relative = false;
					if(line[x] == '(') {
						x++;
						if(line[x] == '%') {
							x++;
							if(line[x] == 'r') {
								x++;
								if(line[x] == 'i') {
									x++;
									if(line[x] == 'p') {
										x++;
										if(line[x] == ')') {//Found a relative address
											x++;
											relative = true;
										}
									}
								}
							}
						}
					}
					
					//Get hex's value:
					stringstream ss;
					long long tempval;
					ss << std::hex << hexval;
					ss >> tempval;
					
					//Calculate hex's value, compensating for relative addresses if necessary:
					unsigned long long val;
					if(relative) {
						val = current_addr + numbytes + tempval;
					} else {
						val = tempval;
					}
					
					//If value falls within appropriate address range, modify it in binary code:
					if(val >= addr && val <= maxaddr) {
						bool foundByte = false;
						int index_change;
						int temp = index_hex - 1;
						while(!foundByte) {
							int y = temp + 1;
							int z = index_val;
							//while(line[y] != 0 && (line[y] != line[z] || line[y+1] == ' ')) {
							while(line[y] != 0 && (line[y] != line[z])) {
								y++;
							}
							temp = y;
							
							if(y >= index_posthex) {
								static bool seenError = false;
								if(!seenError) {
									cerr << "WARNING em~428: problem correcting addresses in CPU code:\n";
									cerr << "\t The modified executable may not work due to incorrect addresses\n";
									cerr << "\t   inside the CPU code after modifying the size of the GPU code.\n";
									seenError = true;
								}
								break;
							}
							
							index_change = y + 3;
							index_change -= 3*hexsize / 2;
							
							//Handle case where we accidentally skipped first half of highest byte if it's less than 0x10:
							bool skipChar = false;
							if(line[y+1] == ' ') {
								skipChar = true;
							}
							
							int a = hexsize;
							bool fail = false;
							while(a && !fail) {
								if(line[y] == line[z] || skipChar) {
									if(skipChar) {
										skipChar = false;
									} else {
										y++;
										z++;
										a--;
									}
									if(line[y] == line[z]) {
										y++;
										z++;
										a--;
									} else {
										fail = true;
									}
								} else {
									fail = true;
								}
								y -= 5;
							}
							
							foundByte = !fail;
						}
						
						if(foundByte) {
							//Convert memory address to file offset, removing higher bits:
							int offset_change = (index_change - index_hex)/3;
							offset_change += (current_addr & 0xfffff);
						
							//The value to change:
							unsigned long long change = tempval;
							if(toWriteAddr >= 0) {
								if(offset_change > toWriteAddr) {
									int temp1 = offset_change;
									offset_change = toWriteAddr;
									toWriteAddr = temp1;
									temp1 = hexsize;
									hexsize = toWriteSize;
									toWriteSize = temp1;
									change = toWriteVal;
									toWriteVal = tempval;
								}
							
								//Read & write data until the offset of the value we want to change:
								while(currentOffset < offset_change) {
									int c = fgetc(in);
									fputc(c, out);
									currentOffset++;
								}
							
								//Write changed value:
								int blah = 0;
								while(blah < hexsize/2) {
									fgetc(in);
									unsigned long long blah2 = (sizeChange + change) >> (8*blah);
									int blah3 = blah2 & 0xff;
									fputc(blah3, out);
									currentOffset++;
									blah++;
								}
							}
							else {
								toWriteAddr = offset_change;
								toWriteSize = hexsize;
								toWriteVal = change;
							}
						}
					}
				}
			}
			else if(line[x] == '#') {//reached comment at end of line
				break;
			}
			else {
				//No address here, let's keep going:
				x++;
			}
		}
		if(toWriteAddr >= 0) {
			//Read & write data until the offset of the value we want to change:
			while(currentOffset < toWriteAddr) {
				int c = fgetc(in);
				fputc(c, out);
				currentOffset++;
			}
			
			//Write changed value:
			int blah = 0;
			while(blah < toWriteSize/2) {
				fgetc(in);
				unsigned long long blah2 = (sizeChange + toWriteVal) >> (8*blah);
				int blah3 = blah2 & 0xff;
				fputc(blah3, out);
				currentOffset++;
				blah++;
			}
			toWriteAddr = -1;
		}
	}
	
	//Write remaining data to output file
	int c = fgetc(in);
	while(c != EOF) {
		fputc(c, out);
		c = fgetc(in);
	}
	fclose(in);
	fclose(out);
	
	return true;
}

int overwriteKernelHelper(const char* oldData, char *& newData, unsigned int original_size, kernelData* kernel, node * calls, unsigned int & targetFatOffset, unsigned int & oldEndELF) {
	std::stringstream sstm;
	sstm << ".text." << kernel->name;
	string temp = sstm.str();
	int len = sstm.str().length();
	char* targetName = (char*) malloc(len+1);
	for(int x = 0; x <= len; x++) {
		targetName[x] = temp[x];
	}
	
	sstm.str("");
	sstm.clear();
	sstm << ".nv.shared." << kernel->name;
	temp = sstm.str();
	len = sstm.str().length();
	char* sharedMemoryName = (char*) malloc(len+1);
	for(int x = 0; x <= len; x++) {
		sharedMemoryName[x] = temp[x];
	}
	
	node * mustFree = 0;
	addFirst(&mustFree, targetName);
	addFirst(&mustFree, sharedMemoryName);
	
	//Parse each inner ELF, searching them for target kernel as we go:
	uint64_t loc = 0x0;
	int oldSize = -1;
	uint64_t elfnew = 0;
	uint64_t elfold = 0;
	int targetElf = 0;
	char * newELFbytes = 0;
	uint64_t numELFbytes = 0;
	while(oldSize < 0 && loc < original_size) {
		//Deal with padding and etc.
		if(loc % 4 != 0) {
			loc += 4 - (loc % 4);
		}
		while(true) {
			if(loc >= original_size || *((unsigned int*)(oldData + loc)) == 0xba55ed50) {
				break;
			} else {
				loc += 4;
			}
		}
		if(loc >= original_size) {
			break;
		}
		
		fatHeader * head = (fatHeader*) &oldData[loc];
		int headloc = loc;
		targetFatOffset = loc;
		
		//Check magic number:
		if(head->magic != 0xba55ed50 || head->unknown != 0x00100001) {
			if(!loc) {
				fprintf(stderr, "WARNING: unrecognized magic number for CUDA fatbin.\n");
			}
			else {
				fprintf(stderr, "SANITY CHECK ERROR ~625: no magic number; possibly misaligned fatbin.\n");
			}
		}
		
		loc += 0x10;
		while(oldSize < 0 && loc < head->size + headloc + 0x10) {
			unsigned int * type = (unsigned int*) &oldData[loc];
			unsigned int * offset = (unsigned int*) &oldData[loc + 4];
			unsigned long long * size = (unsigned long long*) &oldData[loc + 8];
			
			int architecture = oldData[loc + 28];
			elfold = *size;
			loc += *offset;
			if((*type & 0xffff) == 0x2) {//this part of the fatbin may contain target ELF
				if(architecture == kernel->arch) {
					ELF * elf = bytes2ELF(&oldData[loc]);
					loc += *size;
					
					if(!elf) {
						fprintf(stderr,"FATAL ERROR em~458 could not parse GPU ELF\n");
						return -1;
					}
				
					//Look for kernel code section & modify it:
					int index = -1;
					int numSections = getNumSections(elf);
					for(int x = 0; x < numSections; x++) {
						ELF_Section sec = getSection(elf, x);
						const ELF_SHeader shdr = getHeader(elf, sec);
						const char * name = getName(elf, sec);
						index++;
						
						//If section contains kernel function, record/change its data appropriately:
						if(!strcmp(name, targetName)) {
							//Set variables for later use:
							oldSize = shdr.sh_size;
							oldEndELF = loc;
							
							//Overwrite data bytes:
							if(kernel->numBytes >= 0) {
								replaceSectionData(elf, x, kernel->bytes, kernel->numBytes);
							}
							
							//Get kernel's symbol table index:
							kernel->symIndex = shdr.sh_info & 0xff;
							
							//Change register count:
							uint32_t sh_info = shdr.sh_info;
							sh_info &= 0x00ffffff;
							sh_info |= (kernel->numRegisters << 24);
							setSH_info(elf, sec, sh_info);
							break;
						} else {
							//cerr << name << " vs " << targetName << "\n";
						}
					}
					
					if(oldSize >= 0) {
						//Set shared memory size, if appropriate:
						if(kernel->sharedMemory >= 0) {
							bool sharedDone = false;
							for(int x = 0; x < numSections; x++) {
								ELF_Section sec = getSection(elf, x);
								const char * name = getName(elf, sec);
								
								//If section contains shared memory data, change size:
								if(!strcmp(name, sharedMemoryName)) {
									replaceSectionData(elf, x, 0, kernel->sharedMemory);
									sharedDone = true;
								}
							}
							
							//Create shared memory section if appropriate:
							if(kernel->sharedMemory > 0 && !sharedDone) {
								cerr << "WARNING: creating shared memory section in ELF.\n";
								ELF_SHeader smemsec;
								smemsec.sh_type = SHT_NOBITS;
								smemsec.sh_flags = 0x3;
								smemsec.sh_addr = 0;
								smemsec.sh_size = kernel->sharedMemory;
								smemsec.sh_link = 0;
								smemsec.sh_info = index;
								smemsec.sh_addralign = 4;
								smemsec.sh_entsize = 0;
								addSection(elf, smemsec, sharedMemoryName, 0);
								addSectionToSegment(elf, getSection(elf, getNumSections(elf) - 1), getNumSegments(elf) - 1);
							}
						}
					
						//Locate & fix symbol table and .nv.info section
						for(int j = 0; j < numSections; j++) {
							ELF_Section sec = getSection(elf, j);
							const ELF_SHeader shdr = getHeader(elf, sec);
							const char * name = getName(elf, sec);
							
							if(shdr.sh_type == SHT_SYMTAB) {
								if(shdr.sh_size % shdr.sh_entsize) {
									cerr << "SANITY CHECK ERROR em~519: fractional symbols in ELF's symbol table.\n";
								}
								
								//Find & change appropriate values in this symbol table:
								int numSyms = shdr.sh_size / shdr.sh_entsize;
								for(int y = 0; y < numSyms; y++) {
									const ELF_Sym sym = getSymbol(elf, sec, y);
									if(sym.st_info == 0x12 && sym.st_value == 0 && sym.st_shndx == index) {
										if(sym.st_size == (unsigned int) oldSize && kernel->numBytes >= 0) {
											setST_size(elf, sec, y, kernel->numBytes);
										}
									}
									else if(sym.st_info == 0x22 && sym.st_shndx == index) {
										if(calls) {//Note: not fixing this doesn't seem to affect anything at runtime?
											//Fix offset of function call inside function:
											setST_value(elf, sec, y, (long long) calls->value);
											calls = calls->next;
											
											//TODO: also fix st_size (not strictly necessary?)
										} else {
											//fprintf(stderr, "WARNING em~701; weird call count\n");
										}
									}
								}
							}
							else if(!strcmp(".nv.info", name)) {
								const char * cdata = getSectionData(elf, sec);
								char * data = (char*) malloc(sizeof(char) * shdr.sh_size);
								memcpy(data, cdata, shdr.sh_size);
								
								//Find appropriate values in section data:
								for(unsigned int y = 0; y < shdr.sh_size;) {
									CUDA_INFO * ci = (CUDA_INFO*)(data + y);
									
									if(ci->format > maxFormat || ci->format < minFormat) {
										cerr << "ERROR (nonfatal): EIFMT type (0x" << std::hex << (int)ci->format << std::dec << ") out of range.\n";
									}
									if(ci->attribute > maxAttribute || ci->attribute < minAttribute) {
										//cerr << "ERROR (nonfatal): EIATTR type (0x" << std::hex << (int)ci->attribute << std::dec << ") out of range.\n";
									}
									
									int datasize = 0;
									if(ci->format == EIFMT_NVAL) {
										datasize = 0;
									} else if(ci->format == EIFMT_BVAL) {
										datasize = 1;
									} else if(ci->format == EIFMT_HVAL) {
										datasize = 2;
									} else if(ci->format == EIFMT_SVAL) {
										//TODO I don't know if this is correct for all attribute types:
										datasize = 2;
										short * sdata = (short*)ci->data;
										datasize += sdata[0];
									}
									
									if(ci->attribute == EIATTR_MIN_STACK_SIZE) {
										if(ci->format == EIFMT_SVAL) {
											if(datasize == 10) {
												int * temp = (int*) (ci->data + 2);
												int funcid = temp[0];
												if(funcid == kernel->symIndex && kernel->min_stack_size >= 0) {
													temp[1] = kernel->min_stack_size;
												}
											} else {
												cerr << "ERROR: Unexpected datasize (" << datasize << ") for min_stack_size.\n";
											}
										} else {
											cerr << "ERROR: Unexpected format for min_stack_size.\n";
										}
									}
									if(ci->attribute == EIATTR_MAX_STACK_SIZE) {
										if(ci->format == EIFMT_SVAL) {
											if(datasize == 10) {
												int * temp = (int*) (ci->data + 2);
												int funcid = temp[0];
												if(funcid == kernel->symIndex && kernel->max_stack_size >= 0) {
													temp[1] = kernel->max_stack_size;
												}
											} else {
												cerr << "ERROR: Unexpected datasize (" << datasize << ") for max_stack_size.\n";
											}
										} else {
											cerr << "ERROR: Unexpected format for max_stack_size.\n";
										}
									} else if(ci->attribute == EIATTR_FRAME_SIZE) {
										if(ci->format == EIFMT_SVAL) {
											if(datasize == 10) {
												int * temp = (int*) (ci->data + 2);
												int funcid = temp[0];
												if(funcid == kernel->symIndex && kernel->frame_size >= 0) {
													temp[1] = kernel->frame_size;
												}
											}
											else {
												cerr << "ERROR: Unexpected datasize (" << datasize << ") for frame_size.\n";
											}
										}
										else {
											cerr << "ERROR: Unexpected format for frame_size.\n";
										}
									}
									
									y += datasize + 2;
								}
								
								//Put char array back into section data:
								replaceSectionData(elf, j, data, shdr.sh_size);
								free(data);
							}
						}
					
						//Update kernel ELF:
						newELFbytes = ELF2bytes(elf, numELFbytes);
						if(!newELFbytes) {
							cerr << "FATAL ERROR em~641; unable to process ELF.\n";
							cleanELF(elf);
							return -1;
						}
						
						elfnew = numELFbytes;
						if(elfold % 8 == 0 && elfnew % 8 != 0) {
							elfnew += 8 - (elfnew % 8);
						} else if(elfnew % 4 != 0) {
							elfnew += 4 - (elfnew % 4);
						}
					} else {
						targetElf++;
					}
					cleanELF(elf);
				} else {//ELF is not for target architecture; ignore it
					targetElf++;
					loc += *size;
				}
			} else {//not an ELF, ignore it
				loc += *size;
			}
		}
	}
	
	//Don't shrink ELF; this creates unnecessary work for fixing it up later:
	if(elfnew < elfold) {
		elfnew = elfold;
	}
	
	if(oldSize <= 0) {
		fprintf(stderr, "Fatal error ~324; kernel function not found.\n");
		cleanNodesFully(mustFree, &free);
		return -1;
	} else {
		//Put fatbin's new data into an array:
		newData = (char*) calloc(original_size + elfnew + 0x40 - elfold, 1);
		unsigned int x = 0;//current index in old data
		unsigned int y = 0;//current index in new data
		int num = 0;//id of current inner ELF
		while(x < original_size) {
			//Deal with padding and etc.
			if(x + 4 >= original_size) {
				while(x < original_size) {
					newData[y++] = oldData[x++];
				}
			} else {
				if(x % 4 != 0) {
					int realign = 4 - (x % 4);
					for(int z = 0; z < realign; z++) {
						newData[y++] = oldData[x++];
					}
				}
				while(x < original_size) {
					if(*((unsigned int*)(oldData + x)) == 0xba55ed50) {
						break;
					} else {
						if(x + 4 < original_size) {
							newData[y++] = oldData[x++];
							newData[y++] = oldData[x++];
							newData[y++] = oldData[x++];
							newData[y++] = oldData[x++];
						} else {
							while(x < original_size) {
								newData[y++] = oldData[x++];
							}
						}
					}
				}
			}
			if(x >= original_size) {
				y -= (x - original_size);
				x = original_size;
				break;
			}
			
			fatHeader * head = (fatHeader*) &oldData[x];
			fatHeader * newHead = (fatHeader*) &newData[y];
			int headx = x;//offset of this header
			int headchange = 0;//size change to this fat block in new data
			
			y += 0x10;
			x += 0x10;
			while(x < headx + head->size + 0x10) {
				//Get relevant metadata for maybe-ELF:
				unsigned int * type = (unsigned int*) &oldData[x];
				unsigned int * offset = (unsigned int*) &oldData[x + 4];
				unsigned long long * size = (unsigned long long*) &oldData[x + 8];
				unsigned long long * newsize = (unsigned long long*) &newData[y + 8];
				
				if((*type & 0xffff) == 0x2) {//an inner ELF
					if(num == targetElf) {//changed ELF
						//Copy metadata:
						unsigned int z = *offset;
						for(; z > 0; z--) {
							newData[y] = oldData[x];
							x++;
							y++;
						}
						
						//Adjust new sizes:
						if(elfnew > *size) {
							elfnew += 0x40 - ((elfnew - *size) % 0x40);
						} else if(elfnew < *size) {
							elfnew += (*size - elfnew) % 0x40;
						}
						headchange += elfnew - *size;
						*newsize = elfnew;
						
						//Copy adjusted ELF into new data:
						z = 0;
						while(z < numELFbytes) {
							newData[y] = newELFbytes[z];
							y++;
							z++;
						}
						free(newELFbytes);
						newELFbytes = 0;
						
						//If necessary for alignment, add zero-padding after ELF:
						while(z < elfnew) {
							newData[y] = 0;
							y++;
							z++;
						}
						
						//Increment x
						x += *size;
					} else {//unchanged ELF
						int z = *offset + *size;
						for(; z > 0; z--) {
							newData[y++] = oldData[x++];
						}
					}
					
					num++;
				} else {//not an ELF
					//Copy non-ELF into new data
					int z = *offset + *size;
					for(; z > 0; z--) {
						newData[y++] = oldData[x++];
					}
				}
			}
			
			//Set new data's header for this fat container, with adjusted size
			newHead->magic = head->magic;
			newHead->unknown = head->unknown;
			newHead->size = head->size + headchange;
		}
		
		cleanNodesFully(mustFree, &free);
		return y;
	}
	
	if(newELFbytes) {
		free(newELFbytes);
	}
	cleanNodesFully(mustFree, &free);
	return -1;
}

/**
 * Fallback function to get contents of a specified kernel function.
 * This version should work on non-ELFs.
 * @param filedata The contents of the executable file
 * @param numBytes The size of the executable file
 * @param kernel The name of the kernel function
 * @param kernelNumber The index of the kernel function; only used if kernel is NULL
 * @return a kernelData pointer, or 0 on failure
 */
kernelData * fallbackGetKernelFunction(const char * filedata, int numBytes, char * kernel, int kernelNumber) {
	for(unsigned int x = 0; x < numBytes - sizeof(fatHeader); x += 0x8) {
		fatHeader * head = (fatHeader*) &filedata[x];
		if(head->magic == 0xba55ed50 && head->unknown == 0x00100001) {
			int fatsize = head->size;
			int totalsize = fatsize + 0x10;
			while(true) {
				int padding = 0;
				while(padding < 16 && !filedata[x + totalsize + padding]) {
					padding++;
				}
				
				fatHeader * head = (fatHeader*) &filedata[x + totalsize + padding];
				if(head->magic == 0xba55ed50 && head->unknown == 0x00100001) {
					totalsize += head->size + 0x10 + padding;
				} else {
					break;
				}
			}
			
			return getKernelFunctionHelper(filedata + x, totalsize, kernel, kernelNumber);
		}
	}
	
	cerr << "ERROR em~899: unable to find fatbin. Is this really a CUDA program?\n";
	return 0;
}

kernelData * getKernelFunction(char * file, char * kernel, int kernelNumber, bool forceFallback) {
	//Turn ELF file into an ELF struct:
	std::ifstream inelf(file, ifstream::binary);
	if(!inelf) {
		cerr << "ERROR: unable to read file.\n";
		return 0;
	}
	inelf.seekg(0, std::ios::end);
	unsigned int exeSize = inelf.tellg();
	inelf.seekg(0, std::ios::beg);
	std::string contents((std::istreambuf_iterator<char>(inelf)), std::istreambuf_iterator<char>());
	const char * filedata = contents.c_str();
	ELF * elf = bytes2ELF(filedata);
	inelf.close();
	if(forceFallback || !elf) {
		if(!forceFallback) {
			cerr << "WARNING: not an ELF. Attempting fallback kernel identification.\n";
			cerr << "\tNote: if kernel is found via fallback, we can't increase its size.\n";
		}
		return fallbackGetKernelFunction(filedata, exeSize, kernel, kernelNumber);
	}
	
	//Locate fatbin section
	const char * target = ".nv_fatbin";
	int numSections = getNumSections(elf);
	for(int x = 0; x < numSections; x++) {
		ELF_Section section = getSection(elf, x);
		const ELF_SHeader shdr = getHeader(elf, section);
		const char * name = getName(elf, section);
		
		//Check if section is .nv_fatbin:
		if(name && !strcmp(target, name)) {
			const char * bytes = getSectionData(elf, section);
			kernelData* answer = getKernelFunctionHelper(bytes, shdr.sh_size, kernel, kernelNumber);
			cleanELF(elf);
			return answer;
		}
	}
	
	//Locate fatbin section on CUDA 4.0
	target = ".rodata";
	for(int x = 0; x < numSections; x++) {
		ELF_Section section = getSection(elf, x);
		const ELF_SHeader shdr = getHeader(elf, section);
		const char * name = getName(elf, section);
		
		//Check if section is .nv_fatbin:
		if(name && !strcmp(target, name)) {
			const char * bytes = getSectionData(elf, section);
			kernelData* answer = getKernelFunctionHelper(bytes, shdr.sh_size, kernel, kernelNumber);
			cleanELF(elf);
			return answer;
		}
	}
	
	cleanELF(elf);
	return 0;
}

kernelData * getKernelFunctionHelper(const char * bytes, unsigned int size, char * kernel, int kernelNumber) {
	char* target = 0;
	int index = 0;
	kernelData* kern = 0;
	unsigned long long maxLoc = size;
	
	if(kernel) {
		target = (char*) malloc(strlen(kernel)+1);
		strcpy(target, kernel);
	}
	
	//Search each ELF for target kernel:
	unsigned long long loc = 0x0;
	while(!kern && loc < size) {
		//Deal with padding and etc.
		if(loc % 4 != 0) {
			loc += 4 - (loc % 4);
		}
		while(true) {
			if(loc >= size || *((unsigned int*)(bytes + loc)) == 0xba55ed50) {
				break;
			} else {
				loc += 4;
			}
		}
		if(loc >= size) {
			break;
		}
		
		fatHeader * head = (fatHeader*) &bytes[loc];
		int headloc = loc;
		
		//Check magic number:
		if(head->magic != 0xba55ed50 || head->unknown != 0x00100001) {
			if(!loc) {
				fprintf(stderr, "WARNING: unrecognized magic number for CUDA fatbin\n");
			}
			else {
				fprintf(stderr, "SANITY CHECK ERROR ~1052: no magic number; possible misaligned fatbin.\n");
			}
		}
		
		loc += 0x10;
		if(loc >= maxLoc) {
			fprintf(stderr, "SANITY CHECK ERROR ~1044: fatbin is located out of bounds.\n");
			return 0;
		}
		while(!kern && loc < head->size + headloc + 0x10) {
			unsigned int * type = (unsigned int*) &bytes[loc];
			unsigned int * offset = (unsigned int*) &bytes[loc + 4];
			unsigned long long * size = (unsigned long long*) &bytes[loc + 8];
			
			int architecture = bytes[loc + 28];
			
			if(*size > headloc + head->size || loc + *offset + *size > headloc + 0x10 + head->size) {
				fprintf(stderr, "SANITY CHECK FAILED ~1053: fatbin values out of bounds.\n");
				return 0;
			}
			
			loc += *offset;
			if(loc >= maxLoc) {
				fprintf(stderr, "SANITY CHECK FAILED ~1060: fatbin loc out of bounds.\n");
				return 0;
			}
			
			if((*type & 0xffff) == 0x2) {//this part of the fatbin contains an ELF
				//Parse raw ELF data:
				ELF * elf = bytes2ELF(bytes + loc);
				loc += *size;
				if(!elf) {
					cerr << "SANITY CHECK ERROR em~885: unable to parse ELF.\n";
					return 0;
				}
				if(loc > maxLoc) {
					fprintf(stderr, "SANITY CHECK ERROR em~889: fatbin loc out of bounds.\n");
					return 0;
				}
				
				//Look for kernel code section:
				int scnIndex = 0;
				int numSections = getNumSections(elf);
				for(int x = 0; x < numSections; x++) {
					ELF_Section section = getSection(elf, x);
					const ELF_SHeader shdr = getHeader(elf, section);
					const char * name = getName(elf, section);
					scnIndex = x;
					
					if(!target) {
						bool containsKernel = !strncmp(name, ".text.", 6);
						if(containsKernel) {
							if(index == kernelNumber) {
								target = (char*) malloc(strlen(name) + 2); //note: the extra malloc'd space avoids string errors elsewhere
								strcpy(target, name + 6);
							}
							else {
								index++;
							}
						}
					}
					
					if(target && strlen(name) > 6 && !strcmp(name + 6, target)) {//this section contains the kernel function
						//Copy data into a single char array:
						char * bytes = (char*) malloc(shdr.sh_size);
						memcpy(bytes, getSectionData(elf, section), shdr.sh_size);
						
						//Prepare return value:
						kern = (kernelData*) malloc(sizeof(kernelData));
						kern->sharedMemory = 0;
						kern->min_stack_size = -1;
						kern->max_stack_size = -1;
						kern->frame_size = -1;
						kern->bytes = bytes;
						kern->name = target;
						kern->arch = architecture;
						kern->functionNames = 0;
						kern->numBytes = shdr.sh_size;
						kern->numRegisters = shdr.sh_info >> 24;
						kern->symIndex = shdr.sh_info & 0xff;
						
						break;
					}
				}
				
				if(kern) {
					for(int x = 0; x < numSections; x++) {
						ELF_Section section = getSection(elf, x);
						const ELF_SHeader shdr = getHeader(elf, section);
						const char * name = getName(elf, section);
						
						//If section contains shared memory data, note size of shared memory
						if(!strncmp(".nv.shared.", name, 11) && !strcmp(name + 11, kern->name)) {
							kern->sharedMemory = shdr.sh_size;
						}
						
						//Elseif symbol table, get subroutine names (if we can)
						else if(shdr.sh_type == SHT_SYMTAB) {
							if(shdr.sh_size % shdr.sh_entsize) {
								cerr << "SANITY CHECK ERROR em~956: fractional symbol count.\n";
							}
							
							//Find & change appropriate values in this symbol table:
							int numSyms = shdr.sh_size / shdr.sh_entsize;
							for(int y = 0; y < numSyms; y++) {
								const ELF_Sym sym = getSymbol(elf, section, y);
								if(sym.st_info == 0x22 && sym.st_shndx == scnIndex) {
									const char * symName = getName(elf, shdr, sym);
									char * copy = (char*) malloc(strlen(symName) + 1);
									strcpy(copy, symName);
									addLast(&kern->functionNames, copy);
								}
							}
						}
						
						//Elseif .nv.info section, get local memory metadata
						else if(!strcmp(".nv.info", name)) {
							const char * bytes = getSectionData(elf, section);
							
							//Find appropriate values in section data:
							for(unsigned int x = 0; x < shdr.sh_size;) {
								CUDA_INFO * ci = (CUDA_INFO*)(bytes+x);
								
								if(ci->format > maxFormat || ci->format < minFormat) {
									cerr << "ERROR: EIFMT type (0x" << std::hex << (int)ci->format << std::dec << ") out of range.\n";
								}
								if(ci->attribute > maxAttribute || ci->attribute < minAttribute) {
									//cerr << "ERROR: EIATTR type (0x" << std::hex << (int)ci->attribute << std::dec << ") out of range.\n";
								}
								
								int datasize = 0;
								if(ci->format == EIFMT_NVAL) {
									datasize = 0;
								} else if(ci->format == EIFMT_BVAL) {
									datasize = 1;
								} else if(ci->format == EIFMT_HVAL) {
									datasize = 2;
								} else if(ci->format == EIFMT_SVAL) {
									//TODO I don't know if this is correct for all attribute types:
									datasize = 2;
									short * sdata = (short*)ci->data;
									datasize += sdata[0];
								}
								
								if(ci->attribute == EIATTR_MIN_STACK_SIZE) {
									if(ci->format == EIFMT_SVAL) {
										if(datasize == 10) {
											int * temp = (int*) (ci->data + 2);
											int funcid = temp[0];
											if(funcid == kern->symIndex) {
												kern->min_stack_size = temp[1];
											}
										} else {
											cerr << "ERROR: Unexpected datasize (" << datasize << ") for min_stack_size.\n";
										}
									} else {
										cerr << "ERROR: Unexpected format for min_stack_size.\n";
									}
								}
								if(ci->attribute == EIATTR_MAX_STACK_SIZE) {
									if(ci->format == EIFMT_SVAL) {
										if(datasize == 10) {
											int * temp = (int*) (ci->data + 2);
											int funcid = temp[0];
											if(funcid == kern->symIndex) {
												kern->max_stack_size = temp[1];
											}
										} else {
											cerr << "ERROR: Unexpected datasize (" << datasize << ") for max_stack_size.\n";
										}
									} else {
										cerr << "ERROR: Unexpected format for max_stack_size.\n";
									}
								} else if(ci->attribute == EIATTR_FRAME_SIZE) {
									if(ci->format == EIFMT_SVAL) {
										if(datasize == 10) {
											int * temp = (int*) (ci->data + 2);
											int funcid = temp[0];
											if(funcid == kern->symIndex) {
												kern->frame_size = temp[1];
											}
										} else {
											cerr << "ERROR: Unexpected datasize (" << datasize << ") for frame_size.\n";
										}
									} else {
										cerr << "ERROR: Unexpected format for frame_size.\n";
									}
								}
								
								x += datasize + 2;
							}
						}
					}
				}
				
				cleanELF(elf);
			}
			else {//not an ELF, ignore it
				loc += *size;
				if(loc > maxLoc) {
					fprintf(stderr, "SANITY CHECK FAILED em~1058: fatbin loc out of bounds.\n");
					return 0;
				}
			}
		}
	}
	
	//Cleanup & return:
	if(kern && kern->bytes) {
		return kern;
	}
	else {
		if(target) {
			free(target);
		}
		return 0;
	}
}

/**
 * Fallback function which returns a list of kernel function names within the specified executable.
 * This should work even if executable is not an ELF.
 * @param filedats The contents of the executable.
 * @param numBytes The size of the executable.
 * @return a list of char*
 */
node * fallbackGetKernelNames(const char * filedata, int numBytes) {
	for(unsigned int x = 0; x < numBytes - sizeof(fatHeader); x += 0x8) {
		fatHeader * head = (fatHeader*) &filedata[x];
		if(head->magic == 0xba55ed50 && head->unknown == 0x00100001) {
			int fatsize = head->size;
			int totalsize = fatsize + 0x10;
			while(true) {
				int padding = 0;
				while(padding < 16 && !filedata[x + totalsize + padding]) {
					padding++;
				}
				
				fatHeader * head = (fatHeader*) &filedata[x + totalsize + padding];
				if(head->magic == 0xba55ed50 && head->unknown == 0x00100001) {
					totalsize += head->size + 0x10 + padding;
				} else {
					break;
				}
			}
			
			return getKernelNamesHelper(filedata + x, totalsize);
		}
	}
	
	cerr << "ERROR em~1227: unable to find fatbin.\n";
	return 0;	
}

node * getKernelNames(char * file, bool forceFallback) {
	//Turn ELF file into an ELF struct:
	std::ifstream inelf(file, ifstream::binary);
	if(!inelf) {
		cerr << "ERROR: unable to read file.\n";
		return 0;
	}
	inelf.seekg(0, std::ios::end);
	unsigned int exeSize = inelf.tellg();
	inelf.seekg(0, std::ios::beg);
	std::string contents((std::istreambuf_iterator<char>(inelf)), std::istreambuf_iterator<char>());
	const char * filedata = contents.c_str();
	ELF * elf = bytes2ELF(filedata);
	inelf.close();
	if(forceFallback || !elf) {
		if(!forceFallback) {
			cerr << "WARNING: not an ELF. Attempting fallback kernel identification.\n";
			cerr << "\tNote: if kernel is found via fallback, we can't increase its size.\n";
		}
		return fallbackGetKernelNames(filedata, exeSize);
	}
	
	//Locate fatbin section
	const char * target = ".nv_fatbin";
	int numSections = getNumSections(elf);
	for(int x = 0; x < numSections; x++) {
		ELF_Section section = getSection(elf, x);
		const ELF_SHeader shdr = getHeader(elf, section);
		const char * name = getName(elf, section);
		
		//Check if section is .nv_fatbin:
		if(name && !strcmp(target, name)) {
			const char * bytes = getSectionData(elf, section);
			node * answer = getKernelNamesHelper(bytes, shdr.sh_size);
			cleanELF(elf);
			return answer;
		}
	}
	
	//Locate fatbin section on CUDA 4.0
	target = ".rodata";
	for(int x = 0; x < numSections; x++) {
		ELF_Section section = getSection(elf, x);
		const ELF_SHeader shdr = getHeader(elf, section);
		const char * name = getName(elf, section);
		
		//Check if section is .nv_fatbin:
		if(name && !strcmp(target, name)) {
			const char * bytes = getSectionData(elf, section);
			node * answer = getKernelNamesHelper(bytes, shdr.sh_size);
			cleanELF(elf);
			return answer;
		}
	}
	
	cleanELF(elf);
	return 0;
}

node * getKernelNamesHelper(const char* bytes, int size) {
	node * names = 0;
	int maxLoc = size;
	
	//Searching host ELFs for kernels:
	long long loc = 0x0;
	while(loc < size) {
		//Deal with padding and etc.
		if(loc % 4 != 0) {
			loc += 4 - (loc % 4);
		}
		while(true) {
			if(loc >= size || *((unsigned int*)(bytes + loc)) == 0xba55ed50) {
				break;
			} else {
				loc += 4;
			}
		}
		if(loc >= size) {
			break;
		}
		
		fatHeader * head = (fatHeader*) &bytes[loc];
		int headloc = loc;
		
		//Check magic number:
		if(head->magic != 0xba55ed50 || head->unknown != 0x00100001) {
			if(!loc) {
				fprintf(stderr, "WARNING: unrecognized magic number for CUDA fatbin\n");
			}
			else {
				fprintf(stderr, "SANITY CHECK ERROR ~1294: no magic; possible misaligned fatbin.\n");
			}
		}
		
		loc += 0x10;
		if(loc >= maxLoc) {
			fprintf(stderr, "SANITY CHECK ERROR ~1270: fatbin is located out of bounds.\n");
			return 0;
		}
		while(loc < (long long)head->size + headloc + 0x10) {
			unsigned int * type = (unsigned int*) &bytes[loc];
			unsigned int * offset = (unsigned int*) &bytes[loc + 4];
			unsigned long long * size = (unsigned long long*) &bytes[loc + 8];
			
			int architecture = bytes[loc + 28];
			
			if(*size > headloc + head->size || loc + *offset + *size > headloc + 0x10 + head->size) {
				fprintf(stderr, "SANITY CHECK FAILED ~1289: fatbin values out of bounds.\n");
				return 0;
			}
			
			loc += *offset;
			if(loc >= maxLoc) {
				fprintf(stderr, "SANITY CHECK FAILED ~1296: fatbin is located out of bounds.\n");
				return 0;
			}
			
			if((*type & 0xffff) == 0x2) {//this part of the fatbin contains an ELF
				//Parse raw ELF data:
				ELF * elf = bytes2ELF(bytes + loc);
				loc += *size;
				if(!elf) {
					cerr << "SANITY CHECK ERROR em~1179: unable to parse ELF.\n";
					return 0;
				}
				if(loc > maxLoc) {
					fprintf(stderr, "SANITY CHECK ERROR em~1183: fatbin is located out of bounds.\n");
					return 0;
				}
				
				//Look for kernel code section:
				int numSections = getNumSections(elf);
				for(int x = 0; x < numSections; x++) {
					ELF_Section section = getSection(elf, x);
					const char * scn_name = getName(elf, section);
					
					if(!strncmp(scn_name, ".text.", 6)) {//section contains a kernel function
						char * name = (char*) malloc(6 + strlen(scn_name)-5);
						strcpy(name + 6, scn_name + 6);
						name[0] = 's';
						name[1] = 'm';
						name[2] = '_';
						name[3] = '0'+(architecture/10);
						name[4] = '0'+(architecture%10);
						name[5] = ' ';
						addLast(&names, name);
					}
				}
				
				//Cleanup
				cleanELF(elf);
			}
			else {//not an ELF, ignore it
				if(loc > maxLoc) {
					fprintf(stderr, "SANITY CHECK FAILED em~1213: fatbin is located out of bounds.\n");
					return 0;
				}
				loc += *size;
			}
		}
	}
	
	//Return:
	return names;
}
