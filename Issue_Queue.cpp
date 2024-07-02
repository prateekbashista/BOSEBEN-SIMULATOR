#include "includes.h"

class ISSUE_QUEUE
{
    struct iq_entry *head, *tail;
    int capacity;
    int current; 

    public:

    ISSUE_QUEUE() : head(nullptr), tail(nullptr)
    {
        capacity = 1; 
        current = 0;
    }
    ISSUE_QUEUE(int capacity) : head(nullptr), tail(nullptr)
    {
        this->capacity = capacity;
        current = 0;
    }
    
    void update_entry(struct iq_entry **new_iq,BYTE r1_sel, BYTE r1_re, BYTE r2_sel, BYTE r2_re, 
                BYTE wsel, BYTE regfile_we)
    {
        struct iq_entry *new_iq_update = *new_iq;
        struct iq_entry *iter = head;
        int op1 = 0, op2 = 0; // Signals to check RMT if value not dependent

        while(iter->next != tail)
        {
            if(iter->dest_reg == r1_sel && r1_re)
            {
                new_iq_update->op1_TAG = iter->dest_rob_TAG;
                op1 = 1;
            }
            if(iter->dest_reg == r1_sel && r1_re)
            {
                new_iq_update->op2_TAG = iter->dest_rob_TAG;
                op2 = 1;
            }
            iter = iter->next;
        }    

    }
    void push_to_iq(REG PC, WORD insn, BYTE r1_sel, BYTE r1_re, BYTE r2_sel, BYTE r2_re, 
                BYTE wsel, BYTE regfile_we)
    {
        if(current == capacity)
        {
            stall_iq = 1;
        }
        else
        {
            struct iq_entry *new_iq = new struct iq_entry;
            new_iq->dest_reg = (regfile_we) ? rd_addr(insn) : -1;
            new_iq->PC = PC;
            new_iq->opcode = opcode(insn);
            new_iq->dest_rob_TAG = issued_id; 
            new_iq->op1_TAG = -1;
            new_iq->op2_TAG = -1;
            new_iq->op_value1 = -1;
            new_iq->op_value2 = -1;
            
            if(r1_re || r2_re)
            {
                update_entry(&new_iq,r1_re,r1_sel,r2_re,r2_sel, wsel, regfile_we);
            }
            tail->next = new_iq;
            tail = new_iq;
        }

    }
    void wakeup_operand(uint64_t id, int reg_num)
    {
        struct iq_entry *iter  = head;

        while(iter)
        {   
            if(iter->dest_rob_TAG == id)
            {
                if(reg_num == 1) 
                    iter->wakeup1 = 1;
                if(reg_num == 2)
                    iter->wakeup2 = 1;
            }
            iter = iter->next;
        }

        delete iter;
    }
    
    

};