// ===========================================================================
//
//             Copyright (C) 2004-2006 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Executive/BootLoaderInfo.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Nov/13
//
// ===========================================================================
///
///	\file
///
/// \brief	Defines the BootLoaderInfo class, which encapsulates the
///			architecture-specific structures that the boot loader passes to
///			Precursor in an architecture-neutral API.
///
// ===========================================================================

#ifndef _KERNEL_EXECUTIVE_BOOTLOADERINFO_H_
#define _KERNEL_EXECUTIVE_BOOTLOADERINFO_H_


#include <stddef.h>
#include "Kernel/MM/PmmRegion.h"
#include "Kernel/MM/IPmmRegionList.h"


/// \brief	Describes a module loaded by the boot loader.
typedef struct
{
	PmmRegion	Extents;		///< The physical memory range occupied by the module.
	const char*	ModuleString;	///< Module "command line" string -- might be empty.
} BootModuleInfo;


/// \brief	Forward declaration of the BootLoaderInfo object type.
///
/// BootLoaderInfo is a class that encapsulates architecture-specific information from the
/// bootloader in an architecture-neutral API.
typedef struct BootLoaderInfoStruct BootLoaderInfo;



/// \brief	Gets the command line passed to the kernel by the bootloader.
///
/// \param info	the boot loader info containing the kernel command line.
///
/// \return a null-terminated string, which may be empty (i.e. -- a single null character).
const char* BootLoaderInfo_getKernelCommandLine( const BootLoaderInfo* info );


/// \brief	Gets an iterator over the list of RAM regions detected by the bootloader.
///
/// \param info	the boot loader info containing the memory map.
///
/// The memory regions in the list returned by this function reflect usable RAM detected by the
/// bootloader, and nothing else (i.e. -- they do not include memory-mapped I/O regions). Also,
/// the memory regions do not exclude the areas occupied by the kernel image, modules, or the
/// boot loader structures themselves. Instead, these "reserved" regions can be enumerated by
/// calling BootLoaderInfo_getModuleMemMap() and BootLoaderInfo_getReservedMemMap(). Note that if
/// a region appears in this list, that does not imply that it is available for use.
///
/// \note
/// There are no guarantees about the order of the regions in the returned list.
///
/// \return an IPmmRegionList that can be used to enumerate RAM regions.
IPmmRegionList BootLoaderInfo_getRamMemMap( BootLoaderInfo* info );


/// \brief	Gets an iterator over the list of reserved physical address regions detected by the
///			bootloader.
///
/// \param info	the boot loader info containing the memory map.
///
/// The list returned by this function contains regions reported by the bootloader that are
/// reserved by the system hardware (e.g. -- memory-mapped I/O regions for the motherboard
/// chipset). Note that if a region does not appear in this list, that does not imply that it
/// is available RAM.
///
/// \note
/// There are no guarantees about the order of the regions in the returned list.
///
/// \return an IPmmRegionList that can be used to enumerate reserved physical address regions.
IPmmRegionList BootLoaderInfo_getReservedMemMap( BootLoaderInfo* info );


/// \brief	Gets an iterator over the list of used RAM regions detected by the bootloader.
///
/// \param info	the boot loader info containing the memory map.
///
/// The memory regions in the list returned by this function reflect RAM detected by the bootloader
/// that is already being used for some important purpose. This includes the areas occupied by the
/// kernel image, modules, and boot loader structures themselves.
///
/// \note
/// There are no guarantees about the order of the regions in the returned list.
///
/// \return an IPmmRegionList that can be used to enumerate RAM regions.
IPmmRegionList BootLoaderInfo_getModuleMemMap( BootLoaderInfo* info );


/// \brief	Gets the number of modules loaded by the bootloader, not including the kernel image.
///
/// \param info	the boot loader info containing the module list.
///
/// \return the number of modules loaded by the bootloader, not including the kernel image.
size_t BootLoaderInfo_getNumModules( const BootLoaderInfo* info );


/// \brief	Returns one module from the list of modules loaded by the bootloader (not including
///			the kernel image).
///
/// \param info		the boot loader info containing the module list.
/// \param index	the position of the module to retrieve in the list.
///
/// \pre
/// 0 <= \a index < BootLoaderInfo_getNumModules( info )
///
/// The modules returned by this function are exactly as specified by the boot loader. There is
/// no guarantee about their ordering, or about whether or not they overlap with each other. They
/// shouldn't overlap, but that's up to the bootloader to enforce, not this class.
///
/// It is illegal to call this method if BootLoaderInfo_getNumModules( info ) returns zero.
///
/// \return a BootModuleInfo structure from the module list.
BootModuleInfo BootLoaderInfo_getModule( const BootLoaderInfo* info, size_t index );


#endif

