bits 32
global sys_call_isr

extern sys_call

sys_call_isr:
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
	
	call sys_call

	mov esp, eax
	
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
