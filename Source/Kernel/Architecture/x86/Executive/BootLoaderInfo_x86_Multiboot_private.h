// ===========================================================================
//
//             Copyright (C) 2004-2006 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/Executive/BootLoaderInfo_x86_Multiboot_private.h
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
/// \brief	Defines the architecture-specific parts of the BootLoaderInfo class
///			that are used by other parts of the Executive. These parts are
///			considered to have "package" visibility.
///
// ===========================================================================

#ifndef _KERNEL_ARCH_X86_EXECUTIVE_BOOTLOADERINFO_X86_MULTIBOOT_PRIVATE_H_
#define _KERNEL_ARCH_X86_EXECUTIVE_BOOTLOADERINFO_X86_MULTIBOOT_PRIVATE_H_


/// \brief	Forward declaration of the MultibootInfo structure.
struct MultibootInfoStruct;

/// \brief	Forward declaration of the BootLoaderInfo structure.
struct BootLoaderInfoStruct;


/// \brief	Initializes the global BootLoaderInfo instance to wrap the given MultibootInfo
///			structure.
///
/// \param mbInfo	a pointer to the MultibootInfo structure. This memory was allocated by the
///					bootloader and will not be "freed" by this method. It is up to the caller to
///					keep track of the memory occupied by the MultibootInfo.
///
/// This method must be called before calling BootLoaderInfo_getInstance().
///
/// \return a pointer to the BootLoaderInfo that encapsulates the MultibootInfo structure. The
///			BootLoaderInfo does not consume any additional "dynamic" memory and therefore the caller
///			need not track the memory pointed to by the return value of this function.
void BootLoaderInfo_init( struct MultibootInfoStruct* mbInfo );


/// \brief	Provides access to the global instance of BootLoaderInfo.
///
/// \return a pointer to the BootLoaderInfo singleton.
struct BootLoaderInfoStruct* BootLoaderInfo_getInstance( void );


#endif

