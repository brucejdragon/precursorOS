// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/Executive/BootLoaderInfoTranslator_x86_Multiboot.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Nov/27
//
// ===========================================================================
///
///	\file
///
/// \brief	Contains the architecture-specific implementation of the
///			BootLoaderInfoTranslator class for the x86 Multiboot specification.
///
/// \note
/// There is no corresponding header file because this class is only invoked
/// from the assembly-language startup stub in the Boot subsystem.
// ===========================================================================


#include <stddef.h>
#include <stdint.h>
#include "Multiboot.h"
#include "BootLoaderInfo_x86_Multiboot_private.h"
#include "Kernel/MM/MM.h"
#include "Kernel/KCommon/KMem.h"
#include "Kernel/KCommon/KString.h"



// Private data.

/// \brief	Space to hold the MultibootInfo structure.
///
/// This is declared as an array of 32-bit integers for alignment purposes.
static uint32_t s_mbInfoSpace[1024 / sizeof( uint32_t )];



// Private functions.

/// \brief	Maps a null-terminated string into the kernel's virtual address space.
///
/// \param strPhysAddr the physical address of the string to map.
///
/// \note
/// For now, the implementation of this method is very simple but limited. If any part of the
/// string strays outside the first 4MB of the physical address space, this method returns NULL.
/// This makes physical-to-virtual address translation as simple as adding the virtual base
/// address of kernel space to every physical address.
///
/// \retval char*	the string was mapped successfully.
/// \retval NULL	a problem occurred during mapping.
static const char* BootLoaderInfoTranslator_mapString( phys_addr_t strPhysAddr )
{
	const phys_addr_t KERNEL_END = (phys_addr_t) MM_MB( 4 );

	if (KERNEL_END <= strPhysAddr)
	{
		return NULL;
	}

	const char* strBegin	= (const char*) (strPhysAddr + MM_KERNEL_VIRTUAL_BASE);
	const char* kEnd		= (const char*) (KERNEL_END + MM_KERNEL_VIRTUAL_BASE);
	const char* str			= strBegin;

	while (*str++ != '\0')
	{
		// Avoid the page-fault (and inevitable triple-fault) by testing at each increment.
		if (kEnd <= str)
		{
			return NULL;
		}
	}

	// Made it through -- the string is OK for translation.
	return strBegin;
}


/// \brief	Maps the Multiboot info structure into the kernel's virtual address space.
///
/// \param mbInfoPhysAddr the physical address of the Multiboot info structure.
///
/// \note
/// For now, the implementation of this method is very simple but limited. If any part of the
/// Multiboot structure strays outside the first 4MB of the physical address space, this method
/// returns NULL. This makes physical-to-virtual address translation as simple as adding the
/// virtual base address of kernel space to every physical address.
///
/// \retval MultibootInfo*	the MultibootInfo was mapped successfully.
/// \retval NULL			a problem occurred during mapping.
static MultibootInfo* BootLoaderInfoTranslator_mapMultibootInfo( phys_addr_t mbInfoPhysAddr )
{
	// This really just ensures that the MultibootInfo is already mapped and adds
	// MM_KERNEL_VIRTUAL_BASE to all the physical addresses.

	const phys_addr_t KERNEL_END = (phys_addr_t) MM_MB( 4 );

	// See if it starts within the kernel's 4MB page.
	if (KERNEL_END <= mbInfoPhysAddr)
	{
		return NULL;
	}

	// Make sure it ends within the kernel's 4MB page.
	if (KERNEL_END < mbInfoPhysAddr + sizeof( MultibootInfo ))
	{
		return NULL;
	}

	// The structure itself is safe to touch.
	MultibootInfo* mbInfo = (MultibootInfo*) (mbInfoPhysAddr + MM_KERNEL_VIRTUAL_BASE);

	if ((mbInfo->CommandLinePresent) && (mbInfo->CommandLine != NULL))
	{
		// Fixup the kernel command line.
		const char* commandLine =
			BootLoaderInfoTranslator_mapString( (phys_addr_t) mbInfo->CommandLine );
		if (commandLine == NULL)
		{
			return NULL;
		}

		mbInfo->CommandLine = commandLine;
	}
	else
	{
		mbInfo->CommandLine = NULL;
	}

	if (mbInfo->MemMapPresent)
	{
		// See if the mmap starts within the kernel's 4MB page.
		if (KERNEL_END <= (phys_addr_t) mbInfo->MMapAddr)
		{
			return NULL;
		}

		// Make sure it ends within the kernel's 4MB page.
		if (KERNEL_END < (((phys_addr_t) mbInfo->MMapAddr) + mbInfo->MMapLength))
		{
			return NULL;
		}

		mbInfo->MMapAddr =
			(MBMemmapNode*) (((phys_addr_t) mbInfo->MMapAddr) + MM_KERNEL_VIRTUAL_BASE);
	}
	else
	{
		mbInfo->MMapAddr = NULL;
	}

	if (mbInfo->ModulesPresent)
	{
		// See if the module list starts within the kernel's 4MB page.
		if (KERNEL_END <= (phys_addr_t) mbInfo->ModsAddr)
		{
			return NULL;
		}

		// Make sure it ends within the kernel's 4MB page.
		if (KERNEL_END < (phys_addr_t) (mbInfo->ModsAddr + mbInfo->ModsCount))
		{
			return NULL;
		}

		mbInfo->ModsAddr =
			(MBModuleInfo*) (((phys_addr_t) mbInfo->ModsAddr) + MM_KERNEL_VIRTUAL_BASE);

		// Now make sure all the module names are ok.
		for (size_t i = 0; i < mbInfo->ModsCount; i++)
		{
			MBModuleInfo* module = mbInfo->ModsAddr + i;

			if (module->ModString != NULL)
			{
				const char* modName =
					BootLoaderInfoTranslator_mapString( (phys_addr_t) module->ModString );

				if (modName == NULL)
				{
					return NULL;
				}
				module->ModString = modName;
			}
		}
	}
	else
	{
		mbInfo->ModsAddr = NULL;
	}

	return mbInfo;
}


/// \brief	Attempts to copy part of the Multiboot structure to the given location in the static
///			destination buffer.
///
/// \param dest		pointer to the place to start copying in the destination buffer.
/// \param source	pointer to the part of the Multiboot structure to copy.
/// \param bytes	the number of bytes to copy.
///
/// This function determines whether there is enough room in the destination buffer before making
/// the copy. In other words, this function knows how large the destination buffer is because it
/// is a static array.
///
/// \retval void*	the first properly-aligned location in the buffer after the structure that was
///					just successfully copied.
/// \retval NULL	there was not enough room in the buffer or \a dest is not 32-bit aligned;
///					nothing was copied.
static void* BootLoaderInfoTranslator_copyStructurePart(
	void*		dest,
	const void*	source,
	size_t		bytes
)
{
	const uint8_t* const mbInfoSpaceEnd = (uint8_t*) s_mbInfoSpace + sizeof( s_mbInfoSpace );
	uint8_t*	mbInfoSpace				= (uint8_t*) dest;
	size_t		mbInfoSpaceSizeInBytes	= mbInfoSpaceEnd - mbInfoSpace;

	if ((bytes > mbInfoSpaceSizeInBytes) || !KMem_isAligned32( (uintptr_t) mbInfoSpace ))
	{
		return NULL;
	}

	// Copy it over.
	KMem_copy( dest, source, bytes );

	// Update our view of the target space to exclude the structure just copied.
	// Nudge forward a bit for alignment purposes, if required.
	mbInfoSpace += bytes;
	mbInfoSpace = (uint8_t*) KMem_makeAligned32( (uintptr_t) mbInfoSpace );
	return (void*) mbInfoSpace;
}


/// \brief	Copies the relevant parts of the MultibootInfo structure into a pre-allocated static
///			buffer in the kernel's data area.
///
/// \param originalMbInfo	the MultibootInfo structure at the original physical memory location
///							where the bootloader created it, but mapped into the virtual address
///							space.
///
/// By copying the MultibootInfo structure into the kernel's data area, it is possible to trivially
/// "free" the memory occupied by the original MultibootInfo structure. This can be done simply by
/// not reporting this memory as being reserved. The new copy of the MultibootInfo structure will
/// be reserved by virtue of being in the kernel's data area.
///
/// \retval MultibootInfo*	the MultibootInfo structure was copied successfully.
/// \retval NULL			there was not enough space to hold the MultibootInfo structure.
static MultibootInfo* BootLoaderInfoTranslator_copyMultibootInfo(
	const MultibootInfo* originalMbInfo
)
{
	// Clear out the target area.
	KMem_set( s_mbInfoSpace, 0, sizeof( s_mbInfoSpace ) );

	void* dest = s_mbInfoSpace;

	// Try to copy the structure itself.
	dest =
		BootLoaderInfoTranslator_copyStructurePart( dest, originalMbInfo, sizeof( MultibootInfo ) );

	if (dest == NULL)
	{
		return NULL;
	}

	// From now on, refer to the structure at its new location.
	MultibootInfo* mbInfo = (MultibootInfo*) s_mbInfoSpace;

	if (mbInfo->MemMapPresent)
	{
		void* destBeforeCopy = dest;

		// Try to copy the memory map.
		dest =
			BootLoaderInfoTranslator_copyStructurePart(
				dest,
				mbInfo->MMapAddr,
				mbInfo->MMapLength
			);

		if (dest == NULL)
		{
			return NULL;
		}

		// Point to the new location.
		mbInfo->MMapAddr = (MBMemmapNode*) destBeforeCopy;
	}
	else
	{
		mbInfo->MMapAddr = NULL;
	}

	if (mbInfo->ModulesPresent)
	{
		size_t moduleListSizeInBytes = mbInfo->ModsCount * sizeof( MBModuleInfo );

		void* destBeforeCopy = dest;

		// Try to copy the module list.
		dest =
			BootLoaderInfoTranslator_copyStructurePart(
				dest,
				mbInfo->ModsAddr,
				moduleListSizeInBytes
			);

		if (dest == NULL)
		{
			return NULL;
		}

		// Point to the new location.
		mbInfo->ModsAddr = (MBModuleInfo*) destBeforeCopy;
	}
	else
	{
		mbInfo->ModsAddr = NULL;
	}

	if (mbInfo->ModulesPresent)
	{
		// Now copy all the module names.
		for (size_t i = 0; i < mbInfo->ModsCount; i++)
		{
			MBModuleInfo* module = mbInfo->ModsAddr + i;

			if (module->ModString != NULL)
			{
				// Get the length of the string (including the null terminator).
				size_t length = KString_length( module->ModString ) + 1;	// + 1 for null char!

				void* destBeforeCopy = dest;

				// Try to copy the module string (including the trailing null).
				dest =
					BootLoaderInfoTranslator_copyStructurePart(
						dest,
						module->ModString,
						length
					);

				if (dest == NULL)
				{
					return NULL;
				}

				// Point to the new location.
				module->ModString = (const char*) destBeforeCopy;
			}
		}
	}

	// Copy the kernel command line, if present.
	if ((mbInfo->CommandLinePresent) && (mbInfo->CommandLine != NULL))
	{
		// Get the length of the string (including the null terminator).
		size_t length = KString_length( mbInfo->CommandLine ) + 1;	// + 1 for null char!

		void* destBeforeCopy = dest;

		// Try to copy the string (including the trailing null).
		dest = BootLoaderInfoTranslator_copyStructurePart( dest, mbInfo->CommandLine, length );

		if (dest == NULL)
		{
			return NULL;
		}

		// Point to the new location.
		mbInfo->CommandLine = (const char*) destBeforeCopy;
	}
	else
	{
		mbInfo->CommandLine = NULL;
	}

	return mbInfo;
}



// Public functions.

/// \brief	Translates the architecture-specific information structure provided by the
///			bootloader into an architecture-neutral object that the rest of the kernel
///			understands.
///
/// \param mbInfoPhysAddr	the physical address of the Multiboot info structure.
/// \param magic			Multi-boot magic number. Must be the value 0x2BADB002.
///
/// \retval BootLoaderInfo*	the new BootLoaderInfo was created successfully.
/// \retval NULL			a problem occurred during translation.
struct BootLoaderInfoStruct* BootLoaderInfoTranslator_translate(
	phys_addr_t	mbInfoPhysAddr,
	uint32_t	magic
)
{
	// Make sure nothing went wrong in passing control from the boot loader to here.
	if (magic != 0x2BADB002)
	{
		return NULL;
	}

	// Map the Multiboot structure into the kernel's virtual address space.
	MultibootInfo* mbInfo = BootLoaderInfoTranslator_mapMultibootInfo( mbInfoPhysAddr );

	if (mbInfo == NULL)
	{
		return NULL;
	}

	// Now copy the Multiboot structure into the kernel's data area. This saves a lot of
	// trouble later on when we have to report what RAM is initially reserved.
	mbInfo = BootLoaderInfoTranslator_copyMultibootInfo( mbInfo );

	if (mbInfo == NULL)
	{
		return NULL;
	}

	BootLoaderInfo_init( mbInfo );
	return BootLoaderInfo_getInstance();
}


