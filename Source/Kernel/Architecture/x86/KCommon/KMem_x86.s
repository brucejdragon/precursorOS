; ===========================================================================
;
;             Copyright (C) 2004-2005 Bruce Johnston
;
; ===========================================================================
;
;   //osdev/precursor/Source/Kernel/Architecture/x86/KMem_x86.s
;
; ===========================================================================
;
;	Originating Author:	BruceJ
;	Originating Date:	2005/Jan/16
;
; ===========================================================================
; This file contains the implementation of the KMem utility class.
; ===========================================================================



; ===========================================================================
section .text
align 4

global KMem_copy

KMem_copy:
	; Parameters. Avoid using ebp for performance reasons.
	%define	dest		dword [esp + 4]		; Destination address.
	%define source		dword [esp + 8]		; Source address.
	%define numBytes	dword [esp + 12]	; Byte count.

	; ASSUMPTIONS:
	; - DF in EFLAGS is clear, as it should be according to C calling conventions.
	; - ES is set up to access the kernel-mode data segment (it should never be changed anywhere
	;	else in the kernel).

	;***FIXME: Optimize later. For now, just do byte-by-byte copy.
	mov eax, esi	; Save current value.
	mov edx, edi	; Save current value.

	mov edi, dest
	mov esi, source
	mov ecx, numBytes
	rep movsb

	mov edi, edx	; Restore old value.
	mov esi, eax	; Restore old value.
	ret


global KMem_move

KMem_move:
	; Parameters. Avoid using ebp for performance reasons.
	%define	dest		dword [esp + 4]		; Destination address.
	%define source		dword [esp + 8]		; Source address.
	%define numBytes	dword [esp + 12]	; Byte count.

	; ASSUMPTIONS:
	; - DF in EFLAGS is clear, as it should be according to C calling conventions.
	; - ES is set up to access the kernel-mode data segment (it should never be changed anywhere
	;	else in the kernel).

	;***FIXME: Optimize later. For now, just do byte-by-byte move.

	mov eax, esi	; Save current value.
	mov edx, edi	; Save current value.

	mov edi, dest
	mov esi, source
	mov ecx, numBytes

	; Save old flags in case we need to mess with direction flag. Must do this *after* getting
	; parameters above (because it affects esp)!
	pushfd

	; See if we need to copy backwards...
	cmp edi, esi
	jb DoCopy
	std		; Prepare to copy backwards.

DoCopy:
	rep movsb

	popfd			; Restore old flags.

	mov edi, edx	; Restore old value.
	mov esi, eax	; Restore old value.
	ret


global KMem_set

KMem_set:
	; Parameters. Avoid using ebp for performance reasons.
	%define	dest		dword [esp + 4]		; Destination address.
	%define val			byte [esp + 8]		; Value.
	%define numBytes	dword [esp + 12]	; Byte count.

	; ASSUMPTIONS:
	; - DF in EFLAGS is clear, as it should be according to C calling conventions.
	; - ES is set up to access the kernel-mode data segment (it should never be changed anywhere
	;	else in the kernel).

	;***FIXME: Optimize later. For now, just do byte-by-byte fill.
	mov edx, edi	; Save it for later.

	mov edi, dest
	mov al, val
	mov ecx, numBytes
	rep stosb

	mov edi, edx	; Restore old value.
	ret


global KMem_low8

KMem_low8:
	; Parameters. Avoid using ebp for performance reasons.
	%define val		byte [esp + 4]	; Point directly at the desired byte...

	xor eax, eax
	mov al, val
	ret


global KMem_high8

KMem_high8:
	; Parameters. Avoid using ebp for performance reasons.
	%define val		byte [esp + 5]	; Point directly at the desired byte...

	xor eax, eax
	mov al, val
	ret


global KMem_low16

KMem_low16:
	; Parameters. Avoid using ebp for performance reasons.
	%define val		word [esp + 4]	; Point directly at the desired word...

	xor eax, eax
	mov ax, val
	ret


global KMem_high16

KMem_high16:
	; Parameters. Avoid using ebp for performance reasons.
	%define val		word [esp + 6]	; Point directly at the desired word...

	xor eax, eax
	mov ax, val
	ret


global KMem_findLowestSetBit

KMem_findLowestSetBit:
	; Parameters. Avoid using ebp for performance reasons.
	%define val		dword [esp + 4]	; Value to scan.

	bsf eax, val
	jnz .done
	mov eax, 0xFFFFFFFF
.done:
	ret

