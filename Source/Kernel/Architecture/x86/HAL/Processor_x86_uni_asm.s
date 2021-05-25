; ===========================================================================
;
;             Copyright (C) 2004-2005 Bruce Johnston
;
; ===========================================================================
;
;   //osdev/precursor/Source/Kernel/Architecture/x86/HAL/Processor_x86_uni_asm.s
;
; ===========================================================================
;
;	Originating Author:	BruceJ
;	Originating Date:	2004/Nov/01
;
; ===========================================================================
; This file contains the low-level implementation details of the Processor
; class. This represents the true core of the microkernel, where processor
; initialization, context switching, and hardware interrupt dispatching happen.
; ===========================================================================


; Import other methods of Processor that we need.
extern Processor_getCurrent
extern Processor_dispatchToHandler
extern Processor_initGdt
extern Processor_initIdt
extern Processor_initTss
extern Processor_initDispatchTable

; Keep these in synch with the definitions in the C file.
GDT_SIZE	equ 6 * 8
IDT_SIZE	equ 49 * 8
CS0_SEL		equ 2 << 3
SS0_SEL		equ 3 << 3


; ===========================================================================
section .data
align 4
; Used by Processor_hardReset() and Processor_initPrimary(). Triggering an
; interrupt when the IDT has size zero will cause a triple-fault and reboot
; the machine.
ZeroSizeIdt:
	dd	0, 0


; ===========================================================================
section .text
align 4


; Define a macro for interrupt/exception handlers that do not produce an error code.
; The parameter is the interrupt vector number (i.e. -- index into the IDT).
%macro IntHandler 1

global Int%1Handler

Int%1Handler:
	; Start building a TrapFrame with info specific to this vector. EFLAGS, CS, and EIP are
	;  already on the stack. If a hardware stack switch occurred, then the ring 3 SS and ESP
	;  are also on the stack.
	push 0			; No error code for this interrupt.
	push %1			; Put the vector number into the TrapFrame.
	jmp EnterKernel	; The rest is common code.

%endmacro


; Define a macro for interrupt/exception handlers that produce an error code.
; The parameter is the interrupt vector number (i.e. -- index into the IDT).
%macro ErrorIntHandler 1

global ErrorInt%1Handler

ErrorInt%1Handler:
	; Start building a TrapFrame with info specific to this vector. EFLAGS, CS, EIP, and the
	;  error code are already on the stack. If a hardware stack switch occurred, then the ring 3
	;  SS and ESP are also on the stack.
	push %1			; Put the vector number into the TrapFrame.
	jmp EnterKernel	; The rest is common code.

%endmacro


; Define entry points for all interrupt handlers.
IntHandler		0		; #DE -- Divide Error
IntHandler		1	 	; #DB -- Debug Exception (Reserved?)
IntHandler		2		; NMI Interrupt
IntHandler		3		; #BP -- Breakpoint
IntHandler		4		; #OF -- Overflow
IntHandler		5		; #BR -- BOUND range exceeded
IntHandler		6		; #UD -- Undefined opcode
IntHandler		7		; #NM -- No math co-processor
ErrorIntHandler	8		; #DF -- Double fault
IntHandler		9		; Coprocessor segment overrun (386 only)
ErrorIntHandler	10		; #TS -- Invalid TSS
ErrorIntHandler	11		; #NP -- Segment not present
ErrorIntHandler 12		; #SS -- Stack segment fault
ErrorIntHandler	13		; #GP -- General protection fault

; #PF -- Page fault. This is a special case because cr2 needs to be saved.
global ErrorInt14Handler

ErrorInt14Handler:
	; Start building a TrapFrame with info specific to this vector. EFLAGS, CS, EIP, and the
	;  error code are already on the stack. If a hardware stack switch occurred, then the ring 3
	;  SS and ESP are also on the stack.
	push 14				; Put the vector number into the TrapFrame.
	pushad				; Save GPRs. This makes eax expendable so we can save cr2.
	mov eax, cr2		; Save cr2 -- it contains the faulting address.
	push eax
	jmp PFEnterKernel	; The rest (almost) is common code.

IntHandler		15		; Intel reserved.
IntHandler		16		; #MF -- Math fault (x87 FPU)
ErrorIntHandler	17		; #AC -- Alignment check
IntHandler		18		; #MC -- Machine check
IntHandler		19		; #XF -- SIMD FP exception
; 20 through 31 are all Intel reserved.
IntHandler		20
IntHandler		21
IntHandler		22
IntHandler		23
IntHandler		24
IntHandler		25
IntHandler		26
IntHandler		27
IntHandler		28
IntHandler		29
IntHandler		30
IntHandler		31
; The hardware interrupt lines are re-directed to the following vectors by ***FIXME code elsewhere
; in the kernel.
IntHandler		32		; Hardware IRQ0
IntHandler		33		; Hardware IRQ1
IntHandler		34		; Hardware IRQ2 (re-directed from the slave PIC. Should never be triggered.)
IntHandler		35		; Hardware IRQ3
IntHandler		36		; Hardware IRQ4
IntHandler		37		; Hardware IRQ5
IntHandler		38		; Hardware IRQ6
IntHandler		39		; Hardware IRQ7
IntHandler		40		; Hardware IRQ8
IntHandler		41		; Hardware IRQ9 (re-directed IRQ2 line)
IntHandler		42		; Hardware IRQ10
IntHandler		43		; Hardware IRQ11
IntHandler		44		; Hardware IRQ12
IntHandler		45		; Hardware IRQ13
IntHandler		46		; Hardware IRQ14
IntHandler		47		; Hardware IRQ15
; The following vector is used for system calls, by Precursor-specific convention.
; For handy reference, this is 0x30 hex. So, userland code must execute int 30h to invoke a
; Precursor system call.
IntHandler		48


EnterKernel:
	; Finish building the TrapFrame.
	pushad				; Push GPRs.
	push 0				; This is not a PF handler, so don't save cr2.

PFEnterKernel:			; PF handler pushes its own GPRs and saves cr2.
	push ds				; Push segment registers.
	push es
	push fs
	push gs
	mov eax, SS0_SEL	; Switch to kernel data segment selector.
	mov ds, eax     	; SS and CS already point to kernel descriptors due to the hardware stack
	mov es, eax			;  switch. If this is an initial entry into the kernel, then this stack
	mov fs, eax			;  switch just happened. Otherwise, it happend on the first entry, but
	mov gs, eax			;  the kernel itself guarantees that CS and SS will not be changed.

	; Get current Processor* in eax.
	call Processor_getCurrent
	push esp			; Save away address of the TrapFrame.
	push eax			; Call dispatchToHandler on the current Processor*.

	; Call the C handler -- it returns the active TrapFrame, which may have been changed.
	call Processor_dispatchToHandler
	; No need to clean up after dispatchToHandler, since it returns the new stack pointer to use.

	; Switch stacks to active TrapFrame. This could have been changed by dispatchToHandler. If
	; not, this does not switch stacks -- it just cleans the params pushed on this stack before
	; dispatchToHandler was called above. If so, this instruction effects a context switch.
	mov esp, eax
	pop gs				; Restore segment registers. If this was an initial entry to the kernel,
	pop fs				;  CS and SS will be restored by hardware. Otherwise, they will remain
	pop es				;  the same until things unwind back to the first kernel entry.
	pop ds
	add esp, 4			; Adjust for cr2.
	popad				; Restore GPRs.
	add esp, 8			; Adjust for error code and vector number.
	iretd				; Dismiss interrupt.


global Processor_initPrimary

Processor_initPrimary:
	; Local variables.
	%define	oldEbx				dword [ebp - 4]		; Saved register.
	%define	gdtAddress			dword [ebp - 8]		; Address of GDT.
	%define	gdtSize				word [ebp - 10]		; Size of GDT in bytes.
	%define gdtInfo				[ebp - 10]			; Base address of 6-byte info about GDT.
	%define	idtAddress			dword [ebp - 16]	; Address of IDT.
	%define	idtSize				word [ebp - 18]		; Size of IDT in bytes.
	%define idtInfo				[ebp - 18]			; Base address of 6-byte info about IDT.

	enter 20, 0					; Allocate space for remaining local variables (32-bit align).
	mov oldEbx, ebx				; Save modified registers.

	; This function calls C functions before the GDT and IDT are properly initialized. This means
	; that if there is a bug in the C initialization code (or in this function for that matter),
	; bad things can happen. To make sure these "bad things" are predictable, we will install a
	; zero-size IDT until the real IDT, GDT and TSS are installed. This will at least ensure that
	; any failures have a predictable result (i.e. -- automatic reboot).
	lidt [ZeroSizeIdt]			; Load the IDTR with a zero-size IDT.

	call Processor_getCurrent	; Get current Processor* and put it in ebx for easy access.
	mov ebx, eax
	push ebx
	call Processor_initGdt		; Call C-land to initialize the GDT.
	add esp, 4
	mov gdtAddress, eax			; Store location and size of GDT for later use.
	mov gdtSize, GDT_SIZE

	push ebx
	call Processor_initIdt		; Call C-land to initialize the IDT.
	add esp, 4
	mov idtAddress, eax			; Store location and size of IDT for later use.
	mov idtSize, IDT_SIZE

	push ebx
	call Processor_initTss		; Call C-land to initialize the TSS. TSS selector returned in ax.
	add esp, 4

	lgdt gdtInfo				; Load the GDTR.
	lidt idtInfo				; Load the IDTR.
	ltr ax						; Load the Task Register.

	jmp CS0_SEL:ReloadCS		; Load kernel code segment selector.

ReloadCS:
	mov eax, SS0_SEL			; Load kernel data segment selector.
	mov ds, eax
	mov es, eax
	mov fs, eax
	mov gs, eax
	mov ss, eax
	
	push ebx
	call Processor_initDispatchTable	; Call C-land to finish initializing the Processor.
	add esp, 4

	mov ebx, oldEbx				; Restore modified registers.
	leave
	ret


global Processor_enableInterrupts

Processor_enableInterrupts:
	sti
	ret


global Processor_disableInterrupts

Processor_disableInterrupts:
	cli
	ret


global Processor_areInterruptsDisabled

Processor_areInterruptsDisabled:
	pushfd
	pop edx
	xor eax, eax		; Must do this before the AND so as not to interfere with the setz.
	and edx, 00000200h
	setz al
	ret


global Processor_halt

Processor_halt:
	cli
	hlt               	; Halt the processor.
	jmp Processor_halt	; Should never get here, but if we do, just loop around and halt again.


global Processor_hardReset

Processor_hardReset:
	lidt [ZeroSizeIdt]	; Load the IDTR with a zero-size IDT.
	int 3				; BOOM! This should cause a triple fault.

	; Just in case it doesn't work, make sure we don't start executing random garbage.
	jmp Processor_halt


global Processor_waitForInterrupt

Processor_waitForInterrupt:
	hlt		; This time, it's ok to return from this function if we wake up.
	ret


