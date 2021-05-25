// ===========================================================================
//
//             Copyright (C) 2004-2006 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/MM/PhysicalMemoryManager.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2006/Mar/26
//
// ===========================================================================
///
///	\file
///
/// \brief	Encapsulates the Physical Memory Manager subsystem of the Precursor
///			microkernel.
///
/// ***FIXME: More to say...
// ===========================================================================

#ifndef _KERNEL_MM_PHYSICALMEMORYMANAGER_H_
#define _KERNEL_MM_PHYSICALMEMORYMANAGER_H_


#include <stddef.h>
#include "Kernel/MM/IPmmRegionList.h"
#include "Kernel/MM/IPmmAllocator.h"


/// \brief	Forward declaration of the PhysicalMemoryManager object type.
typedef struct PhysicalMemoryManagerStruct PhysicalMemoryManager;


/// \brief	Partially initializes the PhysicalMemoryManager so that it can handle initial
///			"bootstrap" physical memory allocations.
///
/// \param ramList		a list of all known RAM regions in the physical address space.
/// \param reservedList	a list of reserved regions in the physical address space (memory-mapped
///						I/O regions, etc.).
/// \param moduleList	a list of all physical memory regions occupied by the kernel and its
///						modules.
///
/// The PhysicalMemoryManager starts in "initialization mode" when this method is called. While
/// in this mode, it creates a watermark allocator (using the given memory maps) to satisfy just
/// enough allocation requests for its own needs. An external agent (called the "initializer") is
/// responsible for making these allocation requests on behalf of the PhysicalMemoryManager. One of
/// these allocation requests is for the PageFrameDatabase, the size of which is the return value
/// of this method. The other requests, if any, may be incidentally incurred by the virtual
/// memory manager in order to map in the memory allocated by the initializer on behalf of the
/// PhysicalMemoryManager.
///
/// When the required memory has been allocated and mapped into the kernel's virtual address space,
/// the initializer will switch the PhysicalMemoryManager to "initialized" mode by calling
/// initStageTwo(), at which time the PMM will replace the initial allocator with the
/// fully-initialized PageFrameDatabase for the purposes of kernel memory allocation.
///
/// \note
/// It is an error to call any other method of this class before calling this method. This method
/// must be called exactly once on the startup processor with interrupts disabled and paging
/// enabled.
///
/// \return the amount of memory required in bytes in order to fully initialize the
///			PhysicalMemoryManager.
size_t PhysicalMemoryManager_initStageOne(
	IPmmRegionList ramList,
	IPmmRegionList reservedList,
	IPmmRegionList moduleList
);


/// \brief	Fully initializes the Physical Memory Manager.
///
/// \param workingSpace	points to a buffer large enough to hold all the state of the
///						PhysicalMemoryManager.
/// \param sizeInBytes	the size of the \a workingSpace buffer in bytes.
///
/// The caller is responsible for allocating \a workingSpace using the PhysicalMemoryManager's
/// initial allocator, and for mapping \a workingSpace into the kernel's virtual address space.
/// The size of working space (i.e. -- \a sizeInBytes) must be greater than or equal to the return
/// value of initStageOne(). The PhysicalMemoryManager will take "ownership" of \a workingSpace,
/// but will never free it.
///
/// This method will switch the PhysicalMemoryManager out of "initialization mode" by replacing its
/// initial watermark allocator with the fully-initialized PageFrameDatabase. At this point, the
/// PhysicalMemoryManager is fully operational and can handle requests from the kernel or from
/// user processes.
///
/// \note
/// It is an error to call this method before calling initStageOne(). It is also an error to call
/// getPageFrameDatabase() before calling this method. This method must be called exactly once on
/// the startup processor with interrupts disabled and paging enabled.
void PhysicalMemoryManager_initStageTwo(
	void*	workingSpace,	// in, own
	size_t	sizeInBytes		// in
);


/// \brief	Returns a pointer to the one-and-only PhysicalMemoryManager object.
///
/// This method can be called only after initStageOne() has been called.
///
/// \return a PhysicalMemoryManager* to be used when calling other PhysicalMemoryManager functions.
volatile PhysicalMemoryManager* PhysicalMemoryManager_getInstance();


/// \brief	Provides access to the PageFrameDatabase, which manages all requests for physical
///			memory when the PMM is fully initialized.
///
/// \param pmm	the PhysicalMemoryManager.
///
/// This method is thread-safe.
///
/// This method can only be called after initStageTwo() has been called.
///
/// \return a pointer to the PageFrameDatabase. ***FIXME: declare PageFrameDatabase.
volatile char* PhysicalMemoryManager_getPageFrameDatabase(
	volatile PhysicalMemoryManager* pmm
);


/// \brief	Provides access to the PhysicalMemoryManager's kernel allocator.
///
/// \param pmm	the PhysicalMemoryManager.
///
/// This method is thread-safe.
///
/// This method can only be called after initStageOne() has been called.
///
/// return	an IPmmAllocator that can be used by the kernel to manage physical memory for its own
///			use.
IPmmAllocator PhysicalMemoryManager_getAllocator( const volatile PhysicalMemoryManager* pmm );


#endif
