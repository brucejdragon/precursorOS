// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/HAL/TrapFrame_x86.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2004/Oct/31
//
// ===========================================================================
///
/// \file
///
/// \brief	This file implements the TrapFrame class for the x86 architecture.
///
// ===========================================================================


#include "Kernel/Architecture/x86/HAL/TrapFrame_x86.h"
#include "Kernel/KCommon/KDebug.h"


size_t TrapFrame_getErrorCode( const TrapFrame* frame )
{
	KDebug_assertArg( frame != NULL );

	// This is IA-32, so size_t is 32 bits in this case. So is the raw error code value.
	return (size_t) frame->errorCode.RawValue;
}


uint32_t TrapFrame_getInterruptVectorNumber( const TrapFrame* frame )
{
	KDebug_assertArg( frame != NULL );

	// Zero-extend the 8-bit x86 vector number to 32 bits.
	return (uint32_t) frame->interruptVectorNumber;
}


bool TrapFrame_isKernelInterrupted( const TrapFrame* frame )
{
	KDebug_assertArg( frame != NULL );

	// The kernel is interrupted if the frame's CS indicates a CPL of 0.
	return (frame->cs.CodeSeg.CPL == 0);
}


