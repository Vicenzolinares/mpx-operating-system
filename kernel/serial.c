#include <mpx/io.h>
#include <mpx/serial.h>
#include <mpx/interrupts.h>
#include <sys_req.h>
#include <stddef.h>

enum uart_registers {
	RBR = 0,	// Receive Buffer
	THR = 0,	// Transmitter Holding
	DLL = 0,	// Divisor Latch LSB
	IER = 1,	// Interrupt Enable
	DLM = 1,	// Divisor Latch MSB
	IIR = 2,	// Interrupt Identification
	FCR = 2,	// FIFO Control
	LCR = 3,	// Line Control
	MCR = 4,	// Modem Control
	LSR = 5,	// Line Status
	MSR = 6,	// Modem Status
	SCR = 7,	// Scratch
};

#define PIC1_DATA_PORT 0x21
#define COM1_IRQ_MASK_BIT 0x10
#define IER_RX_INT 0x01
#define IER_TX_INT 0x02
#define IIR_NO_PENDING 0x01
#define IIR_ID_MASK 0x06
#define IIR_ID_RX_AVAILABLE 0x04
#define IIR_ID_TX_EMPTY 0x02
#define LSR_DATA_READY 0x01
#define LSR_THR_EMPTY 0x20

static int initialized[4] = { 0 };
static struct dcb com1_dcb_init = { 0 };
struct dcb* com1_dcb = &com1_dcb_init;

extern void serial_isr(void *);

static int serial_devno(device dev)
{
	switch (dev) {
	case COM1: return 0;
	case COM2: return 1;
	case COM3: return 2;
	case COM4: return 3;
	}
	return -1;
}

static void dcb_setup(void)
{
	com1_dcb->open = 1;
	com1_dcb->reading = 0;
	com1_dcb->writing = 0;
	com1_dcb->read_buf = NULL;
	com1_dcb->read_size = 0;
	com1_dcb->read_count = 0;
	com1_dcb->write_buf = NULL;
	com1_dcb->write_size = 0;
	com1_dcb->write_count = 0;
	com1_dcb->input_head = 0;
	com1_dcb->input_tail = 0;
	com1_dcb->iocb_head = NULL;
	com1_dcb->iocb_tail = NULL;
}

static void enable_com1_irq(void)
{
	unsigned char mask = inb(PIC1_DATA_PORT);
	mask = (unsigned char)(mask & (unsigned char)~COM1_IRQ_MASK_BIT);
	outb(PIC1_DATA_PORT, mask);
}

static void disable_com1_irq(void)
{
	unsigned char mask = inb(PIC1_DATA_PORT);
	mask = (unsigned char)(mask | COM1_IRQ_MASK_BIT);
	outb(PIC1_DATA_PORT, mask);
}

static size_t ring_used(void)
{
	if (com1_dcb->input_head >= com1_dcb->input_tail) {
		return com1_dcb->input_head - com1_dcb->input_tail;
	}

	return READ_BUFFER_SIZE - (com1_dcb->input_tail - com1_dcb->input_head);
}

static int ring_pop(char *out)
{
	if (ring_used() == 0 || out == NULL) {
		return 0;
	}

	*out = com1_dcb->input_ring[com1_dcb->input_tail];
	com1_dcb->input_tail = (com1_dcb->input_tail + 1) % READ_BUFFER_SIZE;
	return 1;
}

static void ring_push(char ch)
{
	size_t next = (com1_dcb->input_head + 1) % READ_BUFFER_SIZE;
	if (next == com1_dcb->input_tail) {
		return;
	}

	com1_dcb->input_ring[com1_dcb->input_head] = ch;
	com1_dcb->input_head = next;
}

static void try_finish_read_with_buffered_data(void)
{
	char ch = '\0';
	int ended_with_newline = 0;
	while (com1_dcb->reading &&
		com1_dcb->read_count < com1_dcb->read_size &&
		ring_pop(&ch)) {
		if (ch == '\r' || ch == '\n') {
			com1_dcb->read_buf[com1_dcb->read_count] = '\0';
			ended_with_newline = 1;
			break;
		}
		com1_dcb->read_buf[com1_dcb->read_count++] = ch;
	}

	if (com1_dcb->reading &&
		(com1_dcb->read_count == com1_dcb->read_size ||
		 ended_with_newline)) {
		com1_dcb->reading = 0;
		if (com1_dcb->read_buf != NULL && com1_dcb->read_count < com1_dcb->read_size) {
			com1_dcb->read_buf[com1_dcb->read_count] = '\0';
		}
		outb(COM1 + IER, IER_RX_INT | (com1_dcb->writing ? IER_TX_INT : 0));
	}
}

int serial_init(device dev)
{
	int dno = serial_devno(dev);
	if (dno == -1) {
		return -1;
	}
	outb(dev + IER, 0x00);	//disable interrupts
	outb(dev + LCR, 0x80);	//set line control register
	outb(dev + DLL, 115200 / 9600);	//set bsd least sig bit
	outb(dev + DLM, 0x00);	//brd most significant bit
	outb(dev + LCR, 0x03);	//lock divisor; 8bits, no parity, one stop
	outb(dev + FCR, 0xC7);	//enable fifo, clear, 14byte threshold
	outb(dev + MCR, 0x0B);	//enable interrupts, rts/dsr set
	(void)inb(dev);		//read bit to reset port
	initialized[dno] = 1;
	return 0;
}

int serial_out(device dev, const char *buffer, size_t len)
{
	int dno = serial_devno(dev);
	if (dno == -1 || initialized[dno] == 0) {
		return -1;
	}
	for (size_t i = 0; i < len; i++) {
		outb(dev, buffer[i]);
	}
	return (int)len;
}

int serial_poll(device dev, char *buffer, size_t len)
{
	// check if buffer is valid
	if (!buffer || len == 0) {
		return -1;
	}

	int dno = serial_devno(dev);
	if (dno == -1 || initialized[dno] == 0) {
		return -1;
	}

	size_t pos = 0;      // how many chars we have
	size_t cursor = 0;   // where the cursor is

	// keep polling until buffer full or enter pressed
	while (pos < len - 1) {

		// check if data is ready using LSR register
		if (!(inb(dev + LSR) & 0x01)) {
			continue;  // nothing available yet
		}

		unsigned char ch = inb(dev);

		// enter key finishes input
		if (ch == '\r' || ch == '\n') {
			buffer[pos] = '\0';
			char crlf[2] = {'\r', '\n'};
			serial_out(dev, crlf, 2);
			return (int)pos;
		}

		// backspace or delete char (removes char before cursor)
		if (ch == 0x08 || ch == 0x7F) {
			if (cursor > 0) {
				// shift everything left
				for (size_t i = cursor - 1; i < pos; i++) {
					buffer[i] = buffer[i + 1];
				}
				pos--;
				cursor--;

				// redraw the whole line
				serial_out(dev, "\r", 1);
				serial_out(dev, buffer, pos);

				// clear leftover chars
				for (size_t i = pos; i < len; i++) {
					serial_out(dev, " ", 1);
				}

				// put cursor back where it should be
				serial_out(dev, "\r", 1);
				serial_out(dev, buffer, cursor);
			}
			continue;
		}

		// escape sequences for arrow keys and delete
		if (ch == 0x1B) {  // ESC
			while (!(inb(dev + LSR) & 0x01));
			unsigned char next = inb(dev);

			if (next == '[') {
				while (!(inb(dev + LSR) & 0x01));
				unsigned char code = inb(dev);

				if (code == 'A') {  // up arrow
					continue;  // not implemented
				} else if (code == 'B') {  // down arrow
					continue;
				} else if (code == 'C') {  // right arrow
					if (cursor < pos) {
						cursor++;
						serial_out(dev, "\033[C", 3);
					}
					continue;
				} else if (code == 'D') {  // left arrow
					if (cursor > 0) {
						cursor--;
						serial_out(dev, "\033[D", 3);
					}
					continue;
				} else if (code >= '0' && code <= '9') {
					// handle numeric sequences like ESC[3~
					while ((code >= '0' && code <= '9') || code == ';') {
						if (!(inb(dev + LSR) & 0x01)) break;
						code = inb(dev);
					}
					if (code == '~') {  // delete key
						if (cursor < pos) {
							// remove char at cursor
							for (size_t i = cursor; i < pos; i++) {
								buffer[i] = buffer[i + 1];
							}
							pos--;

							// redraw
							serial_out(dev, "\r", 1);
							serial_out(dev, buffer, pos);

							// clear rest
							for (size_t i = pos; i < len; i++) {
								serial_out(dev, " ", 1);
							}

							serial_out(dev, "\r", 1);
							serial_out(dev, buffer, cursor);
						}
					}
					continue;
				}
			}
			continue;
		}

		// filter out control chars we don't want
		if (ch < 32 || ch == 127) {
			continue;
		}

		// normal character input
		if (pos >= len - 1) {
			serial_out(dev, "\a", 1);  // bell when full
			break;
		}

		if (cursor == pos) {
			// append at end
			buffer[pos++] = ch;
			cursor++;
			serial_out(dev, (char *)&ch, 1);
		} else {
			// insert in middle
			for (size_t i = pos; i > cursor; i--) {
				buffer[i] = buffer[i - 1];
			}
			buffer[cursor] = ch;
			pos++;

			// redraw everything
			serial_out(dev, "\r", 1);
			serial_out(dev, buffer, pos);

			cursor++;

			// move cursor to right spot
			serial_out(dev, "\r", 1);
			serial_out(dev, buffer, cursor);
		}
	}

	buffer[pos] = '\0';
	return (int)pos;
}

int serial_open(device dev, int speed)
{
	if (dev != COM1 || speed != 9600) {
		return -1;
	}

	if (com1_dcb->open) {
		return -1;
	}

	if (serial_init(COM1) != 0) {
		return -1;
	}

	dcb_setup();
	idt_install(0x24, serial_isr);
	enable_com1_irq();
	outb(COM1 + IER, IER_RX_INT);
	return 0;
}

int serial_close(device dev)
{
	if (dev != COM1 || !com1_dcb->open) {
		return -1;
	}
	if (com1_dcb->reading || com1_dcb->writing) {
		return -1;
	}

	outb(COM1 + IER, 0x00);
	disable_com1_irq();
	com1_dcb->open = 0;
	com1_dcb->reading = 0;
	com1_dcb->writing = 0;
	com1_dcb->read_buf = NULL;
	com1_dcb->write_buf = NULL;
	com1_dcb->read_size = 0;
	com1_dcb->write_size = 0;
	com1_dcb->read_count = 0;
	com1_dcb->write_count = 0;
	com1_dcb->input_head = 0;
	com1_dcb->input_tail = 0;
	com1_dcb->iocb_head = NULL;
	com1_dcb->iocb_tail = NULL;
	return 0;
}

int serial_read(device dev, char *buf, size_t size)
{
	if (dev != COM1 || !com1_dcb->open || buf == NULL || size == 0) {
		return -1;
	}

	if (com1_dcb->reading) {
		return -1;
	}

	if (size == 1) {
		buf[0] = '\0';
		return 0;
	}

	com1_dcb->read_buf = buf;
	com1_dcb->read_size = size - 1;
	com1_dcb->read_count = 0;
	com1_dcb->reading = 1;
	com1_dcb->read_buf[0] = '\0';

	try_finish_read_with_buffered_data();
	outb(COM1 + IER, IER_RX_INT | (com1_dcb->writing ? IER_TX_INT : 0));
	return 0;
}

int serial_write(device dev, const char *buf, size_t size)
{
	if (dev != COM1 || !com1_dcb->open || buf == NULL || size == 0) {
		return -1;
	}

	if (com1_dcb->writing) {
		return -1;
	}

	com1_dcb->write_buf = buf;
	com1_dcb->write_size = size;
	com1_dcb->write_count = 0;
	com1_dcb->writing = 1;

	serial_output_interrupt();
	if (com1_dcb->writing) {
		outb(COM1 + IER, IER_RX_INT | IER_TX_INT);
	}
	return 0;
}

void serial_interrupt(void *int_frame)
{
	(void)int_frame;

	while ((inb(COM1 + IIR) & IIR_NO_PENDING) == 0) {
		unsigned char reason = (unsigned char)(inb(COM1 + IIR) & IIR_ID_MASK);
		if (reason == IIR_ID_RX_AVAILABLE) {
			serial_input_interrupt();
		} else if (reason == IIR_ID_TX_EMPTY) {
			serial_output_interrupt();
		} else {
			(void)inb(COM1 + LSR);
			break;
		}
	}

	outb(0x20, 0x20);
}

void serial_input_interrupt(void)
{
	while (inb(COM1 + LSR) & LSR_DATA_READY) {
		char ch = (char)inb(COM1 + RBR);
		if (com1_dcb->reading && com1_dcb->read_count < com1_dcb->read_size) {
			if (ch == '\r' || ch == '\n') {
				com1_dcb->read_buf[com1_dcb->read_count] = '\0';
				com1_dcb->reading = 0;
				outb(COM1 + IER, IER_RX_INT | (com1_dcb->writing ? IER_TX_INT : 0));
			} else {
				com1_dcb->read_buf[com1_dcb->read_count++] = ch;
			}
			if (com1_dcb->reading && com1_dcb->read_count == com1_dcb->read_size) {
				com1_dcb->read_buf[com1_dcb->read_count] = '\0';
				com1_dcb->reading = 0;
				outb(COM1 + IER, IER_RX_INT | (com1_dcb->writing ? IER_TX_INT : 0));
			}
		} else {
			ring_push(ch);
		}
	}
}

void serial_output_interrupt(void)
{
	while (com1_dcb->writing &&
		com1_dcb->write_count < com1_dcb->write_size &&
		(inb(COM1 + LSR) & LSR_THR_EMPTY)) {
		outb(COM1 + THR, com1_dcb->write_buf[com1_dcb->write_count++]);
	}

	if (com1_dcb->writing && com1_dcb->write_count >= com1_dcb->write_size) {
		com1_dcb->writing = 0;
		com1_dcb->write_buf = NULL;
		com1_dcb->write_size = 0;
		com1_dcb->write_count = 0;
		outb(COM1 + IER, IER_RX_INT);
	}
}

