//#include "includes.h"
#include "decode_riscv32im.h"
#include <cstdint>

class CPU
{   
    REG PC;

    // REG X0 - X31
    REG X[32]; // Architectural Register File

    BYTE N : 1; // NEGATIVE FLAG
    BYTE Z : 1; // ZERO FLAG
    BYTE P : 1; // POSITIVE FLAG

    public:
    void reset()
    {
        PC = 0;
        N = 0;
        P = 0;
        Z = 0;
        //X[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
    }
};

struct decode_out{
    BYTE r1_sel;
    BYTE r1_re; 
    BYTE r2_sel; 
    BYTE r2_re; 
    BYTE wsel; 
    BYTE regfile_we; 
    BYTE is_load; 
    BYTE is_store; 
    BYTE is_branch; 
    BYTE is_control;
    BYTE is_system;
};

int main(int argc, char **argv)
{
    decode_out dec1;
    WORD insn = 0x2B7; // add x0, x1, x0
    
    decoder(insn, &dec1.r1_sel, &dec1.r1_re, &dec1.r2_sel, &dec1.r2_re, 
                &dec1.wsel, &dec1.regfile_we, &dec1.is_load, &dec1.is_store, 
                &dec1.is_branch, &dec1.is_control, &dec1.is_system);
    

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

