/*****************************************************************************
*
*              Copyright (C) 2004-2005 Bruce Johnston
*
******************************************************************************
*
*   //osdev/precursor/Source/Kernel/Build/precurnl_x86_uni.ld
*
******************************************************************************
*
*	Originating Author:	BruceJ
*	Originating Date:	2005/Feb/03
*
*****************************************************************************/

ENTRY (StartPrecursor)

OUTPUT_FORMAT(binary)
SECTIONS
{
	/* MAINTENANCE NOTE: This entire expression *must* be equal to KERNEL_VIRTUAL_BASE plus 1MB.
	   KERNEL_VIRTUAL_BASE is defined in Kernel/HAL/Mem.h. */
	. = 0xE0000000 + 0x00100000;

	/* NOTE: You need the star after rodata to catch some specially-named sections
	   in optimized builds. */
	.text :
	{
		*(.text)
		*(.rodata*)
	}

	.data ALIGN (0x1000) :
	{
		*(.data)
	}

	.bss :
	{
		*(COMMON)
		*(.bss)
		BssEndPhys = . - 0xE0000000;	/* We want the physical address here. */
	}
}

