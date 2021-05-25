// ===========================================================================
//
//             Copyright (C) 2004-2006 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/Executive/MBMemmapPmmRegionList.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Dec/03
//
// ===========================================================================
///
/// \file
///
/// \brief	Implements the MBMemmapPmmRegionList class, which implements an enumerable
///			list of physical memory regions obtained from the MultibootInfo structure.
///
// ===========================================================================

#ifndef _KERNEL_ARCH_X86_EXECUTIVE_MBMEMMAPPMMREGIONLIST_H_
#define _KERNEL_ARCH_X86_EXECUTIVE_MBMEMMAPPMMREGIONLIST_H_


#include <stdbool.h>
#include "Kernel/MM/IPmmRegionList.h"


// Forward declarations.
struct MBMemmapNodeStruct;
struct MultibootInfoStruct;



/// \brief	Defines the fields of MBMemmapPmmRegionList.
typedef struct MBMemmapPmmRegionListStruct
{
#ifdef _KERNEL_ARCH_X86_EXECUTIVE_MBMEMMAPPMMREGIONLIST_C_

	/// \brief	The current source element for the enumeration.
	const struct MBMemmapNodeStruct* m_currentMemmapNode;

	/// \brief	The MultibootInfo structure containing the source memory map.
	const struct MultibootInfoStruct* m_mbInfo;

	/// \brief	Indicates whether the list enumerates through RAM regions or reserved regions.
	bool m_isRamList;

#else

	#ifndef DOXYGEN_SHOULD_SKIP_THIS
	const struct MBMemmapNodeStruct*	m_reserved0;
	const struct MultibootInfoStruct*	m_reserved1;
	bool								m_reserved2;
	#endif

#endif
} MBMemmapPmmRegionList;



/// \brief	Creates a new MBMemmapPmmRegionList instance.
///
/// \param mbInfo		the MultibootInfo structure containing the source memory information.
/// \param isRamList	indicates whether the list enumerates through RAM regions or reserved
///						regions.
///
/// \return a new MBMemmapPmmRegionList instance.
MBMemmapPmmRegionList MBMemmapPmmRegionList_create(
	const struct MultibootInfoStruct*	mbInfo,
	bool								isRamList
);


/// \brief	Gets a reference to the IPmmRegionList implementation of the given
///			MBMemmapPmmRegionList.
///
/// \param list the MBMemmapPmmRegionList instance.
///
/// \return the IPmmRegionList interface that \a list implements.
IPmmRegionList MBMemmapPmmRegionList_getAsPmmRegionList( MBMemmapPmmRegionList* list );


#endif

