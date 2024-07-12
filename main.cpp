//#include "includes.h"
#include "decode_riscv32im.h"
#include "ROB.cpp"
#include "Issue_Queue.cpp"
#include "INT_FU.cpp"
#include "LSQ.cpp"
#include "RMT.cpp"
#include <cstdint>
#include "ROB.h"
#include <fstream> 
#include <iostream> 


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
    BYTE link_reg; // Link register address
    WORD output_alu; // computed value
    // Flags
    BYTE is_load; // Is instruction a load
    BYTE is_store; // Is intruction a store
    BYTE is_branch; // Is instruction a branch
    BYTE is_control; // Is instruction a control instruction like jump
    BYTE is_system; // Is instruction from CSR, Priviledge mode support
    BYTE is_link; // Link address needed
};

struct MEMORY
{
    WORD data_mem[4096];
    WORD insn_mem[256];

    long long program_end; // end of the program

};

struct CPU
{   
    REG PC;
    REG NEXT_PC;

    // Stages of the CPU
    struct CPU_STAGE *fetch_op, *decode_op, *dispatch_op, *issue_op, *integer_op, *memory_op, *commit_op_mem, *commit_op_int; 

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

        fetch_op = decode_op = dispatch_op = issue_op = integer_op = memory_op = commit_op_mem = commit_op_int = nullptr;
    }
};

// Declaration of Datastructures of CPU
struct CPU cpu;
struct MEMORY mem;
ROB rob(32);
ISSUE_QUEUE iq(16);
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
    CPU_STAGE *work_commit = cpu.commit_op_mem;
    CPU_STAGE *work_int = cpu.commit_op_int;

    if(work_commit->opcode == LOAD)
    {
        lq.update_value(work_commit->PC, work_commit->output_alu); // LQ will write the value to the ROB, so kind of like retire

        // Search Store---------> if older store don't execute, if same address and value there , use the value
        // If works, bypass to IQ as well ---------> iq.wakeup_operand
    }
    else if(work_commit->opcode == STORE)
    {
        sq.update_value(work_commit->PC, work_commit->output_alu); // Store will search regfile or ROB till it's instruction to ask for value.
        // Writing value to ROB
        
    }
    

    if(cpu.commit_op_int != nullptr)
        rob.retire_value(work_commit->PC, work_commit->output_alu); // Normal write back to ROB instead now.



    cpu.commit_op_mem = nullptr;
    cpu.commit_op_int = nullptr;
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

    cpu.commit_op_mem = cpu.memory_op;

}

void integer_execute()
{
    if(cpu.integer_op == nullptr)
        return; 

    CPU_STAGE *fu = cpu.integer_op;

    int_fu(fu->insn,fu->PC, fu->r1_data, fu->r2_data, &(fu->output_alu));
    iq.wakeup_operand(fu->PC, fu->wsel, fu->r1_re, fu->r2_re);
    rob.retire_value(fu->PC, fu->output_alu);
    rmt.ready_rob_bit(fu->wsel);
    cpu.commit_op_int = fu;
}

void issue()
{

    iq_entry *issued_int = iq.select_issue();
    CPU_STAGE *issue_pckt;

    issue_pckt->funct3op = funct3(issued_int->PC);
    issue_pckt->funct7op = funct7(issued_int->PC);
    issue_pckt->r1_data = issued_int->op_value1;
    issue_pckt->r1_re = 1;
    issue_pckt->r2_data = issued_int->op_value2;
    issue_pckt->r2_re = 1;
    issue_pckt->opcode = issued_int->opcode;
    issue_pckt->PC = issued_int->PC;
    issue_pckt->insn = issued_int->insn;
    issue_pckt->regfile_we = 1;
    issue_pckt->wsel = issued_int->dest_reg;

    cpu.integer_op = issue_pckt;

}

void dispatch()
{
    CPU_STAGE *dispatch_p = cpu.dispatch_op;

    rob.add_to_rob(dispatch_p->PC, 
                   dispatch_p->opcode, 
                   dispatch_p->wsel, 
                   (-1), 
                   (-1));
    iq.push_to_iq( dispatch_p->PC, 
                   dispatch_p->insn, 
                   dispatch_p->r1_sel,
                   dispatch_p->r1_re, 
                   dispatch_p->r2_sel, 
                   dispatch_p->r2_re, 
                   dispatch_p->wsel, 
                   dispatch_p->regfile_we);

}

void decode ()
{
    CPU_STAGE *dec = cpu.decode_op;

    decoder(dec->insn,&dec->r1_sel, &dec->r1_re, &dec->r2_sel, &dec->r2_re, 
            &dec->wsel, &dec->regfile_we, &dec->is_load, &dec->is_store, 
            &dec->is_branch, &dec->is_control, &dec->is_system, &dec->is_link);

    cpu.dispatch_op = dec;
}

void fetch()
{
    cpu.decode_op->PC = cpu.PC;
    cpu.decode_op->insn = mem.insn_mem[cpu.PC]; // Fetch fresh insn fro I$ Cache

    cpu.NEXT_PC = cpu.PC++; // This is where branch prediction will be added
}

int main(int argc, char **argv)
{
    cpu.reset();

    // Loading the hexdump in the instruction 
    int i = 0;
    std::ifstream file("test_hexdump.txt",std::fstream::in);

    uint32_t a;
    while(file.eof())
    {
        file>>std::hex >> a;
        mem.insn_mem[i] = a;
        i++;
    }



}

