// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/HAL/IInterruptHandler.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2004/Nov/01
//
// ===========================================================================
///
/// \file
///
/// \brief	Defines the IInterruptHandler interface, which represents an event
///			sink attached to a particular interrupt vector.
///
/// This interface helps keep the HAL simpler by isolating it from the details
/// of the Executive and other kernel components. At its heart, the control
/// flow of an OS kernel is event-driven. The processor's interrupt/exception
/// mechanism (represented by the Processor class) acts as the event source,
/// while the various portions of the kernel invoked when an interrupt or
/// exception occurs act as the event handlers. The IInterruptHandler
/// interface simply formalizes this relationship to help keep the kernel
/// modular and tractable.
///
// ===========================================================================

#ifndef _KERNEL_HAL_IINTERRUPTHANDLER_H_
#define _KERNEL_HAL_IINTERRUPTHANDLER_H_


#include <stdint.h>
#include "Kernel/HAL/TrapFrame.h"



/// \brief	Defines the method signature for IInterruptHandler::handleInterrupt().
///
/// \param this			the object that implements IInterruptHandler.
/// \param trapFrame	the trap frame created to capture the critical state of the running thread
///						at the time the interrupt occurred.
///
/// This method is called when an interrupt occurs, regardless of whether it is caused by hardware
/// or software.
///
/// If the implementation of the handler wishes to initiate a context switch, it must return the
/// address of the new TrapFrame to use for restoring machine state when the current interrupt is
/// dismissed. In order to ensure the consistency of control flow within the kernel, the handler
/// must ensure that interrupts are disabled before it returns. It should actually disable
/// interrupts before invoking whatever scheduler is responsible for determining what value this
/// handler should return. This ensures that once a decision has been made to switch contexts, this
/// decision will be carried out immediately and atomically.
///
/// If this method returns a new TrapFrame, the context switch will happen after the handler
/// returns. At that time, the Processor switches to the kernel stack indicated by the given
/// TrapFrame and restores its machine state using the remaining contents of the TrapFrame.
/// If switching to user mode, it also arranges for that kernel stack to be used the next time the
/// switched-to thread enters the kernel.
///
/// \retval TrapFrame*	pointer to the new trap frame to which the Processor must switch.
/// \retval NULL		no context switch is needed.
typedef TrapFrame* (*IInterruptHandler_handleInterruptFunc)(
	volatile void*	this,
	TrapFrame*		trapFrame
);



/// \brief	Defines the interface dispatch table for IInterruptHandler.
typedef struct
{
	/// \brief	Pointer to a particular implementation of IInterruptHandler::handleInterrupt().
	IInterruptHandler_handleInterruptFunc handleInterrupt;

} IInterruptHandler_itable;



/// \brief	Defines a reference to an implementation of the IInterruptHandler interface.
typedef struct
{
	const IInterruptHandler_itable*	iptr;	///< Pointer to the interface dispatch table.
	volatile void*					obj;	///< Pointer to the object that implements the interface.

} IInterruptHandler;



#endif

