// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/KRunTime/ITextWritable.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2004/Dec/22
//
// ===========================================================================
///
/// \file
///
/// \brief	Defines the ITextWritable interface, which represents an object
///			whose contents can be written as text output.
///
/// This interface makes it possible to output as text any object via a
/// TextWriter.
///
// ===========================================================================

#ifndef _KERNEL_KRUNTIME_ITEXTWRITABLE_H_
#define _KERNEL_KRUNTIME_ITEXTWRITABLE_H_


/// \brief	Forward declaration of TextWriter.
struct TextWriterStruct;



/// \brief	Defines the method signature for ITextWritable::writeTo().
///
/// \param this		the object that implements ITextWritable.
/// \param writer	the TextWriter to use to output the object contents.
///
/// This method is called to output the given object as text to some I/O device.
typedef void (*ITextWritable_writeToFunc)( void* this, struct TextWriterStruct* writer );



/// \brief	Defines the interface dispatch table for ITextWritable.
typedef struct
{
	/// \brief	Pointer to a particular implementation of ITextWritable::writeTo().
	ITextWritable_writeToFunc writeTo;

} ITextWritable_itable;



/// \brief	Defines a reference to an implementation of the ITextWritable interface.
typedef struct
{
	const ITextWritable_itable*	iptr;	///< Pointer to the interface dispatch table.
	void*						obj;	///< Pointer to the object that implements the interface.

} ITextWritable;


#endif

