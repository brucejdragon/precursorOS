; ===========================================================================
;
;             Copyright (C) 2004-2005 Bruce Johnston
;
; ===========================================================================
;
;   //osdev/precursor/Source/Kernel/Architecture/x86/KCommon/KDebug_x86.s
;
; ===========================================================================
;
;	Originating Author:	BruceJ
;	Originating Date:	2005/Feb/09
;
; ===========================================================================
; This file contains the low-level implementation details of the KDebug
; class. It initializes a function that pushes some debugging information
; on the stack, then triggers a breakpoint exception.
; ===========================================================================


; ===========================================================================
section .text
align 4


global KDebug_triggerDebugTrap

KDebug_triggerDebugTrap:
	; The stack currently looks like this:
	;	line	(int)
	;	file	(const char*)
	;	msg		(const char*)
	;	retaddr	<-- top of stack
	; No need to copy anything around -- the breakpoint handler can figure out where to find these
	; items.
	int 3	; Trap to breakpoint handler.
	ret

