// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/KRunTime/ITextStream.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2004/Nov/27
//
// ===========================================================================
///
/// \file
///
/// \brief	Defines the ITextStream interface, which represents a target for
///			text output.
///
/// This interface allows the text-output features of KRunTime to direct text output
/// to any appropriate I/O device, without the need for a direct dependency on
/// the appropriate kernel driver code. Note that this interface is simple and is
/// only meant to be implemented by other components in the kernel itself. It is
/// called only for kernel display tasks (on bootup, during a system failure, etc.).
///
/// Note that every reference to ITextStream is effectively volatile-qualified.
///
// ===========================================================================

#ifndef _KERNEL_KRUNTIME_ITEXTSTREAM_H_
#define _KERNEL_KRUNTIME_ITEXTSTREAM_H_


/// \brief	Defines the method signature for ITextStream::write().
///
/// \param this	the object that implements ITextStream.
/// \param c	a character to output. It can be a tab or newline, which will affect the output
///				formatting appropriately. No other non-printable characters are specially
///				recognized, except for a null character. In the case of a buffering implementation,
///				if \a c is a tab, newline, or a null character, or if the implementation's buffer
///				is full, the implementation must call its ITextStream_flushFunc.
///
/// This method is called to buffer a character of text for output to some I/O device.
typedef void (*ITextStream_writeFunc)( volatile void* this, char c );


/// \brief	Defines the method signature for ITextStream::flush().
///
/// \param this	the object that implements ITextStream.
///
/// This method is called to flush any buffered text to the appropriate I/O device. Implementations
/// that do not perform buffering are free to take no action when this method is called.
typedef void (*ITextStream_flushFunc)( volatile void* this );



/// \brief	Defines the interface dispatch table for ITextStream.
typedef struct
{
	/// \brief	Pointer to a particular implementation of ITextStream::write().
	ITextStream_writeFunc write;

	/// \brief	Pointer to a particular implementation of ITextStream::flush().
	ITextStream_flushFunc flush;

} ITextStream_itable;



/// \brief	Defines a reference to an implementation of the ITextStream interface.
///
/// \note
/// All implementations of ITextStream must be thread-safe. Therefore, all references to
/// ITextStream should be volatile-qualified.
typedef struct
{
	const ITextStream_itable*	iptr;	///< Pointer to the interface dispatch table.
	volatile void*				obj;	///< Pointer to the object that implements the interface.

} ITextStream;


#endif

