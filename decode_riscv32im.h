#ifndef _DECODER_
#define _DECODER_

#include "includes.h"

void decoder(WORD insn,BYTE *r1_sel, BYTE *r1_re, BYTE *r2_sel, BYTE *r2_re, 
                BYTE *wsel, BYTE *regfile_we, BYTE *is_load, BYTE *is_store, 
                BYTE *is_branch, BYTE *is_control, BYTE *is_system, BYTE *is_link)
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

            std::cout<< "\n LUI \n";
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
            std::cout<< "\n AUIPC \n";
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

            std::cout<< "\n JAL \n";
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

            std::cout<< "\n JALR\n";
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

            std::cout<< "\n BRANCH\n";
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

            std::cout<< "\n LOAD\n";
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

            std::cout<< "\n STORE\n";
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

            std::cout<< "\n IMM_INSN\n";
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

            std::cout<< "\n RTYPE\n";
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

                    std::cout<< "\n CSRRW | CSRRS | CSRRC\n";
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

                    std::cout<< "\n CSRRWI | CSRRSI | CSRRCI\n";
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
            std::cout<<"\n INSTRUCTION NOT SUPPORTED IN CORE\n"; break;
    }
}

#endif