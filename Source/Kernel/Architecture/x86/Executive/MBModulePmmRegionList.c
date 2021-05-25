// ===========================================================================
//
//             Copyright (C) 2004-2006 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/Executive/MBModulePmmRegionList.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2006/Jan/15
//
// ===========================================================================
///
///	\file
///
/// \brief	Contains the implementation of IPmmRegionList that enumerates in part over
///			the MultibootInfo module list.
///
// ===========================================================================


#include <stddef.h>
#include <stdbool.h>
#include "Kernel/KCommon/KDebug.h"
#include "Kernel/MM/PmmRegion.h"
#include "Kernel/MM/MM.h"
#include "Multiboot.h"

#define _KERNEL_ARCH_X86_EXECUTIVE_MBMODULEPMMREGIONLIST_C_
#include "MBModulePmmRegionList.h"



// Private constants.

/// \brief	Defines private constants for the implementation of MBModulePmmRegionList.
enum MBModulePmmRegionList_consts
{
	STATE_PAST_END = 0,	///< Iterator points past the end of the list.
	STATE_RESET = 1,	///< Iterator has been reset.
	STATE_KERNEL = 2,	///< Iterator points to kernel image region.
	STATE_MODULE = 3	///< Iterator points to a module. The module index is valid.
};



// Private functions

/// \brief	Resets the iterator to the position before the first element (if any).
///
/// \param list the MBModulePmmRegionList.
static void MBModulePmmRegionList_reset( MBModulePmmRegionList* list )
{
	KDebug_assertArg( list != NULL );
	list->m_currentModuleIndex = 0;
	list->m_state = STATE_RESET;
}


/// \brief	Attempts to advance the iterator forward by one position.
///
/// \param list the MBModulePmmRegionList.
///
/// \retval true	the iterator advanced to the next valid position.
/// \retval false	the iterator advanced beyond the last valid position, the iterator was already
///					beyond the last valid position, or the list is empty.
static bool MBModulePmmRegionList_moveNext( MBModulePmmRegionList* list )
{
	KDebug_assertArg( list != NULL );

	// Where we go next depends on where we are and on whether the module list is even available.
	switch (list->m_state)
	{
	case STATE_PAST_END:
		return false;

	case STATE_RESET:
		list->m_state = STATE_KERNEL;
		return true;

	case STATE_KERNEL:
		if (!list->m_mbInfo->ModulesPresent)
		{
			list->m_state = STATE_PAST_END;
			return false;
		}
		else
		{
			list->m_currentModuleIndex = 0;
			list->m_state = STATE_MODULE;
			
			if (list->m_currentModuleIndex == list->m_mbInfo->ModsCount)
			{
				list->m_state = STATE_PAST_END;
				return false;
			}
			else
			{
				return true;
			}
		}

	case STATE_MODULE:		
		KDebug_assert( list->m_mbInfo->ModulesPresent );
		KDebug_assert( list->m_currentModuleIndex < list->m_mbInfo->ModsCount );

		list->m_currentModuleIndex++;
		if (list->m_currentModuleIndex == list->m_mbInfo->ModsCount)
		{
			list->m_state = STATE_PAST_END;
			return false;
		}
		else
		{
			return true;
		}

	default:
		KDebug_assert( false );
		return false;
	}
}


/// \brief	Returns the PmmRegion at the current position in the list.
///
/// \param list the MBModulePmmRegionList.
///
/// If the iterator is not currently on a valid position, this method will cause a bugcheck in
/// checked builds and will return garbage in free builds.
///
/// \return the PmmRegion at the current position in the list.
static PmmRegion MBModulePmmRegionList_getCurrent( const MBModulePmmRegionList* list )
{
	KDebug_assertArg( list != NULL );

	PmmRegion region;
	switch (list->m_state)
	{
	case STATE_KERNEL:
		region =
			PmmRegion_create(
				MM_KERNEL_LOAD_PHYS_ADDR,
				(size_t) (MM_getKernelEndPhysAddr() - MM_KERNEL_LOAD_PHYS_ADDR)
			);
		break;

	case STATE_MODULE:
		{
			KDebug_assert( list->m_currentModuleIndex < list->m_mbInfo->ModsCount );
			MBModuleInfo* mbModule = list->m_mbInfo->ModsAddr + list->m_currentModuleIndex;
			region = PmmRegion_create( mbModule->ModStart, mbModule->ModEnd - mbModule->ModStart );
		}
		break;

	default:
		KDebug_assert( false );
		break;
	}
	return region;
}



/// \brief	Interface dispatch table for MBModulePmmRegionList's implementation
///			of IPmmRegionList.
static IPmmRegionList_itable s_itable =
{
	(IPmmRegionList_resetFunc) MBModulePmmRegionList_reset,
	(IPmmRegionList_moveNextFunc) MBModulePmmRegionList_moveNext,
	(IPmmRegionList_getCurrentFunc) MBModulePmmRegionList_getCurrent
};



// Public functions

// Using structs here so Doxygen doesn't complain...
MBModulePmmRegionList MBModulePmmRegionList_create( const struct MultibootInfoStruct* mbInfo )
{
	KDebug_assertArg( mbInfo != NULL );

	MBModulePmmRegionList list;
	list.m_currentModuleIndex	= 0;
	list.m_mbInfo				= mbInfo;
	list.m_state				= STATE_PAST_END;	// Caller must reset before use.
	return list;
}


IPmmRegionList MBModulePmmRegionList_getAsPmmRegionList( MBModulePmmRegionList* list )
{
	IPmmRegionList ilist;
	ilist.obj	= list;			// Point to the given object.
	ilist.iptr	= &s_itable;	// Point at the right interface dispatch table.
	return ilist;
}

