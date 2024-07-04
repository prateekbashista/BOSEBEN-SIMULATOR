//#include "includes.h"
#include "decode_riscv32im.h"
#include <cstdint>


struct CPU_STAGE
{
    // General Tempelate in for input of the all CPU STAGES

    REG PC; // pc of the instruction being operated on
    WORD insn;
    BYTE opcode, funct3op, funct7op;
    BYTE r1_re, r2_re; // Register Read Enable
    BYTE r1_sel; // R1 Address
    BYTE r2_sel; // R2 Address
    BYTE regfile_we; // Write Enable
    BYTE wsel;// Write Address
    BYTE is_link; // Link address needed
    BYTE link_reg; // Link register address

    // Flags
    BYTE is_load; // Is instruction a load
    BYTE is_store; // Is intruction a store
    BYTE is_branch; // Is instruction a branch
    BYTE is_control; // Is instruction a control instruction like jump
    BYTE is_system; // Is instruction from CSR, Priviledge mode support

};

struct MEMORY
{
    WORD data_mem[4096];
    WORD insn_mem[256];

};

struct CPU
{   
    REG PC;


    // Stages of the CPU
    CPU_STAGE *fetch_op, *decode_op, *dispatch_op, *issue_op, *memory_op, *commit_op; 

    // REG X0 - X31
    REG X[32]; // Architectural Register File

    BYTE O : 1; // OVERFLOW FLAG
    BYTE N : 1; // NEGATIVE FLAG
    BYTE Z : 1; // ZERO FLAG
    BYTE P : 1; // POSITIVE FLAG

    void reset()
    {
        PC = 0;
        N = 0;
        P = 0;
        Z = 0;
        
        for(int i = 0; i<32; i++)
        {
            X[i] = 0;
        } //Reg Reset

        fetch_op = decode_op = dispatch_op = issue_op = memory_op = commit_op = nullptr;
    }
};




int main(int argc, char **argv)
{

    

    std::cout<<"\n add x0, x1, x0";
    std::cout<<"\n R1_SEL : "<<static_cast<int>(dec1.r1_sel);
    std::cout<<"\n R1_RE : "<<static_cast<int>(dec1.r1_re);
    std::cout<<"\n R2_SEL : "<<static_cast<int>(dec1.r2_sel);
    std::cout<<"\n R2_RE : "<<static_cast<int>(dec1.r2_re);
    std::cout<<"\n WSEL : "<<static_cast<int>(dec1.wsel);
    std::cout<<"\n REGFILE_ENBLE : "<<static_cast<int>(dec1.regfile_we);
    std::cout<<"\n IS_LOAD : "<<static_cast<int>(dec1.is_load);
    std::cout<<"\n IS_STORE : "<<static_cast<int>(dec1.is_store);
    std::cout<<"\n IS_BRANCH : "<<static_cast<int>(dec1.is_branch);
    std::cout<<"\n IS_CONTROL : "<<static_cast<int>(dec1.is_branch);
    std::cout<<"\n IS_SYSTEM : "<<static_cast<int>(dec1.is_system);
}

