// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/KCommon/KMath.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2004/Dec/22
//
// ===========================================================================
///
///	\file
///
/// \brief	Defines some very basic integer math functionality for the kernel.
///
// ===========================================================================

#ifndef _KERNEL_KCOMMON_KMATH_H_
#define _KERNEL_KCOMMON_KMATH_H_

#include <stdint.h>


/// \brief	Returns the absolute value of the given integer.
///
/// \param i	a signed integer.
///
/// \return the absolute value of \a i.
static inline uintptr_t KMath_abs( intptr_t i )
{
	return ((i < 0) ? -i : i);
}


#endif

