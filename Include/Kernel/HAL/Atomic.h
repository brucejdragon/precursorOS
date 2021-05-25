// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/HAL/Atomic.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Apr/02
//
// ===========================================================================
///
/// \file
///
/// \brief	Defines functions for atomically comparing and updating values in
///			memory.
///
// ===========================================================================

#ifndef _KERNEL_HAL_ATOMIC_H_
#define _KERNEL_HAL_ATOMIC_H_


#include <stdbool.h>
#include <stdint.h>


/// \brief	Atomically compares the given value to the value at the target address for equality
///			and updates the target value if the comparison succeeds.
///
/// \param targetAddress	the address of the target value to compare with and possibly update.
/// \param compareValue		the value to compare to \a *targetAddress.
/// \param updateValue		the value to assign to \a *targetAddress if the old value of
///							\a *targetAddress is equal to \a compareValue.
///
/// This function is guaranteed to execute atomically with respect to all other processors in
/// the system, and is uninterruptible on the current processor. It also acts as a memory barrier.
///
/// \warning
/// Do not specify the value \a *targetAddress as the \a compareValue or \a updateValue, or you may
/// get unexpected results. The reason for this is that the value parameters are typically read
/// into registers in a non-atomic manner.
///
/// \retval true	the old value at \a *targetAddress was equal to \a compareValue, and its new
///					value is equal to \a updateValue.
/// \retval false	the value of \a *targetAddress is unchanged, and not equal to
///					\a compareValue.
bool Atomic_compareAndSwap(
	volatile uintptr_t*	targetAddress,
	uintptr_t			compareValue,
	uintptr_t			updateValue
);


/// \brief	Atomically swaps the given value with the value at the target address.
///
/// \param targetAddress	the address of the target value to update.
/// \param updateValue		the value to assign to \a *targetAddress.
///
/// This function is guaranteed to execute atomically with respect to all other processors in
/// the system, and is uninterruptible on the current processor. It also acts as a memory barrier.
///
/// \warning
/// Do not specify the value \a *targetAddress as the \a updateValue, or you may get unexpected
/// results. The reason for this is that the value parameter is typically read into a register in
/// a non-atomic manner.
///
/// \returns	the old value at \a *targetAddress before it was replaced with \a updateValue.
uintptr_t Atomic_swap( volatile uintptr_t* targetAddress, uintptr_t updateValue );


/// \brief	Atomically reads the value at the given target address.
///
/// \param targetAddress	the address of the target value to read.
///
/// This function is guaranteed to execute atomically with respect to all other processors in
/// the system, and is uninterruptible on the current processor.
///
/// \returns	the value at \a *targetAddress.
uintptr_t Atomic_read( const volatile uintptr_t* targetAddress );


/// \brief	Atomically writes the given value to the target address.
///
/// \param targetAddress	the address of the target value to update.
/// \param updateValue		the value to assign to \a *targetAddress.
///
/// This function is guaranteed to execute atomically with respect to all other processors in
/// the system, and is uninterruptible on the current processor.
///
/// \warning
/// Do not specify the value \a *targetAddress as the \a updateValue, or you may get unexpected
/// results. The reason for this is that the value parameter is typically read into a register in
/// a non-atomic manner.
void Atomic_write( volatile uintptr_t* targetAddress, uintptr_t updateValue );


#endif

