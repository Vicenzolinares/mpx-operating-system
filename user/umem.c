#include <umem.h>

void allocate_mem(int size)
{

}

void free_mem(const char* address) //address given in hex
{
    
}

void show_allocated_mem() //address given in hex
{
	MCB* current = allocated_list.head;

    //walks through allocate_mem list
    //prints info for each code block
    //info: start address of block (in hexidecimal), size of block (in decimal)
}

void show_free_mem() //address given in hex
{
    //walks through free_mem list
    //prints info for each code block
    //info: start address of block (in hexidecimal), size of block (in decimal)
}
