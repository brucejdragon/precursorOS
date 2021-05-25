; ===========================================================================
;
;             Copyright (C) 2004-2005 Bruce Johnston
;
; ===========================================================================
;
;   //osdev/precursor/Source/Kernel/Architecture/x86/Boot/Boot_x86_uni.s
;
; ===========================================================================
;
;	Originating Author:	BruceJ
;	Originating Date:	2004/Sep/27
;
; ===========================================================================
; Based on the "BareBones" NASM sample at <http://mega-tokyo.com/osfaq2/>.
; Big thanks to Brendan Trotter, Pype.Clicker (a.k.a. Sylvain Martin), Solar (a.k.a.
; Martin Baute), Candy (a.k.a. Peter Bindels), Dreamsmith, bkilgore, and
; beyond_infinity for being patient with my questions. I don't know all of your
; real names, but you know who you are!
;
; This file implements the initial start-up code for the kernel. It is
; responsible for making sure that the kernel is compatible with the bootloader's
; protocol (in the case of x86, by following the Multiboot specification).
; ===========================================================================

global StartPrecursor						; Make entry point visible to linker.
extern kmain								; Make the linker look for kmain() in C-land.
extern Processor_initPrimary				; Need this to initialize the Processor.
extern BootLoaderInfoTranslator_translate	; Need this to make the Multiboot info palatable
											;  to kmain().
extern BssEndPhys							; End of bss (physical address); from linker script.

; Setting up the Multiboot header - see GRUB docs for details.
MODULEALIGN	equ	1<<0				; Align loaded modules on page boundaries.
MEMINFO  	equ	1<<1				; Provide memory map.
USEHEADER	equ	1<<16				; Use Multiboot header, don't rely on ELF.
FLAGS    	equ MODULEALIGN | MEMINFO | USEHEADER
									; This is the Multiboot 'flag' field.
MAGIC    	equ 0x1BADB002  		; 'Magic number' lets bootloader find the header.
CHECKSUM 	equ -(MAGIC + FLAGS)	; Checksum required.

; This is the virtual base address of kernel space. It must be used to convert virtual
; linker addresses into physical addresses for the Multiboot header. Note that this is not
; the virtual address where the kernel image itself is loaded -- just the amount that must
; be subtracted from a virtual address to get a physical address.
;
; MAINTENANCE NOTE: This must match the definition in Kernel/HAL/Mem.h.
KERNEL_VIRTUAL_BASE equ 0xE0000000					; 3.5GB
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22)	; Page dir. index of kernel's 4MB PTE.


; ===========================================================================
section .data
align 0x1000
BootPageDirectory:
	; This page directory entry identity-maps the first 4MB of the 32-bit physical address space.
	; All bits are clear except the following:
	; bit 7: PS	The kernel page is 4MB.
	; bit 1: RW	The kernel page is read/write.
	; bit 0: P	The kernel page is present.
	; This entry must be here -- otherwise the kernel will crash immediately after paging is
	; enabled because it can't fetch the next instruction! It's ok to unmap this page later.
	dd 0x00000083
	times (KERNEL_PAGE_NUMBER - 1) dd 0			; Pages before kernel space.
	; This page directory entry defines a 4MB page containing the kernel image plus bss.
	dd 0x00000083
	times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0	; Pages after the kernel image.

; Used to ensure that any problem with setting up boot-time paging bails immediately.
; Triggering a page fault when the IDT has size zero will cause a triple-fault and reboot
; the machine, which is probably safer than just letting it run into random trouble.
ZeroSizeIdt:
	dd	0, 0


; ===========================================================================
section .text
align 4
MultiBootHeader:
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
	dd MultiBootHeader - KERNEL_VIRTUAL_BASE	; header_addr:		phys. addr. of multiboot header
	dd MultiBootHeader - KERNEL_VIRTUAL_BASE	; load_addr:		start loading at header
	dd 0										; load_end_addr:	text + data occupies whole image
	dd BssEndPhys								; bss_end_addr:		end of BSS section (phys addr).
	dd StartPrecursor - KERNEL_VIRTUAL_BASE		; entry_addr:		OS entry point

; reserve initial kernel stack space -- that's 16k.
STACKSIZE equ 0x4000

StartPrecursor:
	; NOTE: Until paging is set up, the code must be position-independent and use physical
	; addresses, not virtual ones!
	lidt [ZeroSizeIdt - KERNEL_VIRTUAL_BASE]	; Load the IDTR with a zero-size IDT.
	mov ecx, (BootPageDirectory - KERNEL_VIRTUAL_BASE)
	mov cr3, ecx					; Load Page Directory Base Register.

	mov ecx, cr4
	or ecx, 0x00000010				; Set PSE bit in CR4 to enable 4MB pages.
	mov cr4, ecx

	mov ecx, cr0
	or ecx, 0x80000000				; Set PG bit in CR0 to enable paging.

	; Go go gadget paging!
	mov cr0, ecx
	
	; Start fetching instructions in kernel space.
	lea ecx, [StartInHigherHalf]
	jmp ecx							; NOTE: Must be absolute jump!

StartInHigherHalf:
	lidt [ZeroSizeIdt]				; Switch to zero-size IDT in kernel space.
	; Unmap the identity-mapped first 4MB of physical address space. It should not be needed
	; anymore.
	mov dword [BootPageDirectory], 0
	invlpg [0]

	; NOTE: From now on, paging should be enabled. The first 4MB of physical address space is
	; mapped starting at KERNEL_VIRTUAL_BASE. Everything is linked to this address, so no more
	; position-independent code or funny business with virtual-to-physical address translation
	; should be necessary. We now have a higher-half kernel.
	mov esp, StackStart + STACKSIZE	; Set up the stack.

	; Push these first so they don't get lost.
	push eax						; Pass Multiboot magic number.
	push ebx						; Pass Multiboot info structure.
	cld								; Set direction flag to known value.

	call Processor_initPrimary		; Initialize the primary (and only) Processor.

	; Map and translate the Multiboot info.
	call BootLoaderInfoTranslator_translate		; Parameters already pushed above...
	add esp, 8

    ; Call kernel proper.
	push eax
	call kmain
	add esp, 4

LocalHalt:
	hlt								; Halt machine should kernel return.
	jmp LocalHalt					; Just in case.


; ===========================================================================
section .bss
align 32
StackStart:
	resb STACKSIZE		; reserve 16k stack on a quadword boundary

