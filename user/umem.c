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

void show_allocated_mem(const char *address, int size) //address given in hex
{
    (void)address;
    (void)size;
    mpx_write("show_allocated not implemented yet.\r\n");
}

void show_free_mem(const char *address, int size) //address given in hex
{
    (void)address;
    (void)size;
    mpx_write("show_free not implemented yet.\r\n");
}
