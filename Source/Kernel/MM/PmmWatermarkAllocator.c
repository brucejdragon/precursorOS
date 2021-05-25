// ===========================================================================
//
//             Copyright (C) 2004-2006 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/MM/PmmWatermarkAllocator.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2006/Mar/23
//
// ===========================================================================
///
///	\file
///
/// \brief	Contains the implementation of IPmmAllocator that allocates all
///			of physical memory sequentially, but only once.
///
// ===========================================================================


#include <stdbool.h>
#include "Kernel/KCommon/KDebug.h"

#define _KERNEL_MM_PMMWATERMARKALLOCATOR_C_
#include "PmmWatermarkAllocator.h"


// Private functions

/// \brief	Finds any frames in the current region that overlap with the given region and allocates
///			them.
///
/// \param this		the watermark allocator.
/// \param region	the region designating frames to allocate.
///
/// This method is not thread-safe. The caller must acquire the lock on \a this before calling
/// this method.
static void PmmWatermarkAllocator_allocateRegion( PmmWatermarkAllocator* this, PmmRegion region )
{
	KDebug_assertArg( this != NULL );
	
	// First, adjust the given region so that it is page-aligned and its size is a multiple of the
	// page size. This makes subsequent calculations easier.
	PmmRegion_makePageAligned( &region );

	// Attempt to clip the given region to fit within the current region. If the clip operation
	// fails, that means the two regions do not overlap, so there is nothing to allocate.
	if (PmmRegion_clip( &region, this->m_currentRegion ))
	{
		// There is something to allocate. Create an initial frame-sized region and keep advancing
		// it until we are outside of the region to allocate.
		PmmRegion frameRegion = PmmRegion_create( PmmRegion_base( &region ), PAGE_SIZE );
		
		do
		{
			phys_addr_t frameAddr = PmmRegion_base( &frameRegion );
			
			// Special case for the very first frame. Trying to allocate it might cause a bugcheck.
			if (frameAddr != PHYS_NULL)
			{
				// Ignore the return value. We don't really care if it's already been allocated,
				// as long as it's been allocated once.
				PmmBitmapAllocator_allocateFrame( &(this->m_regionBitmapAllocator), frameAddr );
			}
			
			// Go to the next frame. If there's no more room, we're done.
			if (!PmmRegion_advance( &frameRegion ))
			{
				break;
			}
			
		} while (!PmmRegion_below( &region, PmmRegion_base( &frameRegion ) ));
	}
}


/// \brief	Finds any frames in the current region that overlap with the given region and frees
///			them.
///
/// \param this		the watermark allocator.
/// \param region	the region designating frames to free.
///
/// Besides being used to initialize the current "window", this method is also used to detect when
/// there is no more physical memory available.
///
/// This method is not thread-safe. The caller must acquire the lock on \a this before calling
/// this method.
///
/// \retval true	\a region either overlaps the current region (in which case some frames were
///					freed in the current "window"), or it is above it in the physical address space
///					(in which case some frames will be freed in a subsequent "window").
/// \retval false	\a region is below the current region in the physical address space.
static bool PmmWatermarkAllocator_freeRegion( PmmWatermarkAllocator* this, PmmRegion region )
{
	KDebug_assertArg( this != NULL );
	
	// First, adjust the given region so that it is page-aligned and its size is a multiple of the
	// page size. This makes subsequent calculations easier.
	PmmRegion_makePageAligned( &region );

	// Attempt to clip the given region to fit within the current region. If the clip operation
	// fails, that means the two regions do not overlap, so there is nothing to free.
	if (PmmRegion_clip( &region, this->m_currentRegion ))
	{
		// There is something to free. Create an initial frame-sized region and keep advancing
		// it until we are outside of the region to free.
		PmmRegion frameRegion = PmmRegion_create( PmmRegion_base( &region ), PAGE_SIZE );
		
		do
		{
			phys_addr_t frameAddr = PmmRegion_base( &frameRegion );
			
			// Special case for the very first frame. Trying to free it might cause a bugcheck.
			if (frameAddr != PHYS_NULL)
			{
				PmmBitmapAllocator_free( &(this->m_regionBitmapAllocator), frameAddr );
			}
			
			// Go to the next frame. If there's no more room, we're done.
			if (!PmmRegion_advance( &frameRegion ))
			{
				break;
			}
			
		} while (!PmmRegion_below( &region, PmmRegion_base( &frameRegion ) ));

		return true;
	}
	// The given region does not overlap the current region. If it is below the current region,
	// it will never be available for allocation from this allocator.
	else if (PmmRegion_below( &region, PmmRegion_base( &(this->m_currentRegion) ) ))
	{
		return false;		
	}
	else
	{
		return true;
	}
}


/// \brief	Initializes the bitmap allocator for the current region.
///
/// \param this	the watermark allocator.
///
/// This method is not thread-safe. The caller must acquire the lock on \a this before calling
/// this method.
///
/// \retval true	there is more memory available to be allocated, either in the current region or
///					in a subsequent region.
/// \retval false	there is no more memory available to be allocated.
static bool PmmWatermarkAllocator_initCurrentRegion( PmmWatermarkAllocator* this )
{
	KDebug_assertArg( this != NULL );

	// Re-create (or create, if this is the first time this method is being called) the bitmap
	// allocator for the new region. This is safe -- no other threads can be using the bitmap now
	// since it is protected by this allocator's lock.
	this->m_regionBitmapAllocator =
		PmmBitmapAllocator_create(
			this->m_regionBitmapSpace,
			NUM_BLOCKS,
			PmmRegion_base( &(this->m_currentRegion) )
		);
		
	// All the frames in the new window are considered by the bitmap allocator to be
	// allocated. The next step is to free all the RAM regions. While doing this, we will
	// also be testing whether our new window is beyond the last RAM region. If so, it's
	// time to give up -- there is no more memory left to allocate.
	bool moreFramesAvailable = false;
	
	IPmmRegionList ramList = this->m_ramList;
	ramList.iptr->reset( ramList.obj );
	while (ramList.iptr->moveNext( ramList.obj ))
	{
		PmmRegion crnt = ramList.iptr->getCurrent( ramList.obj );
		if (PmmWatermarkAllocator_freeRegion( this, crnt ))
		{
			// If at least one region was freed or will be freed in a higher window, then
			// there is still memory left.
			moreFramesAvailable = true;
		}
	}
	
	// Only continue if there is actually more RAM left to allocate.
	if (moreFramesAvailable)
	{
		// Some of the RAM regions we've just freed might actually contain important things in
		// them already (like the kernel image and stack, for example). The next step is to
		// allocate all these reserved regions in the new window.
		IPmmRegionList reservedList = this->m_reservedList;
		reservedList.iptr->reset( reservedList.obj );
		while (reservedList.iptr->moveNext( reservedList.obj ))
		{
			PmmRegion crnt = reservedList.iptr->getCurrent( reservedList.obj );
			PmmWatermarkAllocator_allocateRegion( this, crnt );
		}
		
		// Done.
		return true;
	}
	else
	{
		// We did not find any regions that are at or above the current window -- there is
		// no more RAM left to allocate.
		return false;
	}
}



/// \brief	Interface dispatch table for PmmWatermarkAllocator's implementation of IPmmAllocator.
static IPmmAllocator_itable s_itable =
{
	(IPmmAllocator_allocateFunc) PmmWatermarkAllocator_allocate,
	(IPmmAllocator_freeFunc) PmmWatermarkAllocator_free
};



// Public functions

PmmWatermarkAllocator PmmWatermarkAllocator_create(
	IPmmRegionList	ramList,
	IPmmRegionList	reservedList,
	size_t*			regionBitmapSpace
)
{
	KDebug_assertArg( regionBitmapSpace != NULL );

	PmmWatermarkAllocator allocator;
	allocator.m_lock				= Lock_create();
	allocator.m_regionBitmapSpace	= regionBitmapSpace;
	allocator.m_ramList				= ramList;
	allocator.m_reservedList		= reservedList;
	allocator.m_currentRegion		= PmmRegion_create( 0, REGION_SIZE_IN_BYTES );

	// Now that the first region is set up, we can initialize the bitmap allocator. Ignore the
	// return value -- if there is no RAM to be allocated, this will be discovered on the first
	// call to allocate().
	PmmWatermarkAllocator_initCurrentRegion( &allocator );
	return allocator;
}


phys_addr_t PmmWatermarkAllocator_allocate(
	volatile PmmWatermarkAllocator*	this,
	void*							colourHint
)
{
	KDebug_assertArg( this != NULL );
	
	// For now, the implementation ignores the colour hint.
	(void) colourHint;

	// Even though the bitmap is lock-free, we still need to lock before using it because we might
	// need to trash it, and we wouldn't want any other threads to be using it at the time...
	Lock_acquire( &(this->m_lock) );
	PmmWatermarkAllocator* lockedThis = (PmmWatermarkAllocator*) this;

	phys_addr_t frameAddr =
		PmmBitmapAllocator_allocate( &(lockedThis->m_regionBitmapAllocator), NULL );
		
	while (frameAddr == PHYS_NULL)
	{
		// The current "window" is empty. Advance to the next one.
		if (!PmmRegion_advance( &(lockedThis->m_currentRegion) ))
		{
			// We failed to advance any further in the physical address space. Since we started at
			// zero and our "window" size is evenly divisible into the size of the physical address
			// space, that must mean that there is absolutely nothing left to give.
			break;	// PHYS_NULL will be returned.
		}		

		// Initialize the bitmap allocator for the current region. Only continue if there is
		// actually more RAM left to allocate.
		if (PmmWatermarkAllocator_initCurrentRegion( lockedThis ))
		{
			// Try again now that we have a new window to allocate from.
			frameAddr =
				PmmBitmapAllocator_allocate( &(lockedThis->m_regionBitmapAllocator), NULL );
		}
		else
		{
			// There is no more RAM left to allocate.
			break;	// PHYS_NULL will be returned.
		}		
	} // end while
	
	Lock_release( &(this->m_lock) );
	return frameAddr;
}


void PmmWatermarkAllocator_free( volatile PmmWatermarkAllocator* this, phys_addr_t frame )
{
	// This implementation does not support freeing. Ignore all the parameters.
	(void) this;
	(void) frame;
	
	KDebug_assert( false );
}


IPmmAllocator PmmWatermarkAllocator_getAsPmmAllocator( volatile PmmWatermarkAllocator* allocator )
{
	IPmmAllocator iAllocator;
	iAllocator.obj	= allocator;	// Point to the given object.
	iAllocator.iptr	= &s_itable;	// Point at the right interface dispatch table.
	return iAllocator;
}
