#ifndef _IQ_
#define _IQ_


#include "includes.h"

class ISSUE_QUEUE
{

    struct iq_entry *IQ;
    int head, tail, head_wrap,tail_wrap;
    int capacity;
    int current; 

    public:

// struct iq_entry
// {
//     // BYTE busy : 1; Redundant with software implementation
//     REG PC;
//     WORD insn;
//     BYTE opcode;
//     uint64_t dest_rob_TAG;
//     uint64_t op1_TAG;
//     uint64_t op2_TAG;
//     WORD op_value1;
//     WORD op_value2;
//     BYTE wakeup1 : 1;
//     BYTE wakeup2 : 1;
// };
    ISSUE_QUEUE(int size)
    {
        IQ = new struct iq_entry[size]; 
        head = tail = 0;
        head_wrap = tail_wrap = 0;
        current = 0;
        capacity = size;
        stall_iq = 0;
    }
    void push_to_iq(REG PC, WORD insn,BYTE opcode, REG Dest_Tag, REG SRC1_T, REG SRC2_T, WORD op_value1, WORD op_value2, BYTE wk1, BYTE wk2,
                    BYTE r1_re, BYTE r2_re, BYTE regfile_we)
    {
        if(head == tail && head_wrap != tail_wrap)
        {
            //IQ full
            stall_iq = 1;
            return;
        }
        else 
        {
            IQ[tail].PC = PC;
            IQ[tail].insn = insn;
            IQ[tail].opcode = opcode;
            IQ[tail].dest_rob_TAG = Dest_Tag;
            IQ[tail].r1_re = r1_re;
            IQ[tail].r2_re = r2_re;
            IQ[tail].regfile_we = regfile_we;

            // Check to see if values from Regfile Available, otherwise set tags
            if(wk1)
            {
                IQ[tail].wakeup1 = 1;
                IQ[tail].op_value1 = op_value1;
                IQ[tail].op1_TAG = -1;
            }
            else
            {
                IQ[tail].wakeup1 = 0;
                IQ[tail].op1_TAG = SRC1_T;
            }
            if(wk2)
            {
                IQ[tail].wakeup2 = 1;
                IQ[tail].op_value2 = op_value2;
                IQ[tail].op2_TAG = -1;
            }
            else
            {
                IQ[tail].wakeup2 = 0;
                IQ[tail].op2_TAG = SRC2_T;
            }
            stall_iq = 0;
            tail++;

            if(tail == capacity)
            {
                tail_wrap = !tail_wrap;
                tail = 0;
            }
        }
    }
    struct iq_entry* select_issue()
    {
        // empty issue queue flag
        if(head == tail && head_wrap == tail_wrap)
            return nullptr;

        struct iq_entry *issue_pkt;

        int flag_none_ready = 1; 

        for(int i = head; i < tail; i++)
        {   
            issue_pkt = &IQ[i];
            if(IQ[i].wakeup1 && IQ[i].wakeup2)
            {
                flag_none_ready = 0;

                // Move the head pointer
                for(int j = i; j>head; j--)
                {
                    if(j == 0)
                    {
                        IQ[j] = IQ[capacity - 1];
                        j = capacity;
                    }
                    else
                    {
                        IQ[j] = IQ[j-1];
                    }
                }
                head++;
                if(head == capacity)
                {
                    head = 0;
                    head_wrap  = !head_wrap;
                }
                return issue_pkt;
            }
        }

        return nullptr;

    }
    void wakeup_operand(WORD rob_tag, BYTE write_en, WORD result)
    {
        for(int i = head; i<tail; i++)
        {
            if(IQ[i].op1_TAG == rob_tag)
            {
                IQ[i].op_value1 = result; // value bypassed from functional unit
                IQ[i].wakeup1 = 1;
            }

            if(IQ[i].op2_TAG == rob_tag)
            {
                IQ[i].op_value2 = result; // value bypassed from functional unit
                IQ[i].wakeup2 = 1;
            }

            if(i == capacity-1)
            {
                i = 0;
            }
        }
    }

    void print_iq()
    {
        int i = head;


        while(i!=tail)
        {
            std::cout<<"PC: "<<IQ[i].PC<<"\nINSN: "<<IQ[i].insn<<"\nOPCODE: "<<(int)IQ[i].opcode<<"\nDEST: "<<IQ[i].dest_rob_TAG
                    <<"\nOP1 TAG: "<<IQ[i].op1_TAG<<"\nOP2 TAG: "<<IQ[i].op2_TAG<<"\nOP VAL1: "<<IQ[i].op_value1
                    <<"\nOP VAL2: "<<IQ[i].op_value2<<"\nR1_RE: "<<(int)IQ[i].r1_re<<"\nR2_RE: "<<(int)IQ[i].r2_re
                    <<"\nREGF_WE: "<<(int)IQ[i].regfile_we<<"\nWK1: "<<(int)IQ[i].wakeup1<<"\nWK2: "<<(int)IQ[i].wakeup2<<"\t"<<std::endl<<std::endl;
            i++;
            if(i == capacity)
                i = 0;

        }
    }

};

#endif