bits 32
global sys_call_isr

;;; System call interrupt handler (vector 0x60). Builds struct context on the
;;; stack (layout must match include/mpx/sys_call.h), calls sys_call, then
;;; switches to the returned context and irets.

extern sys_call

sys_call_isr:
	;;; Reserve full struct context (64 bytes); CPU frame (EIP, CS, EFLAGS)
	;;; remains at [esp+64], [esp+68], [esp+72].
	sub esp, 64

	mov [esp + 20], eax
	mov [esp + 24], ebx
	mov [esp + 28], ecx
	mov [esp + 32], edx
	mov [esp + 36], esi
	mov [esp + 40], edi
	mov [esp + 44], ebp

	mov eax, [esp + 64]
	mov [esp + 56], eax
	mov eax, [esp + 68]
	mov [esp + 52], ax
	mov eax, [esp + 72]
	mov [esp + 48], eax

	mov dword [esp + 4], 0
	mov word [esp + 18], 0

	mov ax, ds
	mov [esp + 8], ax
	mov ax, es
	mov [esp + 10], ax
	mov ax, fs
	mov [esp + 12], ax
	mov ax, gs
	mov [esp + 14], ax
	mov ax, ss
	mov [esp + 16], ax

	mov dword [esp + 60], 0

	lea eax, [esp]
	mov [esp], eax

	push esp
	call sys_call
	add esp, 4

	mov esp, eax

	mov ebx, eax

	push dword [ebx + 48]
	movzx eax, word [ebx + 52]
	push eax
	push dword [ebx + 56]

	mov ax, [ebx + 8]
	mov ds, ax
	mov ax, [ebx + 10]
	mov es, ax
	mov ax, [ebx + 12]
	mov fs, ax
	mov ax, [ebx + 14]
	mov gs, ax
	

	mov eax, [ebx + 20]
	mov ecx, [ebx + 28]
	mov edx, [ebx + 32]
	mov esi, [ebx + 36]
	mov edi, [ebx + 40]
	mov ebp, [ebx + 44]
	mov ebx, [ebx + 24]

	iret
