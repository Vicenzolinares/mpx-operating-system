#ifndef MPX_IO_SCHEDULER_H
#define MPX_IO_SCHEDULER_H

#include <mpx/kpcb.h>
#include <mpx/device.h>
#include <sys_req.h>
#include <stddef.h>
#include <mpx/iocb.h>

void io_schedule(device device, op_code op, char* buf, size_t len, PCB* current_process);

void io_complete(void);

#endif
