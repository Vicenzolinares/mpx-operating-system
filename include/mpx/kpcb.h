#ifndef MPX_KPCB_H
#define MPX_KPCB_H

#include <stddef.h>
#include <memory.h>
#include <string.h>

/**
 @file kpcb.h
 @brief Kernel-level PCB definitions and functions
*/

#define PCB_NAME_LENGTH 8
#define PCB_STACK_SIZE 1024

/**
 PCB class enumeration
*/
typedef enum PCB_class {
    USER = 0,
    SYSTEM = 1,
} PCB_class;

/**
 PCB execution state enumeration
*/
typedef enum execution_state {
    READY = 0,
    RUNNING = 1,
    BLOCKED = 2,
} execution_state;

/**
 PCB dispatching state enumeration
*/
typedef enum dispatching_state {
    NOT_SUSPENDED = 0,
    SUSPENDED = 1,
} dispatching_state;

/**
 PCB structure
*/
typedef struct PCB {
    char name[PCB_NAME_LENGTH + 1];
    PCB_class class;
    int priority;
    execution_state execution;
    dispatching_state dispatch;
    unsigned char *stack;
    unsigned char *stack_pointer;
    struct PCB *next;
    struct PCB *prev;
} PCB;

/**
 PCB queue structure
*/
typedef struct PCB_queue {
    struct PCB *head;
    struct PCB *tail;
} PCB_queue;

// All PCB queues
extern PCB_queue *ready_queue;
extern PCB_queue *blocked_queue;

/**
 Allocates memory for a new PCB and its stack, and initializes the stack to 0
 @return Pointer to the allocated PCB, or NULL on failure
*/
PCB* pcb_allocate(void);

/**
 Frees the memory allocated for a PCB and its stack
 @param pcb Pointer to the PCB to free
 @return 0 on success, non-zero on failure
*/
int pcb_free(PCB *pcb);

/**
 Sets up a PCB with the given name, class, and priority
 @param name The name of the PCB
 @param class The class of the PCB (USER or SYSTEM)
 @param priority The priority of the PCB, 0 (highest) to 9 (lowest)
 @return Pointer to the initialized PCB, or NULL on failure
*/
PCB* pcb_setup(const char *name, PCB_class class, int priority);

/**
 Finds a PCB by its name in the ready and blocked queues
 @param name The name of the PCB to find
 @return Pointer to the found PCB, or NULL if not found
*/
PCB* pcb_find(const char *name);

/**
 Inserts a PCB into the appropriate queue, based on state and (if Ready) priority
 @param pcb A pointer to the PCB to enqueue
 */
void pcb_insert(PCB *pcb);

/**
 Removes a PCB from its current queue, but does not free any associated memory or data structures
 @param pcb A pointer to the PCB to unqueue
 @return 0 on success, non-zero on failure
 */
int pcb_remove(PCB *pcb);

#endif
