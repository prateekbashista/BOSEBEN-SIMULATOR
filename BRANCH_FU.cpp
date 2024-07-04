#include "includes.h"

uint32_t imm12(REG insn)
{
    uint32_t imm;
    imm = (((insn >> 31) & 1) << 12) |
          (((insn >> 7) & 1) << 11) |
          (((insn >> 25) & 0x3F) << 4) |
          ((insn >> 8) & 0x1F);

    return imm;
}
    

void int_fu(WORD insn, REG PC, WORD r1_data, WORD r2_data, WORD *result)
{
    switch(opcode(insn))
    {
        case BEQ :
        {
            if((int)r1_data == (int)r2_data)
            {
                *result = PC + imm12(insn);
            }
            break;
        }
        case BNE :
        {
            if((int)r1_data != (int)r2_data)
            {
                *result = PC + imm12(insn);
            }
            break;
        }
        case BLT :
        {
            if((int)r1_data < (int)r2_data)
            {
                *result = PC + imm12(insn);
            }
            break;
        }
        case BGE : 
        {
            if((int)r1_data >= (int)r2_data)
            {
                *result = PC + imm12(insn);
            }
            break;
        }
        case BLTU :
        {
            if(r1_data < r2_data)
            {
                *result = PC + imm12(insn);
            }
            break;
        }
        case BGEU :
        {
            if(r1_data >= r2_data)
            {
                *result = PC + imm12(insn);
            }
            break;
        }
        
    }
}