#include <includes.h>

int stall_rob;

struct rob_entry{
    BYTE opcode;
    REG PC;
    REG logical_reg;
    REG previous_reg;
    BYTE exception : 1;
};

class ROB{

    rob_entry *head, *tail;
    static int capacity;
    static int current;
    public:
    void add_to_rob();
    void stride();
    void reset_rob();
    
    
};
