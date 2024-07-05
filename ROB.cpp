#include <includes.h>


// The rob will be implemented using a queue.
// The queue will be custom, and thus, it will be made up of rob_entry

class ROB{

    rob_entry *head, *tail;
    int capacity;
    int current;
    uint64_t new_id;

    public:


    ROB() : head(nullptr), tail(nullptr)
    {
        capacity = 1;
        current = 0;
        new_id = 0;
        stall_rob = 0;
    }
    ROB(int capacity) : head(nullptr), tail(nullptr)
    {
        this->capacity = capacity;
        current = 0;
        new_id = 0;
        stall_rob = 0;
    }

    struct rob_entry* peek_head()
    {
        return head;
    }
    void add_to_rob(REG PC, BYTE opcode, REG logical_reg, REG previous_reg, REG VALUE)
    {
        struct rob_entry *new_entry = new struct rob_entry;
        new_id++;
        new_entry->id = new_id;
        new_entry->complete = 0;
        new_entry->PC = PC;
        new_entry->opcode = opcode;
        new_entry->logical_reg = logical_reg;
        new_entry->previous_reg = previous_reg;
        new_entry->VALUE = VALUE;

        if(current = 0)
        {
            head = tail = new_entry;
            stall_rob = 0;
            current++;
        }  
        else if(current == capacity)
        {
            stall_rob = 1;
            delete new_entry;
            new_id --;
        }
        else
        {   
            stall_rob = 0;
            tail->next  = new_entry;
            tail = new_entry;
            issued_id = new_id;
            capacity++;
        }
    }
    void reset_rob()
    {
        ROB(capacity);
    }
    void exception_set(REG PC)
    {
        struct rob_entry* iter = head;

        while(iter!=nullptr)
        {
            if(iter->PC == PC)
            {
                iter->exception = 1;
                break;
            }
            iter = iter->next;
        }    

    }
    void retire_value(REG PC, WORD VALUE)
    {
        struct rob_entry *iter = head;

        while(iter!=nullptr)
        {
            if(iter->PC == PC)
            {
                iter->VALUE = VALUE;
                iter->complete = 1;
                break;
            }
            iter = iter->next;
        }
    }
    struct rob_entry* commit_from_rob()
    {
        if(exception_handle() == true)
        {
                
            struct rob_entry *del = nullptr;
            if(current == 0)
            {
                std::cout<<"\n C++ Error: Trying to decrement an empty ROB";
            }
            else if(head->next == nullptr)
            {
                del  = head;
                delete head;
                head = tail = nullptr;
                current--;
            }
            else
            {
                struct rob_entry *freer = head;
                head = head->next;
                delete freer;
                current--;
            }

            return del;
        }
        else
        {
            return nullptr; 
        }
    }
    bool exception_handle()
    {
        if(head->exception == 1)
        {
            head = tail;
        }
        return true;
    }
    
};
