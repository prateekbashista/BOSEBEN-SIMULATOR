#ifndef _RMT_
#define _RMT_

#include "includes.h"


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
        RMT_mem[logical_reg]->ready_in_rob = 0;
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
            tag1 = RMT_mem[op_reg1]->REG_MAP;
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

};

#endif