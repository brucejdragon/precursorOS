// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Executive/ExceptionDispatcher_private.h
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
/// \brief	Declares the private methods of the ExceptionDispatcher class, some of
///			which are architecture-specific and some of which are not.
///
// ===========================================================================

#ifndef _KERNEL_EXECUTIVE_EXCEPTIONDISPATCHER_PRIVATE_H_
#define _KERNEL_EXECUTIVE_EXCEPTIONDISPATCHER_PRIVATE_H_


#include "ExceptionDispatcher.h"
#include "Kernel/HAL/TrapFrame.h"


/// \brief	Exception handler that handles all unrecoverable exceptions.
///
/// \param this			instance pointer -- should always be NULL.
/// \param trapFrame	the machine state captured when the exception was raised.
///
/// \return	this handler initiates system failure and therefore never returns.
TrapFrame* ExceptionDispatcher_handleUnrecoverableException(
	volatile void*	this,
	TrapFrame*		trapFrame
);


/// \brief	Exception handler that handles all exceptions that, if raised in user mode, should
///			be reflected back to the current thread for handling.
///
/// \param this			instance pointer -- should always be NULL.
/// \param trapFrame	the machine state captured when the exception was raised.
///
/// If this handler determines that the exception was raised in kernel mode, it will initiate
/// system failure and therefore never return.
///
/// \return	NULL, or the TrapFrame of the new thread if a context switch is required.
TrapFrame* ExceptionDispatcher_handleDeliverableException(
	volatile void*	this,
	TrapFrame*		trapFrame
);


#ifndef NDEBUG

/// \brief	Helper that handles assertions caused by kernel-mode code.
///
/// \param trapFrame	the machine state captured when the exception was raised.
///
/// This method initiates system failure and therefore never returns. It is invoked in response
/// to an exception raised by the kernel itself to indicate that an assertion failed. This method
/// is only implemented in the checked build.
void ExceptionDispatcher_doKernelAssertFail( TrapFrame* trapFrame );

#endif // NDEBUG


/// \brief	Helper that initiates system failure, outputting the appropriate message and the given
///			machine state.
///
/// \param trapFrame	the machine state captured when the exception was raised.
///
/// This method initiates system failure and therefore never returns.
void ExceptionDispatcher_doFail( TrapFrame* trapFrame );


#endif

