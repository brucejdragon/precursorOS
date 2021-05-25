// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/Architecture/x86/HAL/TrapFrame_x86.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Jan/01
//
// ===========================================================================
///
/// \file
///
/// \brief	This file defines the TrapFrame class fields for the x86 architecture.
///
/// These fields are defined in a header rather than in TrapFrame_x86.c so
/// that the details of an x86 TrapFrame are available to the entire Architecture
/// library (and selected architecture-specific portions of the rest of the kernel).
///
/// All code in the Architecture library (et al.) that deals with TrapFrames
/// should include this file rather than Kernel/HAL/TrapFrame.h.
///
// ===========================================================================


#ifndef _KERNEL_ARCHITECTURE_X86_HAL_TRAPFRAME_X86_H_
#define _KERNEL_ARCHITECTURE_X86_HAL_TRAPFRAME_X86_H_


#include <stdint.h>
#include "Kernel/HAL/TrapFrame.h"
#include "Kernel/Architecture/x86/HAL/ProtectedMode.h"


/// \brief	Defines the x86-specific fields of the TrapFrame class.
struct TrapFrameStruct
{
	SegmentSelector 	gs;				///< Saved value of GS segment register.
	uint16_t			paddingForGS;	///< For padding purposes.
	SegmentSelector 	fs;				///< Saved value of FS segment register.
	uint16_t			paddingForFS;	///< For padding purposes.
	SegmentSelector 	es;				///< Saved value of ES segment register.
	uint16_t			paddingForES;	///< For padding purposes.
	SegmentSelector 	ds;				///< Saved value of DS segment register.
	uint16_t			paddingForDS;	///< For padding purposes.
	uint32_t			cr2;			///< Saved value of CR2 (page faults only).
	uint32_t			edi;			///< Saved value of EDI general-purpose register.
	uint32_t			esi;			///< Saved value of ESI general-purpose register.
	uint32_t			ebp;			///< Saved value of EBP general-purpose register.
	uint32_t			esp;			///< Saved value of ESP before EAX was pushed.
	uint32_t			ebx;			///< Saved value of EBX general-purpose register.
	uint32_t			edx;			///< Saved value of EDX general-purpose register.
	uint32_t			ecx;			///< Saved value of ECX general-purpose register.
	uint32_t			eax;			///< Saved value of EAX general-purpose register.
	uint8_t				interruptVectorNumber;	///< Vector# of the interrupt that occurred.
	uint8_t				paddingForVector8to15;	///< For padding purposes.
	uint16_t			paddingForVector16to31;	///< For padding purposes.
	GeneralErrorCode	errorCode;		///< Error code. Can be 0.
	uint32_t			eip;			///< Saved value of EIP register; Saved by hardware.
	SegmentSelector 	cs;				///< Saved value of CS segment register; Saved by hardware.
	uint16_t			paddingForCS;	///< For padding purposes.
	EFlagsRegister		eflags;			///< Saved value of EFLAGS register; Saved by hardware.

	// Everything past this point will contain garbage if isKernelInterrupted() returns true.
	// (This is not quite true, as debug info might be hiding there if a kernel breakpoint was
	// triggered. However, the following fields are not safe to access even in that case).

	uint32_t			esp3;			///< Ring 3 ESP. Undefined if isKernelInterrupted() == true.
	SegmentSelector		ss3;			///< Ring 3 SS. Undefined if isKernelInterrupted() == true.
	uint16_t			paddingForSS;	///< For padding purposes.

} PACKED;


#endif

