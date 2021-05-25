// ===========================================================================
//
//             Copyright (C) 2004-2006 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/MM/PmmBitmapAllocator.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2006/Mar/20
//
// ===========================================================================
///
///	\file
///
/// \brief	Contains the implementation of IPmmAllocator that uses a bitmap
///			to track frames.
///
// ===========================================================================


#include "Kernel/KCommon/KDebug.h"
#include "Kernel/KCommon/KMem.h"
#include "Kernel/HAL/Atomic.h"

#define _KERNEL_MM_PMMBITMAPALLOCATOR_C_
#include "PmmBitmapAllocator.h"


// Private functions

/// \brief	Calculates the block number (i.e. -- bitmap array index) containing the bit that tracks
///			the indicated frame.
///
/// \param this			the PmmBitmapAllocator.
/// \param frameNumber	the frame number of the frame.
///
/// In checked builds, this method will cause a bugcheck if \a this is NULL or if \a frameNumber
/// is not within the region managed by this allocator.
///
/// This method is thread-safe. The underlying implementation is lock-free.
///
/// \return an index into the bitmap array.
static size_t PmmBitmapAllocator_getBlockNumberForFrameNumber(
	volatile PmmBitmapAllocator*	this,
	size_t							frameNumber
)
{
	// Note that an atomic read is not necessary for m_baseFrameNumber or m_numBlocks since their
	// values never change during the lifetime of this object.
	KDebug_assertArg( this != NULL );
	KDebug_assertArg( frameNumber >= this->m_baseFrameNumber );

	size_t blockNumber = (frameNumber - this->m_baseFrameNumber) / BITS_PER_BLOCK;
	
	KDebug_assert( blockNumber < this->m_numBlocks );
	return blockNumber;
}


/// \brief	Calculates the index of the bit within a block that tracks the indicated frame.
///
/// \param frameNumber	the frame number of the frame.
///
/// \return an index into a block of the bitmap array; Guaranteed to be valid.
static uint8_t PmmBitmapAllocator_getBitInBlockForFrameNumber( size_t frameNumber )
{
	return frameNumber % BITS_PER_BLOCK;
}


/// \brief	Calculates the physical address of the frame that is tracked by the given bit of the
///			given block number.
///
/// \param this			the PmmBitmapAllocator.
/// \param blockNumber	an index into the bitmap array.
/// \param bitInBlock	the position of the bit in the block that tracks the desired frame.
///
/// In checked builds, this method will cause a bugcheck if \a bitInBlock is greater than or equal
/// to the number of bits in a block, if \a this is NULL, or if \a blockNumber is outside the
/// bounds of the bitmap array.
///
/// This method is thread-safe. The underlying implementation is lock-free.
///
/// \return the physical address of the frame tracked by the indicated bit and block; Guaranteed
///			to be page-aligned.
static phys_addr_t PmmBitmapAllocator_getPhysAddrForBlockNumberAndBitInBlock(
	volatile PmmBitmapAllocator*	this,
	size_t							blockNumber,
	uint8_t							bitInBlock
)
{
	// Note that an atomic read is not necessary for m_baseFrameNumber or m_numBlocks since their
	// values never change during the lifetime of this object.
	KDebug_assertArg( this != NULL );
	KDebug_assertArg( blockNumber < this->m_numBlocks );
	KDebug_assertArg( bitInBlock < BITS_PER_BLOCK );

	size_t frameNumber = ((blockNumber * BITS_PER_BLOCK) + bitInBlock) + this->m_baseFrameNumber;
	return MM_getFrameAddress( frameNumber );
}



/// \brief	Interface dispatch table for PmmBitmapAllocator's implementation of IPmmAllocator.
static IPmmAllocator_itable s_itable =
{
	(IPmmAllocator_allocateFunc) PmmBitmapAllocator_allocate,
	(IPmmAllocator_freeFunc) PmmBitmapAllocator_free
};



// Public functions

PmmBitmapAllocator PmmBitmapAllocator_create(
	size_t*		bitmapSpace,
	size_t		numBlocks,
	phys_addr_t	baseAddress
)
{
	KDebug_assertArg( bitmapSpace != NULL );
	KDebug_assertArg( numBlocks > 0 );
	KDebug_assertArg( MM_isFrameAligned( baseAddress ) );
	
	// Zero out the bitmap space so that every frame is initially allocated.
	KMem_set( bitmapSpace, 0, numBlocks * sizeof( size_t ) );

	PmmBitmapAllocator allocator;
	allocator.m_bitmap				= bitmapSpace;
	allocator.m_lastAllocatedIndex	= 0;
	allocator.m_numBlocks			= numBlocks;
	allocator.m_baseFrameNumber		= MM_getFrameNumber( baseAddress );
	return allocator;
}


size_t PmmBitmapAllocator_blocksToFrames( size_t numBlocks )
{
	return numBlocks * BITS_PER_BLOCK;
}


size_t PmmBitmapAllocator_framesToBlocks( size_t numFrames )
{
	return numFrames / BITS_PER_BLOCK;
}


phys_addr_t PmmBitmapAllocator_allocate( volatile PmmBitmapAllocator* this, void* colourHint )
{
	KDebug_assertArg( this != NULL );
	
	// For now, the implementation ignores the colour hint.
	(void) colourHint;

	// Use atomic read/write to read the last allocated index field. Remember, this is a lock-free
	// implementation and there may be other CPUs attempting to read or write this field at the
	// same time.
	size_t lastAllocatedIndex = Atomic_read( &(this->m_lastAllocatedIndex) );
	
	// Note that an atomic read is not necessary for m_numBlocks since its value never
	// changes during the lifetime of this object.
	size_t beforeLast = (lastAllocatedIndex == 0) ? this->m_numBlocks - 1 : lastAllocatedIndex - 1;

	// Rotate around the bitmap, starting with the last block that had a free frame in it.
	for (size_t i = lastAllocatedIndex; i != beforeLast; i = (i + 1) % this->m_numBlocks)
	{
		// There will be other CPUs trying to read this block at the same time, so use an atomic
		// read.
		size_t block = Atomic_read( &(this->m_bitmap[i]) );

		// Test 32 frames at once.
		while (block != 0)
		{
			// There is at least one free frame, so try to grab it before another CPU does.
			int signedBit = KMem_findLowestSetBit( block );
			
			// "block" is a local variable, so it can't change between the time it passed the
			// "while" condition and the time it was scanned for a set bit. This assertion is
			// therefore OK, and is *not* a race condition.
			KDebug_assert( (0 <= signedBit) && (signedBit < BITS_PER_BLOCK) );

			uint8_t bit = (uint8_t) signedBit;
			
			size_t newBlock = KMem_bitClear( block, bit );

			if (Atomic_compareAndSwap( &(this->m_bitmap[i]), block, newBlock ))
			{
				// Remember to use an atomic write to update m_lastAllocatedIndex.
				Atomic_write( &(this->m_lastAllocatedIndex), i );
				return PmmBitmapAllocator_getPhysAddrForBlockNumberAndBitInBlock( this, i, bit );
			}
			else
			{
				// Something changed in the block. Go around for another pass to
				// see if we missed the last free frame.
				block = Atomic_read( &(this->m_bitmap[i]) );
			}
		}

		// If we fell through, the block was either all allocated already, or some
		// other CPU allocated the last frame before we could get it. Try the next block.
	}
	// *Probably* no memory left. Other CPUs may have freed some while we were searching.
	// The same kind of condition exists in a lock-based system, in that other CPUs may be
	// spinning on the lock waiting to free some frames. Either way, the caller should probably
	// wait a while and re-try one or two times for good measure.
	return PHYS_NULL;
}


phys_addr_t PmmBitmapAllocator_allocateFrame(
	volatile PmmBitmapAllocator*	this,
	phys_addr_t						frameAddr
)
{
	KDebug_assertArg( this != NULL );
	KDebug_assertArg( frameAddr != PHYS_NULL );
	KDebug_assertArg( MM_isFrameAligned( frameAddr ) );
	
	size_t frameNumber = MM_getFrameNumber( frameAddr );
	uint8_t bit = PmmBitmapAllocator_getBitInBlockForFrameNumber( frameNumber );

	// This call does bounds checking for us (in checked builds).
	size_t i = PmmBitmapAllocator_getBlockNumberForFrameNumber( this, frameNumber );
	
	size_t block = 0;
	size_t newBlock = 0;

	// Try to flip our bit in the block. Keep trying as long as other CPUs keep flipping other
	// bits in the same block.
	do
	{
		block = Atomic_read( &(this->m_bitmap[i]) );

		// If the frame is already allocated, give up.
		if (!KMem_isBitSet( block, bit ))
		{
			return PHYS_NULL;
		}
		
		newBlock = KMem_bitClear( block, bit );
	} while (!Atomic_compareAndSwap( &(this->m_bitmap[i]), block, newBlock ));
	
	// We fell through -- that means we successfully allocated the frame.
	return frameAddr;
}


void PmmBitmapAllocator_free( volatile PmmBitmapAllocator* this, phys_addr_t frameAddr )
{
	KDebug_assertArg( this != NULL );
	KDebug_assertArg( frameAddr != PHYS_NULL );
	KDebug_assertArg( MM_isFrameAligned( frameAddr ) );
	
	size_t frameNumber = MM_getFrameNumber( frameAddr );
	uint8_t bit = PmmBitmapAllocator_getBitInBlockForFrameNumber( frameNumber );
	
	// This call does bounds checking for us (in checked builds).
	size_t i = PmmBitmapAllocator_getBlockNumberForFrameNumber( this, frameNumber );

	size_t block = 0;
	size_t newBlock = 0;

	// Try to flip our bit in the block. Keep trying as long as other CPUs keep flipping other
	// bits in the same block.
	do
	{
		block = Atomic_read( &(this->m_bitmap[i]) );

		// The caller isn't allowed to free memory that is already free.		
		KDebug_assert( !KMem_isBitSet( block, bit ) );
		
		newBlock = KMem_bitSet( block, bit );
	} while (!Atomic_compareAndSwap( &(this->m_bitmap[i]), block, newBlock ));
}


IPmmAllocator PmmBitmapAllocator_getAsPmmAllocator( volatile PmmBitmapAllocator* allocator )
{
	IPmmAllocator iAllocator;
	iAllocator.obj	= allocator;	// Point to the given object.
	iAllocator.iptr	= &s_itable;	// Point at the right interface dispatch table.
	return iAllocator;
}
