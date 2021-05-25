// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/MM/ConcatPmmRegionList.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2006/Feb/05
//
// ===========================================================================
///
///	\file
///
/// \brief	Contains the implementation of IPmmRegionList that enumerates over
///			two other IPmmRegionLists in sequence.
///
// ===========================================================================


#include <stddef.h>
#include "Kernel/KCommon/KDebug.h"
#include "Kernel/MM/PmmRegion.h"

#define _KERNEL_MM_CONCATPMMREGIONLIST_C_
#include "Kernel/MM/ConcatPmmRegionList.h"


// Private constants

/// \brief	Defines private constants for the implementation of ConcatPmmRegionList.
enum ConcatPmmRegionList_consts
{
	STATE_RESET		= -1,					///< Iterator has been reset.
	STATE_ON_FIRST	= STATE_RESET + 1,		///< Iterator points into the first list.
	STATE_ON_SECOND	= STATE_ON_FIRST + 1,	///< Iterator points into the second list.
	STATE_PAST_END	= STATE_ON_SECOND + 1	///< Iterator points past the end of the two lists.
};



// Private functions

/// \brief	Resets the iterator to the position before the first element (if any).
///
/// \param list the ConcatPmmRegionList.
static void ConcatPmmRegionList_reset( ConcatPmmRegionList* list )
{
	KDebug_assertArg( list != NULL );
	list->m_firstList.iptr->reset( list->m_firstList.obj );
	list->m_secondList.iptr->reset( list->m_secondList.obj );
	list->m_state = STATE_RESET;
}


/// \brief	Attempts to advance the iterator forward by one position.
///
/// \param list the ConcatPmmRegionList.
///
/// \retval true	the iterator advanced to the next valid position.
/// \retval false	the iterator advanced beyond the last valid position, the iterator was already
///					beyond the last valid position, or the list is empty.
static bool ConcatPmmRegionList_moveNext( ConcatPmmRegionList* list )
{
	KDebug_assertArg( list != NULL );

	switch (list->m_state)
	{
	case STATE_RESET:
		list->m_state = STATE_ON_FIRST;
		// Intentional fall-through...
		
	case STATE_ON_FIRST:
		if (list->m_firstList.iptr->moveNext( list->m_firstList.obj ))
		{
			return true;
		}
		else
		{
			list->m_state = STATE_ON_SECOND;
		}
		// Intentional fall-through...
		
	case STATE_ON_SECOND:
		if (list->m_secondList.iptr->moveNext( list->m_secondList.obj ))
		{
			return true;
		}
		else
		{
			list->m_state = STATE_PAST_END;
		}
		// Intentional fall-through...

	case STATE_PAST_END:
		return false;

	default:
		KDebug_assert( false );
		return false;
	}
}


/// \brief	Returns the PmmRegion at the current position in the list.
///
/// \param list the ConcatPmmRegionList.
///
/// If the iterator is not currently on a valid position, this method will cause a bugcheck in
/// checked builds and will probably cause a system failure in free builds.
///
/// \return the PmmRegion at the current position in the list.
static PmmRegion ConcatPmmRegionList_getCurrent( const ConcatPmmRegionList* list )
{
	KDebug_assertArg( list != NULL );

	PmmRegion region;
	switch (list->m_state)
	{
	case STATE_ON_FIRST:
		region = list->m_firstList.iptr->getCurrent( list->m_firstList.obj );
		break;
		
	case STATE_ON_SECOND:
		region = list->m_secondList.iptr->getCurrent( list->m_secondList.obj );
		break;
		
	default:
		KDebug_assert( false );
		break;
	}
	return region;
}



/// \brief	Interface dispatch table for ConcatPmmRegionList's implementation of IPmmRegionList.
static IPmmRegionList_itable s_itable =
{
	(IPmmRegionList_resetFunc) ConcatPmmRegionList_reset,
	(IPmmRegionList_moveNextFunc) ConcatPmmRegionList_moveNext,
	(IPmmRegionList_getCurrentFunc) ConcatPmmRegionList_getCurrent
};



// Public functions

ConcatPmmRegionList ConcatPmmRegionList_create(
	IPmmRegionList	firstList,
	IPmmRegionList	secondList
)
{
	ConcatPmmRegionList list;
	list.m_firstList = firstList;
	list.m_secondList = secondList;
	list.m_state = STATE_PAST_END;
	return list;
}


IPmmRegionList ConcatPmmRegionList_getAsPmmRegionList( ConcatPmmRegionList* list )
{
	IPmmRegionList ilist;
	ilist.obj	= list;			// Point to the given object.
	ilist.iptr	= &s_itable;	// Point at the right interface dispatch table.
	return ilist;
}

