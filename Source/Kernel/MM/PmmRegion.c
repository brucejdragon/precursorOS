// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/MM/PmmRegion.c
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
/// \brief	Defines a class that represents a contiguous region of physical
///			address space. This region need not be page-aligned.
///
// ===========================================================================


#define _KERNEL_MM_PMMREGION_C_
#include "Kernel/MM/PmmRegion.h"
#define _KERNEL_MM_PMMREGION_C_

#include "Kernel/KCommon/KDebug.h"


// Private functions.

/// \brief	Gets the length of the given region in bytes.
///
/// \param region	the PmmRegion.
///
/// It is always safe to calculate the length because there is no way to create a region that
/// spans the entire physical address space (see PmmRegion_create()).
///
/// \return the length of \a region in bytes.
static phys_size_t PmmRegion_length( const PmmRegion* region )
{
	KDebug_assertArg( region != NULL );
	return region->m_last - region->m_base + 1;
}


/// \brief	Tests whether the given base address and length would make a valid region.
///
/// \param baseAddr	the base address of the proposed region.
/// \param length	the length of the proposed region in bytes.
///
/// \retval true	it would be safe to pass \a baseAddr and \a length to PmmRegion_create().
/// \retval false	\a baseAddr and \a length do not specify a valid region.
static bool PmmRegion_isValidRegion( phys_addr_t baseAddr, phys_size_t length )
{
	// MAINTENANCE NOTE: The second comparison was carefully written to avoid overflow.
	return ((length > 0) && (baseAddr <= MAX_PHYS_ADDR - (length - 1)));
}



// Public functions.

PmmRegion PmmRegion_create( phys_addr_t baseAddr, phys_size_t length )
{
	KDebug_assertArg( PmmRegion_isValidRegion( baseAddr, length ) );

	PmmRegion newRegion;
	newRegion.m_base = baseAddr;
	newRegion.m_last = baseAddr + (length - 1);	// Must do subtraction first to avoid overflow!
	return newRegion;
}


bool PmmRegion_advance( PmmRegion* region )
{
	KDebug_assertArg( region != NULL );
	phys_addr_t last = region->m_last;
	
	if (last == MAX_PHYS_ADDR)
	{
		return false;
	}
	else
	{
		phys_addr_t newBase = last + 1;
		phys_size_t length = PmmRegion_length( region );
		
		if (PmmRegion_isValidRegion( newBase, length ))
		{
			region->m_last += length;
			region->m_base = newBase;
			return true;
		}
		else
		{
			return false;
		}
	}
}


phys_addr_t PmmRegion_base( const PmmRegion* region )
{
	KDebug_assertArg( region != NULL );
	return region->m_base;
}


bool PmmRegion_below( const PmmRegion* region, phys_addr_t addr )
{
	KDebug_assertArg( region != NULL );
	return (region->m_last < addr);
}


bool PmmRegion_clip( PmmRegion* region, PmmRegion clippingRegion )
{
	KDebug_assertArg( region != NULL );

	// If this region is above or below the clipping region, do nothing.
	if (PmmRegion_below( region, clippingRegion.m_base ) ||
		PmmRegion_below( &clippingRegion, region->m_base ))
	{
		return false;
	}
	// This region overlaps the clipping region.
	else
	{
		if (region->m_base < clippingRegion.m_base)
		{
			region->m_base = clippingRegion.m_base;
		}
		
		if (clippingRegion.m_last < region->m_last)
		{
			region->m_last = clippingRegion.m_last;
		}
		return true;
	}
}


phys_addr_t PmmRegion_last( const PmmRegion* region )
{
	KDebug_assertArg( region != NULL );
	return region->m_last;
}


void PmmRegion_makePageAligned( PmmRegion* region )
{
	KDebug_assertArg( region != NULL );

	region->m_base = MM_alignToFrame( region->m_base );

	// MAINTENANCE NOTE: This calculation was carefully written to avoid overflow.
	phys_addr_t lastFrameBaseAddr = MAX_PHYS_ADDR - (PAGE_SIZE - 1);

	// Check for overflow.	
	if (lastFrameBaseAddr <= region->m_last)
	{
		region->m_last = MAX_PHYS_ADDR;
	}
	else
	{
		region->m_last = MM_alignToFrame( region->m_last + PAGE_SIZE ) - 1;
	}
}

