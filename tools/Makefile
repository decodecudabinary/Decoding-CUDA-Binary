CC = @g++
LEXER = @flex
PARSER = @bison

CFLAGS = -Isrc -O3 -Wall
rm = @rm
OBJs = objs/asm2bin.tab.o \
	objs/asm2bin.yy.o \
	objs/binary.o \
	objs/binary20.o \
	objs/binary35.o \
	objs/binary50.o \
	objs/cfghelpers.o \
	objs/common.o \
	objs/cudacommon.o \
	objs/elf.o \
	objs/elfmanip.o \
	objs/output.o
ELFOBJs = objs/elf.o \
	objs/elfmanip.o
DECODEOBJS = objs/decode.tab.o \
	objs/decode.yy.o \
	objs/decode_common.o \
	objs/decode_output.o

ifeq ($(OS),Windows_NT)
	rm = @del
	CFLAGS += -D WINDOWS
	MKDIR_FLAGS = ""

	cuobjdump_location := $(shell where cuobjdump)
	ifneq ($(cuobjdump_location),)
		CFLAGS += -D HAS_CUOBJDUMP
	endif
else
	MKDIR_FLAGS = -p
	UNAME_S := $(shell uname -s)
	UNAME_P := $(shell uname -p)

	cuobjdump_location := $(shell which cuobjdump)
	ifneq ($(cuobjdump_location),)
		CFLAGS += -D HAS_CUOBJDUMP
	endif

	ifeq ($(UNAME_S),Linux)
		CFLAGS += -D LINUX
		ifeq ($(UNAME_P),x86_64)
			CFLAGS += -D AMD64
			CFLAGS += -D LINUX64
		endif
	else ifeq ($(UNAME_P),x86_64)
		CFLAGS += -D AMD64
	endif
endif

default: objs bin2asm asm2bin decode

debug: CFLAGS += -g -O0
debug: default

objs: 
	@mkdir ${MKDIR_FLAGS} "objs"

objs/asm2bin.yy.o: src/asm2bin.l src/common.hpp
	${LEXER} -oobjs/asm2bin.yy.c src/asm2bin.l 
	${CC} ${CFLAGS} -c objs/asm2bin.yy.c -o $@

objs/asm2bin.tab.o: src/asm2bin.ypp src/asm2bin.hpp src/cfghelpers.hpp src/common.hpp
	${PARSER} -oobjs/asm2bin.tab.cpp -dv src/asm2bin.ypp
	${CC} ${CFLAGS} -c objs/asm2bin.tab.cpp -o $@

objs/binary.o: src/binary*.cpp src/binary*.hpp src/common.hpp
	${CC} ${CFLAGS} -c src/binary.cpp -o $@

objs/binary20.o: src/binary20.cpp src/binary20.hpp src/binary.hpp src/common.hpp
	${CC} ${CFLAGS} -c src/binary20.cpp -o $@

objs/binary35.o: src/binary35.cpp src/binary35.hpp src/binary.hpp src/common.hpp
	${CC} ${CFLAGS} -c src/binary35.cpp -o $@

objs/binary50.o: src/binary50.cpp src/binary50.hpp src/binary.hpp src/common.hpp
	${CC} ${CFLAGS} -c src/binary50.cpp -o $@

objs/cfghelpers.o: src/cfghelpers.hpp src/cfghelpers.cpp src/common.hpp src/cudacommon.hpp src/output.hpp
	${CC} ${CFLAGS} -c src/cfghelpers.cpp -o $@

objs/common.o: src/common.cpp src/common.hpp
	${CC} ${CFLAGS} -c src/common.cpp -o $@

objs/cudacommon.o: src/cudacommon.cpp src/cfghelpers.hpp src/cudacommon.hpp src/common.hpp
	${CC} ${CFLAGS} -c src/cudacommon.cpp -o $@

objs/elf.o: src/elf.cpp src/elf.hpp
	${CC} ${CFLAGS} -c src/elf.cpp -o $@

objs/elfmanip.o: src/elfmanip.cpp src/elfmanip.hpp src/common.hpp src/elf.hpp
	${CC} ${CFLAGS} -c src/elfmanip.cpp -o $@

objs/output.o: src/output.cpp src/output.hpp src/common.hpp src/cfghelpers.hpp src/cudacommon.hpp
	${CC} ${CFLAGS} -c src/output.cpp -o $@

bin2asm: ${ELFOBJs} objs/common.o src/elf2asm.cpp
	${CC} ${CFLAGS} ${ELFOBJs} objs/common.o src/elf2asm.cpp -o bin2asm ${ELFLinks}

asm2bin: ${OBJs}
	${CC} ${CFLAGS} ${OBJs} -o asm2bin ${ELFLinks}


objs/decode.yy.o: src/decode.l src/decode_common.hpp
	${LEXER} -oobjs/decode.yy.c src/decode.l
	${CC} ${CFLAGS} -c objs/decode.yy.c -o $@

objs/decode.tab.o: src/decode.ypp src/decode.hpp src/decode_common.hpp
	${PARSER} -oobjs/decode.tab.cpp -dv src/decode.ypp
	${CC} ${CFLAGS} -c objs/decode.tab.cpp -o $@

objs/decode_common.o: src/decode_common.cpp src/decode_common.hpp
	${CC} ${CFLAGS} -c src/decode_common.cpp -o $@

objs/decode_output.o: src/decode_output.cpp src/decode_output.hpp src/decode_common.hpp
	${CC} ${CFLAGS} -c src/decode_output.cpp -o $@

decode: ${DECODEOBJS}
	${CC} ${CFLAGS} ${DECODEOBJS} -o $@

clean:
	${rm} -f asm2bin asm2bin.exe bin2asm bin2asm.exe decode decode.exe *.o .temp*
	${rm} -f objs/*

