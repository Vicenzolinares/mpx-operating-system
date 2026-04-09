#include <umem.h>
#include <sys_req.h>
#include <mpx/device.h>
#include <string.h>

static void mpx_write(const char *s)
{
    if (s != NULL) {
        sys_req(WRITE, COM1, (char *)s, strlen(s));
    }
}

int parse_positive_int(const char *token, int *out_value)
{
    int value = 0;
    int i = 0;

    if (token == NULL || out_value == NULL || token[0] == '\0') {
        return HEAP_ERR_BAD_PARAM;
    }

    while (token[i] != '\0') {
        if (token[i] < '0' || token[i] > '9') {
            return HEAP_ERR_BAD_FORMAT;
        }
        if (value > (2147483647 - (token[i] - '0')) / 10) {
            return HEAP_ERR_BAD_FORMAT;
        }
        value = (value * 10) + (token[i] - '0');
        i++;
    }

    if (value <= 0) {
        return HEAP_ERR_BAD_FORMAT;
    }

    *out_value = value;
    return HEAP_OK;
}

int parse_hex_address(const char *token, void **out_address)
{
    unsigned long value = 0;
    int i = 0;

    if (token == NULL || out_address == NULL || token[0] == '\0') {
        return HEAP_ERR_BAD_PARAM;
    }

    if (token[0] == '0' && (token[1] == 'x' || token[1] == 'X')) {
        i = 2;
    }

    if (token[i] == '\0') {
        return HEAP_ERR_BAD_FORMAT;
    }

    while (token[i] != '\0') {
        value <<= 4;

        if (token[i] >= '0' && token[i] <= '9') {
            value += (unsigned long)(token[i] - '0');
        } else if (token[i] >= 'a' && token[i] <= 'f') {
            value += (unsigned long)(token[i] - 'a' + 10);
        } else if (token[i] >= 'A' && token[i] <= 'F') {
            value += (unsigned long)(token[i] - 'A' + 10);
        } else {
            return HEAP_ERR_BAD_FORMAT;
        }

        i++;
    }

    if (value == 0UL) {
        return HEAP_ERR_BAD_FORMAT;
    }

    *out_address = (void *)value;
    return HEAP_OK;
}

void allocate_mem(int size)
{
    void *ptr;

    if (size <= 0) {
        mpx_write("Error: allocation size must be a positive integer.\r\n");
        return;
    }

    ptr = sys_alloc_mem((size_t)size);
    if (ptr == NULL) {
        mpx_write("Error: allocation failed.\r\n");
        return;
    }

    mpx_write("Allocation successful.\r\n");
}

void free_mem(const char *address) //address given in hex
{
    void *ptr = NULL;

    if (parse_hex_address(address, &ptr) != HEAP_OK) {
        mpx_write("Error: invalid address. Use a non-zero hexadecimal address.\r\n");
        return;
    }

    if (sys_free_mem(ptr) != 0) {
        mpx_write("Error: free failed.\r\n");
        return;
    }

    mpx_write("Free successful.\r\n");
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
        //print current->start_address in hexidecimal, cast all of this into an integer
            char alloc_start[9];
            unsigned int addr = (unsigned int)current->start_address;
            for(int i = 7; i >= 0; i--) {
                alloc_start[7-i] = "0123456789ABCDEF"[(addr >> (i*4)) & 0xF];
            }
            alloc_start[8] = '\0';
            sys_req(WRITE, COM1, alloc_start, strlen(alloc_start));
        char alloc_mid[] = "\r\nSize: ";
        sys_req(WRITE, COM1, alloc_mid, strlen(alloc_mid));
        //print current->size in decimal
            char alloc_size[11];
            size_t sz = current->size;
            int len = 0;
            if (sz == 0) {
                alloc_size[len++] = '0';
            } else {
                while (sz > 0) {
                    alloc_size[len++] = '0' + (sz % 10);
                    sz /= 10;
                }
                // reverse
                for(int j = 0; j < len/2; j++) {
                    char temp = alloc_size[j];
                    alloc_size[j] = alloc_size[len-1-j];
                    alloc_size[len-1-j] = temp;
                }
            }
            alloc_size[len] = '\0';  
            sys_req(WRITE, COM1, alloc_size, strlen(alloc_size));
        char alloc_end[] = "\r\n\r\n";
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
        char free_start[9];
        unsigned int addr = (unsigned int)current->start_address;
        for(int i = 7; i >= 0; i--) {
            free_start[7-i] = "0123456789ABCDEF"[(addr >> (i*4)) & 0xF];
        }
        free_start[8] = '\0';
        sys_req(WRITE, COM1, free_start, 8);
        char free_mid[] = "\r\nSize: ";
        sys_req(WRITE, COM1, free_mid, strlen(free_mid));
        char free_size[11];
        size_t sz = current->size;
        int len = 0;
        if (sz == 0) {
            free_size[len++] = '0';
        } else {
            while (sz > 0) {
                free_size[len++] = '0' + (sz % 10);
                sz /= 10;
            }
            // reverse
            for(int j = 0; j < len/2; j++) {
                char temp = free_size[j];
                free_size[j] = free_size[len-1-j];
                free_size[len-1-j] = temp;
            }
        }
        free_size[len] = '\0';
        sys_req(WRITE, COM1, free_size, len);
        char free_end[] = "\r\n\r\n";
        sys_req(WRITE, COM1, free_end, strlen(free_end));
        current = current->next;
    }
}
