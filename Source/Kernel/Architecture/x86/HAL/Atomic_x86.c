// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/HAL/Atomic_x86.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Oct/08
//
// ===========================================================================
///
///	\file
///
/// \brief	Contains the implementation of some of the Atomic utility functions.
///
/// These implementations are specific to the x86 architecture.
// ===========================================================================


#include "Kernel/HAL/Atomic.h"
#include "Kernel/KCommon/KDebug.h"
#include "Kernel/KCommon/KMem.h"


// On all IA-32 processors since the 486, reads and writes of 32-bit values to 32-bit-aligned
// addresses are guaranteed to be atomic. Therefore, the only things required to ensure atomicity
// on this architecture are:
// - A guarantee that the target address is aligned; and
// - The "volatile" keyword to prevent GCC's optimizer from doing any aggressive inlining and
//   register-caching of the value to be read/written.


uintptr_t Atomic_read( const volatile uintptr_t* targetAddress )
{
	KDebug_assert( KMem_isAligned32( (uintptr_t) targetAddress ) );
	return *targetAddress;
}


void Atomic_write( volatile uintptr_t* targetAddress, uintptr_t updateValue )
{
	KDebug_assert( KMem_isAligned32( (uintptr_t) targetAddress ) );
	*targetAddress = updateValue;
}

