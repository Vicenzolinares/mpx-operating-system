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
	// Validate input parameters
	if (!buffer || len == 0) {
		return -1;
	}

	int dno = serial_devno(dev);
	if (dno == -1 || initialized[dno] == 0) {
		return -1;
	}

	size_t pos = 0;  // Current position in buffer

	// Poll for keyboard input until buffer is full or Enter is pressed
	while (pos < len - 1) {
		// Poll the Line Status Register (LSR) bit 0 to check if data is available
		if (!(inb(dev + LSR) & 0x01)) {
			continue;  // No data available, continue polling
		}

		// Read the character from the Receive Buffer Register (RBR)
		unsigned char ch = inb(dev);

		// Handle Enter/Return key - complete the input
		if (ch == '\r' || ch == '\n') {
			buffer[pos] = '\0';  // Null-terminate the string
			// Echo the newline
			char crlf[2] = {'\r', '\n'};
			serial_out(dev, crlf, 2);
			return (int)pos;
		}

		// Handle Backspace (0x08) or Delete character (0x7F)
		if (ch == 0x08 || ch == 0x7F) {
			if (pos > 0) {
				pos--;  // Move back one position
				// Echo backspace: move cursor back, space, move cursor back again
				char bs[3] = {'\b', ' ', '\b'};
				serial_out(dev, bs, 3);
			}
			continue;
		}

		// Handle Escape sequences (for arrow keys and Delete key)
		// Linux/Ubuntu terminals use ANSI escape sequences
		if (ch == 0x1B) {  // ESC character
			// Wait for the next character in the sequence
			while (!(inb(dev + LSR) & 0x01));
			unsigned char next = inb(dev);

			if (next == '[') {  // ANSI escape sequence
				// Wait for the command character(s)
				while (!(inb(dev + LSR) & 0x01));
				unsigned char code = inb(dev);

				// Handle arrow keys - Linux terminals send ESC[A/B/C/D
				// Extended sequences like ESC[1;2A (with modifiers) are also possible
				if (code == 'A') {  // Up arrow (ESC[A)
					// Arrow keys are typically used for command history
					// For now, we just consume the sequence without action
					continue;
				} else if (code == 'B') {  // Down arrow (ESC[B)
					continue;
				} else if (code == 'C') {  // Right arrow (ESC[C)
					// Could be used for cursor movement in the future
					continue;
				} else if (code == 'D') {  // Left arrow (ESC[D)
					// Could be used for cursor movement in the future
					continue;
				} else if (code >= '0' && code <= '9') {
					// Handle sequences that start with numbers (e.g., ESC[3~ for Delete)
					// Consume any additional numbers and semicolons (for extended sequences)
					while ((code >= '0' && code <= '9') || code == ';') {
						if (!(inb(dev + LSR) & 0x01)) break;
						code = inb(dev);
					}
					// Check if this is the Delete key sequence (ESC[3~)
					if (code == '~') {
						// Delete key pressed - same behavior as backspace
						if (pos > 0) {
							pos--;
							char bs[3] = {'\b', ' ', '\b'};
							serial_out(dev, bs, 3);
						}
					}
					// For other numeric sequences ending in ~, just consume them
					continue;
				}
				// For any other escape sequence characters, just consume and continue
			}
			continue;  // Escape sequence handled, continue polling
		}

		// Validate and filter out other control characters (except printable ones)
		// Control characters are 0x00-0x1F and 0x7F, but we've already handled some
		if (ch < 32 || ch == 127) {
			continue;  // Ignore other control characters
		}

		// Handle normal printable characters
		// Check if we have space in the buffer
		if (pos >= len - 1) {
			// Buffer is full, send bell character to indicate
			char bell = '\a';
			serial_out(dev, &bell, 1);
			break;
		}

		// Store the character and echo it back
		buffer[pos++] = ch;
		serial_out(dev, (char *)&ch, 1);
	}

	// Null-terminate the buffer
	buffer[pos] = '\0';
	return (int)pos;
}
