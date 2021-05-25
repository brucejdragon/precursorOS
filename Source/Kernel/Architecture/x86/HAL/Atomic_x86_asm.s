; ===========================================================================
;
;             Copyright (C) 2004-2005 Bruce Johnston
;
; ===========================================================================
;
;   //osdev/precursor/Source/Kernel/Architecture/x86/HAL/Atomic_x86_asm.s
;
; ===========================================================================
;
;	Originating Author:	BruceJ
;	Originating Date:	2005/Apr/02
;
; ===========================================================================
; This file contains the implementation of some of the Atomic utility functions.
; ===========================================================================



; ===========================================================================
section .text
align 4

global Atomic_compareAndSwap

Atomic_compareAndSwap:
		; Parameters. The function is so short there isn't any point in using ebp.
		%define	targetAddress dword [esp + 4]		; Target address.
		%define compareValue dword [esp + 8]		; Value with which to compare.
		%define updateValue dword [esp + 12]		; Value to write to target address, maybe.

		mov eax, compareValue
		mov ecx, updateValue
		mov edx, targetAddress
lock	cmpxchg [edx], ecx
		setz al
		movzx eax, al	; Can't use xor to clear eax before setz because xor changes ZF.
		ret


global Atomic_swap

Atomic_swap:
		; Parameters. The function is so short there isn't any point in using ebp.
		%define	targetAddress dword [esp + 4]	; Target address.
		%define updateValue dword [esp + 8]		; Value to write to target address.

		mov eax, updateValue
		mov edx, targetAddress
lock	xchg [edx], eax	; lock prefix is supposedly redundant here, but no harm in being clear.
		ret


