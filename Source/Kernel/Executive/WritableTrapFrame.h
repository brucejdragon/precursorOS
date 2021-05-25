// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Executive/WritableTrapFrame.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Apr/24
//
// ===========================================================================
///
///	\file
///
/// \brief	Defines an implementation of ITextWritable that writes the contents
///			of a TrapFrame to a TextWriter.
///
// ===========================================================================

#ifndef _KERNEL_EXECUTIVE_WRITABLETRAPFRAME_H_
#define _KERNEL_EXECUTIVE_WRITABLETRAPFRAME_H_


#include "Kernel/HAL/TrapFrame.h"
#include "Kernel/KRunTime/ITextWritable.h"



/// \brief	Creates an implementation of ITextWritable from the given TrapFrame that
///			will output its contents to a TextWriter.
///
/// \param trapFrame	the TrapFrame to be output.
///
/// \return an ITextWritable reference.
ITextWritable WritableTrapFrame_getAsTextWritable( const TrapFrame* trapFrame );


#endif

