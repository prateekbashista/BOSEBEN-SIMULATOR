#ifndef _RMT_
#define _RMT_

#include "includes.h"


struct rmt_entry
{
    uint64_t REG_MAP;
    REG ARCH_REG;
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
            RMT_mem[i] = new rmt_entry(0,0,0);

        }

    }
    void reset()
    {
        RMT();
    }
    void rmt_update(REG logical_reg, uint64_t rob_tag, BYTE )
    {
        RMT_mem[logical_reg]->ARCH_REG = logical_reg;
        RMT_mem[logical_reg]->REG_MAP = rob_tag;
        RMT_mem[logical_reg]->rmt_valid = 1;
        RMT_mem[logical_reg]->ready_in_rob = 0;
    }
    void ready_rob_bit(REG logical_reg)
    {
        RMT_mem[logical_reg]->ready_in_rob = 1;
    }

};

#endif