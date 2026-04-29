#include <mpx/io_scheduler.h>

void io_schedule(device device, op_code op, char* buf, size_t len, PCB* current_process) {
    IOCB *iocb = sys_alloc_mem(sizeof(IOCB));
    iocb->pcb = current_process;
    if (device == COM1) iocb->dcb = com1_dcb;
    if (op == READ) iocb->operation = IOCB_READ;
    else iocb->operation = IOCB_WRITE;
    iocb->buffer = buf;
    iocb->buffer_size = len;
    iocb->next = NULL;

    if (device == COM1) {
        if (com1_dcb->iocb_head == NULL) {
            com1_dcb->iocb_head = iocb;
            com1_dcb->iocb_tail = iocb;
        }
        else {
            com1_dcb->iocb_tail->next = iocb;
        }

        if (com1_dcb->open == 0) return;
        else {
            if (op == READ) serial_read(device, buf, len);
            else serial_write(device, buf, len);
        }
    }
}

void io_complete(void) {
    if (com1_dcb->open == 1)
    {
        if (com1_dcb->iocb_head == NULL) return;
        unblock_pcb(com1_dcb->iocb_head->pcb->name);
        IOCB *next = com1_dcb->iocb_head->next;
        sys_free_mem(com1_dcb->iocb_head);
        if (next == NULL) {
            com1_dcb->iocb_head = NULL;
            com1_dcb->iocb_tail = NULL;
            return;
        }
        com1_dcb->iocb_head = next;
        if (next->operation == IOCB_READ) {
            serial_read(COM1, next->buffer, next->buffer_size);
        }
        else {
            serial_write(COM1, next->buffer, next->buffer_size);
        }
    }
}
