#include "includes.h"
#include "decode_riscv32im.h"
#include "ROB.h"
#include "Issue_Queue.h"
#include "INT_FU.h"
#include "LSQ.h"
#include "RMT.h"
#include "ROB.h"
#include <cstdint>
#include <fstream> 
#include <iostream> 
#include <stdlib.h>


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
    

    void PC_viewer()
    {
        std::cout<<"\t PC = "<<PC;
    }
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
        NEXT_PC = 0;
        N = 0;
        P = 0;
        Z = 0;
        
        for(int i = 0; i<32; i++)
        {
            X[i] = 0;
        } //Reg Reset

        fetch_op = new CPU_STAGE;
        decode_op = new CPU_STAGE;
        dispatch_op = new CPU_STAGE;
        issue_op = new CPU_STAGE;
        integer_op = new CPU_STAGE;
        memory_op = new CPU_STAGE;
        commit_op_mem = new CPU_STAGE;
        commit_op_int = new CPU_STAGE;
                
        /*fetch_op =*/ decode_op = dispatch_op = issue_op = integer_op = memory_op = commit_op_mem = commit_op_int = nullptr;

    }

    void print_regfile()
    {   
        std::cout<<"\nREGFILE STATE\n";
        for(int i = 0; i<32; i++)
        {
            std::cout<<"X"<<i<<"="<<X[i]<<std::endl;
        } 
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
    std::cout<<"\n========RETIRE==========\n";
    // Nothing to commit
    if(rob.peek_head() == nullptr)
    {
        std::cout<<"\n No Payload to retire\n";
        std::cout<<"\n===========================\n";
        return;
    }
    // struct rob_entry *temp = rob.peek_head();
    // if(temp->complete == 1)
    // {
    //     temp = rob.commit_from_rob();

    //     if(temp->opcode == STORE)
    //     {
    //         mem.data_mem[cpu.X[temp->logical_reg]] = temp->VALUE; // Wrong right now
    //     }
    //     else
    //     {
    //         cpu.X[temp->logical_reg] = temp->VALUE;    
    //     }
    
    //     delete temp;
    // }

    struct rob_entry *retire_payload = rob.commit_to_REG();

    if(retire_payload != nullptr && retire_payload->valid)
    {   
        std::cout<<"\n REG : "<<retire_payload->logical_reg<<std::endl;
        std::cout<<"\n VALUE : "<<retire_payload->VALUE<<std::endl;
        cpu.X[retire_payload->logical_reg] = retire_payload->VALUE;
        rmt.ready_rob_bit_0(retire_payload->logical_reg);
    }
    else 
    {
        std::cout<<"\n NOT COMPLETE TO RETIRE\n"<<std::endl;
    }
        
    std::cout<<"\n===========================\n";

}

void commit()
{
    CPU_STAGE *work_commit = cpu.commit_op_mem;
    CPU_STAGE *work_int = cpu.commit_op_int;

    // if(cpu.commit_op_int != nullptr)
    // {
    //     std::cout<<"PC (COMMIT) : "<<work_int->insn;

    //     if(work_commit->opcode == LOAD)
    //     {
    //         lq.update_value(work_commit->PC, work_commit->output_alu); // LQ will write the value to the ROB, so kind of like retire

    //         // Search Store---------> if older store don't execute, if same address and value there , use the value
    //         // If works, bypass to IQ as well ---------> iq.wakeup_operand
    //     }
    //     else if(work_commit->opcode == STORE)
    //     {
    //         sq.update_value(work_commit->PC, work_commit->output_alu); // Store will search regfile or ROB till it's instruction to ask for value.
    //         // Writing value to ROB
            
    //     }
        
    // }
    //     if(cpu.commit_op_int != nullptr)
    //         rob.retire_value(work_commit->PC, work_commit->output_alu); // Normal write back to ROB instead now.

    std::cout<<"\n========COMMIT==========\n";
    if(work_int == nullptr)
    {
        std::cout<<"\n No Payload to commit\n";
        std::cout<<"\n===========================\n";
        return;
    }
        
    
    WORD dest = rob.deposit_value(work_int->wsel, work_int->output_alu);
    std::cout<<"Register Value (Commit) :"<<(int)work_int->output_alu<<std::endl;
    std::cout<<"Destination Register (Commit) :"<<(int)dest;
    rmt.ready_rob_bit(dest);
    

    cpu.commit_op_mem = nullptr;
    cpu.commit_op_int = nullptr;
    std::cout<<"\n===========================\n";
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
    std::cout<<"\n========INT_FU==========\n";
    if(cpu.integer_op == nullptr)
    {   std::cout<<"\n No INT Payload\n";
         std::cout<<"\n===========================\n";
        return; 
    }

    
    CPU_STAGE *fu = cpu.integer_op;

    // The value broadcasted on the common data bus (CDB)
    int_fu(fu->insn,fu->PC, fu->r1_data, fu->r2_data, &(fu->output_alu));
    std::cout<<"\nAns : "<<fu->output_alu<<std::endl;
    std::cout<<"\nROB_TAG INTEGER EXECUTE: "<<(int)fu->wsel<<std::endl;
    iq.wakeup_operand(fu->wsel,fu->regfile_we, fu->output_alu); // wakeup the operands and bypass the value
    cpu.commit_op_int = fu;
    std::cout<<"\n===========================\n";
}

void issue()
{
    std::cout<<"\n========ISSUE==========\n";
    iq_entry *issued_int = iq.select_issue();
    
    if(issued_int == nullptr)
    {
        std::cout<<"\n No Issue Ready\n";
         std::cout<<"\n===========================\n";
        return;
    }    

    //std::cout<<"\n Issue Ready\n";
    CPU_STAGE *issue_pckt = new CPU_STAGE;

    issue_pckt->funct3op = funct3(issued_int->insn);
    issue_pckt->funct7op = funct7(issued_int->insn);
    issue_pckt->r1_data = issued_int->op_value1;
    issue_pckt->r1_re = issued_int->r1_re;
    issue_pckt->r2_data = issued_int->op_value2;
    issue_pckt->r2_re = issued_int->r2_re;
    issue_pckt->opcode = issued_int->opcode;
    issue_pckt->PC = issued_int->PC;
    issue_pckt->insn = issued_int->insn;
    issue_pckt->regfile_we = issued_int->regfile_we;
    issue_pckt->wsel = issued_int->dest_rob_TAG;

    std::cout<<"\nROB_TAG INTEGER ISSUE: "<<(int)issue_pckt->wsel;

    cpu.integer_op = issue_pckt;
    std::cout<<"\n===========================\n";
}

void dispatch()
{

    std::cout<<"\n========DISPATCH==========\n";
    CPU_STAGE *dispatch_p = cpu.dispatch_op;

    if(cpu.dispatch_op == nullptr)
    {   std::cout<<"\n No Dispatch Payload\n"; 
        std::cout<<"\n===========================\n";
        return;
    }

    // Add the entry to rob and get the Rob tag
    int rob_tag = rob.push_to_rob(dispatch_p->PC, 
                                  dispatch_p->opcode, 
                                  dispatch_p->wsel);
    
    std::cout<<"ROB TAG :"<<rob_tag<<std::endl;
    // Read SRC Values
    BYTE rd_rob1, rd_rob2;
    WORD src_t1,src_t2;
    rmt.return_operands(dispatch_p->r1_sel,dispatch_p->r2_sel,
                        rd_rob1,rd_rob2,src_t1,src_t2);
    std::cout<<"SRC_T1: "<<src_t1<<std::endl;
    std::cout<<"SRC_T2: "<<src_t2<<std::endl;
    
    // Push new mapping to RMT with ROB Tag
    rmt.rmt_update(dispatch_p->wsel, rob_tag);
    
    WORD val1 = NULL,val2 = NULL;
    BYTE wk1 = 0, wk2 = 0;

    if(rd_rob1 == 0)
    {
        val1 = cpu.X[dispatch_p->r1_sel];
        std::cout<<"\nVAL1 = "<<val1<<std::endl;
        wk1 = 1;
    }
    else if(rd_rob1 == 1)
    {
        val1 = rob.read_val(src_t1);
        std::cout<<"\nVAL1 = "<<val1<<std::endl;
        wk1 = 1;
    }
    
    if(rd_rob2 == 0)
    {
        val2 = cpu.X[dispatch_p->r2_sel];
        std::cout<<"\nVAL1 = "<<val2<<std::endl;
        wk2 = 1;
    }
    else if(rd_rob2 == 1)
    {
        val2 = rob.read_val(src_t2);
        std::cout<<"\nVAL1 = "<<val2<<std::endl;
        wk2 = 1;
    }
    
    // BYTE wk1 = (val1 == NULL) ? 0 : 1;
    // BYTE wk2 = (val2 == NULL) ? 0 : 1;

    std::cout<<"\nwakeup1 = "<<(int)wk1<<std::endl;
    std::cout<<"\nwakeup2 = "<<(int)wk2<<std::endl;


    // Push the renamed instruction to IQ
    iq.push_to_iq(dispatch_p->PC,dispatch_p->insn,
                  dispatch_p->opcode,rob_tag,
                  src_t1,src_t2,val1,val2,wk1,wk2,
                  dispatch_p->r1_re,dispatch_p->r2_re,
                   dispatch_p->regfile_we);
    
    iq.print_iq();

    std::cout<<"\n===========================\n";

}

void decode ()
{
    std::cout<<"\n========DECODE==========\n";
    CPU_STAGE *dec = cpu.decode_op;
    
    if(cpu.decode_op == nullptr)
    {   
        std::cout<<"\n No Decode Payload\n";
        std::cout<<"\n===========================\n";
         return;
    }

    decoder(dec->insn,&dec->r1_sel, &dec->r1_re, &dec->r2_sel, &dec->r2_re, 
            &dec->wsel, &dec->regfile_we, &dec->is_load, &dec->is_store, 
            &dec->is_branch, &dec->is_control, &dec->is_system, &dec->is_link);

    cpu.dispatch_op = dec;
    std::cout<<"\n===========================\n";
}

void fetch()
{
    CPU_STAGE *dec_packet = new CPU_STAGE;
    dec_packet->PC = cpu.NEXT_PC;
    dec_packet->insn = 0x00210113;
    cpu.fetch_op->PC = dec_packet->PC;//cpu.PC;
    cpu.fetch_op->insn = dec_packet->insn;//mem.insn_mem[cpu.PC]; // Fetch fresh insn fro I$ Cache

    cpu.NEXT_PC = ++cpu.PC; // This is where branch prediction will be added
    cpu.decode_op = cpu.fetch_op;
}

int main(int argc, char **argv)
{
    cpu.reset();

    // Loading the hexdump in the instruction 
    int i = 0;
    cpu.PC = 0;
    cpu.NEXT_PC = 0;
    while(i<9)
    {   
        std::cout<<"\n******************************************\n";
        std::cout<<"\nCYCLE : "<<i<<std::endl;
        
        retire();
        commit();
        memory();
        integer_execute();
        issue();
        dispatch();
        decode();
        fetch();
        // if(i == 19)
        //     std::cout<<"PC = "<<cpu.commit_op_int->insn;
        i++;
        std::cout<<"\n******************************************\n";
    }
    rob.print_ROB();
    rmt.print_RMT();
    cpu.print_regfile();

    return 0;
}

