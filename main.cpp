//#include "includes.h"
#include "decode_riscv32im.h"
#include "ROB.cpp"
#include "Issue_Queue.cpp"
#include "LSQ.cpp"
#include "RMT.cpp"
#include <cstdint>


struct CPU_STAGE
{
    // General Tempelate in for input of the all CPU STAGES

    REG PC; // pc of the instruction being operated on
    WORD insn;
    BYTE opcode, funct3op, funct7op;
    BYTE r1_re, r2_re; // Register Read Enable
    BYTE r1_sel; // R1 Address
    WORD r1_data; // R1 Data
    BYTE r2_sel; // R2 Address
    WORD r2_data; // R2 Data 
    BYTE regfile_we; // Write Enable
    BYTE wsel;// Write Address
    BYTE is_link; // Link address needed
    BYTE link_reg; // Link register address
    WORD output_alu; // computed value
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
    struct CPU_STAGE *fetch_op, *decode_op, *dispatch_op, *issue_op, *integer_op, *memory_op, *commit_op; 

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

        fetch_op = decode_op = dispatch_op = issue_op = integer_op = memory_op = commit_op = nullptr;
    }
};

// Declaration of Datastructures of CPU
struct CPU cpu;
struct MEMORY mem;
ROB rob(32);
ISSUE_QUEUE iq_int(8);
ISSUE_QUEUE iq_mem(4);
LOAD_QUEUE lq(4);
STORE_QUEUE sq(4);
RMT rmt;


void retire()
{
    // Nothing to commit
    if(rob.peek_head() == nullptr)
        return;

    struct rob_entry *temp = rob.peek_head();

    if(temp->complete == 1)
    {
        temp = rob.commit_from_rob();

        if(temp->opcode == STORE)
        {
            mem.data_mem[cpu.X[temp->logical_reg]] = temp->VALUE; // Wrong right now
        }
        else
        {
            cpu.X[temp->logical_reg] = temp->VALUE;    
        }
    
        delete temp;
    }
}

void commit()
{
    CPU_STAGE *work_commit = cpu.commit_op;

    if(work_commit->opcode == LOAD)
    {
        lq.update_value(work_commit->PC, work_commit->output_alu); // LQ will write the value to the ROB, so kind of like retire

        // Search Store---------> if older store don't execute, if same address and value there , use the value
    }
    else if(work_commit->opcode == STORE)
    {
        sq.update_value(work_commit->PC, work_commit->output_alu); // Store will search regfile or ROB till it's instruction to ask for value.
    }
    else
    {
        rob.retire_value(work_commit->PC, work_commit->output_alu); // Normal write back to ROB instead now.
    }

    cpu.commit_op = nullptr;
    
}
void memory()
{
    if(cpu.memory_op == nullptr)
        return;
    CPU_STAGE *work_commit = cpu.memory_op;

    if(work_commit->opcode == LOAD)
    {
        if(!stall_lq)
            lq.add_to_lq(work_commit->PC, work_commit->wsel, work_commit->r1_re,work_commit->r1_sel);
    }
    else if (work_commit->opcode == STORE)
    {
        if(!stall_sq)
            sq.add_to_sq(work_commit->PC,work_commit->r1_re,work_commit->r1_sel, work_commit->r2_re,work_commit->r2_sel);
    }

    cpu.commit_op = cpu.memory_op;

}


int main(int argc, char **argv)
{


}

