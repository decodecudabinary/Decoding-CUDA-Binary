#ifndef ELF_HPP
#define ELF_HPP

#include <stdint.h>

/**
 * @file elf.hpp
 * Defines structures and functions used for analyzing/modifying ELFs.
 * This should (hopefully) replace the libelf library in cubin-analyze.
 */

//Section types:
#define SHT_NULL        0
#define SHT_PROGBITS    1
#define SHT_SYMTAB      2
#define SHT_STRTAB      3
#define SHT_RELA        4
#define SHT_HASH        5
#define SHT_DYNAMIC     6
#define SHT_NOTE        7
#define SHT_NOBITS      8
#define SHT_REL         9
#define SHT_SHLIB       10
#define SHT_DYNSYM      11
#define SHT_NUM         12
#define SHT_LOPROC      0x70000000
#define SHT_HIPROC      0x7fffffff
#define SHT_LOUSER      0x80000000
#define SHT_HIUSER      0xffffffff

//Program Header types:
#define PT_NULL       0
#define PT_LOAD       1
#define PT_DYNAMIC    2
#define PT_INTERP     3
#define PT_NOTE       4
#define PT_SHLIB      5
#define PT_PHDR       6

/**
 * Struct containing an ELF's identifier in its header.
 */
typedef struct ELF_Identifier {
	uint32_t ei_mag;
	uint8_t ei_class;
	uint8_t ei_data;
	uint8_t ei_version;
	uint8_t ei_osabi;
	uint8_t ei_abiversion;
	uint8_t ei_pad[7];
} ELF_Identifier;

/**
 * Struct containing a 64-bit ELF's header.
 */
typedef struct ELF64_Header {
	ELF_Identifier e_ident;
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint64_t e_entry;
	uint64_t e_phoff;
	uint64_t e_shoff;
	uint32_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
} ELF64_Header;

/**
 * Struct containing a 32-bit ELF's header.
 */
typedef struct ELF32_Header {
	ELF_Identifier e_ident;
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint32_t e_entry;
	uint32_t e_phoff;
	uint32_t e_shoff;
	uint32_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
} ELF32_Header;

/**
 * Struct containing one of a 64-bit ELF's section headers.
 */
typedef struct ELF64_SHeader {
	uint32_t sh_name;
	uint32_t sh_type;
	uint64_t sh_flags;
	uint64_t sh_addr;
	uint64_t sh_offset;
	uint64_t sh_size;
	uint32_t sh_link;
	uint32_t sh_info;
	uint64_t sh_addralign;
	uint64_t sh_entsize;
} ELF64_SHeader;

/**
 * Struct containing one of a 32-bit ELF's section headers.
 */
typedef struct ELF32_SHeader {
	uint32_t sh_name;
	uint32_t sh_type;
	uint32_t sh_flags;
	uint32_t sh_addr;
	uint32_t sh_offset;
	uint32_t sh_size;
	uint32_t sh_link;
	uint32_t sh_info;
	uint32_t sh_addralign;
	uint32_t sh_entsize;
} ELF32_SHeader;

/**
 * Struct containing one of an ELF's section headers.
 */
typedef struct ELF_SHeader {
	uint32_t sh_name;
	uint32_t sh_type;
	uint64_t sh_flags;
	uint64_t sh_addr;
	uint64_t sh_offset;
	uint64_t sh_size;
	uint32_t sh_link;
	uint32_t sh_info;
	uint64_t sh_addralign;
	uint64_t sh_entsize;
} ELF_SHeader;

/**
 * Struct containing one of a 64-bit ELF's sections.
 */
typedef struct ELF64_Section {
	ELF64_SHeader sheader;
	char * data;
} ELF64_Section;

/**
 * Struct containing one of a 32-bit ELF's sections.
 */
typedef struct ELF32_Section {
	ELF32_SHeader sheader;
	char * data;
} ELF32_Section;

/**
 * Struct containing one of an ELF's sections.
 */
typedef struct ELF_Section {
	union {
		ELF64_Section * sec64;
		ELF32_Section * sec32;
	};
} ELF_Section;

/**
 * Struct containing a program header for one of a 64-bit ELF's segments.
 */
typedef struct ELF64_PHeader {
	uint32_t p_type;
	uint32_t p_flags;
	uint64_t p_offset;
	uint64_t p_vaddr;
	uint64_t p_paddr;
	uint64_t p_filesz; //Reminder: unlike p_memsz, does not include NOBITS sections
	uint64_t p_memsz;
	uint64_t p_align;
	
	/**
	 * List of sections contained by this memory region.
	 * Set to 0 for type PHDR, which only contains pheaders.
	 */
	ELF64_Section ** sections;
	
	/**
	 * Size of the sections array, or 0 for PHDR segment.
	 */
	int numSections;
} ELF64_PHeader;

/**
 * Struct containing a program header for one of a 32-bit ELF's segments.
 */
typedef struct ELF32_PHeader {
	uint32_t p_type;
	uint32_t p_flags;
	uint32_t p_offset;
	uint32_t p_vaddr;
	uint32_t p_paddr;
	uint32_t p_filesz; //Reminder: unlike p_memsz, does not include NOBITS sections
	uint32_t p_memsz;
	uint32_t p_align;
	
	/**
	 * List of sections contained by this memory region.
	 * Set to 0 for type PHDR, which only contains pheaders.
	 */
	ELF32_Section ** sections;
	
	/**
	 * Size of the sections array, or 0 for PHDR segment.
	 */
	int numSections;
} ELF32_PHeader;

/**
 * Struct containing a 64-bit ELF.
 */
typedef struct ELF64 {
	ELF64_Header header;
	ELF64_Section ** sections;
	ELF64_PHeader ** pheaders;
} ELF64;

/**
 * Struct containing a 32-bit ELF.
 */
typedef struct ELF32 {
	ELF32_Header header;
	ELF32_Section ** sections;
	ELF32_PHeader ** pheaders;
} ELF32;

/**
 * Struct containing an ELF, either 32-bit or 64-bit.
 */
typedef struct ELF {
	union {
		ELF64 elf64;
		ELF32 elf32;
	};
	bool x64;
} ELF;

/**
 * Struct containing a symbol from a 32-bit ELF's symbol table.
 */
typedef struct {
	uint32_t st_name;
	uint32_t st_value;
	uint32_t st_size;
	unsigned char st_info;
	unsigned char st_other;
	uint16_t st_shndx;
} ELF32_Sym;

/**
 * Struct containing a symbol from a 64-bit ELF's symbol table.
 */
typedef struct {
	uint32_t st_name;
	unsigned char st_info;
	unsigned char st_other;
	uint16_t st_shndx;
	uint64_t st_value;
	uint64_t st_size;
} ELF64_Sym;

/**
 * Struct containing a symbol from an ELF's symbol table.
 */
typedef struct {
	uint32_t st_name;
	unsigned char st_info;
	unsigned char st_other;
	uint16_t st_shndx;
	uint64_t st_value;
	uint64_t st_size;
} ELF_Sym;

/**
 * Struct containing a line from a 32-bit ELF's dynamic section.
 */
typedef struct ELF32_Dyn {
	int32_t d_tag;
	union {
		uint32_t d_val;
		uint32_t d_ptr;
	} d_un;
} ELF32_Dyn;

/**
 * Struct containing a line from a 64-bit ELF's dynamic section.
 */
typedef struct ELF64_Dyn {
	int64_t d_tag;
	union {
		uint64_t d_val;
		uint64_t d_ptr;
	} d_un;
} ELF64_Dyn;

/**
 * Struct containing a line from a 32-bit ELF's relocation section.
 */
typedef struct {
	uint32_t r_offset;
	uint32_t r_info;
	int32_t r_addend;
} ELF32_Rela;

/**
 * Struct containing a line from a 64-bit ELF's relocation section.
 */
typedef struct
{
	uint64_t r_offset;
	uint64_t r_info;
	int64_t r_addend;
} ELF64_Rela1;

/**
 * Struct containing a line from a 64-bit ELF's relocation section.
 * It doesn't seem like I used this struct anywhere?
 */
typedef struct {
	uint64_t r_offset;
	uint32_t r_sym;
	uint8_t r_ssym;
	uint8_t r_type3;
	uint8_t r_type2;
	uint8_t r_type;
	int64_t r_addend;
} ELF64_Rela2;

/**
 * Struct containing a line from a 64-bit ELF's relocation section.
 * It doesn't seem like I used this struct anywhere?
 */
typedef struct {
	uint64_t r_offset;
	uint8_t r_type;
	uint8_t r_type2;
	uint8_t r_type3;
	uint8_t r_ssym;
	uint32_t r_sym;
	int64_t r_addend;
} ELF64_Rela3;

/**
 * Encode an ELF into data structures.
 * @param elfbytes The entire ELF as a byte array.
 * @return a #ELF encoding the given ELF.
 */
ELF * bytes2ELF(const char * elfbytes);

/**
 * Convert an ELF to a byte array.
 * @param elf The #ELF64 struct which encodes the ELF.
 * @param size A by-reference integer to return the number of bytes.
 * @return a char array which, if written to file, results in the same ELF.
 */
char * ELF2bytes(ELF * elf, uint64_t & size);

/**
 * Returns the raw data for an ELF section.
 * @param elf
 * @param section
 * @return a const char array
 */
const char * getSectionData(ELF * elf, const ELF_Section section);

/**
 * Replace the data for an ELF section.
 * If section has NOBITS type, it is resized; data parameter is ignored.
 * @param elf The ELF.
 * @param sectionID The index of the section.
 * @param data The new data; a duplicate of this array is used.
 * @param dataSize The size of the new data.
 * @param oldElfEnd When modifying CUDA, the original address of the end of the modified ELF.
 */
void replaceSectionData(ELF * elf, int sectionID, char * data, unsigned int dataSize, unsigned int oldElfEnd = 0);

/**
 * Returns the number of sections in an #ELF.
 * @param elf The ELF.
 * @return an int
 */
int getNumSections(ELF * elf);

/**
 * Returns the number of segments (program headers) in an #ELF.
 * @param elf The ELF.
 * @return an int
 */
int getNumSegments(ELF * elf);

/**
 * Gets an ELF section by index.
 * @param elf The #ELF containing the section.
 * @param index The index of the section.
 * @pre There are more than index sections in the ELF.
 */
ELF_Section getSection(ELF * elf, int index);

/**
 * Gets a (read-only) symbol from a symbol table section in an elf.
 * @param elf
 * @param section
 * @param index
 * @return an #ELF_Sym
 */
const ELF_Sym getSymbol(ELF * elf, const ELF_Section section, int index);

/**
 * Gets the header for an ELF section.
 * @param elf
 * @param section The #ELF_Section
 * @return A read-only ELF_SHeader
 */
const ELF_SHeader getHeader(ELF * elf, const ELF_Section section);

/**
 * Returns the name of the given section.
 * @param elf The elf containing the section.
 * @param section The section for which the name is bring retrieved.
 * @return a char pointer to somehwere in the elf's string table.
 */
const char * getName(ELF * elf, const ELF_Section section);

/**
 * Returns the name of the given symbol.
 * @param elf The elf containing the symbol.
 * @param symtab The header for the symtab section containing the symbol.
 * @param sym The symbol for which the name is bring retrieved.
 * @return a char pointer to somehwere in the elf's string table.
 */
const char * getName(ELF * elf, ELF_SHeader symtab, const ELF_Sym sym);

/**
 * Adds a new symbol with specified name to a symbol table section in an ELF.
 * The st_name value is automatically set.
 * @param elf
 * @param sec
 * @param symbol
 * @param name
 */
void addSymbol(ELF * elf, const ELF_Section sec, ELF_Sym symbol, char * name);

/**
 * Adds a new section with specified name to an ELF.
 * The sh_addr, sh_offset, and sh_name values are automatically set.
 * @param elf
 * @param shdr
 * @param name
 * @param data
 */
void addSection(ELF * elf, ELF_SHeader shdr, char * name, char * data);

/**
 * Adds an existing section to an existing segment in an ELF.
 * @param elf
 * @param section
 * @param segment
 */
void addSectionToSegment(ELF * elf, const ELF_Section section, int segment);

/**
 * Sets the sh_info attribute for an ELF section.
 * @param elf
 * @param section
 * @param info
 */
void setSH_info(ELF * elf,  const ELF_Section section, uint32_t info);

/**
 * Sets the st_size attribute for an ELF symbol.
 * @param elf
 * @param symtab
 * @param index The index of the symbol inside its section
 * @param size
 */
void setST_size(ELF * elf,  const ELF_Section symtab, int index, uint64_t size);

/**
 * Sets the st_value attribute for an ELF symbol.
 * @param elf
 * @param symtab
 * @param index The index of the symbol inside its section
 * @param value
 */
void setST_value(ELF * elf,  const ELF_Section symtab, int index, uint64_t value);

/**
 * Frees memory in use by an #ELF struct.
 * @param elf
 */
void cleanELF(ELF * elf);

#endif

