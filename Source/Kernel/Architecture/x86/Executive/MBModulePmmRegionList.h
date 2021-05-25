// ===========================================================================
//
//             Copyright (C) 2004-2006 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/Executive/MBModulePmmRegionList.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2006/Jan/15
//
// ===========================================================================
///
/// \file
///
/// \brief	Implements the MBModulePmmRegionList class, which implements an enumerable
///			list of used RAM regions obtained in part from the module list of the MultibootInfo
///			structure.
///
// ===========================================================================

#ifndef _KERNEL_ARCH_X86_EXECUTIVE_MBMODULEPMMREGIONLIST_H_
#define _KERNEL_ARCH_X86_EXECUTIVE_MBMODULEPMMREGIONLIST_H_


#include "Kernel/MM/IPmmRegionList.h"


// Forward declaration.
struct MultibootInfoStruct;



/// \brief	Defines the fields of MBModulePmmRegionList.
typedef struct MBModulePmmRegionListStruct
{
#ifdef _KERNEL_ARCH_X86_EXECUTIVE_MBMODULEPMMREGIONLIST_C_

	/// \brief	The current state of the enumeration.
	int m_state;
	
	/// \brief	The index of the current module, if the enumeration state is valid.
	size_t m_currentModuleIndex;

	/// \brief	The MultibootInfo structure containing the source module list.
	const struct MultibootInfoStruct* m_mbInfo;

#else

	#ifndef DOXYGEN_SHOULD_SKIP_THIS
	int									m_reserved0;
	size_t								m_reserved1;
	const struct MultibootInfoStruct*	m_reserved2;
	#endif

#endif
} MBModulePmmRegionList;



/// \brief	Creates a new MBModulePmmRegionList instance.
///
/// \param mbInfo the MultibootInfo structure containing the source module information.
///
/// \return a new MBModulePmmRegionList instance.
MBModulePmmRegionList MBModulePmmRegionList_create( const struct MultibootInfoStruct* mbInfo );


/// \brief	Gets a reference to the IPmmRegionList implementation of the given
///			MBModulePmmRegionList.
///
/// \param list the MBModulePmmRegionList instance.
///
/// \return the IPmmRegionList interface that \a list implements.
IPmmRegionList MBModulePmmRegionList_getAsPmmRegionList( MBModulePmmRegionList* list );


#endif

