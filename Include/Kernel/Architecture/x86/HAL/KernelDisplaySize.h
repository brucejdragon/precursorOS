// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/Architecture/x86/HAL/KernelDisplaySize.h
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
/// \brief	Defines architecture-specific constants for the dimensions of the
///			kernel display, measured in characters.
///
// ===========================================================================

#ifndef _KERNEL_HAL_KERNELDISPLAYSIZE_H_
#define _KERNEL_HAL_KERNELDISPLAYSIZE_H_


/// \brief	Defines constants for the kernel display size.
typedef enum
{
	KDISPLAY_NUM_COLUMNS	= 80,	///< Width of the display in characters.
	KDISPLAY_NUM_ROWS		= 60	///< Height of the display in characters.
} KernelDisplaySize;


#endif

