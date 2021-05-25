// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/Executive/ExceptionDispatcher_x86.c
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
/// \brief	Contains the architecture-specific implementation of the
///			ExceptionDispatcher class for the x86 architecture.
///
// ===========================================================================


#include "ExceptionDispatcher.h"
#include "ExceptionDispatcher_private.h"
#include "WritableTrapFrame.h"
#include "Kernel/HAL/TrapFrame.h"
#include "Kernel/HAL/Processor.h"
#include "Kernel/Architecture/x86/HAL/TrapFrame_x86.h"
#include "Kernel/Architecture/x86/HAL/ProtectedMode.h"
#include "Kernel/Architecture/x86/HAL/PrecursorVectors_x86.h"
#include "Kernel/KRunTime/KShutdown.h"


#ifndef NDEBUG

/// \brief	Defines the format of debug information stored on the stack when an assertion fails on
///			an x86 system.
typedef struct
{
	// MAINTENANCE NOTE: The padding is equal to the size of the trap frame minus the user-mode
	// ESP and SS registers, which are pushed when entering the kernel from user-mode.
	char 		Padding[sizeof(struct TrapFrameStruct) - sizeof(uint32_t) * 2];	///< Padding.
	uint32_t	EIP;		///< Address following the call to Processor_triggerDebugTrap().
	const char*	Message;	///< Assertion error message.
	const char* File;		///< Source file where the assertion occurred.
	int			Line;		///< Line number where the assertion occurred.
} AssertionInfo;


void ExceptionDispatcher_doKernelAssertFail( TrapFrame* trapFrame )
{
	// The AssertionInfo structure overlaps with the TrapFrame so that we can grab the debug
	// info that was pushed on the stack before the breakpoint exception was triggered.
	AssertionInfo* info = (AssertionInfo*) trapFrame;

	volatile KShutdown* kshutdown = KShutdown_getInstance();
	KShutdown_fail(
		kshutdown,
		"SYSTEM FAILURE (DEBUG CHECK): %s\n%s, Line %d\n\nMachine State:\n\n%O\n",
		info->Message,
		info->File,
		info->Line,
		WritableTrapFrame_getAsTextWritable( trapFrame )
	);
}

#endif // NDEBUG


TrapFrame* ExceptionDispatcher_handleDeliverableException(
	volatile void*	this,
	TrapFrame*		trapFrame
)
{
	(void) this;

	if (TrapFrame_isKernelInterrupted( trapFrame ))
	{
#ifndef NDEBUG
		// Check for breakpoint exceptions.
		if (TrapFrame_getInterruptVectorNumber( trapFrame ) == INT3_BP_BREAKPOINT)
		{
			ExceptionDispatcher_doKernelAssertFail( trapFrame );
		}
#endif
		// An exception happened in the kernel. Time to crash.
		ExceptionDispatcher_doFail( trapFrame );
		return NULL;
	}
	else
	{
		//***FIXME: Implement signals/user-mode exceptions/etc.
		KDebug_assertMsg( false, "User-mode not implemented yet!" );
		return NULL;
	}
}



/// \brief	The IInterruptHandler interface dispatch table for handling deliverable exceptions.
static IInterruptHandler_itable s_deliverableHandlerTable =
{
	ExceptionDispatcher_handleDeliverableException
};



/// \brief	The IInterruptHandler interface dispatch table for handling unrecoverable exceptions.
static IInterruptHandler_itable s_unrecoverableHandlerTable =
{
	ExceptionDispatcher_handleUnrecoverableException
};



void ExceptionDispatcher_initForCurrentProcessor( void )
{
	IInterruptHandler deliverableHandler;
	deliverableHandler.iptr = &s_deliverableHandlerTable;
	deliverableHandler.obj	= NULL;

	IInterruptHandler unrecoverableHandler;
	unrecoverableHandler.iptr	= &s_unrecoverableHandlerTable;
	unrecoverableHandler.obj	= NULL;

	static const uint32_t deliverableVectors[] =
	{
		INT0_DE_DIVIDE_ERROR,
		INT3_BP_BREAKPOINT,
		INT4_OF_OVERFLOW,
		INT5_BR_BOUND_RANGE_EXCEEDED,
		INT6_UD_INVALID_OPCODE,
		INT7_NM_NO_MATH_COPROCESSOR,
		INT11_NP_SEG_NOT_PRESENT,
		INT12_SS_STACK_SEG_FAULT,
		INT13_GP_GENERAL_PROTECTION,
		INT14_PF_PAGE_FAULT,	//***FIXME -- move elsewhere.
		INT16_MF_X87_MATH_FAULT,
		INT17_AC_ALIGNMENT_CHECK,
		INT19_XF_SIMD_FP_EXCEPTION,
		INT_SYS_CALL			//***FIXME -- move elsewhere.
	};

	static const uint32_t unrecoverableVectors[] =
	{
		INT1_DB_RESERVED,
		INT2_NMI_INTERRUPT,
		INT8_DF_DOUBLE_FAULT,
		INT9_CO_PROCESSOR_SEG_OVERRUN,
		INT10_TS_INVALID_TSS,
		INT15_RESERVED,
		INT18_MC_MACHINE_CHECK,
		INT20_RESERVED,
		INT21_RESERVED,
		INT22_RESERVED,
		INT23_RESERVED,
		INT24_RESERVED,
		INT25_RESERVED,
		INT26_RESERVED,
		INT27_RESERVED,
		INT28_RESERVED,
		INT29_RESERVED,
		INT30_RESERVED,
		INT31_RESERVED
	};

	KDebug_assert( Processor_areInterruptsDisabled() );

	// NOTE: It is safe to cast away volatile here, because by this method's contract it should
	// only be called with interrupts disabled for the current processor.
	Processor* processor = (Processor*) Processor_getCurrent();

	for (size_t i = 0; i < sizeof( deliverableVectors ) / sizeof( uint32_t ); i++)
	{
		Processor_registerHandler( processor, deliverableHandler, deliverableVectors[i] );
	}

	for (size_t i = 0; i < sizeof( unrecoverableVectors ) / sizeof( uint32_t ); i++)
	{
		Processor_registerHandler( processor, unrecoverableHandler, unrecoverableVectors[i] );
	}
}


