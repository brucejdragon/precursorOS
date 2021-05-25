; ===========================================================================
;
;             Copyright (C) 2004-2005 Bruce Johnston
;
; ===========================================================================
;
;   //osdev/precursor/Source/Kernel/Architecture/x86/HAL/LockImpl_x86_uni_asm.s
;
; ===========================================================================
;
;	Originating Author:	BruceJ
;	Originating Date:	2005/Mar/31
;
; ===========================================================================
; This file contains part of the implementation of the Lock class.
; ===========================================================================



; ===========================================================================
section .text
align 4

global Lock_acquire

Lock_acquire:
	; Parameters. The function is so short there isn't any point in using ebp.
	%define	lock dword [esp + 4]		; Address of the Lock structure.
	mov eax, lock
	pushfd
	cli
	pop dword [eax]
	ret


global Lock_release

Lock_release:
	; Parameters. The function is so short there isn't any point in using ebp.
	%define	lock dword [esp + 4]		; Address of the Lock structure.
	mov eax, lock
	push dword [eax]
	popfd
	ret

