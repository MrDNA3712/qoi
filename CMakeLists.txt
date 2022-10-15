cmake_minimum_required (VERSION 3.8)
if (unix)
    set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
    set(ASM_SOURCE linux/convert.asm)
else()
    set(CMAKE_ASM_NASM_OBJECT_FORMAT win64)
    set(ASM_SOURCE win/convert.asm)
endif()

project ("qoi_converter")

enable_language(ASM_NASM)
set(CMAKE_NASM_LINK_EXECUTABLE CMAKE_C_LINK_EXECUABLE)
add_executable (qoi_converter main.c ${ASM_SOURCE})
