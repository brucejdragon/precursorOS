// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/Executive/MBMemFieldsPmmRegionList.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Dec/04
//
// ===========================================================================
///
///	\file
///
/// \brief	Contains the implementation of IPmmRegionList that enumerates over
///			the MultibootInfo "upper" and "lower" memory fields.
///
// ===========================================================================


#include <stddef.h>
#include <stdbool.h>
#include "Kernel/KCommon/KDebug.h"
#include "Kernel/MM/PmmRegion.h"
#include "Kernel/MM/MM.h"
#include "Multiboot.h"

#define _KERNEL_ARCH_X86_EXECUTIVE_MBMEMFIELDSPMMREGIONLIST_C_
#include "MBMemFieldsPmmRegionList.h"


// Private constants.

/// \brief	Defines private constants for the implementation of MBMemFieldsPmmRegionList.
enum MBMemFieldsPmmRegionList_consts
{
	STATE_RESET	= -1,					///< Iterator has been reset.
	STATE_LOWER	= STATE_RESET + 1,		///< Iterator points at "lower memory" field.
	STATE_UPPER	= STATE_LOWER + 1,		///< Iterator points at "upper memory" fields.
	STATE_PAST_END	= STATE_UPPER + 1	///< Iterator points past the end of the list.
};



// Private functions

/// \brief	Resets the iterator to the position before the first element (if any).
///
/// \param list the MBMemFieldsPmmRegionList.
static void MBMemFieldsPmmRegionList_reset( MBMemFieldsPmmRegionList* list )
{
	KDebug_assertArg( list != NULL );
	list->m_state = STATE_RESET;
}


/// \brief	Attempts to advance the iterator forward by one position.
///
/// \param list the MBMemFieldsPmmRegionList.
///
/// \retval true	the iterator advanced to the next valid position.
/// \retval false	the iterator advanced beyond the last valid position, the iterator was already
///					beyond the last valid position, or the list is empty.
static bool MBMemFieldsPmmRegionList_moveNext( MBMemFieldsPmmRegionList* list )
{
	KDebug_assertArg( list != NULL );

	// Where we go next depends on where we are and on whether the memory fields are even
	// available.
	if (list->m_mbInfo->MemFieldsPresent)
	{
		switch (list->m_state)
		{
		case STATE_RESET:
		case STATE_LOWER:
			list->m_state++;
			return true;

		case STATE_UPPER:
			list->m_state++;
			// Intentional fall-through...
		case STATE_PAST_END:
			return false;

		default:
			KDebug_assert( false );
			return false;
		}
	}
	else
	{
		switch (list->m_state)
		{
		case STATE_RESET:
			list->m_state = STATE_PAST_END;
			// Intentional fall-through...
		case STATE_PAST_END:
			return false;

		default:
			KDebug_assert( false );
			return false;
		}
	}
}


/// \brief	Returns the PmmRegion at the current position in the list.
///
/// \param list the MBMemFieldsPmmRegionList.
///
/// If the iterator is not currently on a valid position, this method will cause a bugcheck in
/// checked builds and will return garbage in free builds.
///
/// \return the PmmRegion at the current position in the list.
static PmmRegion MBMemFieldsPmmRegionList_getCurrent( const MBMemFieldsPmmRegionList* list )
{
	KDebug_assertArg( list != NULL );

	PmmRegion region;
	switch (list->m_state)
	{
	case STATE_LOWER:
		// We won't bother leaving room for the real-mode IVT or BIOS data area, or whatever is
		// down there. Precursor is strictly a Protected-Mode OS.
		region = PmmRegion_create( 0, (size_t) MM_KB( list->m_mbInfo->MemLower ) );
		break;

	case STATE_UPPER:
		region =
			PmmRegion_create(
				(phys_addr_t) MM_MB( 1 ),
				(size_t) MM_KB( list->m_mbInfo->MemUpper )
			);
		break;

	default:
		KDebug_assert( false );
		break;
	}
	return region;
}



/// \brief	Interface dispatch table for MBMemFieldsPmmRegionList's implementation
///			of IPmmRegionList.
static IPmmRegionList_itable s_itable =
{
	(IPmmRegionList_resetFunc) MBMemFieldsPmmRegionList_reset,
	(IPmmRegionList_moveNextFunc) MBMemFieldsPmmRegionList_moveNext,
	(IPmmRegionList_getCurrentFunc) MBMemFieldsPmmRegionList_getCurrent
};



// Public functions

// Using structs here so Doxygen doesn't complain...
MBMemFieldsPmmRegionList MBMemFieldsPmmRegionList_create( const struct MultibootInfoStruct* mbInfo )
{
	KDebug_assertArg( mbInfo != NULL );

	MBMemFieldsPmmRegionList list;
	list.m_mbInfo	= mbInfo;
	list.m_state	= STATE_PAST_END;	// Caller must reset before use.
	return list;
}


IPmmRegionList MBMemFieldsPmmRegionList_getAsPmmRegionList( MBMemFieldsPmmRegionList* list )
{
	IPmmRegionList ilist;
	ilist.obj	= list;			// Point to the given object.
	ilist.iptr	= &s_itable;	// Point at the right interface dispatch table.
	return ilist;
}

