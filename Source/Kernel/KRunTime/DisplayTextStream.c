// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/KRunTime/DisplayTextStream.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Jan/01
//
// ===========================================================================
///
///	\file
///
/// \brief	Contains the implementation of ITextStream that writes all text output
///			to the KernelDisplay object.
///
// ===========================================================================


#include <stddef.h>
#include "Kernel/HAL/KernelDisplay.h"
#include "Kernel/HAL/Processor.h"
#include "Kernel/HAL/Lock.h"
#include "Kernel/KRunTime/DisplayTextStream.h"
#include "Kernel/KRunTime/KShutdown.h"
#include "Kernel/KCommon/KDebug.h"
#include "Kernel/KCommon/KMem.h"


/// \brief	Contains numerical constants for the DisplayTextStream class.
enum DisplayTextStream_consts
{
	/// \brief	Buffer this many chars before flushing.
	BUFFER_SIZE = KDISPLAY_NUM_COLUMNS * KDISPLAY_NUM_ROWS,

	/// \brief	When buffer index reaches this, flush.
	BUFFER_UPPER_BOUND = BUFFER_SIZE
};



/// \brief	Defines the fields of the DisplayTextStream class.
typedef struct
{
	char			m_buffer[BUFFER_SIZE];	///< Buffer for collecting up characters to display.
	int				m_currentBufferIndex;	///< Position in buffer to store next character.
	KernelDisplay*	m_kernelDisplay;		///< The target for all output.
	Lock			m_lock;					///< Protects the buffer and kernel display.
} DisplayTextStream;



// Private functions

/// \brief	Acquires the lock on the DisplayTextStream unless the system is in shutdown mode.
///
/// \param displayStream	the DisplayTextStream instance, which implements ITextStream.
///
/// \return \a displayStream stripped of its volatile qualifier, indicating that it is
///			synchronized (either by its lock or by shutdown mode).
static DisplayTextStream* DisplayTextStream_lock( volatile DisplayTextStream* displayStream )
{
	volatile KShutdown* kshutdown = KShutdown_getInstance();

	if (!KShutdown_isInShutdownMode( kshutdown ))
	{
		Lock_acquire( &(displayStream->m_lock) );
	}
	return (DisplayTextStream*) displayStream;
}


/// \brief	Releases the lock on the DisplayTextStream unless the system is in shutdown mode.
///
/// \param displayStream	the DisplayTextStream instance, which implements ITextStream.
static void DisplayTextStream_unlock( DisplayTextStream* displayStream )
{
	volatile KShutdown* kshutdown = KShutdown_getInstance();

	if (!KShutdown_isInShutdownMode( kshutdown ))
	{
		Lock_release( &(displayStream->m_lock) );
	}
}


/// \brief	Implements ITextStream::flush() to copy the DisplayTextStream's buffer to the
///			KernelDisplay.
///
/// \param displayStream	the DisplayTextStream instance, which implements ITextStream.
static void DisplayTextStream_flush( DisplayTextStream* displayStream )
{
	KDebug_assertArg( displayStream != NULL );
	KDebug_assert(
		(0 <= displayStream->m_currentBufferIndex) &&
		(displayStream->m_currentBufferIndex <= BUFFER_UPPER_BOUND)
	);

	// Send the buffer to KernelDisplay.
	KernelDisplay_print(
		displayStream->m_kernelDisplay,
		displayStream->m_buffer,
		displayStream->m_currentBufferIndex
	);

	// Reset the index to the beginning.
	displayStream->m_currentBufferIndex = 0;
}


/// \brief	Implements ITextStream::write() to buffer text in preparation for output to the
///			KernelDisplay.
///
/// \param displayStream	the DisplayTextStream instance that implements ITextStream.
/// \param c				character to add to the output buffer. See the definition of the
///							ITextStream interface for a description of allowable control
///							characters.
static void DisplayTextStream_write( DisplayTextStream* displayStream, char c )
{
	KDebug_assertArg( displayStream != NULL );

	switch (c)
	{
	case '\t':
		DisplayTextStream_flush( displayStream );
		KernelDisplay_tab( displayStream->m_kernelDisplay );
		break;

	case '\n':
		DisplayTextStream_flush( displayStream );
		KernelDisplay_newLine( displayStream->m_kernelDisplay );
		break;

	case '\0':
		DisplayTextStream_flush( displayStream );
		break;

	default:
		// If the buffer is full, flush first.
		if (displayStream->m_currentBufferIndex == BUFFER_UPPER_BOUND)
		{
			DisplayTextStream_flush( displayStream );
		}

		KDebug_assert(
			(0 <= displayStream->m_currentBufferIndex) &&
			(displayStream->m_currentBufferIndex < BUFFER_UPPER_BOUND)
		);

		displayStream->m_buffer[displayStream->m_currentBufferIndex] = c;
		displayStream->m_currentBufferIndex++;
		break;
	}
}


/// \brief	Implements ITextStream::flush() to copy the DisplayTextStream's buffer to the
///			KernelDisplay in a thread-safe manner.
///
/// \param displayStream	the DisplayTextStream instance, which implements ITextStream.
static void DisplayTextStream_lockedFlush( volatile DisplayTextStream* displayStream )
{
	DisplayTextStream* lockedStream = DisplayTextStream_lock( displayStream );
	DisplayTextStream_flush( lockedStream );
	DisplayTextStream_unlock( lockedStream );
}


/// \brief	Implements ITextStream::write() to buffer text in a thread-safe manner in preparation
///			for output to the KernelDisplay.
///
/// \param displayStream	the DisplayTextStream instance that implements ITextStream.
/// \param c				character to add to the output buffer. See the definition of the
///							ITextStream interface for a description of allowable control
///							characters.
static void DisplayTextStream_lockedWrite( volatile DisplayTextStream* displayStream, char c )
{
	DisplayTextStream* lockedStream = DisplayTextStream_lock( displayStream );
	DisplayTextStream_write( lockedStream, c );
	DisplayTextStream_unlock( lockedStream );
}



/// \brief	Interface dispatch table for DisplayTextStream's implementation of ITextStream.
static ITextStream_itable s_itable =
{
	(ITextStream_writeFunc) DisplayTextStream_lockedWrite,
	(ITextStream_flushFunc) DisplayTextStream_lockedFlush
};


/// \brief	Global instance of DisplayTextStream.
static volatile DisplayTextStream s_instance;



// Public functions

void DisplayTextStream_init( void )
{
	KernelDisplay_init();

	KDebug_assert( Processor_areInterruptsDisabled() );

	// NOTE: The contract for this method states that all other processors should be halted and
	// interrupts should be disabled before this method is called. Therefore, it is OK to cast
	// away volatile here.
	KMem_set( ((DisplayTextStream*) &s_instance)->m_buffer, 0, BUFFER_SIZE );

	s_instance.m_currentBufferIndex	= 0;	// Reset the index to the beginning.
	s_instance.m_lock = Lock_create();

	// NOTE: We are casting away volatile here because this class, by default, has unsynchronized
	// semantics. We are sort of taking responsibility for synchronization away from KernelDisplay,
	// and passing it up the chain to SynchronizedTextStream.
	s_instance.m_kernelDisplay = (KernelDisplay*) KernelDisplay_getInstance();
}


void DisplayTextStream_reset( void )
{
	KernelDisplay_reset( s_instance.m_kernelDisplay );

	KDebug_assert( Processor_areInterruptsDisabled() );

	// NOTE: The contract for this method states that all other processors should be halted and
	// interrupts should be disabled before this method is called. Therefore, it is OK to cast
	// away volatile and use the unlocked flush() method here.
	DisplayTextStream_flush( (DisplayTextStream*) &s_instance );
}


ITextStream DisplayTextStream_getTextStream()
{
	ITextStream stream;
	stream.obj	= &s_instance;	// Point to the global DisplayTextStream object.
	stream.iptr	= &s_itable;	// Point at the right interface dispatch table.
	return stream;
}


