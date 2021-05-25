// ===========================================================================
//
//             Copyright (C) 2004-2006 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/MM/IPmmAllocator.h
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
/// \brief	Defines the IPmmAllocator interface, which encapsulates a component
///			that is responsible for tracking the allocation and freeing of
///			physical memory pages (a.k.a. -- "frames") on behalf of the kernel
///			itself.
///
/// Note that every reference to IPmmAllocator is effectively volatile-qualified.
///
// ===========================================================================

#ifndef _KERNEL_MM_IPMMALLOCATOR_H_
#define _KERNEL_MM_IPMMALLOCATOR_H_


#include "Kernel/MM/MM.h"


/// \brief	Defines the method signature for IPmmAllocator_allocate().
///
/// \param this			the object that implements IPmmAllocator.
/// \param colourHint	a hint to the allocator that it should try to allocate a frame of the same
///						"colour" as the given virtual address.
///
/// This method allocates a single frame on behalf of the caller. In order for the frame to be
/// re-used by another part of the system later, the caller must later free the frame by calling
/// IPmmAllocator_free() (as long as the implementation supports it). Note that this method does
/// not map the frame into any virtual address spaces or fill it with zeroes -- that is the
/// caller's responsibility.
///
/// The \a colourHint parameter can be used to improve CPU cache utilization. The "colour" of a
/// frame is a numeric value derived from the frame's physical address that determines which cache
/// set the frame will occupy. For direct-mapped or n-way set-associative caches, it is best to
/// avoid mapping frames with the same colour to virtual pages that will likely be referenced
/// together frequently. For example, an array spanning multiple pages of virtual address space
/// should not be mapped to frames of the same colour, as this will likely cause cache thrashing.
/// The manner in which the set number is calculated is machine-specific and is the responsibility
/// of the Memory Manager.
///
/// A good policy to avoid cache thrashing is to allocate each frame such that its colour is the
/// same as the colour of the virtual page to which it will be mapped. Obviously, this policy is
/// beyond the scope of an IPmmAllocator implementation to implement, but the implementation can
/// help with this policy by paying attention to the \a colourHint parameter.
///
/// Note that the \a colourHint parameter is just a hint. The implementation is encouraged to find
/// an exact match to the colour, or a near match if there is no exact match, but there is no
/// guarantee that such a match will be found. This method returns PHYS_NULL if and only if there
/// are no more frames available, regardless of colour.
///
/// \retval phys_addr_t	the physical address of the frame just allocated; Guaranteed to be
///						page-aligned.
/// \retval PHYS_NULL	there are no more free frames.
typedef phys_addr_t (*IPmmAllocator_allocateFunc)( volatile void* this, void* colourHint );


/// \brief	Defines the method signature for IPmmAllocator_free().
///
/// \param this			the object that implements IPmmAllocator.
/// \param frameAddr	the physical address of the frame to free.
///
/// The \a frameAddr passed to this method must be a page-aligned address and cannot be zero
/// (PHYS_NULL). It also must be within the bounds of the physical address space region being
/// managed by the allocator. If these checks fail, the implementation is encouraged to cause a
/// bugcheck in checked builds, and (in the case of non-page-aligned addresses) to truncate
/// \a frameAddr to the next lowest page boundary before freeing in free builds.
///
/// The \a frameAddr must have already been allocated. If not, the implementation is encouraged to
/// cause a bugcheck in checked builds. Generally, \a frameAddr must have been allocated by a
/// previous call to IPmmAllocator_allocate() on the same instance. This is not enforced, however.
/// For example, when some implementations of IPmmAllocator are created, their initial state is to
/// consider all frames to have been allocated. This means that the caller can free whatever frames
/// it wishes, at least once per frame. This is a useful means of initializing such allocators.
/// This is not necessarily true in general, however.
typedef void (*IPmmAllocator_freeFunc)( volatile void* this, phys_addr_t frameAddr );



/// \brief	Defines the interface dispatch table for IPmmAllocator.
typedef struct
{
	/// \brief	Pointer to a particular implementation of IPmmAllocator_allocate().
	IPmmAllocator_allocateFunc allocate;

	/// \brief	Pointer to a particular implementation of IPmmAllocator_free().
	IPmmAllocator_freeFunc free;

} IPmmAllocator_itable;



/// \brief	Defines a reference to an implementation of the IPmmAllocator interface.
typedef struct
{
	const IPmmAllocator_itable*	iptr;	///< Pointer to the interface dispatch table.
	volatile void*				obj;	///< Pointer to the object that implements the interface.

} IPmmAllocator;


#endif

