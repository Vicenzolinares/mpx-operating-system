bits 32
global serial_isr

extern serial_interrupt

serial_isr:
	push ebp
	push edi
	push esi
	push edx
	push ecx
	push ebx
	push eax

	push ss
	push gs
	push fs
	push es
	push ds

	push esp
	call serial_interrupt
	add esp, 4

	pop ds
	pop es
	pop fs
	pop gs
	pop ss

	pop eax
	pop ebx
	pop ecx
	pop edx
	pop esi
	pop edi
	pop ebp

	iret
