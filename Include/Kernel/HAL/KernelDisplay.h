// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/HAL/KernelDisplay.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2004/Sep/25
//
// ===========================================================================
///
/// \file
///
/// \brief	Implements the KernelDisplay class, which encapsulates the default
///			display device used by the kernel to display important messages.
///
/// The assumption is that the kernel will not need to display any unimportant
/// messages, so this class will be used sparingly. Note that it is not intended
/// to be used by user-mode display drivers. Once booted, the kernel will not
/// use this class unless it encounters a system failure, at which point it will
/// take direct control of the display hardware, switch to its preferred mode,
/// and display some debugging information.
///
/// The design of this class makes several assumptions about the system on which
/// the kernel is running. Firstly, it assumes that the preferred mode for kernel
/// display can display text somehow (using ANSI characters). Secondly, it assumes
/// that the display supports at least 16 colors. Thirdly, it assumes that there
/// is one unambiguously chosen display device that the kernel should use (e.g. --
/// standard VGA on x86-based PCs).
///
/// The basic capabilities of KernelDisplay include line-oriented output with
/// automatic wrapping of text and scrolling. However, it does not include any
/// formatting features (e.g. -- clients must call KernelDisplay_newLine() to
/// move to a new line, and KernelDisplay_tab() to move the current output location
/// several positions to the right).
///
/// \note
/// The kernel display is a shared resource, but mutually exclusive access to this
/// resource is not guaranteed by the KernelDisplay class itself. It is up to client
/// code to enforce mutually-exclusive access to all the methods of this class. This
/// means both disabling interrupts while calling this class, as well as acquiring
/// the appropriate spinlock on MP systems.
///
// ===========================================================================

#ifndef _KERNEL_HAL_KERNELDISPLAY_H_
#define _KERNEL_HAL_KERNELDISPLAY_H_


#include <stddef.h>
#include "HAL/KernelDisplaySize.h"
#include "Kernel/HAL/KernelDisplayColor.h"


/// \brief	Forward declaration of the KernelDisplay object type.
typedef struct KernelDisplayStruct KernelDisplay;


/// \brief	Initializes the KernelDisplay singleton.
///
/// This function must be called before KernelDisplay_getInstance() can be called. It performs
/// whatever hardware-specific initialization is required to be able to display text in the
/// default foreground and background colors. It then clears the display and prepares for text
/// output in the top-left corner.
///
/// This function should be called exactly once during second-phase bootup (i.e. -- when paging
/// is enabled and the C language environment of the kernel is already initialized properly).
/// Interrupts must be disabled on the current processor.
void KernelDisplay_init( void );


/// \brief	Returns a pointer to the one-and-only KernelDisplay object.
///
/// \return a KernelDisplay* to be used when calling other KernelDisplay functions.
volatile KernelDisplay* KernelDisplay_getInstance( void );


/// \brief	Clears the display to the current background color and prepares for text output in
///			the top-left corner.
///
/// \param kdisplay	the KernelDisplay.
void KernelDisplay_clear( KernelDisplay* kdisplay );


/// \brief	Prints the characters in the given buffer on the display at the current position using
///			the current foreground and background colors.
///
/// \param kdisplay	the KernelDisplay.
/// \param buffer	the buffer of characters to output. It is assumed that it contains only
///					printable characters. In particular, characters such as newlines and tabs
///					will have no special effect -- client code must call KernelDisplay_newLine()
///					or KernelDisplay_tab() explicitly.
/// \param length	the length of \a buffer in bytes.
///
/// If \a length characters would go past the end of the display, this function scrolls the
/// display automatically to make room. If the length is so large that not all characters can fit
/// on the display at once, the beginning of \a buffer is truncated until the remaining characters
/// fit (with room for one more at the bottom-right corner, where the output location is finally
/// moved to).
void KernelDisplay_print( KernelDisplay* kdisplay, const char buffer[], size_t length );


/// \brief	Gets the current background color of the display.
///
/// \param kdisplay	the KernelDisplay.
///
/// \return the current background color of kdisplay.
KernelDisplayColor KernelDisplay_getBackgroundColor( const KernelDisplay* kdisplay );


/// \brief	Gets the current foreground color of the display.
///
/// \param kdisplay	the KernelDisplay.
///
/// \return the current foreground color of kdisplay.
KernelDisplayColor KernelDisplay_getForegroundColor( const KernelDisplay* kdisplay );


/// \brief	Sets the current background color of the display.
///
/// \param kdisplay			the KernelDisplay.
/// \param backgroundColor	the new background color to set.
void KernelDisplay_setBackgroundColor(
	KernelDisplay*		kdisplay,
	KernelDisplayColor	backgroundColor
);


/// \brief	Sets the current foreground color of the display.
///
/// \param kdisplay			the KernelDisplay.
/// \param foregroundColor	the new foreground color to set.
void KernelDisplay_setForegroundColor(
	KernelDisplay*		kdisplay,
	KernelDisplayColor	foregroundColor
);


/// \brief	Moves the current output location to the beginning of the next line.
///
/// \param kdisplay	the KernelDisplay.
///
/// If the current output location is already at the bottom-most line, this
/// function scrolls all the text currently on the screen and resets the
/// output location to the beginning of the current (i.e. -- the bottom-most)
/// line.
void KernelDisplay_newLine( KernelDisplay* kdisplay );


/// \brief	Moves the current output location several characters to the right.
///
/// \param kdisplay	the KernelDisplay.
///
/// If the output location goes beyond the right-most column, it is moved down to
/// the next line, then moved to the right. In this case, if the output location is
/// already at the bottom-most line, scrolling takes place first.
void KernelDisplay_tab( KernelDisplay* kdisplay );


/// \brief	Sets the display hardware and KernelDisplay object back to the state they were in
///			immediately after KernelDisplay_init() was called.
///
/// \param kdisplay	the KernelDisplay.
///
/// This function should be called when the kernel needs to halt the system. It basically steals
/// the display device from whatever user-mode process might be using it at the time, so it should
/// not be used under any other circumstance.
void KernelDisplay_reset( KernelDisplay* kdisplay );


#endif

