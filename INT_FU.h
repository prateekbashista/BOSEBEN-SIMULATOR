#ifndef _INTFU_
#define _INTFU_

#include "includes.h"

uint32_t sext(REG insn,uint32_t amt)
{
    uint32_t mask; 
    mask  = ((insn & 0x80000000) >> 31) ? 0xFFFFFFFF : 0; 
    return ((insn >> amt) | ( mask << amt));
}
    

void int_fu(WORD insn, REG PC, WORD r1_data, WORD r2_data, WORD *result)
{
    switch(opcode(insn))
    {
        case LUI :
        {
            *result = insn & 0xFFFFF000;
            break;
        }
        case AUIPC :
        {
            *result = PC + (insn & 0xFFFFF000);
            break;
        }
        case JAL :
        {
            *result = PC + ((insn >>20) & 1) | (insn >> 1) & 0x3FF | 
                            (insn >> 12) & 0xFF;

            break;
        }
        case JALR : 
        {
            *result = r1_data + (insn >> 20) & 0xFFF;
            break;
        }
        case LOAD :
        {
            *result = r1_data + (insn >> 20);
            break;
        }
        case STORE :
        {
            *result = r1_data + (((insn >> 25) << 5) | ((insn >> 7) & 0x1F));
            break;
        }
        case IMM_INSN :
        {
            switch(funct3(insn))
            {
                case ADDI :
                {   
                    std::cout<<"\nOp1 : "<<r1_data<<std::endl;
                    std::cout<<"\nOp2 : "<<(int)sext(insn,20)<<std::endl;
                    *result = r1_data + sext(insn,20);
                    break;
                }
                case SLTI :
                {
                    *result = (sext(insn,20) > r1_data) ? 1:0;
                    break;
                }
                case SLTIU :
                {
                    *result = ((insn >> 20) > r1_data) ? 1 : 0;
                    break;
                }
                case XORI :
                {
                    *result = r1_data ^ sext(insn,20);
                    break;
                }
                case ORI :
                {
                    *result = r1_data | sext(insn,20);
                    break;
                }       
                case ANDI :
                {
                    *result = r1_data & sext(insn,20);
                    break;
                }         
                case SLLI :
                {
                    *result = r1_data << ((insn >> 20) & 0x1F);
                    break;
                }
                case 0x5 :
                {
                    *result = (funct7(insn) == 0) ? r1_data >> ((insn >> 20) & 0x1F):
                                sext(r1_data , ((insn >> 20) & 0x1F)) ;
                    break;
                }
                
            }
            break;
        }
        case RTYPE_INSN :
        {
            switch(funct3(insn))
            {
                case 0x0 :
                {
                    *result = (funct7(insn) == 0) ? r1_data + r2_data :
                                r1_data - r2_data;

                    break;
                }
                case SLL :
                {
                    *result = r1_data << r2_data;
                    break;
                }
                case SLT :
                {
                    *result = (r1_data < r2_data) ? 1 : 0;
                    break;
                }
                case SLTU :
                {
                    *result = (r1_data < r2_data) ? 1 : 0;
                    break;
                }
                case XOR :
                {
                    *result = r1_data ^ r2_data;
                    break;
                }
                case 0x5 :
                {
                    *result = (funct7(insn) == 0) ? r1_data >> (r2_data):
                                sext(r1_data , r2_data) ;
                    break;
                }
                case OR :
                {
                    *result = r1_data | r2_data;
                    break;
                }
                case AND :
                {
                    *result = r1_data & r2_data;
                    break;
                }
            }
            break;
        }
    }
}

#endif