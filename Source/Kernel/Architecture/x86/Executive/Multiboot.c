// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/Executive/Multiboot.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Nov/13
//
// ===========================================================================
///
/// \file
///
/// \brief	This file defines utility functions for sorting and validating parts
///			of the Multiboot information structure.
///
// ===========================================================================


#include <stdint.h>
#include "Multiboot.h"
#include "Kernel/KCommon/KDebug.h"


size_t MBMemmapNode_getAdjustedLength( const MBMemmapNode* node )
{
	KDebug_assertArg( node != NULL );
	KDebug_assertArg( node->BaseAddrHigh == 0 );

	// MAINTENANCE NOTE: The second part of this comparison has been specially crafted to
	// avoid overflow and underflow.
	if ((node->LengthHigh != 0) ||
		(MAX_PHYS_ADDR - node->LengthLow < node->BaseAddrLow))
	{
		size_t realLength = MAX_PHYS_ADDR - node->BaseAddrLow;

		// Normally we'd just add 1 to finish the calculation (len = last - base + 1).
		// However, in the case where the base address is zero (allowable from the first part
		// of the "if" test above), adding 1 would result in overflow. This reflects the fact
		// that there can't be a 4GB region -- there can be at most a 4GB - 1 byte region.
		if (realLength < MAX_PHYS_ADDR)
		{
			realLength++;
		}
		return realLength;
	}
	else
	{
		// No adjustment needed.
		return node->LengthLow;
	}
}


const MBMemmapNode* MBMemmapNode_getNext( const MBMemmapNode* node )
{
	KDebug_assertArg( node != NULL );
	// MAINTENANCE NOTE: Watch the casting here! It is required to make the pointer arithmetic
	// work properly.
	return (const MBMemmapNode*) (((uintptr_t) node) + sizeof node->Size + node->Size);
}


bool MultibootInfo_isValidMemmapNode( const MultibootInfo* mbInfo, const MBMemmapNode* node )
{
	// Don't bother asserting that node is not NULL -- checking node is the whole point of
	// this function.
	KDebug_assertArg( mbInfo != NULL );

	if (!mbInfo->MemMapPresent)
	{
		return false;	// There is no memory map.
	}

	// MAINTENANCE NOTE: The calculation for end must be done by casting begin to an integral
	// type first. You can't just add MMapLength / sizeof( MBMemmapNode ) to begin, because these
	// structures are actually variable-length.
	const MBMemmapNode* begin	= mbInfo->MMapAddr;
	const MBMemmapNode* end		= (const MBMemmapNode*) (((uintptr_t) begin) + mbInfo->MMapLength);
	return ((begin <= node) && (node < end));
}

