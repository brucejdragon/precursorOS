// ===========================================================================
//
//             Copyright (C) 2004-2007 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/HAL/KernelDisplayColor.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2007/May/26
//
// ===========================================================================
///
/// \file
///
/// \brief	Defines architecture-neutral constants for the colors that can
///			be used to display text on the kernel display.
///
// ===========================================================================

#ifndef _KERNEL_HAL_KERNELDISPLAYCOLOR_H_
#define _KERNEL_HAL_KERNELDISPLAYCOLOR_H_


/// \brief	Represents all the 16 possible colors that can be used as foreground and background
///			colors of the kernel display.
typedef enum
{
	// MAINTENANCE NOTE: Keep these enum values 0-based and contiguous.
	KDISPLAY_COLOR_BLACK = 0,
	KDISPLAY_COLOR_WHITE,
	KDISPLAY_COLOR_DARKGRAY,
	KDISPLAY_COLOR_LIGHTGRAY,
	KDISPLAY_COLOR_DARKRED,
	KDISPLAY_COLOR_LIGHTRED,
	KDISPLAY_COLOR_DARKGREEN,
	KDISPLAY_COLOR_LIGHTGREEN,
	KDISPLAY_COLOR_DARKBLUE,
	KDISPLAY_COLOR_LIGHTBLUE,
	KDISPLAY_COLOR_DARKCYAN,
	KDISPLAY_COLOR_LIGHTCYAN,
	KDISPLAY_COLOR_MAGENTA,
	KDISPLAY_COLOR_PINK,
	KDISPLAY_COLOR_BROWN,
	KDISPLAY_COLOR_YELLOW
} KernelDisplayColor;


#endif
