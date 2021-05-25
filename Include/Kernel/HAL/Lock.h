// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/HAL/Lock.h
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
/// \brief	Defines the methods of the Lock class.
///
/// Locks enforce mutual exclusion by disabling interrupts on the acquiring
/// processor, and in the case of MP systems, by an atomic test-and-set to a
/// shared memory location.
///
/// Note that it is not safe to acquire locks recursively. In the case of a UP
/// implementation, the result is that interrupts will never be re-enabled. In
/// the case of an MP implementation, the result is deadlock. It is safe for a
/// thread or interrupt handler to hold more than one lock, however.
// ===========================================================================

#ifndef _KERNEL_HAL_LOCK_H_
#define _KERNEL_HAL_LOCK_H_


#include "HAL/LockImpl.h"	// Architecture-specific header that defines Lock structure.


/// \brief	Creates a new Lock that is ready to be acquired.
///
/// \return a new Lock instance.
Lock Lock_create( void );


/// \brief	Acquires the Lock.
///
/// \param lock	the Lock to acquire.
///
/// While the lock is acquired, interrupts are disabled on the acquiring processor, and no other
/// processor can execute code that is guarded by the same Lock.
void Lock_acquire( volatile Lock* lock );


/// \brief	Releases the Lock.
///
/// \param lock	the Lock to release.
///
/// When the lock is released, interrupts may or may not be enabled on the releasing processor,
/// depending on whether they were enabled before the Lock was acquired. Once released, other
/// processors can acquire the Lock.
void Lock_release( volatile Lock* lock );


#endif

