// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/Executive/MBMemmapPmmRegionList.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Dec/03
//
// ===========================================================================
///
///	\file
///
/// \brief	Contains the implementation of IPmmRegionList that enumerates over
///			a list of MultibootInfo memory map nodes.
///
// ===========================================================================


#include <stddef.h>
#include "Kernel/KCommon/KDebug.h"
#include "Kernel/MM/PmmRegion.h"
#include "Multiboot.h"

#define _KERNEL_ARCH_X86_EXECUTIVE_MBMEMMAPPMMREGIONLIST_C_
#include "MBMemmapPmmRegionList.h"



// Private functions

/// \brief	Indicates whether the current node of the list is valid.
///
/// \param list the MBMemmapPmmRegionList.
///
/// \retval true	the current MBMemmapNode* is valid.
/// \retval false	the current MBMemmapNode* is not valid.
static inline bool MBMemmapPmmRegionList_isCurrentNodeValid( const MBMemmapPmmRegionList* list )
{
	KDebug_assertArg( list != NULL );
	return MultibootInfo_isValidMemmapNode( list->m_mbInfo, list->m_currentMemmapNode );
}


/// \brief	Resets the iterator to the position before the first element (if any).
///
/// \param list the MBMemmapPmmRegionList.
static void MBMemmapPmmRegionList_reset( MBMemmapPmmRegionList* list )
{
	KDebug_assertArg( list != NULL );
	list->m_currentMemmapNode = NULL;
}


/// \brief	Attempts to advance the iterator forward by one position.
///
/// \param list the MBMemmapPmmRegionList.
///
/// If \a list is a RAM list, this method will skip over any memory map nodes whose type is not
/// "available RAM". Otherwise, it will skip nodes whose type is "available RAM".
///
/// \retval true	the iterator advanced to the next valid position.
/// \retval false	the iterator advanced beyond the last valid position, the iterator was already
///					beyond the last valid position, or the list is empty.
static bool MBMemmapPmmRegionList_moveNext( MBMemmapPmmRegionList* list )
{
	KDebug_assertArg( list != NULL );

	bool memMapPresent	= list->m_mbInfo->MemMapPresent;
	bool isReset		= (list->m_currentMemmapNode == NULL);
	bool isCurrentValid	= MBMemmapPmmRegionList_isCurrentNodeValid( list );

	if (!memMapPresent || (!isCurrentValid && !isReset))
	{
		// Gone past the end, or there is no memory map.
		return false;
	}
	else if (isReset)
	{
		// There is a memory map, and we've just been reset.
		list->m_currentMemmapNode = list->m_mbInfo->MMapAddr;
	}
	else
	{
		// We were already on a valid node. Go to the next one.
		list->m_currentMemmapNode = MBMemmapNode_getNext( list->m_currentMemmapNode );
	}

	// Keep skipping ahead until we find a valid node of the correct type.
	while (true)
	{
		if (!MBMemmapPmmRegionList_isCurrentNodeValid( list ))
		{
			return false;	// Gone past the end.
		}
		else if ((list->m_isRamList) && (list->m_currentMemmapNode->Type == 1))
		{
			return true;
		}
		else if (!(list->m_isRamList) && (list->m_currentMemmapNode->Type != 1))
		{
			return true;
		}

		// Fallthrough -- continue skipping ahead.
		list->m_currentMemmapNode = MBMemmapNode_getNext( list->m_currentMemmapNode );
	}
}


/// \brief	Returns the PmmRegion at the current position in the list.
///
/// \param list the MBMemmapPmmRegionList.
///
/// If the iterator is not currently on a valid position, this method will cause a bugcheck in
/// checked builds and will probably cause a system failure in free builds.
///
/// \return the PmmRegion at the current position in the list.
static PmmRegion MBMemmapPmmRegionList_getCurrent( const MBMemmapPmmRegionList* list )
{
	KDebug_assertArg( list != NULL );
	KDebug_assert( MBMemmapPmmRegionList_isCurrentNodeValid( list ) );
	KDebug_assert( list->m_currentMemmapNode->BaseAddrHigh == 0 );

	return
		PmmRegion_create(
			list->m_currentMemmapNode->BaseAddrLow,
			MBMemmapNode_getAdjustedLength( list->m_currentMemmapNode )
		);
}



/// \brief	Interface dispatch table for MBMemmapPmmRegionList's implementation of IPmmRegionList.
static IPmmRegionList_itable s_itable =
{
	(IPmmRegionList_resetFunc) MBMemmapPmmRegionList_reset,
	(IPmmRegionList_moveNextFunc) MBMemmapPmmRegionList_moveNext,
	(IPmmRegionList_getCurrentFunc) MBMemmapPmmRegionList_getCurrent
};



// Public functions

// Using structs here so Doxygen doesn't complain...
MBMemmapPmmRegionList MBMemmapPmmRegionList_create(
	const struct MultibootInfoStruct*	mbInfo,
	bool								isRamList
)
{
	KDebug_assertArg( mbInfo != NULL );

	MBMemmapPmmRegionList list;
	list.m_currentMemmapNode	= NULL;
	list.m_mbInfo				= mbInfo;
	list.m_isRamList			= isRamList;
	return list;
}


IPmmRegionList MBMemmapPmmRegionList_getAsPmmRegionList( MBMemmapPmmRegionList* list )
{
	IPmmRegionList ilist;
	ilist.obj	= list;			// Point to the given object.
	ilist.iptr	= &s_itable;	// Point at the right interface dispatch table.
	return ilist;
}

