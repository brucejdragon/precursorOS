// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/HAL/TrapFrame.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2004/Oct/31
//
// ===========================================================================
///
/// \file
///
/// \brief	Defines the TrapFrame class, which represents a special stack
///			frame created by a Processor object when an interrupt or exception
///			occurs.
///
/// The internal details of a TrapFrame are architecture-specific. Therefore,
/// TrapFrame is just an opaque handle type as far as most of the rest of the
/// kernel is concerned. The internals of a TrapFrame are manipulated only
/// within the Architecture library and select portions of the kernel where
/// machine state must be manipulated at a higher level (e.g. -- when register
/// contents must be formatted and dumped to the kernel display).
///
/// There is no TrapFrame constructor, since the creation of TrapFrames is
/// triggered more or less asynchronously by hardware.
///
/// Note that a TrapFrame does not capture all the context of the interrupted
/// thread -- usually only enough to ensure that it does not get lost if another
/// interrupt occurs.
///
// ===========================================================================

#ifndef _KERNEL_HAL_TRAPFRAME_H_
#define _KERNEL_HAL_TRAPFRAME_H_


#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


/// \brief	Forward declaration of the TrapFrame object type.
typedef struct TrapFrameStruct TrapFrame;


/// \brief	Returns the error code that indicates the reason for the exception that resulted in
///			the creation of the TrapFrame.
///
/// \param frame	the TrapFrame.
///
/// Not all interrupt/exception types produce an error code. Which do and which don't is
/// architecture-specific. For those that don't, this method returns 0.
///
/// \note
/// The size of the return value of this method is architecture-dependent. It is guaranteed to be
/// large enough to hold a memory address, for those cases where an error code consists of a memory
/// address (e.g. -- for page faults).
///
/// \return an integer of the natural machine width (e.g. -- 32 bits for x86; 64 bits for x86-64)
///			containing the error code (or 0 if there is no error code).
size_t TrapFrame_getErrorCode( const TrapFrame* frame );


/// \brief	Returns the vector number of the interrupt that resulted in the creation of the
///			TrapFrame.
///
/// \param frame	the TrapFrame.
///
/// The interpretation of an interrupt vector number is architecture-specific. It can correspond to
/// either an external hardware interrupt, or an exception caused by software. Each such interrupt
/// source has a unique vector number, and this number is used to register a single handler for
/// that interrupt source.
///
///	\return an integer containing the vector number.
uint32_t TrapFrame_getInterruptVectorNumber( const TrapFrame* frame );


/// \brief	Determines whether the interrupted thread whose state was captured by this TrapFrame
///			was executing in kernel mode at the time it was interrupted.
///
/// \param frame	the TrapFrame.
///
/// \return	\c true if the kernel interrupted itself; \c false if the interrupted code was executing
///			in user mode.
bool TrapFrame_isKernelInterrupted( const TrapFrame* frame );


#endif

