#ifndef MPX_UMEM_H
#define MPX_UMEM_H

#include <lib_mem.h>
#include <memory.h>
#include <sys_req.h>
#include <mpx/device.h>
#include <string.h>


#define DEFAULT_HEAP_SIZE 65536U


#define HEAP_OK 0
#define HEAP_ERR_BAD_PARAM -1
#define HEAP_ERR_BAD_FORMAT -2
#define HEAP_ERR_ALLOC_FAIL -3
#define HEAP_ERR_FREE_FAIL -4


void allocate_mem(const char *size_token);

void free_mem(const char *address_token);

void show_allocated_mem(void);
void show_free_mem(void);

#endif
