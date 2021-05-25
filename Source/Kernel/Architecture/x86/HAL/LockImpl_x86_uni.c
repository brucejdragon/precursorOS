// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/HAL/LockImpl_x86_uni.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Mar/31
//
// ===========================================================================
///
/// \file
///
/// \brief	This file implements part of the LockImpl class for the x86 UP
///			architecture.
///
// ===========================================================================


#define _KERNEL_HAL_LOCK_C_
#include "Kernel/HAL/Lock.h"
#undef _KERNEL_HAL_LOCK_C_

#include "Kernel/KCommon/KMem.h"


// Public functions.

Lock Lock_create( void )
{
	Lock newLock;
	KMem_set( &newLock, 0, sizeof( Lock ) );
	return newLock;
}

