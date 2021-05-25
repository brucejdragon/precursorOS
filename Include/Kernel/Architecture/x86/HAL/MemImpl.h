// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/Architecture/x86/HAL/MemImpl.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Nov/27
//
// ===========================================================================
///
/// \file
///
/// \brief	Defines memory-related constants, and utilities for the HAL and
///			the rest of the kernel. The definitions are for the 32-bit x86
///			architecture.
///
// ===========================================================================

#ifndef _KERNEL_ARCH_X86_HAL_MEMIMPL_H_
#define _KERNEL_ARCH_X86_HAL_MEMIMPL_H_


/// \brief	Defines some constants describing the kernel's virtual address space.
enum Mem_consts
{
	// MAINTENANCE NOTE: This is defined here so that it's accessible to other parts of the HAL; it is a special case.
	// In general, MM constants should be private to MM.
	KERNEL_VIRTUAL_BASE = 0xE0000000	///< Virtual base address of kernel space. 3.5 GB on x86.
};

#endif

