#include <mpx/gdt.h>
#include <mpx/interrupts.h>
#include <mpx/serial.h>
#include <mpx/vm.h>
#include <sys_req.h>
#include <string.h>
#include <memory.h>
#include <umem.h>
#include <comhand.h>
#include <processes.h>  
#include <mpx/kpcb.h>
#include <upcb.h>

static void klogv(device dev, const char *msg)
{
    char prefix[] = "klogv: ";
    serial_out(dev, prefix, strlen(prefix));
    serial_out(dev, msg, strlen(msg));
    serial_out(dev, "\r\n", 2);
}

void kmain(void)
{
    // 0) Serial I/O -- <mpx/serial.h>
	// If we don't initialize the serial port, we have no way of
	// performing I/O. So we need to do that before anything else so we
	// can at least get some output on the screen.
	// Note that here, you should call the function *before* the output
	// via klogv(), or the message won't print. In all other cases, the
	// output should come first as it describes what is about to happen.
    serial_init(COM1);
    klogv(COM1, "Initialized serial I/O on COM1...");

    // 1) Global Descriptor Table (GDT) -- <mpx/gdt.h>
	// Keeps track of the various memory segments (Code, Data, Stack, etc.)
	// required by the x86 architecture. This needs to be initialized before
	// interrupts can be configured.
    klogv(COM1, "Initializing Global Descriptor Table...");
    gdt_init();

    // 2) Interrupt Descriptor Table (IDT) -- <mpx/interrupts.h>
	// Keeps track of where the various Interrupt Vectors are stored. It
	// needs to be initialized before Interrupt Service Routines (ISRs) can
	// be installed.
    klogv(COM1, "Initializing Interrupt Descriptor Table...");
    idt_init();

    // 3) Disable Interrupts -- <mpx/interrupts.h>
	// You'll be modifying how interrupts work, so disable them to avoid
	// crashing.
    klogv(COM1, "Disabling interrupts...");
    cli();

   // 4) Interrupt Request (IRQ) -- <mpx/interrupts.h>
	// The x86 architecture requires ISRs for at least the first 32
	// Interrupt Request (IRQ) lines.
    klogv(COM1, "Initializing Interrupt Request routines...");
    irq_init();

    // 5) Programmable Interrupt Controller (PIC) -- <mpx/interrupts.h>
	// The x86 architecture uses a Programmable Interrupt Controller (PIC)
	// to map hardware interrupts to software interrupts that the CPU can
	// then handle via the IDT and its list of ISRs.
    klogv(COM1, "Initializing Programmable Interrupt Controller...");
    pic_init();

    // 6) Reenable interrupts -- <mpx/interrupts.h>
	// Now that interrupt routines are set up, allow interrupts to happen
	// again.
    klogv(COM1, "Enabling Interrupts...");
    sti();

    // 7) Virtual Memory (VM) -- <mpx/vm.h>
	// Virtual Memory (VM) allows the CPU to map logical addresses used by
	// programs to physical address in RAM. This allows each process to
	// behave as though it has exclusive access to memory. It also allows,
	// in more advanced systems, the kernel to swap programs between RAM and
	// storage (such as a hard drive or SSD), and to set permissions such as
	// Read, Write, or Execute for pages of memory. VM is managed through
	// Page Tables, data structures that describe the logical-to-physical
	// mapping as well as manage permissions and other metadata.
    klogv(COM1, "Initializing Virtual Memory...");
    vm_init();

    // 8) MPX Modules -- *headers vary*
	// Module specific initialization -- not all modules require this.
    klogv(COM1, "Initializing MPX modules...");
    klogv(COM1, "Initializing heap manager...");
    initialize_heap(50000);
	// R4: create commhand and idle processes

    // create commhand process
    klogv(COM1, "Creating commhand process...");
    create_pcb("commhand", SYSTEM, 0);
    PCB *cmd = pcb_find("commhand");

    struct context *cmd_context;

	cmd->stack_pointer = cmd->stack_pointer - sizeof(struct context);
	cmd_context = (struct context *) cmd->stack_pointer;
	cmd_context->cs = 0x08;
	cmd_context->ds = 0x10;
	cmd_context->es = 0x10;
	cmd_context->fs = 0x10;
	cmd_context->gs = 0x10;
	cmd_context->ss = 0x10;
	cmd_context->ebp = (int) cmd->stack;
	cmd_context->eip = (int) comhand;
	cmd_context->eflags = 0x0202;
	cmd_context->eax = 0;
	cmd_context->ebx = 0;
	cmd_context->ecx = 0;
	cmd_context->edx = 0;
	cmd_context->esi = 0;
	cmd_context->edi = 0;

    // create idle process
    klogv(COM1, "Creating idle process...");
    create_pcb("idle", SYSTEM, 9);
    PCB *idle = pcb_find("idle");
    
    struct context *idle_context;

	idle->stack_pointer = idle->stack_pointer - sizeof(struct context);
	idle_context = (struct context *) idle->stack_pointer;
	idle_context->cs = 0x08;
	idle_context->ds = 0x10;
	idle_context->es = 0x10;
	idle_context->fs = 0x10;
	idle_context->gs = 0x10;
	idle_context->ss = 0x10;
	idle_context->ebp = (int) idle->stack;
	idle_context->eip = (int) sys_idle_process;
	idle_context->eflags = 0x0202;
	idle_context->eax = 0;
	idle_context->ebx = 0;
	idle_context->ecx = 0;
	idle_context->edx = 0;
	idle_context->esi = 0;
	idle_context->edi = 0;

    // 9) YOUR command handler -- *create and #include an appropriate .h file*
	// Pass execution to your command handler so the user can interact with
	// the system.
    klogv(COM1, "Transferring control to commhand...");
    __asm__ volatile ("int $0x60" :: "a"(IDLE));

    // 10) System Shutdown -- *headers to be determined by your design*
	// After your command handler returns, take care of any clean up that
	// is necessary.
    klogv(COM1, "Starting system shutdown...");

    // 11) Halt CPU -- *no headers necessary, no changes necessary*
	// Execution of kmain() will complete and return to where it was called
	// in boot.s, which will then attempt to power off Qemu or halt the CPU.
    klogv(COM1, "Halting CPU...");
}
