#ifndef _ROB_
#define _ROB_

#include "includes.h"

// The rob will be implemented using a queue.
// The queue will be custom, and thus, it will be made up of rob_entry

/*===============================================================================
Arhitecture of Re-Order Buffer
This Class models a vector based queue for ROB
Following is ROB structure : 
    --------------------------------------------------------------------
    | Valid | Complete | PC | opcode | Logical Reg | Value | exception |
    --------------------------------------------------------------------
    |       |          |    |        |             |       |           | <- head
    --------------------------------------------------------------------
    |       |          |    |        |             |       |           |
    --------------------------------------------------------------------
    |       |          |    |        |             |       |           |
    --------------------------------------------------------------------
    |       |          |    |        |             |       |           | <- tail
    --------------------------------------------------------------------
    |       |          |    |        |             |       |           |
    --------------------------------------------------------------------


================================================================================*/

class ROB{

    struct rob_entry *ROB_QUEUE;
    int head, tail;
    int head_wrap,tail_wrap;
    int capacity;
    int current;

    public:

    ROB(int size)
    {
        ROB_QUEUE = new struct rob_entry[size];

        for(int i = 0; i<size;i++)
        {
            // Initiliasing the entry to be invalid
            ROB_QUEUE[i].valid = 0;
        }

        head = tail = 0;
        head_wrap = tail_wrap = 0;
        stall_rob = 0;
        capacity = size;
        current = 0;
    }

    struct rob_entry* peek_head()
    {
        if(ROB_QUEUE[head].valid)
            return &(ROB_QUEUE[head]);
        
        return nullptr;
    }
    int push_to_rob(REG PC, BYTE opcode, REG Dest)
    {
        if(head == tail && head_wrap != tail_wrap)
        {   
            // Full Condition of the queue
            stall_rob  = 1;
            return -1;
        }
        else 
        {
            // Adding the entry to ROB
            stall_rob = 0;
            ROB_QUEUE[tail].valid = 1;
            ROB_QUEUE[tail].PC = PC;
            ROB_QUEUE[tail].opcode = opcode;
            ROB_QUEUE[tail].logical_reg = Dest;
            int rob_pointer = tail;
            tail++;

            if(tail == capacity)
            {
                tail = 0;
                tail_wrap = !tail_wrap;
            }

            return rob_pointer;
        }
    }
    WORD read_val(WORD rob_tag)
    {
        return ROB_QUEUE[rob_tag].VALUE;
    }
    REG deposit_value(WORD rob_addr, WORD value)
    {
        ROB_QUEUE[rob_addr].VALUE = value;
        ROB_QUEUE[rob_addr].complete = 1;
        return ROB_QUEUE[rob_addr].logical_reg;
    }
    void print_ROB()
    {
        int i = head;
        while(i!=tail)
        {
            std::cout<<"ROB#"<<i<<std::endl;
            std::cout<<"Complete : "<<ROB_QUEUE[i].complete<<std::endl;
            std::cout<<"Valid : "<<ROB_QUEUE[i].valid<<std::endl;
            std::cout<<"PC : "<<ROB_QUEUE[i].PC<<std::endl;
            std::cout<<"OPCODE : "<<(int)ROB_QUEUE[i].opcode<<std::endl;
            std::cout<<"LOGICAL REG : "<<ROB_QUEUE[i].logical_reg<<std::endl;
            std::cout<<"VALUE : "<<ROB_QUEUE[i].VALUE<<std::endl;
            std::cout<<std::endl;
            i++;
            if(i == capacity)
            {
                i = 0;
            }
        }
    }


};


#endif