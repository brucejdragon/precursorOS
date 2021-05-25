// ===========================================================================
//
//             Copyright (C) 2004-2006 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/MM/ConcatPmmRegionList.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2006/Feb/05
//
// ===========================================================================
///
/// \file
///
/// \brief	Implements the ConcatPmmRegionList class, which implements an enumerable
///			list of physical memory regions that is the result of concatenating two
///			IPmmRegionList implementations together.
///
// ===========================================================================

#ifndef _KERNEL_MM_CONCATPMMREGIONLIST_H_
#define _KERNEL_MM_CONCATPMMREGIONLIST_H_


#include "Kernel/MM/IPmmRegionList.h"


/// \brief	Defines the fields of ConcatPmmRegionList.
typedef struct ConcatPmmRegionListStruct
{
#ifdef _KERNEL_MM_CONCATPMMREGIONLIST_C_

	/// \brief	The first section of the concatenated list.
	IPmmRegionList m_firstList;

	/// \brief	The second section of the concatenated list.
	IPmmRegionList m_secondList;

	/// \brief	Indicates the current position of the enumeration.
	int m_state;

#else

	#ifndef DOXYGEN_SHOULD_SKIP_THIS
	IPmmRegionList	m_reserved0;
	IPmmRegionList	m_reserved1;
	int				m_reserved2;
	#endif

#endif
} ConcatPmmRegionList;



/// \brief	Creates a new ConcatPmmRegionList instance.
///
/// \param firstList	the list to comprise the first section of the concatenated list.
/// \param secondList	the list to comprise the second section of the concatenated list.
///
/// \return a new ConcatPmmRegionList instance.
ConcatPmmRegionList ConcatPmmRegionList_create(
	IPmmRegionList	firstList,
	IPmmRegionList	secondList
);


/// \brief	Gets a reference to the IPmmRegionList implementation of the given
///			ConcatPmmRegionList.
///
/// \param list the ConcatPmmRegionList instance.
///
/// \return the IPmmRegionList interface that \a list implements.
IPmmRegionList ConcatPmmRegionList_getAsPmmRegionList( ConcatPmmRegionList* list );


#endif

