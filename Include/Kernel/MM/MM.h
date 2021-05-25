// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/MM/MM.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Nov/13
//
// ===========================================================================
///
/// \file
///
/// \brief	Defines primitive types, constants, and utilities used by the
///			Kernel's Memory Manager.
///
// ===========================================================================

#ifndef _KERNEL_MM_MM_H_
#define _KERNEL_MM_MM_H_

// Just delegate to the architecture-specific include.
#include <stdint.h>
#include "MM/MMImpl.h"
#include "Kernel/KCommon/KDebug.h"


// Here are some architecture-neutral utilities.

/// \brief	Converts the given number of kilobytes to bytes.
///
/// \param kiloBytes	the number of kilobytes.
///
/// \note If \a kiloBytes is greater than the total number of KB virtually addressable for the architecture
/// (e.g. -- greater than 4 * 1024 * 1024 on a 32-bit CPU), this method will assert in checked builds and cause integer
/// overflow in free builds.
///
/// \return the corresponding number of bytes.
static inline uintptr_t MM_KB( size_t kiloBytes )
{
	// MAINTENANCE NOTE: This test has been specifically written to avoid overflow.
	KDebug_assertArg( kiloBytes <= UINTPTR_MAX / 1024 );
	return (kiloBytes * 1024);
}


/// \brief	Converts the given number of megabytes to bytes.
///
/// \param megaBytes	the number of megabytes.
///
/// \note If \a megaBytes is greater than the total number of MB virtually addressable for the architecture
/// (e.g. -- greater than 4 * 1024 on a 32-bit CPU), this method will assert in checked builds and cause integer
/// overflow in free builds.
///
/// \return the corresponding number of bytes.
static inline uintptr_t MM_MB( size_t megaBytes )
{
	// MAINTENANCE NOTE: This test has been specifically written to avoid overflow.
	KDebug_assertArg( megaBytes <= UINTPTR_MAX / (1024 * 1024) );
	return (megaBytes * 1024 * 1024);
}


/// \brief	Converts the given number of gigabytes to bytes.
///
/// \param gigaBytes	the number of gigabytes.
///
/// \note If \a gigaBytes is greater than the total number of GB virtually addressable for the architecture
/// (e.g. -- greater than 4 on a 32-bit CPU), this method will assert in checked builds and cause integer overflow in
/// free builds.
///
/// \return the corresponding number of bytes.
static inline uintptr_t MM_GB( size_t gigaBytes )
{
	// MAINTENANCE NOTE: This test has been specifically written to avoid overflow.
	KDebug_assertArg( gigaBytes <= UINTPTR_MAX / (1024 * 1024 * 1024) );
	return (gigaBytes * 1024 * 1024 * 1024);
}


// Here are some portable utilities that depend on the constants defined in MMImpl.h.


/// \brief	Returns the nearest physical address at or below the given address that is on a frame
///			boundary.
///
/// \param paddr	a physical address.
///
/// \return the nearest frame-aligned physical address at or below \a paddr.
static inline phys_addr_t MM_alignToFrame( phys_addr_t paddr )
{
	return (paddr & ~(FRAME_OFFSET_MASK));
}


/// \brief	Gets the physical address of the frame with the given frame number.
///
/// \param frameNumber	the number of the frame.
///
/// \return the physical address corresponding to frameNumber.
static inline phys_addr_t MM_getFrameAddress( size_t frameNumber )
{
	return (phys_addr_t) (frameNumber << PAGE_BITS);
}


/// \brief	Gets the number of the frame containing the given physical address.
///
/// \param paddr	a physical address within a frame.
///
/// \return the number of the frame containing \a paddr.
static inline size_t MM_getFrameNumber( phys_addr_t paddr )
{
	return (size_t) (paddr >> PAGE_BITS);
}


/// \brief	Checks whether the given physical address is aligned on a frame boundary.
///
/// \param paddr	the physical address to check.
///
/// \retval true	\a paddr is frame-aligned.
/// \retval false	\a paddr is not frame-aligned.
static inline bool MM_isFrameAligned( phys_addr_t paddr )
{
	return ((paddr & FRAME_OFFSET_MASK) == 0);
}


/// \brief	Checks whether the given virtual address is aligned on a page boundary.
///
/// \param vaddr	the virtual address to check.
///
/// \retval true	\a vaddr is page-aligned.
/// \retval false	\a vaddr is not page-aligned.
static inline bool MM_isPageAligned( void* vaddr )
{
	return ((((uintptr_t) vaddr) & PAGE_OFFSET_MASK) == 0);
}


#endif

