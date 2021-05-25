// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/MM/PmmRegion.h
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

#ifndef _KERNEL_MM_PMMREGION_H_
#define _KERNEL_MM_PMMREGION_H_


#include <stddef.h>
#include <stdbool.h>
#include "Kernel/MM/MM.h"


/// \brief	Represents a contiguous, not necessarily page-aligned region of
///			physical address space.
typedef struct
{
#ifdef _KERNEL_MM_PMMREGION_C_

	phys_addr_t	m_base;	///< Points to the lowest byte in the region.
	phys_addr_t	m_last;	///< Points to the highest byte in the region.

#else

	#ifndef DOXYGEN_SHOULD_SKIP_THIS
	phys_addr_t	m_reserved0;
	phys_addr_t	m_reserved1;
	#endif

#endif
} PmmRegion;


/// \brief	Creates a new PmmRegion.
///
/// \param baseAddr	the base address of the new region.
/// \param length	the length of the new region in bytes.
///
/// \note
/// It is not possible to create a single region that spans the entire physical address space. At
/// most a region can include all but the very last byte, because the largest value of the length
/// parameter is MAX_PHYS_SIZE, which can be no greater than MAX_PHYS_ADDR.
///
/// \pre
/// \a baseAddr + \a length can be at most MAX_PHYS_ADDR. If overflow would occur for this
/// addition, a bugcheck will occur in checked builds. Also, \a length must be greater than zero.
///
/// \return a new PmmRegion starting at \a baseAddr of length \a length.
PmmRegion PmmRegion_create( phys_addr_t baseAddr, phys_size_t length );


/// \brief	Attempts to move the given region up to the next equal-sized "window" of the physical
///			address space.
///
/// \param region	the PmmRegion.
///
/// \retval true	the region now begins at the next byte past its former last byte and has the
///					same length.
/// \retval false	the region is unchanged because there wasn't enough room left in the physical
///					address space to move it.
bool PmmRegion_advance( PmmRegion* region );


/// \brief	Returns the address of the first byte in the region.
///
/// \param region	the PmmRegion.
///
/// \return	the address of the first byte in \a region.
phys_addr_t PmmRegion_base( const PmmRegion* region );


/// \brief	Indicates whether the given region preceeds the given physical address.
///
/// \param region	the PmmRegion.
/// \param addr		the physical address to which to compare \a region.
///
/// \retval true	\a region's last byte is somewhere below \a addr.
/// \retval false	\a region's last byte is at \a addr or somewhere above it.
bool PmmRegion_below( const PmmRegion* region, phys_addr_t addr );


/// \brief	Adjusts this region so that it fits within the given clipping region.
///
/// \param region			the PmmRegion to clip.
/// \param clippingRegion	the PmmRegion defining the clipping region.
///
/// \retval true	\a region was modified to fit within \a clippingRegion, or it already fit
///					entirely within \a clippingRegion.
/// \retval false	\a region and \a clippingRegion do not overlap; \a region was not modified.
bool PmmRegion_clip( PmmRegion* region, PmmRegion clippingRegion );


/// \brief	Returns the address of the last byte in the region.
///
/// \param region	the PmmRegion.
///
/// \return	the address of the last byte in \a region.
phys_addr_t PmmRegion_last( const PmmRegion* region );


/// \brief	Adjusts the base address and length of the region so that it begins on a page boundary
///			and is a multiple of the page size.
///
/// \param region	the PmmRegion.
///
/// This method will either leave the region as it is if it is already page-aligned, or it will
/// make it larger by lowering the base address and/or increasing the length. It will never make
/// the region smaller.
void PmmRegion_makePageAligned( PmmRegion* region );


#endif

