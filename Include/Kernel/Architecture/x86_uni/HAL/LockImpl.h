// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/Architecture/x86_uni/HAL/LockImpl.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Mar/29
//
// ===========================================================================
///
/// \file
///
/// \brief	Implements the Lock class for the x86 uniprocessor architecture.
///
/// This is the x86 uniprocessor implementation of the Lock class. acquire()
/// simply stores the old version EFLAGS, then clears IF with the CLI
/// instruction. release() restores the old EFLAGS, which will only re-enable
/// interrupts if the IF bit was set in the old EFLAGS.
///
// ===========================================================================

#ifndef _KERNEL_HAL_LOCKIMPL_H_
#define _KERNEL_HAL_LOCKIMPL_H_


#include "HAL/ProtectedMode.h"


/// \brief	Defines the architecture-specific fields of the Lock class.
typedef struct LockStruct
{
#ifdef _KERNEL_HAL_LOCK_C_

	/// \brief	Contains the value of the EFLAGS register prior to when the lock was acquired.
	EFlagsRegister m_oldEFlags;

#else

	#ifndef DOXYGEN_SHOULD_SKIP_THIS
	EFlagsRegister m_reserved;
	#endif

#endif
} PACKED Lock;


#endif

