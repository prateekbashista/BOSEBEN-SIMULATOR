#ifndef _INSN_MEM_INIT_
#define _INSN_MEM_INIT_

#include "includes.h"

void memory_initialiser()
{
    std::ifstream infile;
    infile.open("riscv_asm.s", std::ios::in);
}
#endif