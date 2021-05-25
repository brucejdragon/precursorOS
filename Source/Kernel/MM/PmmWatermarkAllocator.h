// ===========================================================================
//
//             Copyright (C) 2004-2006 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/MM/PmmWatermarkAllocator.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2006/Mar/23
//
// ===========================================================================
///
/// \file
///
/// \brief	Defines the PmmWatermarkAllocator class, which implements a frame
///			allocator that allocates physical memory sequentially but never
///			frees it.
///
/// The watermark allocator is very useful for the PMM's initialization mode.
/// It keeps a fixed-size bitmap in static memory, which is accounted for as
/// part of the kernel image by the boot loader. This means it is usable simply
/// by providing it with a list of regions describing valid RAM, and another
/// list of regions describing any RAM images, structures, or reserved regions
/// that should be considered to already be allocated.
///
/// The watermark allocator works by using a "sliding window" of free frames
/// that is managed by a bitmap allocator. A bitmap allocator is used so that
/// as many frames as possible can be fit into the current "window" with a
/// limited memory footprint. The area of physical memory tracked by the current
/// window is recorded in a PmmRegion structure. When moving to a new window,
/// the allocator runs through its "ram" and "reserved" lists, in that order,
/// freeing and then allocating respectively any regions that overlap with the
/// new window. Once all frames in the current window are allocated, it moves
/// on to the next window. Freeing frames is not supported, but this is fine
/// for an initial allocator. The lack of support for free() is where the
/// watermark allocator gets its name.
///
// ===========================================================================

#ifndef _KERNEL_MM_PMMWATERMARKALLOCATOR_H_
#define _KERNEL_MM_PMMWATERMARKALLOCATOR_H_


#include <stddef.h>
#include "Kernel/MM/IPmmAllocator.h"
#include "Kernel/MM/IPmmRegionList.h"
#include "Kernel/MM/PmmRegion.h"
#include "Kernel/MM/MM.h"
#include "Kernel/HAL/Lock.h"
#include "PmmBitmapAllocator.h"


// Public constants

/// \brief	Defines constants for the implementation of PmmWatermarkAllocator.
enum PmmWatermarkAllocator_consts
{
	// MAINTENANCE NOTE: NUM_BLOCKS must be a power of two so that the physical address space can
	// be evenly divided into equal-sized "windows"!
	
	NUM_BLOCKS = 128,	///< Number of blocks per region (e.g. -- 16 MB on 32-bit architectures).
	REGION_SIZE_IN_BYTES = NUM_BLOCKS * BITS_PER_BLOCK * PAGE_SIZE	///< Size of "window" in bytes.
};



/// \brief	Defines the fields of PmmWatermarkAllocator.
typedef struct PmmWatermarkAllocator
{
#ifdef _KERNEL_MM_PMMWATERMARKALLOCATOR_C_

	/// \brief	Underlying storage for the region bitmap allocator.
	size_t* m_regionBitmapSpace;
		
	/// \brief	Allocator that tracks the current region of physical address space.
	PmmBitmapAllocator m_regionBitmapAllocator;

	/// \brief	Specifies the bounds of the current region of physical address space.	
	PmmRegion m_currentRegion;

	/// \brief	A list of all physical address regions that correspond to usable RAM.
	IPmmRegionList m_ramList;

	/// \brief	A list of physical address regions that are already in use, either by in-RAM
	///			structures or by memory-mapped ROM or devices.
	IPmmRegionList m_reservedList;

	/// \brief	A lock to synchronize access to the allocator.
	Lock m_lock;

#else

	#ifndef DOXYGEN_SHOULD_SKIP_THIS
	size_t*				m_reserved0;
	PmmBitmapAllocator	m_reserved1;
	PmmRegion			m_reserved2;
	IPmmRegionList		m_reserved3;
	IPmmRegionList		m_reserved4;
	Lock				m_reserved5;
	#endif

#endif
} PmmWatermarkAllocator;



/// \brief	Creates a new PmmWatermarkAllocator instance.
///
/// \param ramList				list of usable RAM regions.
/// \param reservedList			list of reserved physical address regions.
/// \param regionBitmapSpace	pointer to the array that will hold the bitmap.
///
/// The caller is responsible for allocating the \a regionBitmapSpace array. It must have exactly
/// NUM_BLOCKS elements.
///
/// \return a new PmmWatermarkAllocator instance.
PmmWatermarkAllocator PmmWatermarkAllocator_create(
	IPmmRegionList	ramList,
	IPmmRegionList	reservedList,
	size_t*			regionBitmapSpace
);


/// \brief	Implementation of IPmmAllocator_allocate().
///
/// \param this			the allocator from which to allocate.
/// \param colourHint	the colour hint; Currently ignored by this implementation.
///
/// This method is thread-safe.
///
/// ***FIXME: Pay attention to the colour hint!
///
/// \retval phys_addr_t	the physical address of the frame just allocated; Guaranteed to be
///						page-aligned.
/// \retval PHYS_NULL	there are no more free frames.
phys_addr_t PmmWatermarkAllocator_allocate(
	volatile PmmWatermarkAllocator*	this,
	void*							colourHint
);


/// \brief	Implementation of IPmmAllocator_free().
///
/// \param this		the allocator to which to free.
/// \param frame	the physical address of the frame to free.
///
/// The watermark allocator does not support the freeing of frames. Therefore, in checked builds
/// this method will cause a bugcheck. In free builds, this method does nothing.
///
/// This method is thread-safe.
void PmmWatermarkAllocator_free( volatile PmmWatermarkAllocator* this, phys_addr_t frame );


/// \brief	Gets a reference to the IPmmAllocator implementation of the given
///			PmmWatermarkAllocator.
///
/// \param allocator the PmmWatermarkAllocator instance.
///
/// \return the IPmmAllocator interface that \a allocator implements.
IPmmAllocator PmmWatermarkAllocator_getAsPmmAllocator( volatile PmmWatermarkAllocator* allocator );


#endif
