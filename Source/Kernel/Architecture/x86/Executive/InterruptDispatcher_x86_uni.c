// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/Executive/InterruptDispatcher_x86_uni.c
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
/// \brief	Contains the x86 UP-specific implementation of the InterruptDispatcher class.
///
// ===========================================================================


#include <stddef.h>
#include "InterruptDispatcher.h"
#include "Kernel/HAL/IInterruptHandler.h"
#include "Kernel/HAL/InterruptController.h"
#include "Kernel/HAL/Processor.h"
#include "Kernel/Architecture/x86/HAL/PrecursorVectors_x86.h"
#include "Kernel/KCommon/KDebug.h"

//***FIXME: For temporary debugging only.
#include "Kernel/KRunTime/KOut.h"



/// \brief	Defines the state associated with an InterruptDispatcher instance.
typedef struct
{
	//***FIXME: Empty for now.
} InterruptDispatcher;



/// \brief	Interrupt handler that handles timer interrupts.
///
/// \param this			the InterruptDispatcher that is handling the interrupt.
/// \param trapFrame	the machine state captured when the interrupt occurred.
///
/// This handler is responsible for keeping time as well as for thread scheduling.
/// REVISIT: In the future, these two activities might be triggered by separate
/// interrupt sources, but for now the same handler deals with both.
///
/// \retval NULL		the current thread can continue to run.
/// \retval	TrapFrame*	the context of the new thread to run.
static TrapFrame* InterruptDispatcher_handleTimerInterrupt(
	volatile InterruptDispatcher*	this,
	TrapFrame*						trapFrame
)
{
	(void) this;	//***FIXME: Unused for now.
	uint32_t irq = TrapFrame_getInterruptVectorNumber( trapFrame ) - INT_HW_IRQ0;

	KDebug_assert( Processor_areInterruptsDisabled() );

	// It is safe to cast away volatile here because this handler is called with interrupts
	// disabled.
	InterruptController* pic = (InterruptController*) InterruptController_getForCurrentProcessor();
	InterruptController_endOfInterrupt( pic, irq );
	return NULL;	//***FIXME: Implement timers and scheduler.
}


/// \brief	Interrupt handler that handles all device interrupts other than timer interrupts.
///
/// \param this			the InterruptDispatcher that is handling the interrupt.
/// \param trapFrame	the machine state captured when the interrupt occurred.
///
/// \retval NULL		the current thread can continue to run.
/// \retval	TrapFrame*	the context of the new thread to run.
static TrapFrame* InterruptDispatcher_handleDeliverableInterrupt(
	volatile InterruptDispatcher*	this,
	TrapFrame*						trapFrame
)
{
	(void) this;	//***FIXME: Unused for now.
	uint32_t irq = TrapFrame_getInterruptVectorNumber( trapFrame ) - INT_HW_IRQ0;

//***FIXME: Temporary debugging only.
#ifndef NDEBUG
	KOut_write( "IRQ %ld... ", irq );
#else
	(void) trapFrame;
#endif

	KDebug_assert( Processor_areInterruptsDisabled() );

	// It is safe to cast away volatile here because this handler is called with interrupts
	// disabled.
	InterruptController* pic = (InterruptController*) InterruptController_getForCurrentProcessor();
	InterruptController_endOfInterrupt( pic, irq );
	return NULL;	//***FIXME: Implement device interrupts.
}



/// \brief	The IInterruptHandler interface dispatch table for handling timer interrupts.
static IInterruptHandler_itable s_timerHandlerTable =
{
	(IInterruptHandler_handleInterruptFunc) InterruptDispatcher_handleTimerInterrupt
};



/// \brief	The IInterruptHandler interface dispatch table for handling device interrupts other
///			than timer interrupts.
static IInterruptHandler_itable s_deliverableHandlerTable =
{
	(IInterruptHandler_handleInterruptFunc) InterruptDispatcher_handleDeliverableInterrupt
};



/// \brief	The one-and-only instance of InterruptDispatcher. Only one is needed for UP systems.
static volatile InterruptDispatcher s_instance;



void InterruptDispatcher_initForCurrentProcessor( void )
{
	IInterruptHandler deliverableHandler;
	deliverableHandler.iptr = &s_deliverableHandlerTable;
	deliverableHandler.obj	= &s_instance;

	IInterruptHandler timerHandler;
	timerHandler.iptr	= &s_timerHandlerTable;
	timerHandler.obj	= &s_instance;

	static const uint32_t deliverableVectors[] =
	{
		INT_HW_IRQ1,
		INT_HW_IRQ2,
		INT_HW_IRQ3,
		INT_HW_IRQ4,
		INT_HW_IRQ5,
		INT_HW_IRQ6,
		INT_HW_IRQ7,
		INT_HW_IRQ8,
		INT_HW_IRQ9,
		INT_HW_IRQ10,
		INT_HW_IRQ11,
		INT_HW_IRQ12,
		INT_HW_IRQ13,
		INT_HW_IRQ14,
		INT_HW_IRQ15
	};

	KDebug_assert( Processor_areInterruptsDisabled() );

	// NOTE: It is safe to cast away volatile here, since interrupts must be disabled according to
	// this method's contract.
	Processor* processor = (Processor*) Processor_getCurrent();
	
	for (size_t i = 0; i < sizeof( deliverableVectors ) / sizeof( uint32_t ); i++)
	{
		Processor_registerHandler( processor, deliverableHandler, deliverableVectors[i] );
	}

	// REVISIT: In x86 UP systems, use the PIT for scheduling and timing.
	Processor_registerHandler( processor, timerHandler, INT_HW_IRQ0 );

	// Make sure the interrupts are sent our way.
	InterruptController_initForCurrentProcessor();
}


