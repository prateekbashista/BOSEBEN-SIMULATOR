#include "decode_riscv32im.h"

void decoder(WORD insn,BYTE *r1_sel, BYTE *r1_re, BYTE *r2_sel, BYTE *r2_re, 
                BYTE *wsel, BYTE *regfile_we, BYTE *is_load, BYTE *is_store, 
                BYTE *is_branch, BYTE *is_control, BYTE *is_system)
{   
    switch(opcode(insn))
    {
        case LUI: 
        {   
            *r1_re = 0;
            *r2_re = 0;
            *wsel = rd_addr(insn);
            *regfile_we = 1;
            *is_load = 0;
            *is_store = 0;
            *is_branch = 0;
            *is_control = 0;
            *is_system = 0;
            break;
        } 
        case AUIPC:
        {   
            *r1_re = 0;
            *r2_re = 0;
            *wsel = rd_addr(insn);
            *regfile_we = 1;
            *is_load = 0;
            *is_store = 0;
            *is_branch = 0;
            *is_control = 0;
            *is_system = 0;
            break;
        } 
        case JAL:
        {   
            *r1_re = 0;
            *r2_re = 0;
            *wsel = rd_addr(insn);
            *regfile_we = 1;
            *is_load = 0;
            *is_store = 0;
            *is_branch = 0;
            *is_control = 1;
            *is_system = 0;
            break;
        }
        case JALR:
        {   
            *r1_re = 1;
            *r1_sel = rs1_addr(insn);
            *r2_re = 0;
            *wsel = rd_addr(insn);
            *regfile_we = 1;
            *is_load = 0;
            *is_store = 0;
            *is_branch = 0;
            *is_control = 1;
            *is_system = 0;
            break;
        }  
        case BRANCH :
        {
            *r1_re = 1;
            *r1_sel = rs1_addr(insn);
            *r2_re = 1;
            *r2_sel = rs2_addr(insn);
            *regfile_we = 0;
            *is_load = 0;
            *is_store = 0;
            *is_branch = 1;
            *is_control = 0;
            *is_system = 0;
            break;
        }
        case LOAD :
        {
            *r1_re = 1;
            *r1_sel = rs1_addr(insn);
            *r2_re = 0;
            *wsel = rd_addr(insn);
            *regfile_we = 1;
            *is_load = 1;
            *is_store = 0;
            *is_branch = 0;
            *is_control = 0;
            *is_system = 0;
            break;
        }
        case STORE :
        {
            *r1_re = 1;
            *r1_sel = rs1_addr(insn);
            *r2_re = 1;
            *r2_sel = rs2_addr(insn);
            *regfile_we = 0;
            *is_load = 0;
            *is_store = 1;
            *is_branch = 0;
            *is_control = 0;
            *is_system = 0;
            break;
        }
        case IMM_INSN :
        {
            *r1_re = 1;
            *r1_sel = rs1_addr(insn);
            *r2_re = 0;
            *wsel = rd_addr(insn);
            *regfile_we = 1;
            *is_load = 0;
            *is_store = 0;
            *is_branch = 0;
            *is_control = 0;
            *is_system = 0;
            break;
        }
        case RTYPE_INSN : 
        {   
            
            *r1_re = 1;
            *r1_sel = rs1_addr(insn);
            *r2_re = 1;
            *r2_sel = rs2_addr(insn);
            *regfile_we = 1;
            *wsel = rd_addr(insn);
            *is_load = 0;
            *is_store = 0;
            *is_branch = 0;
            *is_control = 0;
            *is_system = 0;
            break;
        }
        case SYSTEM_INSN :
        {
            switch(funct3(insn))
            {
                case CSRRW | CSRRS | CSRRC :
                {
                    *r1_re = 1;
                    *r1_sel = rs1_addr(insn);
                    *r2_re = 0;
                    *wsel = rd_addr(insn);
                    *regfile_we = 1;
                    *is_load = 0;
                    *is_store = 0;
                    *is_branch = 0;
                    *is_control = 0;
                    *is_system = 1;
                    break;
                }
                case CSRRWI | CSRRSI | CSRRCI : 
                {
                    *r1_re = 0;
                    *r2_re = 0;
                    *wsel = rd_addr(insn);
                    *regfile_we = 1;
                    *is_load = 0;
                    *is_store = 0;
                    *is_branch = 0;
                    *is_control = 0;
                    *is_system = 1;
                    break;
                }
                default : 
                {
                    *r1_re = 0;
                    *r2_re = 0;
                    *regfile_we = 0;
                    *is_load = 0;
                    *is_store = 0;
                    *is_branch = 0;
                    *is_control = 0;
                    *is_system = 1;
                    break;
                }

            }
            break;
        }
        default :
            std::cout<<"\n INSTRUCTION NOT SUPPORTED IN CORE"; break;
    }
}