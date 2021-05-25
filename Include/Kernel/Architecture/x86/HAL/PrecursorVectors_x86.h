// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/Architecture/x86/HAL/PrecursorVectors_x86.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Apr/30
//
// ===========================================================================
///
/// \file
///
/// \brief	This file defines x86-specific interrupt vectors for hardware
///			interrupts and system calls.
///
// ===========================================================================

#ifndef _KERNEL_ARCH_X86_HAL_PRECURSORVECTORS_X86_H_
#define _KERNEL_ARCH_X86_HAL_PRECURSORVECTORS_X86_H_


/// \brief	Defines the various interrupt/exception vector numbers that are specific to Precursor
///			on the x86 architecture.
typedef enum
{
	INT_HW_IRQ0		= 32,	///< Hardware IRQ 0.
	INT_HW_IRQ1		= 33,	///< Hardware IRQ 1.
	INT_HW_IRQ2		= 34,	///< Hardware IRQ 2.
	INT_HW_IRQ3		= 35,	///< Hardware IRQ 3.
	INT_HW_IRQ4		= 36,	///< Hardware IRQ 4.
	INT_HW_IRQ5		= 37,	///< Hardware IRQ 5.
	INT_HW_IRQ6		= 38,	///< Hardware IRQ 6.
	INT_HW_IRQ7		= 39,	///< Hardware IRQ 7.
	INT_HW_IRQ8		= 40,	///< Hardware IRQ 8.
	INT_HW_IRQ9		= 41,	///< Hardware IRQ 9.
	INT_HW_IRQ10	= 42,	///< Hardware IRQ 10.
	INT_HW_IRQ11	= 43,	///< Hardware IRQ 11.
	INT_HW_IRQ12	= 44,	///< Hardware IRQ 12.
	INT_HW_IRQ13	= 45,	///< Hardware IRQ 13.
	INT_HW_IRQ14	= 46,	///< Hardware IRQ 14.
	INT_HW_IRQ15	= 47,	///< Hardware IRQ 15.
	INT_SYS_CALL	= 48	///< System call vector.

} KernelInterruptVector;


#endif

