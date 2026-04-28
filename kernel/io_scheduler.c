#include <mpx/io_scheduler.h>


void io_schedule(device device, op_code op, char* buf, size_t len, PCB* current_process) {
    IOCB *iocb = sys_alloc_mem(sizeof(IOCB));
    iocb->pcb = current_process;
    iocb->dcb = //todo
    if (op == READ) iocb->operation = READ;
    else iocb->operation = WRITE;
    iocb->buffer = buf;
    iocb->buffer_size = len;
    iocb->next = NULL;

    put in queue //todo

    if (device is busy /*todo*/) return;
    else {
        if (op == READ) serial_read(device /*todo*/, buf, len);
        else serial_write(device /*todo*/, buf, len);
    }
}

void io_complete(void) {
    check each device //todo
    if (op is done/*todo*/)
    {
        unblock pbc //todo
        free old iocb //todo
        get next iocb //todo
        call serial read or write //todo
    }
}
