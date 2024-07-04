#ifndef _INCLUDES_
#define _INCLUDES_

#include "datatypes.h"

/*************INSTRUCTION DECLARATION***********/
#define LUI (BYTE)(0x37)
#define AUIPC (BYTE)(0x17)
#define JAL (BYTE)(0x6F)
#define JALR (BYTE)(0x67)
#define BRANCH (BYTE)(0x63) // Branch Instructions
#define BEQ (BYTE)(0x0)
#define BNE (BYTE)(0x1)
#define BLT (BYTE)(0x4)
#define BGE (BYTE)(0x5)
#define BLTU (BYTE)(0x6)
#define BGEU (BYTE)(0x7)
#define LOAD (BYTE)(0x03) // Load Instructions
#define LB (BYTE)(0x0)
#define LH (BYTE)(0x1)
#define LW (BYTE)(0x2)
#define LBU (BYTE)(0x4)
#define LHU (BYTE)(0x5)
#define STORE (BYTE)(0x23) // Store Instructions
#define SB (BYTE)(0x0)
#define SH (BYTE)(0x1)
#define SW (BYTE)(0x2)
#define IMM_INSN (BYTE)(0x13) // I TYPE Instructions
#define ADDI (BYTE)(0x0)
#define SLTI (BYTE)(0x2)
#define SLTIU (BYTE)(0x3)
#define XORI (BYTE)(0x4)
#define ORI (BYTE)(0x6)
#define ANDI (BYTE)(0x7)
#define SLLI (BYTE)(0x1)
#define SRLI (BYTE)(0x5)
#define SRAI (BYTE)(0x5)
#define RTYPE_INSN (BYTE)(0x33) // R TYPE Instructions
#define ADD (BYTE)(0x0)
#define SUB (BYTE)(0x0)
#define SLL (BYTE)(0x1)
#define SLT (BYTE)(0x2)
#define SLTU (BYTE)(0x3)
#define XOR (BYTE)(0x4)
#define SRL (BYTE)(0x5)
#define SRA (BYTE)(0x5)
#define OR (BYTE)(0x6)
#define AND (BYTE)(0x7)
#define FENCES (BYTE)(0x0F)
#define FENCE (BYTE)(0x0)
#define FENCEI (BYTE)(0x1)
#define SYSTEM_INSN (BYTE)(0x73) // SYSTEM INSTRUCTION
#define ECALL (BYTE)(0x0)
#define EBREAK (BYTE)(0x0)
#define CSRRW (BYTE)(0x1)
#define CSRRS (BYTE)(0x2)
#define CSRRC (BYTE)(0x3)
#define CSRRWI (BYTE)(0x5)
#define CSRRSI (BYTE)(0x6)
#define CSRRCI (BYTE)(0x7)
/***********************************************/

/********** INSTRUCTION IDENTIFIERS ************/
#define opcode(insn) ((BYTE)(insn  & 0x7F))
#define funct3(insn) ((BYTE)((insn >> 12) & 0x7))
#define funct7(insn) ((BYTE)((insn >> 25) & 0x7F))
#define rd_addr(insn) ((BYTE)((insn >> 7) & 0x1F))
#define rs1_addr(insn) ((BYTE)((insn >> 15) & 0x1F))
#define rs2_addr(insn) ((BYTE)((insn >> 20) & 0x1F))
/**************************************************/

int stall_rob;

struct rob_entry{
    uint64_t id;
    REG PC;
    BYTE opcode;
    REG logical_reg;
    REG previous_reg;
    WORD VALUE;
    BYTE exception : 1;
    struct rob_entry *next;
};

int stall_iq;
uint64_t issued_id; // New Id issued to a rob entry for dependency tracking

struct iq_entry
{
    // BYTE busy : 1; Redundant with software implementation
    REG dest_reg;
    REG PC;
    BYTE opcode;
    uint64_t dest_rob_TAG;
    uint64_t op1_TAG;
    uint64_t op2_TAG;
    WORD op_value1;
    WORD op_value2;
    BYTE wakeup1 : 1;
    BYTE wakeup2 : 1;
    struct iq_entry *next;
};

struct iq_entry *select;
int stall_lq; 
int stall_sq;

uint64_t lsq_age_cnt = 0;

#endif