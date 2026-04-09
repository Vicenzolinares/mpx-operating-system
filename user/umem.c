#include <umem.h>

static void umem_write(const char *msg)
{
	if (msg == NULL)
	{
		return;
	}

	sys_req(WRITE, COM1, (char *)msg, strlen(msg));
}

static int is_decimal_digit(char c)
{
	return c >= '0' && c <= '9';
}

static int is_hex_digit(char c)
{
	return (c >= '0' && c <= '9')
		|| (c >= 'a' && c <= 'f')
		|| (c >= 'A' && c <= 'F');
}

static int hex_digit_value(char c)
{
	if (c >= '0' && c <= '9')
	{
		return c - '0';
	}

	if (c >= 'a' && c <= 'f')
	{
		return 10 + (c - 'a');
	}

	return 10 + (c - 'A');
}

static int parse_positive_decimal(const char *token, size_t *value)
{
	size_t parsed = 0;

	if (token == NULL || value == NULL || token[0] == '\0')
	{
		return -1;
	}

	for (int i = 0; token[i] != '\0'; i++)
	{
		if (!is_decimal_digit(token[i]))
		{
			return -1;
		}

		parsed = parsed * 10 + (size_t)(token[i] - '0');
	}

	if (parsed == 0)
	{
		return -1;
	}

	*value = parsed;
	return 0;
}

static int parse_hex_address(const char *token, void **address)
{
	size_t parsed = 0;
	int start = 0;

	if (token == NULL || address == NULL || token[0] == '\0')
	{
		return -1;
	}

	if (token[0] == '0' && (token[1] == 'x' || token[1] == 'X'))
	{
		start = 2;
	}

	if (token[start] == '\0')
	{
		return -1;
	}

	for (int i = start; token[i] != '\0'; i++)
	{
		if (!is_hex_digit(token[i]))
		{
			return -1;
		}

		parsed = (parsed * 16) + (size_t)hex_digit_value(token[i]);
	}

	if (parsed == 0)
	{
		return -1;
	}

	*address = (void *)parsed;
	return 0;
}

static void write_hex_address(void *address)
{
	const char hex_chars[] = "0123456789ABCDEF";
	size_t value = (size_t)address;
	char reversed[(sizeof(size_t) * 2) + 1];
	char output[(sizeof(size_t) * 2) + 5];
	int reverse_len = 0;
	int out = 0;

	if (value == 0)
	{
		umem_write("0x0\r\n");
		return;
	}

	while (value > 0)
	{
		reversed[reverse_len++] = hex_chars[value & 0xF];
		value >>= 4;
	}

	output[out++] = '0';
	output[out++] = 'x';

	for (int i = reverse_len - 1; i >= 0; i--)
	{
		output[out++] = reversed[i];
	}

	output[out++] = '\r';
	output[out++] = '\n';
	output[out] = '\0';

	umem_write(output);
}

void allocate_mem(const char *size_token)
{
	size_t size = 0;

	if (parse_positive_decimal(size_token, &size) != 0)
	{
		umem_write("Error: size must be a positive decimal integer.\r\n");
		return;
	}

	void *address = allocate_memory(size);
	if (address == NULL)
	{
		umem_write("Error: allocation failed.\r\n");
		return;
	}

	write_hex_address(address);
}

void free_mem(const char *address_token)
{
	void *address = NULL;

	if (parse_hex_address(address_token, &address) != 0)
	{
		umem_write("Error: address must be a valid hex value.\r\n");
		return;
	}

	if (free_memory(address) != 0)
	{
		umem_write("Error: free failed.\r\n");
	}
}

void show_allocated_mem(void) //address given in hex
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

void show_free_mem(void) //address given in hex
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
