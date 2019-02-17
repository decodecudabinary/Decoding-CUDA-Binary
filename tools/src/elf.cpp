#include <iostream> //needed for cerr
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elf.hpp"
using namespace std;

ELF * bytes2ELF(const char * elfbytes) {
	ELF * elf = (ELF*) malloc(sizeof(ELF));
	
	//Set ELF header:
	bool x64 = true;
	memcpy(&(elf->elf64.header.e_ident), elfbytes, 16);
	if(elf->elf64.header.e_ident.ei_mag != 0x464c457f) {//magic not found; not an ELF file
		free(elf);
		return 0;
	}
	
	//Check whether ELF is 32-bit or 64-bit:
	if(elf->elf64.header.e_ident.ei_class == 1) {
		memcpy(&(elf->elf32.header.e_type), elfbytes + 16, 36);
		x64 = false;
	} else if(elf->elf64.header.e_ident.ei_class == 2) {
		memcpy(&(elf->elf64.header.e_type), elfbytes + 16, 48);
	} else {
		cerr << "SANITY CHECK ERROR e~56: ELF has invalid class.\n";
		free(elf);
		return 0;
	}
	elf->x64 = x64;
	
	//Sanity checks for ELF header:
	if(elf->elf64.header.e_ident.ei_data == 2) {
		cerr << "SANITY CHECK ERROR e~59: big-endian ELF.\n";
		free(elf);
		return 0;
	} else if(elf->elf64.header.e_ident.ei_data != 1) {
		cerr << "SANITY CHECK ERROR e~62: no-endian ELF????.\n";
		free(elf);
		return 0;
	}
	if(x64 && elf->elf64.header.e_ehsize != 64) {
		cerr << "SANITY CHECK ERROR e~73: ELF header has unexpected size value (" << elf->elf64.header.e_ehsize << ").\n";
		free(elf);
		return 0;
	}
	if(!x64 && elf->elf32.header.e_ehsize != 52) {
		cerr << "SANITY CHECK ERROR e~76: ELF header has unexpected size value (" << elf->elf32.header.e_ehsize << ").\n";
		free(elf);
		return 0;
	}
	if((!x64 && elf->elf32.header.e_phentsize != 32) || (x64 && elf->elf64.header.e_phentsize != 56)) {
		cerr << "SANITY CHECK ERROR e~77: Unexpected phentsize (" << elf->elf64.header.e_phentsize << ").\n";
		free(elf);
		return 0;
	}
	if(elf->elf64.header.e_ident.ei_version != 1) {
		cerr << "WARNING e~65: ELF has ident version " << elf->elf64.header.e_ident.ei_version << ".\n";
	}
	if(!x64 && elf->elf32.header.e_version != 1) {
		if(elf->elf64.header.e_machine != 190) { //not a CUDA ELF
			cerr << "WARNING e~70: ELF has version " << elf->elf32.header.e_version << ".\n";
		}
	} else if(x64 && elf->elf64.header.e_version != 1) {
		if(elf->elf32.header.e_machine != 190) { //not a CUDA ELF
			cerr << "WARNING e~71: ELF has version " << elf->elf64.header.e_version << ".\n";
		}
	}
	
	int numELFSections = (x64 ? elf->elf64.header.e_shnum : elf->elf32.header.e_shnum);
	int numSegments = (x64 ? elf->elf64.header.e_phnum : elf->elf32.header.e_phnum);
	
	//Set section headers:
	const char * pointer = elfbytes + (x64 ? elf->elf64.header.e_shoff : elf->elf32.header.e_shoff);
	//unsigned int prevOffset = 0;
	if(x64) {
		elf->elf64.sections = (ELF64_Section**) malloc(sizeof(ELF64_Section*) * elf->elf64.header.e_shnum);
	} else {
		elf->elf32.sections = (ELF32_Section**) malloc(sizeof(ELF32_Section*) * elf->elf32.header.e_shnum);
	}
	for(int x = 0; x < numELFSections; x++) {
		if(x64) {
			elf->elf64.sections[x] = (ELF64_Section*) malloc(sizeof(ELF64_Section));
			memcpy(&elf->elf64.sections[x]->sheader, pointer, elf->elf64.header.e_shentsize);
			pointer += elf->elf64.header.e_shentsize;
		} else {
			elf->elf32.sections[x] = (ELF32_Section*) malloc(sizeof(ELF32_Section));
			memcpy(&elf->elf32.sections[x]->sheader, pointer, elf->elf32.header.e_shentsize);
			pointer += elf->elf32.header.e_shentsize;
		}
		
		if(x64) {
			if(elf->elf64.sections[x]->sheader.sh_type == SHT_NOBITS) {
				elf->elf64.sections[x]->data = 0;
			} else {
				elf->elf64.sections[x]->data = (char*) malloc(elf->elf64.sections[x]->sheader.sh_size + 8);//the +8 will hopefully make valgrind stop whining
			}
		} else {
			if(elf->elf32.sections[x]->sheader.sh_type == SHT_NOBITS) {
				elf->elf32.sections[x]->data = 0;
			} else {
				elf->elf32.sections[x]->data = (char*) malloc(elf->elf32.sections[x]->sheader.sh_size + 8);//the +8 will hopefully make valgrind stop whining
			}
		}
	}
	
	//Set section data for each section:
	for(int x = 0; x < numELFSections; x++) {
		if((x64 ? elf->elf64.sections[x]->sheader.sh_type : elf->elf32.sections[x]->sheader.sh_type) != SHT_NOBITS) {
			pointer = elfbytes + (x64 ? elf->elf64.sections[x]->sheader.sh_offset : elf->elf32.sections[x]->sheader.sh_offset);
			memcpy((x64 ? elf->elf64.sections[x]->data : elf->elf32.sections[x]->data), pointer, (x64 ? elf->elf64.sections[x]->sheader.sh_size : elf->elf32.sections[x]->sheader.sh_size));
		}
	}
	
	//Set program headers:
	pointer = elfbytes + (x64 ? elf->elf64.header.e_phoff : elf->elf32.header.e_phoff);
	if(x64) {
		elf->elf64.pheaders = (ELF64_PHeader**) malloc(sizeof(ELF64_PHeader*) * elf->elf64.header.e_phnum);
	} else {
		elf->elf32.pheaders = (ELF32_PHeader**) malloc(sizeof(ELF32_PHeader*) * elf->elf32.header.e_phnum);
	}
	for(int x = 0; x < numSegments; x++) {
		uint32_t type;
		uint64_t offset;
		uint64_t vaddr;
		uint64_t paddr;
		uint64_t filesz;
		uint64_t memsz;
		int align;
		
		if(x64) {
			elf->elf64.pheaders[x] = (ELF64_PHeader*) malloc(sizeof(ELF64_PHeader));
			memcpy(elf->elf64.pheaders[x], pointer, elf->elf64.header.e_phentsize);
			pointer += elf->elf64.header.e_phentsize;
			
			type = elf->elf64.pheaders[x]->p_type;
			offset = elf->elf64.pheaders[x]->p_offset;
			vaddr = elf->elf64.pheaders[x]->p_vaddr;
			paddr = elf->elf64.pheaders[x]->p_paddr;
			filesz = elf->elf64.pheaders[x]->p_filesz;
			memsz = elf->elf64.pheaders[x]->p_memsz;
			align = elf->elf64.pheaders[x]->p_align;
		} else {
			elf->elf32.pheaders[x] = (ELF32_PHeader*) malloc(sizeof(ELF32_PHeader));
			memcpy(elf->elf32.pheaders[x], pointer, elf->elf32.header.e_phentsize);
			pointer += elf->elf32.header.e_phentsize;
			
			type = elf->elf32.pheaders[x]->p_type;
			offset = elf->elf32.pheaders[x]->p_offset;
			vaddr = elf->elf32.pheaders[x]->p_vaddr;
			paddr = elf->elf32.pheaders[x]->p_paddr;
			filesz = elf->elf32.pheaders[x]->p_filesz;
			memsz = elf->elf32.pheaders[x]->p_memsz;
			align = elf->elf32.pheaders[x]->p_align;
		}
		
		bool insane = false;
		if(memsz < filesz) {
			cerr << "WARNING e~118: segment" << x << ": memory size smaller than file size (" << memsz << "/" << filesz << ").\n";
			//insane = true;
		}
		if(paddr != vaddr) {
			cerr << "SANITY CHECK ERROR e~121: segment" << x << ": unexpected addressing.\n";
			insane = true;
		}
		if(type != PT_LOAD) {
			if(vaddr % align) {
				cerr << "SANITY CHECK ERROR e~124: segment" << x << ": broken alignment (" << align << ") in segment " << x << ".\n";
				insane = true;
			}
			if(paddr % align) {
				cerr << "SANITY CHECK ERROR e~187: segment" << x << ": broken alignment (" << align << ") in segment " << x << ".\n";
				insane = true;
			}
		}
		
		int numSections = 0;
		if(!insane) {
			for(int y = 0; y < numELFSections; y++) {
				unsigned int shoff = (x64 ? elf->elf64.sections[y]->sheader.sh_offset : elf->elf32.sections[y]->sheader.sh_offset);
				unsigned int shsize = (x64 ? elf->elf64.sections[y]->sheader.sh_size : elf->elf32.sections[y]->sheader.sh_size);
				if(shoff >= offset && shoff < offset + memsz) {
					if(shoff + shsize >= offset) {
						if(shoff + shsize < offset + memsz + align) {
							numSections++;
						} else {
							//cerr << "WARNING e~135: section " << y << " partially contained by segment " << x << ".\n";
							//insane = true;
							break;
						}
					} else {
						//cerr << "WARNING e~139: section " << y << " partially contained by segment " << x << ".\n";
						//insane = true;
						break;
					}
				} else if(shoff + shsize > offset) {
					if(shoff + shsize <= offset + memsz) {
						//cerr << "WARNING e~144: section " << y << " partially contained by segment " << x << ".\n";
						//insane = true;
						break;
					}
				}
			}
		}
		
		if(insane) {
			for(int y = 0; y <= x; y++) {
				if(x64) {
					free(elf->elf64.pheaders[y]);
				} else {
					free(elf->elf32.pheaders[y]);
				}
			}
			if(x64) {
				free(elf->elf64.pheaders);
			} else {
				free(elf->elf32.pheaders);
			}
			for(int y = 0; y < numELFSections; y++) {
				if(x64) {
					free(elf->elf64.sections[y]->data);
					free(elf->elf64.sections[y]);
				} else {
					free(elf->elf32.sections[y]->data);
					free(elf->elf32.sections[y]);
				}
			}
			if(x64) {
				free(elf->elf64.sections);
			} else { 
				free(elf->elf32.sections);
			}
			free(elf);
			return 0;
		}
		
		if(x64) {
			elf->elf64.pheaders[x]->numSections = numSections;
			elf->elf64.pheaders[x]->sections = 0;
		} else {
			elf->elf32.pheaders[x]->numSections = numSections;
			elf->elf32.pheaders[x]->sections = 0;
		}
		if(numSections) {
			if(x64) {
				elf->elf64.pheaders[x]->sections = (ELF64_Section**) malloc(sizeof(ELF64_Section*) * numELFSections);
			} else {
				elf->elf32.pheaders[x]->sections = (ELF32_Section**) malloc(sizeof(ELF32_Section*) * numELFSections);
			}
			numSections = 0;
			for(int y = 0; y < numELFSections; y++) {
				unsigned int shoff = (x64 ? elf->elf64.sections[y]->sheader.sh_offset : elf->elf32.sections[y]->sheader.sh_offset);
				unsigned int shsize = (x64 ? elf->elf64.sections[y]->sheader.sh_size : elf->elf32.sections[y]->sheader.sh_size);
				
				if(shoff >= offset && shoff < offset + memsz) {
					if(shoff + shsize >= offset) {
						if(shoff + shsize < offset + memsz + align) {
							if(x64) {
								elf->elf64.pheaders[x]->sections[numSections] = elf->elf64.sections[y];
							} else {
								elf->elf32.pheaders[x]->sections[numSections] = elf->elf32.sections[y];
							}
							numSections++;
						}
					}
				}
			}
		}
	}
	
	return elf;
}

char * ELF2bytes(ELF * elf, uint64_t & size) {
	bool x64 = elf->x64;
	int numSections = (x64 ? elf->elf64.header.e_shnum : elf->elf32.header.e_shnum);
	
	//Calculate ELF size:
	size = 0;
	for(int x = 0; x < numSections; x++) {
		uint64_t tempsize = (x64 ? elf->elf64.sections[x]->sheader.sh_offset: elf->elf32.sections[x]->sheader.sh_offset);
		if((x64 ? elf->elf64.sections[x]->sheader.sh_type : elf->elf32.sections[x]->sheader.sh_type) != SHT_NOBITS) {
			tempsize += (x64 ? elf->elf64.sections[x]->sheader.sh_size : elf->elf32.sections[x]->sheader.sh_size);
		}
		if(tempsize > size) {
			size = tempsize;
		}
	}
	uint64_t size2 = (x64 ?
			elf->elf64.header.e_phoff + (elf->elf64.header.e_phnum * elf->elf64.header.e_phentsize) :
			elf->elf32.header.e_phoff + (elf->elf32.header.e_phnum * elf->elf32.header.e_phentsize));
	if(size < size2) {
		size = size2;
	}
	size2 = (x64 ?
		elf->elf64.header.e_shoff + (numSections * elf->elf64.header.e_shentsize) :
		elf->elf32.header.e_shoff + (numSections * elf->elf32.header.e_shentsize));
	if(size < size2) {
		size = size2;
	}
	
	//Malloc byte array:
	char * elfbytes = (char*) calloc(size, 1);
	
	//Write ELF header:
	void * cpyfrom;
	if(x64) {
		cpyfrom = &(elf->elf64.header);
	} else {
		cpyfrom = &(elf->elf32.header);
	}
	memcpy(elfbytes, cpyfrom, (x64 ? elf->elf64.header.e_ehsize : elf->elf32.header.e_ehsize));
	
	//Write section table:
	char * pointer = elfbytes + (x64 ? elf->elf64.header.e_shoff : elf->elf32.header.e_shoff);
	for(int x = 0; x < numSections; x++) {
		if(x64) {
			memcpy(pointer, &elf->elf64.sections[x]->sheader, elf->elf64.header.e_shentsize);
			pointer += elf->elf64.header.e_shentsize;
		} else {
			memcpy(pointer, &elf->elf32.sections[x]->sheader, elf->elf32.header.e_shentsize);
			pointer += elf->elf32.header.e_shentsize;
		}
	}
	
	//Write section data:
	for(int x = 0; x < numSections; x++) {
		if(x64) {
			if(elf->elf64.sections[x]->sheader.sh_type != SHT_NOBITS) {
				pointer = elfbytes + elf->elf64.sections[x]->sheader.sh_offset;
				memcpy(pointer, elf->elf64.sections[x]->data, elf->elf64.sections[x]->sheader.sh_size);
			}
		} else {
			if(elf->elf32.sections[x]->sheader.sh_type != SHT_NOBITS) {
				pointer = elfbytes + elf->elf32.sections[x]->sheader.sh_offset;
				memcpy(pointer, elf->elf32.sections[x]->data, elf->elf32.sections[x]->sheader.sh_size);
			}
		}
	}
	
	//Write program headers:
	if(x64) {
		pointer = elfbytes + elf->elf64.header.e_phoff;
		for(int x = 0; x < elf->elf64.header.e_phnum; x++) {
			memcpy(pointer, elf->elf64.pheaders[x], elf->elf64.header.e_phentsize);
			pointer += elf->elf64.header.e_phentsize;
		}
	} else {
		pointer = elfbytes + elf->elf32.header.e_phoff;
		for(int x = 0; x < elf->elf32.header.e_phnum; x++) {
			memcpy(pointer, elf->elf32.pheaders[x], elf->elf32.header.e_phentsize);
			pointer += elf->elf32.header.e_phentsize;
		}
	}
	
	return elfbytes;
}

const char * getSectionData(ELF * elf, const ELF_Section section) {
	bool x64 = elf->x64;
	if(x64) {
		return section.sec64->data;
	} else {
		return section.sec32->data;
	}
}

void replaceSectionData(ELF * elf, int sectionID, char * data, unsigned int dataSize, unsigned int oldElfEnd) {
	bool x64 = elf->x64;
	int numSections = (x64 ? elf->elf64.header.e_shnum : elf->elf32.header.e_shnum);
	bool cuelf;
	void * section;
	if(x64) {
		section = elf->elf64.sections[sectionID];
		cuelf = (elf->elf64.header.e_machine == 190);
	} else {
		section = elf->elf32.sections[sectionID];
		cuelf = (elf->elf32.header.e_machine == 190);
	}
	
	bool nobits = (x64 ? elf->elf64.sections[sectionID]->sheader.sh_type : elf->elf32.sections[sectionID]->sheader.sh_type) == SHT_NOBITS;
	if(nobits) {
		if(data) {
			cerr << "SANITY CHECK ERROR e~212: trying to overwrite NOBITS section.\n";
		}
	}
	
	//Overwrite section's data and fix size:
	unsigned int oldSize;
	unsigned int addr;
	unsigned int soff;
	bool noAddress;
	if(x64) {
		ELF64_Section * section64 = (ELF64_Section *) section;
		oldSize = section64->sheader.sh_size;
		addr = section64->sheader.sh_addr;
		soff = section64->sheader.sh_offset;
		section64->sheader.sh_size = dataSize;
		noAddress = (section64->sheader.sh_addr == 0);
	
		if(!nobits && data != section64->data) {
			if(oldSize < dataSize) {
				free(section64->data);
				section64->data = (char*) malloc(dataSize);
			}
			memcpy(section64->data, data, dataSize);
		}
	} else {
		ELF32_Section * section32 = (ELF32_Section *) section;
		oldSize = section32->sheader.sh_size;
		addr = section32->sheader.sh_addr;
		soff = section32->sheader.sh_offset;
		section32->sheader.sh_size = dataSize;
		noAddress = (section32->sheader.sh_addr == 0);
	
		if(!nobits && data != section32->data) {
			if(oldSize < dataSize) {
				free(section32->data);
				section32->data = (char*) malloc(dataSize);
			}
			memcpy(section32->data, data, dataSize);
		}
	}
	
	//Fix offset/address for other sections:
	//TODO fix alignments as necessary
	//Note: have to start from section 0 instead of sectionID+1 now that we know sections may be unordered on some systems
	for(int x = 0; x < numSections; x++) {
		if(!nobits) {
			if(x64) {//x64 ELF
				if(elf->elf64.sections[x]->sheader.sh_offset > soff) {
					elf->elf64.sections[x]->sheader.sh_offset = elf->elf64.sections[x]->sheader.sh_offset + dataSize - oldSize;
					if(elf->elf64.sections[x]->sheader.sh_addr) {
						elf->elf64.sections[x]->sheader.sh_addr = elf->elf64.sections[x]->sheader.sh_addr + dataSize - oldSize;
					}
				}
			} else {//x86 ELF
				if(elf->elf32.sections[x]->sheader.sh_offset > soff) {
					elf->elf32.sections[x]->sheader.sh_offset = elf->elf32.sections[x]->sheader.sh_offset + dataSize - oldSize;
					if(elf->elf32.sections[x]->sheader.sh_addr) {
						elf->elf32.sections[x]->sheader.sh_addr = elf->elf32.sections[x]->sheader.sh_offset + dataSize - oldSize;
					}
				}
			}
		}
	}
	
	//If necessary, fix program headers location:
	if(x64) {
		if((elf->elf64.header.e_phoff) > soff && !nobits) {
			elf->elf64.header.e_phoff = elf->elf64.header.e_phoff + dataSize - oldSize;
		}
	} else {
		if((elf->elf32.header.e_phoff) > soff && !nobits) {
			elf->elf32.header.e_phoff = elf->elf32.header.e_phoff + dataSize - oldSize;
		}
	}
	
	//If necessary, fix section headers location:
	if(x64) {
		if(elf->elf64.header.e_shoff > soff && !nobits) {
			elf->elf64.header.e_shoff = elf->elf64.header.e_shoff + dataSize - oldSize;
		}
	} else {
		if(elf->elf32.header.e_shoff > soff && !nobits) {
			elf->elf32.header.e_shoff = elf->elf32.header.e_shoff + dataSize - oldSize;
		}
	}
	
	//Fix program headers:
	for(int x = 0; x < (x64 ? elf->elf64.header.e_phnum : elf->elf32.header.e_phnum); x++) {
		unsigned int p_offset = (x64 ? elf->elf64.pheaders[x]->p_offset : elf->elf32.pheaders[x]->p_offset);
		unsigned int p_filesz = (x64 ? elf->elf64.pheaders[x]->p_filesz : elf->elf32.pheaders[x]->p_filesz);
		unsigned int sh_offset = (x64 ? ((ELF64_Section *)section)->sheader.sh_offset : ((ELF32_Section *)section)->sheader.sh_offset);
		if(p_offset + p_filesz < sh_offset) {
			//nothing
		} else if(p_offset > sh_offset) {
			if(x64) {
				elf->elf64.pheaders[x]->p_offset = elf->elf64.pheaders[x]->p_offset + dataSize - oldSize;
				if(elf->elf64.pheaders[x]->p_vaddr) {
					elf->elf64.pheaders[x]->p_vaddr = elf->elf64.pheaders[x]->p_vaddr + dataSize - oldSize;
				}
				if(elf->elf64.pheaders[x]->p_paddr) {
					elf->elf64.pheaders[x]->p_paddr = elf->elf64.pheaders[x]->p_paddr + dataSize - oldSize;
				}
			} else {
				elf->elf32.pheaders[x]->p_offset = elf->elf32.pheaders[x]->p_offset + dataSize - oldSize;
				if(elf->elf32.pheaders[x]->p_vaddr && !cuelf) {
					elf->elf32.pheaders[x]->p_vaddr = elf->elf32.pheaders[x]->p_vaddr + dataSize - oldSize;
				}
				if(elf->elf32.pheaders[x]->p_paddr && !cuelf) {
					elf->elf32.pheaders[x]->p_paddr = elf->elf32.pheaders[x]->p_paddr + dataSize - oldSize;
				}
			}
		} else {
			if(x64) {
				for(int y = 0; y < elf->elf64.pheaders[x]->numSections; y++) {
					if(elf->elf64.pheaders[x]->sections[y] == section) {
						elf->elf64.pheaders[x]->p_memsz = elf->elf64.pheaders[x]->p_memsz + dataSize - oldSize;
						if(!nobits) {
							elf->elf64.pheaders[x]->p_filesz = elf->elf64.pheaders[x]->p_filesz + dataSize - oldSize;
						}
						break;
					}
				}
			} else {
				for(int y = 0; y < elf->elf32.pheaders[x]->numSections; y++) {
					if(elf->elf32.pheaders[x]->sections[y] == section) {
						elf->elf32.pheaders[x]->p_memsz = elf->elf32.pheaders[x]->p_memsz + dataSize - oldSize;
						if(!nobits) {
							elf->elf32.pheaders[x]->p_filesz = elf->elf32.pheaders[x]-> p_filesz + dataSize - oldSize;
						}
						break;
					}
				}
			}
		}
	}
	
	if(noAddress) {
		return;
	}
	
	//TODO: pray this works okay:
	unsigned int maxaddr = 0x800000u;
	
	//Fix addresses inside sections (symbol tables, etcetera):
	for(int x = 0; x < numSections; x++) {
		unsigned int stype = (x64 ? elf->elf64.sections[x]->sheader.sh_type : elf->elf32.sections[x]->sheader.sh_type);
		unsigned int ssize = (x64 ? elf->elf64.sections[x]->sheader.sh_size : elf->elf32.sections[x]->sheader.sh_size);
		unsigned int sentsize = (x64 ? elf->elf64.sections[x]->sheader.sh_entsize : elf->elf32.sections[x]->sheader.sh_entsize);
		char * data = (x64 ? elf->elf64.sections[x]->data : elf->elf32.sections[x]->data);
		
		if(stype == SHT_SYMTAB || stype == SHT_DYNSYM) {//symbol table
			//Change symbol values as appropriate:
			if(x64) {
				for(unsigned int x = 0; x < ssize; x += sizeof(ELF64_Sym)) {
					ELF64_Sym * sym = (ELF64_Sym*)(data + x);
					if(sym->st_value >= addr + oldSize && sym->st_value < maxaddr) {
						sym->st_value = sym->st_value + dataSize - oldSize;
					}
				}
			} else {
				for(unsigned int x = 0; x < ssize; x += sizeof(ELF32_Sym)) {
					ELF32_Sym * sym = (ELF32_Sym*)(data + x);
					if(sym->st_value >= addr + oldSize && sym->st_value < maxaddr) {
						sym->st_value = sym->st_value + dataSize - oldSize;
					}
				}
			}
		} else if(stype == SHT_RELA) {//relocation table
			if(x64) {
				for(unsigned int x = 0; x < ssize; x += sentsize) {
					ELF64_Rela1 * rela = (ELF64_Rela1*)(data + x);
					if(rela->r_offset >= addr + oldSize && rela->r_offset < maxaddr) {
						rela->r_offset = rela->r_offset + dataSize - oldSize;
						
					}
				}
			} else {
				for(unsigned int x = 0; x < ssize; x += sizeof(ELF32_Rela)) {
					ELF32_Rela * rela = (ELF32_Rela*)(data + x);
					if(rela->r_offset >= addr + oldSize && rela->r_offset < maxaddr) {
						rela->r_offset = rela->r_offset + dataSize - oldSize;
					}
				}
			}
		} else if(stype == SHT_DYNAMIC) {//dynamic table
			if(x64) {
				for(unsigned int x = 0; x < ssize; x += sizeof(ELF64_Dyn)) {
					ELF64_Dyn * dyn = (ELF64_Dyn*)(data + x);
					if(dyn->d_tag) {
						if(dyn->d_un.d_ptr >= addr + oldSize && dyn->d_un.d_ptr < maxaddr) {
							dyn->d_un.d_ptr = dyn->d_un.d_ptr + dataSize - oldSize;
						}
					}
				}
			} else {
				for(unsigned int x = 0; x < ssize; x += sizeof(ELF32_Dyn)) {
					ELF32_Dyn * dyn = (ELF32_Dyn*)(data + x);
					if(dyn->d_tag) {
						if(dyn->d_un.d_ptr >= addr + oldSize && dyn->d_un.d_ptr < maxaddr) {
							dyn->d_un.d_ptr = dyn ->d_un.d_ptr + dataSize - oldSize;
						}
					}
				}
			}
		} else if(!strncmp(getName(elf, getSection(elf, x)), ".got", 4)) {//global offset table
			for(unsigned int x = 0; x < ssize; x += sizeof(unsigned int)) {
				unsigned int * address = (unsigned int*)(data + x);
				if(*address >= addr + oldSize && *address < maxaddr) {
					*address = *address + dataSize - oldSize;
				}
			}
		} else if(oldElfEnd && !strcmp(getName(elf, getSection(elf, x)), ".nvFatBinSegment")) {//Fix up nvFatBinSegment addresses
			//Change anything that looks like an address in the right range:
			for(unsigned int x = 0; x < ssize; x += 4) {
				unsigned int * address = (unsigned int*)(data + x);
				if(*address >= oldElfEnd && *address < maxaddr) {
					*address = *address + dataSize - oldSize;
				}
			}
		}
	}
}

int getNumSections(ELF * elf) {
	bool x64 = elf->x64;
	return (x64 ? elf->elf64.header.e_shnum : elf->elf32.header.e_shnum);
}

int getNumSegments(ELF * elf) {
	bool x64 = elf->x64;
	return (x64 ? elf->elf64.header.e_phnum : elf->elf32.header.e_phnum);
}

ELF_Section getSection(ELF * elf, int index) {
	ELF_Section ret;
	bool x64 = elf->x64;
	if(x64) {
		ret.sec64 = elf->elf64.sections[index];
	} else {
		ret.sec32 = elf->elf32.sections[index];
	}
	return ret;
}

const ELF_Sym getSymbol(ELF * elf, const ELF_Section section, int index) {
	ELF_Sym ret;
	bool x64 = elf->x64;
	if(x64) {
		ELF64_Sym * sym64 = ((ELF64_Sym*)(section.sec64->data + (index * section.sec64->sheader.sh_entsize)));
		ret.st_name = sym64->st_name;
		ret.st_info = sym64->st_info;
		ret.st_other = sym64->st_other;
		ret.st_shndx = sym64->st_shndx;
		ret.st_value = sym64->st_value;
		ret.st_size = sym64->st_size;
	} else {
		ELF32_Sym * sym32 = ((ELF32_Sym*)(section.sec32->data + (index * section.sec32->sheader.sh_entsize)));
		ret.st_name = sym32->st_name;
		ret.st_info = sym32->st_info;
		ret.st_other = sym32->st_other;
		ret.st_shndx = sym32->st_shndx;
		ret.st_value = sym32->st_value;
		ret.st_size = sym32->st_size;
	}
	return ret;
}

const ELF_SHeader getHeader(ELF * elf, const ELF_Section section) {
	ELF_SHeader ret;
	bool x64 = elf->x64;
	if(x64) {
		ret.sh_name = section.sec64->sheader.sh_name;
		ret.sh_type = section.sec64->sheader.sh_type;
		ret.sh_flags = section.sec64->sheader.sh_flags;
		ret.sh_addr = section.sec64->sheader.sh_addr;
		ret.sh_offset = section.sec64->sheader.sh_offset;
		ret.sh_size = section.sec64->sheader.sh_size;
		ret.sh_link = section.sec64->sheader.sh_link;
		ret.sh_info = section.sec64->sheader.sh_info;
		ret.sh_addralign = section.sec64->sheader.sh_addralign;
		ret.sh_entsize = section.sec64->sheader.sh_entsize;
	} else {
		ret.sh_name = section.sec32->sheader.sh_name;
		ret.sh_type = section.sec32->sheader.sh_type;
		ret.sh_flags = section.sec32->sheader.sh_flags;
		ret.sh_addr = section.sec32->sheader.sh_addr;
		ret.sh_offset = section.sec32->sheader.sh_offset;
		ret.sh_size = section.sec32->sheader.sh_size;
		ret.sh_link = section.sec32->sheader.sh_link;
		ret.sh_info = section.sec32->sheader.sh_info;
		ret.sh_addralign = section.sec32->sheader.sh_addralign;
		ret.sh_entsize = section.sec32->sheader.sh_entsize;
	}
	return ret;
}

const char * getName(ELF * elf, const ELF_Section section) {
	bool x64 = elf->x64;
	if(x64) {
		ELF64_Section * strsec = elf->elf64.sections[elf->elf64.header.e_shstrndx];
		return reinterpret_cast<const char*>(strsec->data + section.sec64->sheader.sh_name);
	} else {
		ELF32_Section * strsec = elf->elf32.sections[elf->elf32.header.e_shstrndx];
		return reinterpret_cast<const char *>(strsec->data + section.sec32->sheader.sh_name);
	}
}

const char * getName(ELF * elf, ELF_SHeader symtab, const ELF_Sym sym) {
	bool x64 = elf->x64;
	if(x64) {
		ELF64_Section * strsec = elf->elf64.sections[symtab.sh_link];
		return reinterpret_cast<const char*>(strsec->data + sym.st_name);
	} else {
		ELF32_Section * strsec = elf->elf32.sections[symtab.sh_link];
		return reinterpret_cast<const char *>(strsec->data + sym.st_name);
	}
}

void addSymbol(ELF * elf, const ELF_Section sec, ELF_Sym symbol, char * name) {
	bool x64 = elf->x64;
	int numSections = (x64 ? elf->elf64.header.e_shnum : elf->elf32.header.e_shnum);
	int numSyms;
	if(x64) {
		numSyms = sec.sec64->sheader.sh_size / sec.sec64->sheader.sh_entsize;
	} else {
		numSyms = sec.sec32->sheader.sh_size / sec.sec32->sheader.sh_entsize;
	}
	
	if(x64) {
		ELF64_Section * strsec = elf->elf64.sections[elf->elf64.header.e_shstrndx];
		strsec->data = (char*) realloc(strsec->data, strsec->sheader.sh_size + 1 + strlen(name));
		symbol.st_name = strsec->sheader.sh_size;
		memcpy(strsec->data + strsec->sheader.sh_size, name, 1 + strlen(name));
		replaceSectionData(elf, elf->elf64.header.e_shstrndx, strsec->data, strsec->sheader.sh_size + 1 + strlen(name));
	} else {
		ELF32_Section * strsec = elf->elf32.sections[elf->elf32.header.e_shstrndx];
		strsec->data = (char*) realloc(strsec->data, strsec->sheader.sh_size + 1 + strlen(name));
		symbol.st_name = strsec->sheader.sh_size;
		memcpy(strsec->data + strsec->sheader.sh_size, name, 1 + strlen(name));
		replaceSectionData(elf, elf->elf32.header.e_shstrndx, strsec->data, strsec->sheader.sh_size + 1 + strlen(name));
	}
	
	const char * olddata = getSectionData(elf, sec);
	char * newdata = (char*) malloc((numSyms + 1) * (x64 ? sec.sec64->sheader.sh_entsize : sec.sec32->sheader.sh_entsize));
	memcpy(newdata, olddata, (x64 ? sec.sec64->sheader.sh_size : sec.sec32->sheader.sh_size));
	
	if(x64) {
		ELF64_Sym * sym = (ELF64_Sym*) (newdata + sec.sec64->sheader.sh_size);
		sym->st_info = symbol.st_info;
		sym->st_other = symbol.st_other;
		sym->st_shndx = symbol.st_shndx;
		sym->st_value = symbol.st_value;
		sym->st_size = symbol.st_size;
	} else {
		ELF32_Sym * sym = (ELF32_Sym*) (newdata + sec.sec64->sheader.sh_size);
		sym->st_info = symbol.st_info;
		sym->st_other = symbol.st_other;
		sym->st_shndx = symbol.st_shndx;
		sym->st_value = symbol.st_value;
		sym->st_size = symbol.st_size;
	}
	
	for(int x = 0; x < numSections; x++) {
		if(x64 && elf->elf64.sections[x] == sec.sec64) {
			replaceSectionData(elf, x, newdata, sec.sec64->sheader.sh_entsize);
			return;
		} else if(!x64 && elf->elf32.sections[x] == sec.sec32) {
			replaceSectionData(elf, x, newdata, sec.sec32->sheader.sh_entsize);
			return;
		}
	}
	
	cerr << "SANITY CHECK ERROR e~760: unable to identify ELF section.\n";
}

void addSection(ELF * elf, ELF_SHeader shdr, char * name, char * data) {
	bool x64 = elf->x64;
	bool cuelf = (x64 ? (elf->elf64.header.e_machine == 190) : (elf->elf32.header.e_machine == 190));
	int numSegments = (x64 ? elf->elf64.header.e_phnum : elf->elf32.header.e_phnum);
	int numSections = (x64 ? elf->elf64.header.e_shnum : elf->elf32.header.e_shnum) + 1;
	bool nobits = shdr.sh_type == SHT_NOBITS;
	
	if(x64) {
		elf->elf64.header.e_shnum++;
		shdr.sh_offset = elf->elf64.sections[numSections - 2]->sheader.sh_offset;
		if(elf->elf64.sections[numSections - 2]->sheader.sh_type != SHT_NOBITS) {
			shdr.sh_offset += elf->elf64.sections[numSections - 2]->sheader.sh_size;
		}
		shdr.sh_addr = 0;
		if(elf->elf64.sections[numSections - 2]->sheader.sh_addr) {
			shdr.sh_addr = elf->elf64.sections[numSections - 2]->sheader.sh_addr;
			if(elf->elf64.sections[numSections - 2]->sheader.sh_type != SHT_NOBITS) {
				shdr.sh_addr += elf->elf64.sections[numSections - 2]->sheader.sh_size;
			}
		}
		if(shdr.sh_offset % shdr.sh_addralign) {
			int align = shdr.sh_addralign - (shdr.sh_offset % shdr.sh_addralign);
			shdr.sh_offset += align;
			if(shdr.sh_addr) {
				shdr.sh_addr += align;
			}
		}
		
		elf->elf64.sections = (ELF64_Section**) realloc(elf->elf64.sections, sizeof(ELF64_Section*) * (numSections));
		elf->elf64.sections[numSections - 1] = (ELF64_Section*) malloc(sizeof(ELF64_Section));
		elf->elf64.sections[numSections - 1]->sheader.sh_type = shdr.sh_type;
		elf->elf64.sections[numSections - 1]->sheader.sh_flags = shdr.sh_flags;
		elf->elf64.sections[numSections - 1]->sheader.sh_addr = shdr.sh_addr;
		elf->elf64.sections[numSections - 1]->sheader.sh_offset = shdr.sh_offset;
		elf->elf64.sections[numSections - 1]->sheader.sh_size = shdr.sh_size;
		elf->elf64.sections[numSections - 1]->sheader.sh_link = shdr.sh_link;
		elf->elf64.sections[numSections - 1]->sheader.sh_info = shdr.sh_info;
		elf->elf64.sections[numSections - 1]->sheader.sh_addralign = shdr.sh_addralign;
		elf->elf64.sections[numSections - 1]->sheader.sh_entsize = shdr.sh_entsize;
		elf->elf64.sections[numSections - 1]->data = data;
	} else {
		elf->elf32.header.e_shnum++;
		shdr.sh_offset = elf->elf32.sections[numSections - 2]->sheader.sh_offset;
		if(elf->elf32.sections[numSections - 2]->sheader.sh_type != SHT_NOBITS) {
			shdr.sh_offset += elf->elf32.sections[numSections - 2]->sheader.sh_size;
		}
		shdr.sh_addr = 0;
		if(elf->elf32.sections[numSections - 2]->sheader.sh_addr) {
			shdr.sh_addr = elf->elf32.sections[numSections - 2]->sheader.sh_addr;
			if(elf->elf32.sections[numSections - 2]->sheader.sh_type != SHT_NOBITS) {
				shdr.sh_addr += elf->elf32.sections[numSections - 2]->sheader.sh_size;
			}
		}
		if(shdr.sh_offset % shdr.sh_addralign) {
			int align = shdr.sh_addralign - (shdr.sh_offset % shdr.sh_addralign);
			shdr.sh_offset += align;
			if(shdr.sh_addr) {
				shdr.sh_addr += align;
			}
		}
		
		elf->elf32.sections = (ELF32_Section**) realloc(elf->elf32.sections, sizeof(ELF32_Section*) * (numSections));
		elf->elf32.sections[numSections - 1] = (ELF32_Section*) malloc(sizeof(ELF32_Section));
		elf->elf32.sections[numSections - 1]->sheader.sh_type = shdr.sh_type;
		elf->elf32.sections[numSections - 1]->sheader.sh_flags = shdr.sh_flags;
		elf->elf32.sections[numSections - 1]->sheader.sh_addr = shdr.sh_addr;
		elf->elf32.sections[numSections - 1]->sheader.sh_offset = shdr.sh_offset;
		elf->elf32.sections[numSections - 1]->sheader.sh_size = shdr.sh_size;
		elf->elf32.sections[numSections - 1]->sheader.sh_link = shdr.sh_link;
		elf->elf32.sections[numSections - 1]->sheader.sh_info = shdr.sh_info;
		elf->elf32.sections[numSections - 1]->sheader.sh_addralign = shdr.sh_addralign;
		elf->elf32.sections[numSections - 1]->sheader.sh_entsize = shdr.sh_entsize;
		elf->elf32.sections[numSections - 1]->data = data;
	}
	
	//If necessary, fix program headers location:
	if(x64) {
		if((elf->elf64.header.e_phoff) >= shdr.sh_offset && !nobits) {
			elf->elf64.header.e_phoff += shdr.sh_size;
		}
	} else {
		if((elf->elf32.header.e_phoff) >= shdr.sh_offset && !nobits) {
			elf->elf32.header.e_phoff += shdr.sh_size;
		}
	}
	
	//If necessary, fix section headers location:
	if(x64) {
		if(elf->elf64.header.e_shoff >= shdr.sh_offset && !nobits) {
			elf->elf64.header.e_shoff += shdr.sh_size;
		}
	} else {
		if(elf->elf32.header.e_shoff >= shdr.sh_offset && !nobits) {
			elf->elf32.header.e_shoff += shdr.sh_size;
		}
	}
	
	//Pad section name to avoid problems with program segment alignment:
	int namesize = strlen(name) + 1;
	if(namesize % 32) {
		namesize += 32 - (namesize % 32);
	}
	
	//Set section name appropriately:
	if(x64) {
		ELF64_Section * strsec = elf->elf64.sections[elf->elf64.header.e_shstrndx];
		strsec->data = (char*) realloc(strsec->data, strsec->sheader.sh_size + namesize);
		elf->elf64.sections[numSections - 1]->sheader.sh_name = strsec->sheader.sh_size;
		memset(strsec->data + strsec->sheader.sh_size, 0, namesize);//avoids valgrind whining about uninitialized values
		memcpy(strsec->data + strsec->sheader.sh_size, name, 1 + strlen(name));
		replaceSectionData(elf, elf->elf64.header.e_shstrndx, strsec->data, strsec->sheader.sh_size + namesize);
	} else {
		ELF32_Section * strsec = elf->elf32.sections[elf->elf32.header.e_shstrndx];
		strsec->data = (char*) realloc(strsec->data, strsec->sheader.sh_size + namesize);
		elf->elf32.sections[numSections - 1]->sheader.sh_name = strsec->sheader.sh_size;
		memset(strsec->data + strsec->sheader.sh_size, 0, namesize);//avoids valgrind whining about uninitialized values
		memcpy(strsec->data + strsec->sheader.sh_size, name, 1 + strlen(name));
		replaceSectionData(elf, elf->elf32.header.e_shstrndx, strsec->data, strsec->sheader.sh_size + namesize);
	}
	
	//Compensate for size increase of section header table:
	if(x64) {
		if((elf->elf64.header.e_phoff) > elf->elf64.header.e_shoff) {
			elf->elf64.header.e_phoff += sizeof(ELF64_SHeader);
		}
	} else {
		if((elf->elf32.header.e_phoff) > elf->elf64.header.e_shoff) {
			elf->elf32.header.e_phoff += sizeof(ELF32_SHeader);
		}
	}
	if(x64) {
		for(int x = 0; x < numSegments; x++) {
			if(elf->elf64.pheaders[x]->p_offset > elf->elf64.header.e_shoff) {
				elf->elf64.pheaders[x]->p_offset += sizeof(ELF64_SHeader);
				if(elf->elf64.pheaders[x]->p_vaddr && !cuelf) {
					elf->elf64.pheaders[x]->p_vaddr += sizeof(ELF64_SHeader);
				}
				if(elf->elf64.pheaders[x]->p_paddr && !cuelf) {
					elf->elf64.pheaders[x]->p_paddr += sizeof(ELF64_SHeader);
				}
			}
		}
	} else {
		for(int x = 0; x < numSegments; x++) {
			if(elf->elf32.pheaders[x]->p_offset > elf->elf32.header.e_shoff) {
				elf->elf32.pheaders[x]->p_offset += sizeof(ELF32_SHeader);
				if(elf->elf32.pheaders[x]->p_vaddr && !cuelf) {
					elf->elf32.pheaders[x]->p_vaddr += sizeof(ELF32_SHeader);
				}
				if(elf->elf32.pheaders[x]->p_paddr && !cuelf) {
					elf->elf32.pheaders[x]->p_paddr += sizeof(ELF32_SHeader);
				}
			}
		}
	}
	for(int x = 0; x < numSections; x++) {
		if(x64) {
			if(elf->elf64.sections[x]->sheader.sh_offset > elf->elf64.header.e_shoff) {
				elf->elf64.sections[x]->sheader.sh_offset += sizeof(ELF64_SHeader);
			}
		} else {
			if(elf->elf32.sections[x]->sheader.sh_offset > elf->elf32.header.e_shoff) {
				elf->elf32.sections[x]->sheader.sh_offset += sizeof(ELF32_SHeader);
			}
		}
	}
}

void addSectionToSegment(ELF * elf, const ELF_Section section, int segment) {
	bool x64 = elf->x64;
	bool cuelf = (x64 ? (elf->elf64.header.e_machine == 190) : (elf->elf32.header.e_machine == 190));
	bool nobits = (x64 ? section.sec64->sheader.sh_type : section.sec32->sheader.sh_type) == SHT_NOBITS;
	int numSegments = (x64 ? elf->elf64.header.e_phnum : elf->elf32.header.e_phnum);
	
	if(x64) {
		elf->elf64.pheaders[segment]->sections = (ELF64_Section**) realloc(elf->elf64.pheaders[segment]->sections, sizeof(ELF64_Section*) * (1 + elf->elf64.pheaders[segment]->numSections));
		elf->elf64.pheaders[segment]->sections[elf->elf64.pheaders[segment]->numSections++] = section.sec64;
	} else {
		elf->elf32.pheaders[segment]->sections = (ELF32_Section**) realloc(elf->elf32.pheaders[segment]->sections, sizeof(ELF32_Section*) * (1 + elf->elf32.pheaders[segment]->numSections));
		elf->elf32.pheaders[segment]->sections[elf->elf32.pheaders[segment]->numSections++] = section.sec32;
	}
	
	if(x64) {
		//Workaround for nvcc's preference to set unused segments to offset 0:
		if(segment > 1 && !elf->elf64.pheaders[segment]->p_offset) {
			elf->elf64.pheaders[segment]->p_offset = section.sec64->sheader.sh_offset;
		}
		
		unsigned int extra = 0;
		if(elf->elf64.pheaders[segment]->p_offset + elf->elf64.pheaders[segment]->p_memsz < section.sec64->sheader.sh_offset) {
			extra = section.sec64->sheader.sh_offset - (elf->elf64.pheaders[segment]->p_offset + elf->elf64.pheaders[segment]->p_memsz);
			if(extra > section.sec64->sheader.sh_addralign) {
				cerr << "ERROR e~951: ELF segment difference is greater than alignment.\n";
			}
		}
		
		elf->elf64.pheaders[segment]->p_memsz += section.sec64->sheader.sh_size + extra;
		if(!nobits) {
			elf->elf64.pheaders[segment]->p_filesz += section.sec64->sheader.sh_size + extra;
		}
		
		for(int x = segment + 1; x < numSegments; x++) {
			elf->elf64.pheaders[x]->p_offset += section.sec64->sheader.sh_size + extra;
			if(elf->elf64.pheaders[x]->p_vaddr && !cuelf) {
				elf->elf64.pheaders[x]->p_vaddr += section.sec64->sheader.sh_size + extra;
			}
			if(elf->elf64.pheaders[x]->p_paddr && !cuelf) {
				elf->elf64.pheaders[x]->p_paddr += section.sec64->sheader.sh_size + extra;
			}
		}
	} else {
		//Workaround for nvcc's preference to set unused segments to offset 0:
		if(segment > 1 && !elf->elf32.pheaders[segment]->p_offset) {
			elf->elf32.pheaders[segment]->p_offset = section.sec32->sheader.sh_offset;
		}
		
		unsigned int extra = 0;
		if(elf->elf32.pheaders[segment]->p_offset + elf->elf32.pheaders[segment]->p_memsz < section.sec32->sheader.sh_offset) {
			extra = section.sec32->sheader.sh_offset - (elf->elf32.pheaders[segment]->p_offset + elf->elf32.pheaders[segment]->p_memsz);
			if(extra > section.sec32->sheader.sh_addralign) {
				cerr << "ERROR e~979: ELF segment difference greater than alignment.\n";
			}
		}
		
		elf->elf32.pheaders[segment]->p_memsz += section.sec32->sheader.sh_size + extra;
		if(!nobits) {
			elf->elf32.pheaders[segment]->p_filesz += section.sec32->sheader.sh_size + extra;
		}
		
		for(int x = segment + 1; x < numSegments; x++) {
			elf->elf32.pheaders[x]->p_offset += section.sec32->sheader.sh_size + extra;
			if(elf->elf32.pheaders[x]->p_vaddr && !cuelf) {
				elf->elf32.pheaders[x]->p_vaddr += section.sec32->sheader.sh_size + extra;
			}
			if(elf->elf32.pheaders[x]->p_paddr && !cuelf) {
				elf->elf32.pheaders[x]->p_paddr += section.sec32->sheader.sh_size + extra;
			}
		}
	}
}

void setSH_info(ELF * elf,  const ELF_Section section, uint32_t info) {
	bool x64 = elf->x64;
	if(x64) {
		section.sec64->sheader.sh_info = info;
	} else {
		section.sec32->sheader.sh_info = info;
	}
}

void setST_size(ELF * elf,  const ELF_Section symtab, int index, uint64_t size) {
	bool x64 = elf->x64;
	if(x64) {
		ELF64_Sym * sym64 = ((ELF64_Sym*)(symtab.sec64->data + (index * symtab.sec64->sheader.sh_entsize)));
		sym64->st_size = size;
	} else {
		ELF32_Sym * sym32 = ((ELF32_Sym*)(symtab.sec32->data + (index * symtab.sec32->sheader.sh_entsize)));
		sym32->st_size = size;
	}
}

void setST_value(ELF * elf,  const ELF_Section symtab, int index, uint64_t value) {
	bool x64 = elf->x64;
	if(x64) {
		ELF64_Sym * sym64 = ((ELF64_Sym*)(symtab.sec64->data + (index * symtab.sec64->sheader.sh_entsize)));
		sym64->st_value = value;
	} else {
		ELF32_Sym * sym32 = ((ELF32_Sym*)(symtab.sec32->data + (index * symtab.sec32->sheader.sh_entsize)));
		sym32->st_value = value;
	}
}

void cleanELF(ELF * elf) {
	bool x64 = elf->x64;
	int numELFSections = (x64 ? elf->elf64.header.e_shnum : elf->elf32.header.e_shnum);
	int numELFSegments = (x64 ? elf->elf64.header.e_phnum : elf->elf32.header.e_phnum);
	
	for(int y = 0; y < numELFSegments; y++) {
		if(x64) {
			free(elf->elf64.pheaders[y]->sections);
			free(elf->elf64.pheaders[y]);
		} else {
			free(elf->elf32.pheaders[y]->sections);
			free(elf->elf32.pheaders[y]);
		}
	}
	if(x64) {
		free(elf->elf64.pheaders);
	} else {
		free(elf->elf32.pheaders);
	}
	for(int y = 0; y < numELFSections; y++) {
		if(x64) {
			if(elf->elf64.sections[y]->data) {
				free(elf->elf64.sections[y]->data);
			}
			free(elf->elf64.sections[y]);
		} else {
			if(elf->elf32.sections[y]->data) {
				free(elf->elf32.sections[y]->data);
			}
			free(elf->elf32.sections[y]);
		}
	}
	if(x64) {
		free(elf->elf64.sections);
	} else { 
		free(elf->elf32.sections);
	}
	free(elf);
}

