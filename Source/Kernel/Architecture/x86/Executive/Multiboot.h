// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/Executive/Multiboot.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Nov/13
//
// ===========================================================================
///
/// \file
///
/// \brief	This file defines the structures passed to Precursor by a
///			Multiboot-compliant bootloader such as GRUB. These structures are
///			specific to the x86 architecture.
///
// ===========================================================================

#ifndef _KERNEL_ARCHITECTURE_X86_EXECUTIVE_MULTIBOOT_H_
#define _KERNEL_ARCHITECTURE_X86_EXECUTIVE_MULTIBOOT_H_


#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "Kernel/MM/MM.h"


#ifdef __GNUC__ //////////////////////////////////////////////////////////////

	#define PACKED __attribute__((packed))

#else ////////////////////////////////////////////////////////////////////////

	#error Currently only GCC is supported for building the kernel!

#endif ///////////////////////////////////////////////////////////////////////


/// \brief	Defines the Multiboot structure describing a kernel module.
typedef struct MBModuleInfoStruct
{
	phys_addr_t	ModStart;	///< Physical address of first byte of the boot module.
	phys_addr_t	ModEnd;		///< Physical address of the first byte after the boot module.
	const char*	ModString;	///< Module string -- can be NULL.
	uint32_t	Reserved;	///< Reserved -- must be 0.
} PACKED MBModuleInfo;



/// \brief	Defines the Multiboot structure describing a region of the physical address space.
typedef struct MBMemmapNodeStruct
{
	size_t		Size;			///< Size of the rest of the struct (excluding this field) in bytes.
	phys_addr_t	BaseAddrLow;	///< Base address of region.
	uint32_t	BaseAddrHigh;	///< Precursor only supports 32-bit physical addresses. Must be 0.
	size_t		LengthLow;		///< Length of region in bytes.
	uint32_t	LengthHigh;		///< Precursor only supports 32-bit physical addresses. Must be 0.
	uint32_t	Type;			///< 1 = RAM; Everything else = reserved area (e.g. -- mapped I/O).
} PACKED MBMemmapNode;


/// \brief	Calculates the length of the region, adjusted for Precursor's 32-bit physical address
///			space limitation.
///
/// \param node	the memory region descriptor.
///
/// If this method is called on a region with a base address outside the 32-bit physical address
/// space, it is considered to be a logic error and a bugcheck will occur in checked builds.
///
/// \return the length of the region, possibly truncated to fit within a 32-bit physical address
///			space.
size_t MBMemmapNode_getAdjustedLength( const MBMemmapNode* node );


/// \brief	Returns a pointer to the next MBMemmapNode after this node.
///
/// \param node the memory region descriptor.
///
/// \note
/// The returned pointer will not be valid if \a node is the last node in the memory map. You can
/// test this by calling MultibootInfo_isValidMemmapNode() on the Multiboot info structure.
///
/// \return a pointer to the next MBMemmapNode.
const MBMemmapNode* MBMemmapNode_getNext( const MBMemmapNode* node );



/// \brief	Defines the root Multiboot information structure, which in turn points
///			to other structures as required.
///
/// Some of the fields of this structure are left undefined because they are ignored by Precursor.
typedef struct MultibootInfoStruct
{
	uint32_t
		MemFieldsPresent : 1,				///< 1 = MemLower and MemUpper have valid values.
		Ignored1 : 1,						///< Ignored by Precursor.
		CommandLinePresent : 1,				///< 1 = CommandLine has a valid value.
		ModulesPresent : 1,					///< 1 = ModsCount and ModsAddr have valid values.
		Ignored4to5 : 2,					///< Ignored by Precursor.
		MemMapPresent : 1,					///< 1 = MMapLength and MMapAddr have valid values.
		Ignored7to11 : 5,					///< Ignored by Precursor.
		Reserved12to31 : 20;				///< Reserved -- all bits must be 0.
	size_t			MemLower;				///< Amount of lower memory (from 0) in KB. Max 640KB.
	size_t			MemUpper;				///< Amount of upper memory (from 1MB) in KB.
	uint32_t		IgnoredBootDevice;		///< Ignored by Precursor.
	const char*		CommandLine;			///< 0-terminated kernel command line string, or NULL.
	size_t			ModsCount;				///< Number of modules loaded.
	MBModuleInfo*	ModsAddr;				///< Address of the first module structure.
	uint32_t		IgnoredHeaderInfo[4];	///< Ignored by Precursor.
	size_t			MMapLength;				///< Size of the memory map in bytes.
	MBMemmapNode*	MMapAddr;				///< Address of the first entry in the memory map.
	uint32_t		IgnoredDrives[2];		///< Ignored by Precursor.
	uint32_t		IgnoredConfigTable;		///< Ignored by Precursor.
	uint32_t		IgnoredBootLoaderName;	///< Ignored by Precursor.
	uint32_t		IgnoredApmTable;		///< Ignored by Precursor.
	uint32_t		IgnoredVbwInfo[6];		///< Ignored by Precursor.
} PACKED MultibootInfo;



/// \brief	Indicates whether the given MBMemmapNode pointer is valid.
///
/// \param mbInfo	the MultibootInfo structure.
/// \param node		the MBMemmapNode pointer to check.
///
/// \retval true	\a node points to a structure within \a mbInfo's memory map.
/// \retval false	\a node points outside the memory map, or there is no memory map.
bool MultibootInfo_isValidMemmapNode( const MultibootInfo* mbInfo, const MBMemmapNode* node );


#endif

