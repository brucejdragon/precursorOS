// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Executive/ExceptionDispatcher.c
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
/// \brief	Contains the architecture-independent implementation of the
///			ExceptionDispatcher class.
///
// ===========================================================================


#include <stddef.h>
#include "ExceptionDispatcher_private.h"
#include "WritableTrapFrame.h"
#include "Kernel/KRunTime/KShutdown.h"
#include "Kernel/KCommon/KDebug.h"


TrapFrame* ExceptionDispatcher_handleUnrecoverableException(
	volatile void*	this,
	TrapFrame*		trapFrame
)
{
	// There is nothing we can do to handle the exception. Time to crash.
	(void) this;
	ExceptionDispatcher_doFail( trapFrame );
	return NULL;
}


void ExceptionDispatcher_doFail( TrapFrame* trapFrame )
{
	//***FIXME: Config for verbose vs. silent mode? i18n
	volatile KShutdown* kshutdown = KShutdown_getInstance();
	KShutdown_fail(
		kshutdown,
		"SYSTEM FAILURE\n%s\n%s\n\nMachine State:\n\n%O\n",
		"An unrecoverable error has occurred and the system must be shut down.",
		"We apologize for the inconvenience.",
		WritableTrapFrame_getAsTextWritable( trapFrame )
	);
}

