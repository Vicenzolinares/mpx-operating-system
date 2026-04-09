#include <umem.h>

#include <string.h>
#include <sys_req.h>
#include <mpx/device.h>

static void umem_write(const char *msg)
{
	if (msg == NULL)
	{
		return;
	}

	sys_req(WRITE, COM1, (char *)msg, strlen(msg));
}

static int is_decimal_digit(char c)
{
	return c >= '0' && c <= '9';
}

static int is_hex_digit(char c)
{
	return (c >= '0' && c <= '9')
		|| (c >= 'a' && c <= 'f')
		|| (c >= 'A' && c <= 'F');
}

static int hex_digit_value(char c)
{
	if (c >= '0' && c <= '9')
	{
		return c - '0';
	}

	if (c >= 'a' && c <= 'f')
	{
		return 10 + (c - 'a');
	}

	return 10 + (c - 'A');
}

static int parse_positive_decimal(const char *token, size_t *value)
{
	size_t parsed = 0;

	if (token == NULL || value == NULL || token[0] == '\0')
	{
		return -1;
	}

	for (int i = 0; token[i] != '\0'; i++)
	{
		if (!is_decimal_digit(token[i]))
		{
			return -1;
		}

		parsed = parsed * 10 + (size_t)(token[i] - '0');
	}

	if (parsed == 0)
	{
		return -1;
	}

	*value = parsed;
	return 0;
}

static int parse_hex_address(const char *token, void **address)
{
	size_t parsed = 0;
	int start = 0;

	if (token == NULL || address == NULL || token[0] == '\0')
	{
		return -1;
	}

	if (token[0] == '0' && (token[1] == 'x' || token[1] == 'X'))
	{
		start = 2;
	}

	if (token[start] == '\0')
	{
		return -1;
	}

	for (int i = start; token[i] != '\0'; i++)
	{
		if (!is_hex_digit(token[i]))
		{
			return -1;
		}

		parsed = (parsed * 16) + (size_t)hex_digit_value(token[i]);
	}

	if (parsed == 0)
	{
		return -1;
	}

	*address = (void *)parsed;
	return 0;
}

static void write_hex_address(void *address)
{
	const char hex_chars[] = "0123456789ABCDEF";
	size_t value = (size_t)address;
	char reversed[(sizeof(size_t) * 2) + 1];
	char output[(sizeof(size_t) * 2) + 5];
	int reverse_len = 0;
	int out = 0;

	if (value == 0)
	{
		umem_write("0x0\r\n");
		return;
	}

	while (value > 0)
	{
		reversed[reverse_len++] = hex_chars[value & 0xF];
		value >>= 4;
	}

	output[out++] = '0';
	output[out++] = 'x';

	for (int i = reverse_len - 1; i >= 0; i--)
	{
		output[out++] = reversed[i];
	}

	output[out++] = '\r';
	output[out++] = '\n';
	output[out] = '\0';

	umem_write(output);
}

void allocate_mem(const char *size_token)
{
	size_t size = 0;

	if (parse_positive_decimal(size_token, &size) != 0)
	{
		umem_write("Error: size must be a positive decimal integer.\r\n");
		return;
	}

	void *address = allocate_memory(size);
	if (address == NULL)
	{
		umem_write("Error: allocation failed.\r\n");
		return;
	}

	write_hex_address(address);
}

void free_mem(const char *address_token)
{
	void *address = NULL;

	if (parse_hex_address(address_token, &address) != 0)
	{
		umem_write("Error: address must be a valid hex value.\r\n");
		return;
	}

	if (free_memory(address) != 0)
	{
		umem_write("Error: free failed.\r\n");
	}
}
