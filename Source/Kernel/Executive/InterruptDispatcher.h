// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Executive/InterruptDispatcher.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Jul/15
//
// ===========================================================================
///
///	\file
///
/// \brief	Defines the InterruptDispatcher class, which handles all hardware
///			interrupts.
///
// ===========================================================================

#ifndef _KERNEL_EXECUTIVE_INTERRUPTDISPATCHER_H_
#define _KERNEL_EXECUTIVE_INTERRUPTDISPATCHER_H_


/// \brief	Instructs the InterruptDispatcher to register its handlers with the current Processor
///			and to initialize the Processor's interrupt controller.
///
/// This method must be called once on each processor in the system during kernel initialization.
/// It must be called with interrupts disabled for the current processor.
///
/// This method is responsible for initializing the InterruptController for the current Processor.
void InterruptDispatcher_initForCurrentProcessor( void );


#endif

