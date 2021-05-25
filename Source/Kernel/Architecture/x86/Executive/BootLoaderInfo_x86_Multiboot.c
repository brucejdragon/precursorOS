// ===========================================================================
//
//             Copyright (C) 2004-2006 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/Executive/BootLoaderInfo_x86_Multiboot.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Nov/16
//
// ===========================================================================
///
///	\file
///
/// \brief	Defines the x86 Multiboot-compliant implementation of the
///			BootLoaderInfo class.
///
// ===========================================================================


#include <stdint.h>
#include "BootLoaderInfo.h"
#include "BootLoaderInfo_x86_Multiboot_private.h"
#include "Multiboot.h"
#include "MBMemmapPmmRegionList.h"
#include "MBMemFieldsPmmRegionList.h"
#include "MBModulePmmRegionList.h"
#include "Kernel/KCommon/KMem.h"
#include "Kernel/KCommon/KDebug.h"



/// \brief	Definition of the BootLoaderInfo object type.
///
/// This implementation of BootLoaderInfo encapsulates the Multiboot info structure.
struct BootLoaderInfoStruct
{
	/// \brief	The actual boot loader structure.
	const MultibootInfo* m_mbInfo;

	/// \brief	The list of RAM regions to use if the Multiboot memory map is present.
	MBMemmapPmmRegionList m_ramList;

	/// \brief	The list (of size two) of the "lower" and "upper" memory regions to use if the
	///			Multiboot memory map is not present.
	MBMemFieldsPmmRegionList m_fallbackRamList;

	/// \brief	The list of reserved physical address regions as reported by the Multiboot memory
	///			map.
	MBMemmapPmmRegionList m_reservedList;

	/// \brief	The list of RAM regions that are already in use.
	MBModulePmmRegionList m_moduleList;
};



/// \brief	The global instance of BootLoaderInfo.
static BootLoaderInfo s_instance;



// Using structs here so Doxygen doesn't complain...
void BootLoaderInfo_init( struct MultibootInfoStruct* mbInfo )
{
	KDebug_assertArg( mbInfo != NULL );

	KMem_set( &s_instance, 0, sizeof( BootLoaderInfo ) );
	s_instance.m_mbInfo				= mbInfo;
	s_instance.m_ramList			= MBMemmapPmmRegionList_create( mbInfo, true );
	s_instance.m_fallbackRamList	= MBMemFieldsPmmRegionList_create( mbInfo );
	s_instance.m_reservedList		= MBMemmapPmmRegionList_create( mbInfo, false );
	s_instance.m_moduleList			= MBModulePmmRegionList_create( mbInfo );
}


// Using structs here so Doxygen doesn't complain...
struct BootLoaderInfoStruct* BootLoaderInfo_getInstance( void )
{
	return &s_instance;
}


const char* BootLoaderInfo_getKernelCommandLine( const BootLoaderInfo* info )
{
	KDebug_assertArg( info != NULL );

	if (info->m_mbInfo->CommandLinePresent)
	{
		const char* cmdline = info->m_mbInfo->CommandLine;
		return (cmdline != NULL) ? cmdline : "";
	}
	else
	{
		return "";
	}
}


IPmmRegionList BootLoaderInfo_getRamMemMap( BootLoaderInfo* info )
{
	KDebug_assertArg( info != NULL );
	if (info->m_mbInfo->MemMapPresent)
	{
		return MBMemmapPmmRegionList_getAsPmmRegionList( &(info->m_ramList) );
	}
	else
	{
		return MBMemFieldsPmmRegionList_getAsPmmRegionList( &(info->m_fallbackRamList) );
	}
}


IPmmRegionList BootLoaderInfo_getReservedMemMap( BootLoaderInfo* info )
{
	KDebug_assertArg( info != NULL );
	return MBMemmapPmmRegionList_getAsPmmRegionList( &(info->m_reservedList) );
}


IPmmRegionList BootLoaderInfo_getModuleMemMap( BootLoaderInfo* info )
{
	KDebug_assertArg( info != NULL );
	return MBModulePmmRegionList_getAsPmmRegionList( &(info->m_moduleList) );
}


size_t BootLoaderInfo_getNumModules( const BootLoaderInfo* info )
{
	KDebug_assertArg( info != NULL );

	if (info->m_mbInfo->ModulesPresent)
	{
		return info->m_mbInfo->ModsCount;
	}
	else
	{
		return 0;
	}
}


BootModuleInfo BootLoaderInfo_getModule( const BootLoaderInfo* info, size_t index )
{
	KDebug_assertArg( info != NULL );
	KDebug_assertArg( info->m_mbInfo->ModulesPresent );
	KDebug_assertArg( index < info->m_mbInfo->ModsCount );

	MBModuleInfo* mbModule = info->m_mbInfo->ModsAddr + index;

	BootModuleInfo module;
	module.Extents = PmmRegion_create( mbModule->ModStart, mbModule->ModEnd - mbModule->ModStart );
	module.ModuleString = (mbModule->ModString != NULL) ? mbModule->ModString : "";
	return module;
}

