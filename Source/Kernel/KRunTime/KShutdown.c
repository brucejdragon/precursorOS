// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/KRunTime/KShutdown.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Apr/06
//
// ===========================================================================
///
///	\file
///
/// \brief	Contains the implementation of the KShutdown class.
///
// MAINTENANCE NOTE: Avoid asserting when in shutdown mode. Hard-to-debug
// re-entrancy problems may result.
// ===========================================================================


#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include "Kernel/KRunTime/KShutdown.h"
#include "Kernel/KRunTime/KOut.h"
#include "Kernel/KRunTime/DisplayTextStream.h"
#include "Kernel/HAL/Atomic.h"
#include "Kernel/HAL/Processor.h"
#include "Kernel/HAL/ShutdownHardware.h"
#include "Kernel/KCommon/KDebug.h"


/// \brief	Defines the fields of the KShutdown class.
struct KShutdownStruct
{
	// MAINTENANCE NOTE: All fields must be stored as uintptr_t so that they can be read and
	// written using the Atomic class.

	/// \brief	ID of the processor that initiated shutdown mode, or -1 if not in shutdown mode.
	uintptr_t	m_shutdownProcessorID;

	uintptr_t	m_rebootOnFail;					///< Whether to halt or reboot on system failure.
	uintptr_t	m_rebootDelayInMilliseconds;	///< Delay before rebooting, in milliseconds.
};



/// \brief	Global instance of KShutdown.
static volatile KShutdown s_instance;



// Private functions

/// \brief	Attempts to initiate shutdown mode.
///
/// \param kshutdown	the KShutdown instance managing kernel shutdown.
///
/// If this method is being called for the first time, the kernel successfully enters shutdown
/// mode. It informs all other processors in the system (if any) to halt, then resets the kernel
/// display. If another processor already called this method before the current processor, then
/// this method will force the current processor to wait in an infinite loop for interrupts to
/// occur. Eventually it should receive an IPI telling it to halt. If for some reason the
/// processor that first initiated system shutdown ends up re-entering this method, the system
/// will be hard reset immediately. In the latter two cases, this function will never return.
static void KShutdown_initiateShutdownMode( volatile KShutdown* kshutdown )
{
	KDebug_assertArg( kshutdown != NULL );

	int thisProcessorID = Processor_getID( Processor_getCurrent() );

	// We don't know whether we're in shutdown mode yet. We might be competing with other
	// processors, so we must do this atomically.
	if (!Atomic_compareAndSwap( &(kshutdown->m_shutdownProcessorID), -1, thisProcessorID ))
	{
		// The system is already in shutdown mode. First make sure we're not the same processor
		// that entered shutdown mode in the first place (this could happen if the shutdown code
		// itself caused an exception, or if an NMI occurred at an inopportune moment).
		int shutdownProcessorID	= (int) Atomic_read( &(kshutdown->m_shutdownProcessorID) );

		if (thisProcessorID == shutdownProcessorID)
		{
			Processor_hardReset();	// Take the system down, right now!
		}
		else
		{
			// Wait perpetually to be told to shut down.
			while (true)
			{
				Processor_waitForInterrupt();
			}
		}

		// One way or another, we should never fall through here.
	}

	// We got the flag, and it now indicates that it's this processor that initiated shutdown
	// mode. Shut down all the other processors.
	ShutdownHardware_haltAllOtherProcessors();

	// Now we're the only processor running. Grab the kernel display from whoever is using it.
	//***FIXME: It would be nice if this were a nop if it could somehow determine that the
	// kernel already controlled the display.
	DisplayTextStream_reset();

	// Now, we're in complete control of everything. Since we're in shutdown mode, DisplayText
	// stream will no longer use any locks, so it is safe to use (i.e. -- it will not cause
	// deadlock if it is being re-entered).
}


/// \brief	Helper that prints a stock informational message to the kernel display, then halts
///			the system.
///
/// \param kshutdown	the KShutdown instance managing kernel shutdown.
///
/// This function should only be called in shutdown mode. It never returns.
static void KShutdown_doHalt( KShutdown* kshutdown )
{
	(void) kshutdown;
	KOut_writeLine( "System halted." );		//***FIXME: i18n?
	ShutdownHardware_halt();
}


/// \brief	Helper that prints a stock informational message to the kernel display, then reboots
///			the system.
///
/// \param kshutdown	the KShutdown instance managing kernel shutdown.
///
/// This function should only be called in shutdown mode. It never returns. The reboot delay,
/// if any, is set in the \a kshutdown object.
static void KShutdown_doReboot( KShutdown* kshutdown )
{
	// No need to get this atomically -- no other processors should even be running.
	int delayInMilliseconds = (int) kshutdown->m_rebootDelayInMilliseconds;
	int delayInSeconds = delayInMilliseconds / 1000;

	// Don't taunt the user... "What was that message that flashed on the screen right before
	// the machine rebooted? Now I'll never know!"
	if (delayInSeconds > 0)
	{
		//***FIXME: i18n?
		KOut_writeLine( "System rebooting in %d seconds.\n", delayInSeconds );

		//***FIXME: micro-timer needed...
		for (int i = 0; i < delayInSeconds; i++)
		{
			for (int j = 0; j < 30000000; j++)
			{
				; // Do nothing.
			}
			KOut_write( "." );
		}
	}
	ShutdownHardware_reboot();
}



// Public functions

void KShutdown_init( void )
{
	s_instance.m_shutdownProcessorID		= (uintptr_t) -1;
	s_instance.m_rebootOnFail				= (uintptr_t) true;
	s_instance.m_rebootDelayInMilliseconds	= 10000;
}


volatile KShutdown* KShutdown_getInstance( void )
{
	return &s_instance;
}


bool KShutdown_isInShutdownMode( const volatile KShutdown* kshutdown )
{
	KDebug_assertArg( kshutdown != NULL );
	int shutdownProcessorID = (int) Atomic_read( &(kshutdown->m_shutdownProcessorID) );
	return shutdownProcessorID != -1;
}


bool KShutdown_isRebootOnFailEnabled( const volatile KShutdown* kshutdown )
{
	KDebug_assertArg( kshutdown != NULL );
	return (bool) Atomic_read( &(kshutdown->m_rebootOnFail) );
}


void KShutdown_setRebootOnFailEnabled( volatile KShutdown* kshutdown, bool rebootOnFail )
{
	KDebug_assertArg( kshutdown != NULL );
	Atomic_write( &(kshutdown->m_rebootOnFail), (uintptr_t) rebootOnFail );
}


int KShutdown_getRebootDelayInMilliseconds( const volatile KShutdown* kshutdown )
{
	KDebug_assertArg( kshutdown != NULL );
	return (int) Atomic_read( &(kshutdown->m_rebootDelayInMilliseconds) );
}


void KShutdown_setRebootDelayInMilliseconds(
	volatile KShutdown*	kshutdown,
	int					delayInMilliseconds
)
{
	KDebug_assertArg( kshutdown != NULL );
	KDebug_assertArg( delayInMilliseconds >= 0 );
	Atomic_write( &(kshutdown->m_rebootDelayInMilliseconds), (uintptr_t) delayInMilliseconds );
}


void KShutdown_fail( volatile KShutdown* kshutdown, const char* formatString, ... )
{
	KDebug_assertArg( kshutdown != NULL );
	KDebug_assertArg( formatString != NULL );

	va_list args;
	va_start( args, formatString );

	KShutdown_initiateShutdownMode( kshutdown );

	// If we made it this far, we're in shutdown mode and this processor is in complete control.
	// NOTE: This makes it safe to cast away volatile.
	KShutdown* unsynchKShutdown = (KShutdown*) kshutdown;

	KOut_vWrite( formatString, args );

	va_end( args );

	// No need to get this atomically -- no other processors should even be running.
	bool reboot = (bool) unsynchKShutdown->m_rebootOnFail;

	if (reboot)
	{
		KShutdown_doReboot( unsynchKShutdown );
	}
	else
	{
		KShutdown_doHalt( unsynchKShutdown );
	}
}


void KShutdown_halt( volatile KShutdown* kshutdown )
{
	KDebug_assertArg( kshutdown != NULL );

	KShutdown_initiateShutdownMode( kshutdown );

	// If we made it this far, we're in shutdown mode and this processor is in complete control.
	// NOTE: This makes it safe to cast away volatile.
	KShutdown* unsynchKShutdown = (KShutdown*) kshutdown;

	KShutdown_doHalt( unsynchKShutdown );
}


void KShutdown_reboot( volatile KShutdown* kshutdown )
{
	KDebug_assertArg( kshutdown != NULL );

	KShutdown_initiateShutdownMode( kshutdown );

	// If we made it this far, we're in shutdown mode and this processor is in complete control.
	// NOTE: This makes it safe to cast away volatile.
	KShutdown* unsynchKShutdown = (KShutdown*) kshutdown;

	KShutdown_doReboot( unsynchKShutdown );
}

