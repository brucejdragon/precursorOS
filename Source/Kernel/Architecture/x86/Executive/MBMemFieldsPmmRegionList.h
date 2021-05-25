// ===========================================================================
//
//             Copyright (C) 2004-2006 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/Executive/MBMemFieldsPmmRegionList.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Dec/04
//
// ===========================================================================
///
/// \file
///
/// \brief	Implements the MBMemFieldsPmmRegionList class, which implements an enumerable
///			list of free RAM regions obtained from the "upper" and "lower" memory fields of
///			the MultibootInfo structure.
///
// ===========================================================================

#ifndef _KERNEL_ARCH_X86_EXECUTIVE_MBMEMFIELDSPMMREGIONLIST_H_
#define _KERNEL_ARCH_X86_EXECUTIVE_MBMEMFIELDSPMMREGIONLIST_H_


#include "Kernel/MM/IPmmRegionList.h"


// Forward declaration.
struct MultibootInfoStruct;



/// \brief	Defines the fields of MBMemFieldsPmmRegionList.
typedef struct MBMemFieldsPmmRegionListStruct
{
#ifdef _KERNEL_ARCH_X86_EXECUTIVE_MBMEMFIELDSPMMREGIONLIST_C_

	/// \brief	The current state of the enumeration.
	int m_state;

	/// \brief	The MultibootInfo structure containing the source memory fields.
	const struct MultibootInfoStruct* m_mbInfo;

#else

	#ifndef DOXYGEN_SHOULD_SKIP_THIS
	int									m_reserved0;
	const struct MultibootInfoStruct*	m_reserved1;
	#endif

#endif
} MBMemFieldsPmmRegionList;



/// \brief	Creates a new MBMemFieldsPmmRegionList instance.
///
/// \param mbInfo the MultibootInfo structure containing the source memory information.
///
/// \return a new MBRamPmmRegionList instance.
MBMemFieldsPmmRegionList MBMemFieldsPmmRegionList_create( const struct MultibootInfoStruct* mbInfo );


/// \brief	Gets a reference to the IPmmRegionList implementation of the given
///			MBMemFieldsPmmRegionList.
///
/// \param list the MBMemFieldsPmmRegionList instance.
///
/// \return the IPmmRegionList interface that \a list implements.
IPmmRegionList MBMemFieldsPmmRegionList_getAsPmmRegionList( MBMemFieldsPmmRegionList* list );


#endif

