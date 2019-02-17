========================
========BIN2ASM=========
========================

Our bin2asm program is used to extract assembly code and some metadata, using NVIDIA's cuobjdump tool for assistance.

Here are some examples of how to run the program, and brief descriptions of the results.

./bin2asm a.out
This outputs a list of CUDA kernel functions in a.out, pre-pended with unique ID numbers for identification.

./bin2asm a.out 3
This outputs the assembly code of the kernel with ID number 3.

./bin2asm -hex a.out 3
This outputs the assembly code of the kernel with ID number 3, including binary code.
This format is necessary when gathering assembly&binary for our decode tool.