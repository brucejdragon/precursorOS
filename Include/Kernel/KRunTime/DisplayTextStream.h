// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/KRunTime/DisplayTextStream.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2004/Dec/31
//
// ===========================================================================
///
///	\file
///
/// \brief	Defines an implementation of ITextStream that writes all text output
///			to the KernelDisplay object.
///
// ===========================================================================

#ifndef _KERNEL_KRUNTIME_DISPLAYTEXTSTREAM_H_
#define _KERNEL_KRUNTIME_DISPLAYTEXTSTREAM_H_


#include <stdbool.h>
#include "Kernel/KRunTime/ITextStream.h"


/// \brief	Initializes the global DisplayTextStream implementation.
///
/// This function must be called before DisplayTextStream_getTextStream() is called. Calling this
/// function results in the KernelDisplay object also being initialized.
///
/// \note
/// Client code is responsible for synchronizing access to this method. It should really only be
/// called by the bootstrap processor with interrupts disabled during kernel initialization.
void DisplayTextStream_init( void );


/// \brief	Resets the global DisplayTextStream implementation.
///
/// This function resets the global KernelDisplay object, then flushes the DisplayTextStream 's
/// buffer. Calling this effectively takes control of the video hardware from whatever user process
/// might be using it (and from whatever driver might be managing it) at the time.
///
/// \note
/// Client code is responsible for synchronizing access to this method. It should really only be
/// called by the system failure handler after interrupts on the local processor are disabled and
/// all other processors (if any) are halted.
void DisplayTextStream_reset( void );


/// \brief	Creates an ITextStream that refers to the single implementation that outputs all text
///			it is given to the KernelDisplay object.
///
/// \note
/// If the returned ITextStream is being called during system shutdown mode (as specified
/// by KShutdown), then it is the caller's (i.e. -- KShutdown's) responsibility to synchronize
/// access to the ITextStream. This is to prevent deadlock in the case where an exception or NMI
/// occurs within the DisplayTextStream itself. If this happens, the kernel will re-enter and
/// call KShutdown, which will in turn re-enter DisplayTextStream. This degree of coupling between
/// DisplayTextStream and KShutdown is somewhat unprecendented, but very necessary.
///
/// \return a ITextStream interface reference.
ITextStream DisplayTextStream_getTextStream( void );


#endif

