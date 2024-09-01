#ifndef _LSQ_
#define _LSQ_


#include "includes.h"


struct lq_entry
{
    REG PC;
    uint64_t age;
    WORD ld_address;
    REG targ_reg;
    struct lq_entry *next;
};

struct sq_entry
{
    uint64_t age;
    REG PC;
    REG value_reg;
    REG addr_reg;
    WORD st_address;
    WORD st_value;
    struct sq_entry *next;
};

class LOAD_QUEUE
{
   
    struct lq_entry *head, *tail;
    int capacity; 
    int current;

    public:

    LOAD_QUEUE(int capacity)
    {
        this->capacity = capacity;
        current = 0;
        head = tail = nullptr;
    }
    void add_to_lq(REG PC, BYTE wsel, BYTE r1_re, BYTE r1_sel)
    {
        if(current == capacity)
        {
            stall_lq = 1;
        }
        else if(current == 0)
        {
            struct lq_entry *new_lq = new struct lq_entry;
            new_lq->age = (++lsq_age_cnt);
            new_lq->PC = PC;
            new_lq->targ_reg = wsel;
            new_lq->ld_address = -1; // Read directly from regfile
            head = tail = new_lq;
            current++;
        }
        else {
            struct lq_entry *new_lq = new struct lq_entry;
            new_lq->age = (++lsq_age_cnt);
            new_lq->PC = PC;
            new_lq->targ_reg = wsel;
            new_lq->ld_address = -1;
            tail->next = new_lq;
            tail = new_lq; 
            current++;
        }
    }
    void update_value(REG PC, WORD address)
    {
        struct lq_entry *iter = head;

        while(iter)
        {
            if(iter->PC == PC)
            {
                iter->ld_address = address;
                break;
            }
            iter = iter->next;
        }
    }

    bool older_store(struct sq_entry **sq_head)
    {
        struct sq_entry *search = *sq_head;

        while(search->age < head->age)
        {
            if(search->st_address == -1)
            {
                return 1;
            }
            search = search->next;
        }
        return 0;
    }


};

class STORE_QUEUE
{
   
    struct sq_entry *head, *tail;
    int capacity; 
    int current;

    public:

    STORE_QUEUE(int capacity)
    {
        this->capacity = capacity;
        current = 0;
        head = tail = nullptr;
    }
    void add_to_sq(REG PC, BYTE r1_re, BYTE r1_sel, BYTE r2_re, BYTE r2_sel)
    {
        if(current == capacity)
        {
            stall_lq = 1;
        }
        else if(current == 0)
        {
            struct sq_entry *new_sq = new struct sq_entry;
            new_sq->age = (++lsq_age_cnt);
            new_sq->PC = PC;
            new_sq->addr_reg = r1_sel;
            new_sq->value_reg = r2_sel;
            new_sq->st_address = -1;
            new_sq->st_value = -1;
            head = tail = new_sq;
            current++;
        }
        else {
            struct sq_entry *new_sq = new struct sq_entry;
            new_sq->age = (++lsq_age_cnt);
            new_sq->PC = PC;
            new_sq->addr_reg = r1_sel;
            new_sq->value_reg = r2_sel;
            new_sq->st_address = -1;
            new_sq->st_value = -1;
            tail->next = new_sq;
            tail = new_sq; 
            current++;
        }
    }
    void update_value(REG PC, WORD address)
    {
        struct sq_entry *iter = head;

        while(iter)
        {
            if(iter->PC == PC)
            {
                iter->st_address = address;
                break;
            }
            iter = iter->next;
        }
    }


};

#endif

