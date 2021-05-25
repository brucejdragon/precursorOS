// ===========================================================================
//
//             Copyright (C) 2004-2006 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/MM/PmmBitmapAllocator.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2006/Mar/20
//
// ===========================================================================
///
/// \file
///
/// \brief	Defines the PmmBitmapAllocator class, which implements a frame
///			allocator that tracks a fixed number of frames with a bitmap.
///
// ===========================================================================

#ifndef _KERNEL_MM_PMMBITMAPALLOCATOR_H_
#define _KERNEL_MM_PMMBITMAPALLOCATOR_H_


#include <stddef.h>
#include "Kernel/MM/IPmmAllocator.h"


// Public constants

/// \brief	Defines constants for the implementation of PmmBitmapAllocator.
enum PmmBitmapAllocator_consts
{
	BITS_PER_BYTE = 8,									///< Magic number (# bits per byte).
	BITS_PER_BLOCK = BITS_PER_BYTE * sizeof( size_t ),	///< # of bits in element of bitmap array.
};



/// \brief	Defines the fields of PmmBitmapAllocator.
typedef struct PmmBitmapAllocator
{
#ifdef _KERNEL_MM_PMMBITMAPALLOCATOR_C_

	/// \brief	Base address of the array holding the bitmap.
	size_t* m_bitmap;

	/// \brief	The number of elements ("blocks") in the bitmap array.
	size_t m_numBlocks;

	/// \brief	Indicates the position in the bitmap array where a free frame was last found.
	///
	/// This is used to speed up searches for free frames.
	size_t m_lastAllocatedIndex;

	/// \brief	The frame number of the first frame of the region being tracked by this allocator.
	size_t m_baseFrameNumber;

#else

	#ifndef DOXYGEN_SHOULD_SKIP_THIS
	size_t*		m_reserved0;
	size_t		m_reserved1;
	size_t		m_reserved2;
	phys_addr_t	m_reserved3;
	#endif

#endif
} PmmBitmapAllocator;



/// \brief	Creates a new PmmBitmapAllocator instance.
///
/// \param bitmapSpace	base address of the array to hold the bitmap.
/// \param numBlocks	size of the \a bitmapSpace array, in elements.
/// \param baseAddress	base physical address of the region to be managed by the allocator.
///
/// If \a bitmapSpace is NULL or numBlocks is zero, a bugcheck will occur in checked builds.
/// If \a baseAddress is not page-aligned, a bugcheck will occur in checked builds.
///
/// The initial state of the allocator is that all frames are allocated.
///
/// \return a new PmmBitmapAllocator instance.
PmmBitmapAllocator PmmBitmapAllocator_create(
	size_t*		bitmapSpace,
	size_t		numBlocks,
	phys_addr_t	baseAddress
);


/// \brief	Calculates the number of frames that can be tracked by the given number of bitmap
///			blocks.
///
/// \param numBlocks	the number of bitmap blocks (i.e. -- elements of the bitmap array).
///
/// return the number of frames that can be tracked by \a numBlocks blocks.
size_t PmmBitmapAllocator_blocksToFrames( size_t numBlocks );


/// \brief	Calculates the number of bitmap blocks that are needed in order to track the given
///			number of frames.
///
/// \param numFrames	the number of frames to track.
///
/// This method is useful for calculating the size of the bitmap array to pass to
/// PmmBitmapAllocator_create().
///
/// return the number of bitmap blocks required in order to track by \a numFrames frames.
size_t PmmBitmapAllocator_framesToBlocks( size_t numFrames );


/// \brief	Implementation of IPmmAllocator_allocate().
///
/// \param this			the allocator from which to allocate.
/// \param colourHint	the colour hint; Currently ignored by this implementation.
///
/// This method is thread-safe. The underlying implementation is lock-free.
///
/// ***FIXME: Pay attention to the colour hint!
///
/// \retval phys_addr_t	the physical address of the frame just allocated; Guaranteed to be
///						page-aligned.
/// \retval PHYS_NULL	there are no more free frames.
phys_addr_t PmmBitmapAllocator_allocate( volatile PmmBitmapAllocator* this, void* colourHint );


/// \brief	Attempts to allocate a specific frame.
///
/// \param this			the allocator from which to allocate.
/// \param frameAddr	the desired frame.
///
/// This method is thread-safe. The underlying implementation is lock-free.
///
/// In checked builds, a bugcheck will occur if \a frameAddr is PHYS_NULL or not page-aligned or
/// not in the region tracked by this allocator. In free builds, \a frameAddr will be truncated
/// down to the nearest page boundary if it is not page-aligned.
///
/// \note
/// This method is not part of the IPmmAllocator interface. It is a special feature of
/// PmmBitmapAllocator.
///
/// \retval phys_addr_t	\a frameAddr, which was just successfully allocated.
/// \retval PHYS_NULL	\a frameAddr is already allocated.
phys_addr_t PmmBitmapAllocator_allocateFrame(
	volatile PmmBitmapAllocator*	this,
	phys_addr_t						frameAddr
);


/// \brief	Implementation of IPmmAllocator_free().
///
/// \param this			the allocator to which to free.
/// \param frameAddr	the physical address of the frame to free.
///
/// In checked builds, a bugcheck will occur if \a frameAddr is PHYS_NULL or not page-aligned or
/// not in the region tracked by this allocator. In free builds, \a frameAddr will be truncated
/// down to the nearest page boundary if it is not page-aligned.
///
/// This method is thread-safe. The underlying implementation is lock-free.
void PmmBitmapAllocator_free( volatile PmmBitmapAllocator* this, phys_addr_t frameAddr );


/// \brief	Gets a reference to the IPmmAllocator implementation of the given PmmBitmapAllocator.
///
/// \param allocator the PmmBitmapAllocator instance.
///
/// \return the IPmmAllocator interface that \a allocator implements.
IPmmAllocator PmmBitmapAllocator_getAsPmmAllocator( volatile PmmBitmapAllocator* allocator );


#endif
