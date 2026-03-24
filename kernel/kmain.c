#include <mpx/gdt.h>
#include <mpx/interrupts.h>
#include <mpx/serial.h>
#include <mpx/vm.h>
#include <sys_req.h>
#include <string.h>
#include <memory.h>
#include <comhand.h>
#include <processes.h>  
#include <kpcb.h>         

static void klogv(device dev, const char *msg)
{
    char prefix[] = "klogv: ";
    serial_out(dev, prefix, strlen(prefix));
    serial_out(dev, msg, strlen(msg));
    serial_out(dev, "\r\n", 2);
}

void kmain(void)
{
    
    serial_init(COM1);
    klogv(COM1, "Initialized serial I/O on COM1...");

    
    klogv(COM1, "Initializing Global Descriptor Table...");
    gdt_init();

  
    klogv(COM1, "Initializing Interrupt Descriptor Table...");
    idt_init();

    
    klogv(COM1, "Disabling interrupts...");
    cli();

   
    klogv(COM1, "Initializing Interrupt Request routines...");
    irq_init();

   
    klogv(COM1, "Initializing Programmable Interrupt Controller...");
    pic_init();

    
    klogv(COM1, "Enabling Interrupts...");
    sti();

 
    klogv(COM1, "Initializing Virtual Memory...");
    vm_init();

   
    klogv(COM1, "Initializing MPX modules...");

  
    klogv(COM1, "Creating command handler...");
    PCB *cmd = pcb_setup("commhand", SYSTEM, 0);

    context *cmd_ctx = (context *)(cmd->stack + PCB_STACK_SIZE - sizeof(context));
    cmd_ctx->eip = (uint32_t)comhand;
    cmd_ctx->eflags = 0x202;
    cmd->stack_pointer = (uint32_t)cmd_ctx;

    pcb_insert(cmd);


    klogv(COM1, "Creating idle process...");
    PCB *idle = pcb_setup("idle", SYSTEM, 9);

    context *idle_ctx = (context *)(idle->stack + PCB_STACK_SIZE - sizeof(context));
    idle_ctx->eip = (uint32_t)sys_idle_process;
    idle_ctx->eflags = 0x202;
    idle->stack_pointer = (uint32_t)idle_ctx;

    pcb_insert(idle);

]
    klogv(COM1, "Starting dispatcher...");
    __asm__ volatile ("int $0x60" :: "a"(IDLE));

   
    klogv(COM1, "Starting system shutdown...");

   
    klogv(COM1, "Halting CPU...");
}
