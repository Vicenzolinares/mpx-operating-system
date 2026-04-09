#ifndef MPX_UMEM_H
#define MPX_UMEM_H

#include <lib_mem.h>
#include <memory.h>


#define DEFAULT_HEAP_SIZE 65536U


#define HEAP_OK 0
#define HEAP_ERR_BAD_PARAM -1
#define HEAP_ERR_BAD_FORMAT -2
#define HEAP_ERR_ALLOC_FAIL -3
#define HEAP_ERR_FREE_FAIL -4


int parse_positive_int(const char *token, int *out_value);
int parse_hex_address(const char *token, void **out_address);


void allocate_mem(int size);
void free_mem(const char *address);
void show_allocated_mem(const char *address, int size);
void show_free_mem(const char *address, int size);

#endif
