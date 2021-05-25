// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Executive/ExceptionDispatcher.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Apr/25
//
// ===========================================================================
///
///	\file
///
/// \brief	Defines the ExceptionDispatcher utility class, which handles
///			all exceptions raised by the processor that are not system calls,
///			hardware interrupts, or page faults.
///
// ===========================================================================

#ifndef _KERNEL_EXECUTIVE_EXCEPTIONDISPATCHER_H_
#define _KERNEL_EXECUTIVE_EXCEPTIONDISPATCHER_H_


/// \brief	Instructs the ExceptionDispatcher to register its handlers with the current Processor.
///
/// This method must be called once on each processor in the system as soon as possible during
/// kernel initialization. It must be called with interrupts disabled for the current processor.
void ExceptionDispatcher_initForCurrentProcessor( void );


#endif

