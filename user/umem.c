#include <umem.h>
#include <string.h>
#include <sys_req.h>
#include <mpx/device.h>
#include <ctype.h>

static void mpx_write(const char *s)
{
	if (s == NULL)
		return;

	sys_req(WRITE, COM1, (char *)s, strlen(s));
}

static int parse_decimal_size(const char *s, size_t *out)
{
	size_t value = 0;
	int saw_digit = 0;

	if (s == NULL || out == NULL)
		return -1;

	while (isspace(*s)) {
		s++;
	}

	while (*s >= '0' && *s <= '9') {
		size_t digit = (size_t)(*s - '0');
		size_t next = value * 10 + digit;

		if (next < value)
			return -1;

		value = next;
		s++;
		saw_digit = 1;
	}

	while (isspace(*s)) {
		s++;
	}

	if (!saw_digit || *s != '\0' || value == 0)
		return -1;

	*out = value;
	return 0;
}

static int hex_value(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

static int parse_hex_address(const char *s, void **out)
{
	size_t value = 0;
	int saw_digit = 0;

	if (s == NULL || out == NULL)
		return -1;

	while (isspace(*s)) {
		s++;
	}

	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
		s += 2;
	}

	while (*s != '\0') {
		int digit = hex_value(*s);

		if (digit < 0)
			break;

		if (value > (((size_t)-1) >> 4))
			return -1;

		value = (value << 4) | (size_t)digit;
		s++;
		saw_digit = 1;
	}

	while (isspace(*s)) {
		s++;
	}

	if (!saw_digit || *s != '\0' || value == 0)
		return -1;

	*out = (void *)value;
	return 0;
}

static void to_hex_string(size_t value, char *out)
{
	char temp[(sizeof(size_t) * 2) + 1];
	int i = 0;
	int j;

	if (value == 0) {
		out[0] = '0';
		out[1] = 'x';
		out[2] = '0';
		out[3] = '\0';
		return;
	}

	while (value > 0) {
		temp[i++] = "0123456789ABCDEF"[value & 0xF];
		value >>= 4;
	}

	out[0] = '0';
	out[1] = 'x';

	for (j = 0; j < i; j++) {
		out[2 + j] = temp[i - 1 - j];
	}

	out[2 + i] = '\0';
}

void allocate_mem(const char *size_arg)
{
	size_t size;
	void *ptr;
	char address_text[(sizeof(size_t) * 2) + 3];

	if (parse_decimal_size(size_arg, &size) != 0) {
		mpx_write("Error: Invalid size for 'allocate memory'.\r\n");
		return;
	}

	ptr = allocate_memory(size);
	if (ptr == NULL) {
		mpx_write("Error: Memory allocation failed.\r\n");
		return;
	}

	to_hex_string((size_t)ptr, address_text);
	mpx_write("Allocated memory at ");
	mpx_write(address_text);
	mpx_write("\r\n");
}

void free_mem(const char *address_arg)
{
	void *address;

	if (parse_hex_address(address_arg, &address) != 0) {
		mpx_write("Error: Invalid address for 'free memory'.\r\n");
		return;
	}

	if (free_memory(address) != 0) {
		mpx_write("Error: Memory free failed.\r\n");
		return;
	}

	mpx_write("Memory freed.\r\n");
}
