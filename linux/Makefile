converter: main.c convert.h convert.o
	gcc main.c convert.h convert.o -o converter
convert.o: convert.asm
	nasm -felf64 convert.asm