bits 32
global sys_call_isr



extern sys_call

sys_call_isr:
	
	sub esp, 56

	
	
	mov [esp + 16], eax
	mov [esp + 20], ebx
	mov [esp + 24], ecx
	mov [esp + 28], edx
	mov [esp + 32], esi
	mov [esp + 36], edi
	mov [esp + 40], ebp

	
	mov eax, [esp + 56]      ; eip
	mov [esp + 52], eax
	mov eax, [esp + 60]      ; cs
	mov [esp + 48], ax
	mov eax, [esp + 64]      ; eflags
	mov [esp + 44], eax

	
	mov ax, ds
	mov [esp + 4], ax
	mov ax, es
	mov [esp + 6], ax
	mov ax, fs
	mov [esp + 8], ax
	mov ax, gs
	mov [esp + 10], ax
	mov ax, ss
	mov [esp + 12], ax

	lea eax, [esp]
	mov [esp], eax

	push esp
	call sys_call
	add esp, 4

	mov esp, eax

	mov ebx, eax

	push dword [ebx + 44]
	movzx eax, word [ebx + 48]
	push eax
	push dword [ebx + 52]

	mov ax, [ebx + 4]
	mov ds, ax
	mov ax, [ebx + 6]
	mov es, ax
	mov ax, [ebx + 8]
	mov fs, ax
	mov ax, [ebx + 10]
	mov gs, ax
	

	mov eax, [ebx + 16]
	mov ecx, [ebx + 24]
	mov edx, [ebx + 28]
	mov esi, [ebx + 32]
	mov edi, [ebx + 36]
	mov ebp, [ebx + 40]
	mov ebx, [ebx + 20]

	iret
