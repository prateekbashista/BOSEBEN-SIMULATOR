#ifndef _RMT_
#define _RMT_

#include "includes.h"

/*========================================================================
Architecture of Rename Map Table (RMT)
- The struct for rmt entry contains the mapping, valid, ready-in-rob
- This class models an RMT for a scalar out of order processor
- Thus, it contains the 2 read ports and 1 write port
- Class Functions : 
    - RMT constructor : intialise the default values to 32 entries of ROB
    - Reset : Calls the constructor
    - RMT_update : when a new entry is addedin ROB, 
                   a new mapping pushed to RMT.
    - return_operands : 
                following values of rd_rob : 1 -> ready in rob
                                             0 -> not in rmt
                                             2 -> not ready in rob
    - ready in rob bit : fucntion to set the bit after result available
=========================================================================*/

struct rmt_entry
{
    WORD REG_MAP;
    BYTE rmt_valid : 1;
    BYTE ready_in_rob : 1;

    rmt_entry(uint64_t REG_MAP, BYTE rmt_valid, BYTE ready_in_rob)
    {
        this->REG_MAP = REG_MAP;
        this->rmt_valid = rmt_valid;
        this->ready_in_rob = ready_in_rob;
    }
};

class RMT
{
    struct rmt_entry *RMT_mem[32];

    public :

    RMT()
    {

        for(int i = 0;i < 32; i++)
        {
            RMT_mem[i] = new rmt_entry(-1,0,0);

        }

    }
    void reset()
    {
        RMT();
    }
    void rmt_update(REG logical_reg, WORD rob_tag)
    {
        RMT_mem[logical_reg]->REG_MAP = rob_tag;
        RMT_mem[logical_reg]->rmt_valid = 1;
        //RMT_mem[logical_reg]->ready_in_rob = 0;
    }
    void return_operands(REG op_reg1, REG op_reg2, BYTE &rd_rob1, BYTE &rd_rob2, WORD &tag1, WORD &tag2)
    {
        if(RMT_mem[op_reg1]->REG_MAP == -1)
        {
            rd_rob1 = 0;
        }
        else if(RMT_mem[op_reg1]->ready_in_rob)
        {
            tag1 = RMT_mem[op_reg1]->REG_MAP;
            rd_rob1 = 1;
        }
        else
        {
            tag1 = (RMT_mem[op_reg1]->REG_MAP);
            rd_rob1 = 2;            
        }
        if(RMT_mem[op_reg2]->REG_MAP == -1)
        {
            rd_rob2 = 0;
        }
        else if(RMT_mem[op_reg2]->ready_in_rob)
        {
            tag2 = RMT_mem[op_reg2]->REG_MAP;
            rd_rob2 = 1;
        }
        else
        {
            tag2 = RMT_mem[op_reg2]->REG_MAP;
            rd_rob2 = 2;            
        }
    }   
    void ready_rob_bit(REG logical_reg)
    {
        RMT_mem[logical_reg]->ready_in_rob = 1;
    }
    void ready_rob_bit_0(REG logical_reg)
    {
        RMT_mem[logical_reg]->ready_in_rob = 0;
    }    
    void print_RMT()
    {
        int i = 0;
        for(i = 0; i<32; i++)
        {    
            std::cout<<"RMT#"<<i<<std::endl;
            std::cout<<"Register Binding : "<<(int)RMT_mem[i]->REG_MAP<<std::endl;
            std::cout<<"Valid : "<<(int)RMT_mem[i]->rmt_valid<<std::endl;
            std::cout<<"Ready in ROB : "<<(int)RMT_mem[i]->ready_in_rob<<std::endl;
            std::cout<<std::endl;
        }
    }

};

#endif