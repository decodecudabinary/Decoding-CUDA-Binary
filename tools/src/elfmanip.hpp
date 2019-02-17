#ifndef ELFMANIP_HPP
#define ELFMANIP_HPP
#include "common.hpp"

/**
 * @file elfmanip.hpp
 * Defines structures and functions used for reading/writing to ELF.
 */

/**
 * This structure can hold the first 0x10 bytes of the .nv_fatbin section.
 * After <size> bytes, either the section ends, or another fatHeader is found.
 */
typedef struct {
	unsigned int magic;
	unsigned int unknown;
	unsigned long long size;
} fatHeader;

/**
 * This structure holds a CUDA_ELF object inside an .nv.info* section.
 */
typedef struct {
	char format;
	char attribute;
	char data[];
} CUDA_INFO;

/**
 * Enum for attribute IDs in CUDA_INFO sections
 */
typedef enum {
	EIATTR_ERROR = 0x00,
	EIATTR_PAD = 0x01,
	EIATTR_IMAGE_SLOT = 0x2,
	EIATTR_JUMPTABLE_RELOCS = 0x3,
	EIATTR_CTAIDZ_USED = 0x4,
	EIATTR_MAX_THREADS = 0x5,
	EIATTR_IMAGE_OFFSET = 0x6,
	EIATTR_IMAGE_SIZE = 0x07,
	EIATTR_TEXTURE_NORMALIZED = 0x08,
	EIATTR_SAMPLER_INIT = 0x09,
	EIATTR_PARAM_CBANK = 0x0a,
	EIATTR_SMEM_PARAM_OFFSETS = 0x0b,
	EIATTR_CBANK_PARAM_OFFSETS = 0x0c,
	EIATTR_SYNC_STACK = 0x0d,
	EIATTR_TEXID_SAMPID_MAP = 0x0e,
	EIATTR_EXTERNS = 0x0f,
	EIATTR_REQNTID = 0x10,
	
	/**
	 * Seems to indicate local-memory stack size.
	 * After two byte size (equal to 0x8), has 4byte function ID, then 4byte frame size.
	 * Function ID is based on index in symbol table.
	 */
	EIATTR_FRAME_SIZE = 0x11,
	
	/**
	 * Seems to indicate local-memory stack size.
	 * After two byte size (equal to 0x8), has 4byte function ID, then 4byte stack size.
	 * Function ID is based on index in symbol table.
	 */
	EIATTR_MIN_STACK_SIZE = 0x12,
	
	EIATTR_SAMPLER_FORCE_UNNORMALIZED = 0x13,
	EIATTR_BINDLESS_IMAGE_OFFSETS = 0x14,
	EIATTR_BINDLESS_TEXTURE_BANK = 0x15,
	EIATTR_BINDLESS_SURFACE_BANK = 0x16,
	EIATTR_KPARAM_INFO = 0x17, //I think this describes a parameter for a kernel function
	EIATTR_SMEM_PARAM_SIZE = 0x18,
	EIATTR_CBANK_PARAM_SIZE = 0x19,
	EIATTR_QUERY_NUMATTRIB = 0x1a,
	EIATTR_MAXREG_COUNT = 0x1b,
	EIATTR_EXIT_INSTR_OFFSETS = 0x1c,
	EIATTR_S2RCTAID_INSTR_OFFSETS = 0x1d,
	
	//error "unknown attribute" for 0x1e and above as of cuobjdump 5.0
	
	EIATTR_CRS_STACK_SIZE = 0x1e,
	EIATTR_NEED_CNP_WRAPPER = 0x1f,
	EIATTR_NEED_CNP_PATCH = 0x20,
	EIATTR_EXPLICIT_CACHING = 0x21,
	
	//error "unknown attribute" for 0x22 and above as of cuobjdump 5.5
	
	EIATTR_ISTYPEP_USED = 0x22,
	
	//error "unknown attribute" for 0x23 and above as of cuobjdump 6.0
	
	EIATTR_MAX_STACK_SIZE = 0x23, //Another attribute that indicates local-memory stack size.
	EIATTR_SUQ_USED = 0x24,
	EIATTR_LD_CACHEMOD_INSTR_OFFSETS = 0x25,
	
	//error "unknown attribute" for 0x26 and above as of cuobjdump 6.5
	
	EIATTR_LOAD_CACHE_REQUEST = 0x26,
	EIATTR_ATOM_SYS_INSTR_OFFSETS = 0x27,
	
	//error "unknown attribute" for 0x28 and above as of cuobjdump 8.0
	
	EIATTR_COOP_GROUP_INSTR_OFFSETS = 0x28,
	EIATTR_COOP_GROUP_MASK_REGIDS = 0x29,
	EIATTR_SW1850030_WAR = 0x2a,
	EIATTR_WMMA_USED = 0x2b,
	
	//error "unknown attribute" for 0x2c and above as of cuobjdump 9.2
	
	//Constants to help with error checking:
	minAttribute = 0x00,
	maxAttribute = 0x2b
} Attribute;

/**
 * Enum for 'Format' values in CUDA_INFO
 */
typedef enum {
	//error "unknown Format" for 0x00
	
	/**
	 * I haven't seen the following value occur naturally.
	 * Object's data is of size 0.
	 * Speculation: NVAL stands for "no value" or "null value" or similar.
	 */
	EIFMT_NVAL = 0x01,
	
	/**
	 * I haven't seen the following value occur naturally.
	 * Object's data is of size 1.
	 * Speculation: BVAL stands for "byte value".
	 */
	EIFMT_BVAL = 0x02,
	
	/**
	 * I haven't seen the following value occur naturally.
	 * Object's data is of size 2.
	 * Speculation: HVAL stands for "half-int value" or something similar.
	 */
	EIFMT_HVAL = 0x03,
	
	/**
	 * This is the value (usually?) used in naturally compiled CUDA binaries.
	 * Object's data starts with a two-byte value indicating number of additional bytes.
	 * (I suspect the size handling may vary depending on the attribute type.)
	 * Speculation: SVAL stands for "(variable-)size value", or perhaps "special value".
	 */
	EIFMT_SVAL = 0x04,
	
	//error "unknown Format" for 0x05 and above as of cuobjdump 5.0 through 8.0
	
	//Constants to help with error checking:
	minFormat = 0x01,
	maxFormat = 0x04
} Format;

/**
 * Overwrites a kernel function inside an ELF.
 * @param file The name of the executable ELF
 * @param kernel Contains the name of the function and the new bytecode
 * @param calls A list of function offsets
 * @param forceFallback Iff true, fallback function is used even on proper ELF files.
 * @return true iff the kernel is successfully overwritten
 */
bool overwriteKernel(char* file, kernelData * kernel, node * calls, bool forceFallback);

/**
 * Helper for overwriteKernel.
 * @param oldData The address of array containing nv_fatbin data
 * @oaram newData
 * @param original_size The number of bytes in oldData
 * @param kernel Contains the kernel function's name & the new bytecode
 * @param calls A list of function offsets
 * @param targetFatOffset Is set to the offset (relative to oldData) of header for fatbin containing target ELF.
 * @param oldEndELF Is set to the offset (relative to oldData) of the end of the target ELF prior to modification.
 * @return the size of the new nv_fatbin data, or -1 on failure
 */
int overwriteKernelHelper(const char * oldData, char *& newData, unsigned int original_size, kernelData * kernel, node * calls, unsigned int & targetFatOffset, unsigned int & oldEndELF);

/**
 * Returns contents of a specified kernel function.
 * @param file The name of the executable ELF
 * @param kernel The name of the kernel function
 * @param kernelNumber The index of the kernel function; only used if kernel is NULL
 * @param forceFallback Iff true, fallback function is used even on proper ELF files.
 * @return a kernelData pointer, or 0 on failure
 */
kernelData* getKernelFunction(char * file, char * kernel, int kernelNumber, bool forceFallback);

/**
 * Helper for getKernelFunction.
 * @param bytes The data in the nv_fatbin section
 * @param size The number of bytes in the nv_fatbin section
 * @param kernel The name of the kernel function
 * @param kernelNumber The index of the kernel function; only used if kernel is NULL
 * @return a kernelData pointer, or 0 on failure
 */
kernelData* getKernelFunctionHelper(const char * bytes, unsigned int size, char * kernel, int kernelNumber);

/**
 * Returns a list of kernel function names within the specified ELF.
 * @param file is the location of the executable ELF
 * @param forceFallback Iff true, fallback function is used even on proper ELF files.
 * @return a list of char*
 */
node * getKernelNames(char * file, bool forceFallback);

/**
 * Helper for getKernelNames.
 * Returns a list of kernel function names from within the fatbin section
 * @param bytes The data in the nv_fatbin section
 * @param size The number of bytes in nv_fatbin
 * @return a list of char*
 */
node * getKernelNamesHelper(const char * bytes, int size);

#endif

