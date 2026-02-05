#include <mpx/io.h>
#include <mpx/serial.h>
#include <sys_req.h>

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

static int initialized[4] = { 0 };

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

<<<<<<< HEAD
	size_t pos = 0;  // Position in buffer
=======
	size_t pos = 0;      // how many chars we have
	size_t cursor = 0;   // where the cursor is
>>>>>>> defc577 (Implement polling-based keyboard input with cursor tracking and special key handling)

	// keep polling until buffer full or enter pressed
	while (pos < len - 1) {
<<<<<<< HEAD
		// Poll (LSR) bit 0 to check if data is available
=======
		// check if data is ready using LSR register
>>>>>>> defc577 (Implement polling-based keyboard input with cursor tracking and special key handling)
		if (!(inb(dev + LSR) & 0x01)) {
			continue;  // nothing available yet
		}

		unsigned char ch = inb(dev);

		// enter key finishes input
		if (ch == '\r' || ch == '\n') {
<<<<<<< HEAD
			buffer[pos] = '\0';  // Terminate the string
			// Echo newline
=======
			buffer[pos] = '\0';
>>>>>>> defc577 (Implement polling-based keyboard input with cursor tracking and special key handling)
			char crlf[2] = {'\r', '\n'};
			serial_out(dev, crlf, 2);
			return (int)pos;
		}

		// backspace or delete char (removes char before cursor)
		if (ch == 0x08 || ch == 0x7F) {
<<<<<<< HEAD
			if (pos > 0) {
				pos--;  // Move back one position
				// Echo backspace
				char bs[3] = {'\b', ' ', '\b'};
				serial_out(dev, bs, 3);
=======
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
>>>>>>> defc577 (Implement polling-based keyboard input with cursor tracking and special key handling)
			}
			continue;
		}

		// escape sequences for arrow keys and delete
		if (ch == 0x1B) {  // ESC
			while (!(inb(dev + LSR) & 0x01));
			unsigned char next = inb(dev);

<<<<<<< HEAD
			if (next == '[') {  
				while (!(inb(dev + LSR) & 0x01));
				unsigned char code = inb(dev);

				
				if (code == 'A') {  // Up arrow (ESC[A)
					
=======
			if (next == '[') {
				while (!(inb(dev + LSR) & 0x01));
				unsigned char code = inb(dev);

				if (code == 'A') {  // up arrow
					continue;  // not implemented
				} else if (code == 'B') {  // down arrow
>>>>>>> defc577 (Implement polling-based keyboard input with cursor tracking and special key handling)
					continue;
				} else if (code == 'C') {  // right arrow
					if (cursor < pos) {
						cursor++;
						serial_out(dev, "\033[C", 3);
					}
					continue;
<<<<<<< HEAD
				} else if (code == 'C') {  // Right arrow (ESC[C)
					
					continue;
				} else if (code == 'D') {  // Left arrow (ESC[D)
					
					continue;
				} else if (code >= '0' && code <= '9') {
					
=======
				} else if (code == 'D') {  // left arrow
					if (cursor > 0) {
						cursor--;
						serial_out(dev, "\033[D", 3);
					}
					continue;
				} else if (code >= '0' && code <= '9') {
					// handle numeric sequences like ESC[3~
>>>>>>> defc577 (Implement polling-based keyboard input with cursor tracking and special key handling)
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
<<<<<<< HEAD
				
					continue;
				}
				
=======
					continue;
				}
>>>>>>> defc577 (Implement polling-based keyboard input with cursor tracking and special key handling)
			}
			continue;
		}

		// filter out control chars we don't want
		if (ch < 32 || ch == 127) {
			continue;
		}

		// normal character input
		if (pos >= len - 1) {
<<<<<<< HEAD
			// Buffer is full, send bell character to indicate
			serial_out(dev, "\a", 1);
=======
			serial_out(dev, "\a", 1);  // bell when full
>>>>>>> defc577 (Implement polling-based keyboard input with cursor tracking and special key handling)
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
