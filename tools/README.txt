========================
=====THIS DIRECTORY=====
========================
This directory contains source code for our assembler generator tool, plus related tools.
We hope to continue updating this, such as to add support for the Volta generation of NVIDIA devices.

========================
=======OUR TOOLS========
========================
The bin2asm tool extract kernel functions' assembly code from nvcc-generated executables.
The asm2bin tool can assemble assembly code for several architectures, overwriting binary in an executable.
The decode tool can generate assembler code for use with the asm2bin tool.

If writing functions to analyze/modify GPU code, we recommend calling them in asm2bin.
There's a line in asm2bin.ypp that says "//This is where we might call functions to modify or optimize the GPU code."

========================
======DEPENDENCIES======
========================
Our tools are all compiled with g++.
The asm2bin and decode tools are dependent on flex and bison.
The bin2asm tool expects that the CUDA Toolkit be installed and part of your PATH.

========================
=====COMPATIBILITY======
========================
The asm2bin and decode tools should be run on a Linux machine for full compatibility.
The decode tool has been tested with compute capabilities between 3.2 and 6.2, inclusive.

========================
======COMPILATION=======
========================
Our tools can be compiled by running make in this directory.

========================
=COMMAND LINE ARGUMENTS=
========================
Run any of the three tools with the -h flag for some details on runtime arguments.
See separate readme files for more info about the distinct tools: bin2asm, asm2bin, and decode.

========================
=====DOCUMENTATION======
========================
Documentation can be generated with Doxygen.
