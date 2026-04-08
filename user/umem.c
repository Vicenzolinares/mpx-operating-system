#include <umem.h>

void allocate_mem(int size)
{

}

void free_mem(const char* address) //address given in hex
{
    
}

void show_allocated_mem() //address given in hex
{
    //walks through allocate_mem list
    //prints info for each code block
    //info: start address of block (in hexidecimal), size of block (in decimal)
    MCB* current = allocated_list->head;
    if(current == NULL)
    {
        char alloc_mem_err[] = "Allocated list empty\r\n";
        sys_req(WRITE, COM1, alloc_mem_err, strlen(alloc_mem_err));
    }
    while(current != NULL)
    {
        char alloc_mcb[] = "MCB allocated\r\nAddress: ";
        sys_req(WRITE, COM1, alloc_mcb, strlen(alloc_mcb));
        print current->start_address in hexidecimal, cast all of this into an integer;
        char alloc_mid[] = "/r/nSize: ";
        sys_req(WRITE, COM1, alloc_mid, strlen(alloc_mid));
        print current->size in decimal;
        char alloc_end[] = "/r/n/r/n";
        sys_req(WRITE, COM1, alloc_end, strlen(alloc_end));
        current = current->next;
    }
}

void show_free_mem() //address given in hex
{
    //walks through free_mem list
    //prints info for each code block
    //info: start address of block (in hexidecimal), size of block (in decimal)
    MCB* current = free_list->head;
    if(current == NULL)
    {
        char free_mem_err[] = "Free list empty\r\n";
        sys_req(WRITE, COM1, free_mem_err, strlen(free_mem_err));
    }
    while(current != NULL)
    {
        char free_mcb[] = "MCB free\r\nAddress: ";
        sys_req(WRITE, COM1, free_mcb, strlen(free_mcb));
        print current->start_address in hexidecimal, cast all of this into an integer;
        char free_mid[] = "/r/nSize: ";
        sys_req(WRITE, COM1, free_mid, strlen(free_mid));
        print current->size in decimal;
        char free_end[] = "/r/n/r/n";
        sys_req(WRITE, COM1, free_end, strlen(free_end));
        current = current->next;
    }
}
