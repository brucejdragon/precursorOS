; ===========================================================================
;
;             Copyright (C) 2004-2005 Bruce Johnston
;
; ===========================================================================
;
;   //osdev/precursor/Source/Kernel/Architecture/x86/HAL/IO.s
;
; ===========================================================================
;
;	Originating Author:	BruceJ
;	Originating Date:	2005/Jan/11
;
; ===========================================================================
; This file contains the implementation of the IO utility class.
; ===========================================================================



; ===========================================================================
section .text
align 4

global IO_in8

IO_in8:
	; Parameters. The function is so short there isn't any point in using ebp.
	%define	port word [esp + 4]		; Port address.

	mov dx, port
	xor eax, eax
	in al, dx
	ret


global IO_in16

IO_in16:
	; Parameters. The function is so short there isn't any point in using ebp.
	%define	port word [esp + 4]		; Port address.

	mov dx, port
	xor eax, eax
	in ax, dx
	ret


global IO_in32

IO_in32:
	; Parameters. The function is so short there isn't any point in using ebp.
	%define	port word [esp + 4]		; Port address.

	mov dx, port
	in eax, dx
	ret


global IO_out8

IO_out8:
	; Parameters. The function is so short there isn't any point in using ebp.
	%define val		byte [esp + 8]	; Value to write.
	%define	port	word [esp + 4]	; Port address.

	mov dx, port
	xor eax, eax
	mov al, val
	out dx, al
	ret


global IO_out16

IO_out16:
	; Parameters. The function is so short there isn't any point in using ebp.
	%define val		word [esp + 8]	; Value to write.
	%define	port	word [esp + 4]	; Port address.

	mov dx, port
	xor eax, eax
	mov ax, val
	out dx, ax
	ret


global IO_out32

IO_out32:
	; Parameters. The function is so short there isn't any point in using ebp.
	%define val		dword [esp + 8]	; Value to write.
	%define	port	word [esp + 4]	; Port address.

	mov dx, port
	mov eax, val
	out dx, eax
	ret

