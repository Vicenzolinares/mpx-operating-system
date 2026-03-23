bits 32
global sys_call_isr

extern sys_call

sys_call_isr:
	push dword ebp
	push dword edi
	push dword esi
	push dword edx
	push dword ecx
	push dword ebx
	push dword eax

	push dword ss
	push dword gs
	push dword fs
	push dword es
	push dword ds

	push dword esp
	
	call sys_call

	mov dword esp, dword eax
	
	pop dword ds
	pop dword es
	pop dword fs
	pop dword gs
	pop dword ss

	pop dword eax
	pop dword ebx
	pop dword ecx
	pop dword edx
	pop dword esi
	pop dword edi
	pop dword ebp

	iret
