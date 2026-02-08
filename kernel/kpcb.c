#include <mpx/kpcb.h>

PCB_queue ready_queue = { .head = NULL, .tail = NULL };
PCB_queue blocked_queue = { .head = NULL, .tail = NULL };
