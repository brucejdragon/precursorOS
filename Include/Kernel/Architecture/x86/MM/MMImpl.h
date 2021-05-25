// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/Architecture/x86/MM/MMImpl.h
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
/// \brief	Defines primitive types, constants, and utilities used by the
///			Kernel's Memory Manager. The definitions are for the 32-bit x86
///			architecture.
///
// ===========================================================================

#ifndef _KERNEL_ARCH_X86_MM_MMIMPL_H_
#define _KERNEL_ARCH_X86_MM_MMIMPL_H_

#include <stdint.h>
#include <stdbool.h>
#include "Kernel/HAL/Mem.h"

/// \brief	Defines the physical address type used by the Memory Manager.
///
/// \note
/// Precursor does not support Intel's Physical Address Extensions (PAE) for the x86 architecture.
/// Therefore physical addresses are limited to 32 bits.
typedef uint32_t phys_addr_t;


/// \brief	Defines the type used to express physical address offsets and physical address region
///			sizes.
///
/// \note
/// Precursor does not support Intel's Physical Address Extensions (PAE) for the x86 architecture.
/// Therefore physical addresses, offsets, and region sizes are limited to 32 bits.
typedef uint32_t phys_size_t;


/// \brief	Defines some constants describing the physical address range.
enum MM_consts
{
	MIN_PHYS_ADDR = 0x00000000,					///< Minimum physical address.
	MAX_PHYS_ADDR = 0xFFFFFFFF,					///< Maximum physical address.
	MAX_PHYS_SIZE = MAX_PHYS_ADDR,				///< Maximum physical address offset/region size.
	PAGE_SIZE = 4 * 1024,						///< Page size in bytes.
	PAGE_BITS = 12,								///< Number of bits for an offset within a page.
	PTE_BITS = 10,								///< Number of bits for the offset of a PTE in a page table.
	FRAME_OFFSET_MASK = 0x00000FFF,				///< Masks out frame number from byte offset.
	PAGE_OFFSET_MASK = FRAME_OFFSET_MASK,		///< Masks out page number from byte offset.
	PTE_INDEX_MASK = 0x003FF000,				///< Masks out the PTE index from a virtual address.
	PDE_INDEX_MASK = 0xFFC00000,				///< Masks out the PDE index from a virtual address.
	PHYS_NULL = 0x00000000,						///< NULL constant for physical addresses.
	MM_KERNEL_LOAD_PHYS_ADDR = 0x00100000,		///< Physical load address of kernel image.
	MM_KERNEL_VIRTUAL_BASE = KERNEL_VIRTUAL_BASE,					///< Virtual base of kernel space; K=3.5GB
	MM_CURRENT_PAGE_TABLES_BASE = KERNEL_VIRTUAL_BASE + 0x01000000,	///< Virtual base of current page tables; K+16MB

	/// \brief	Virtual base address of the current page directory.
	///
	/// The calculation works like this: Let X = MM_CURRENT_PAGE_TABLES_BASE. Because of the way that address
	/// translation works, X must be aligned on a 4MB boundary, so if we let Xi = the 10 most significant bits of X,
	/// then X == (Xi << (PTE_BITS + PAGE_BITS)). Again, because of the way address translation works, X has the value
	/// that it does because Xi is the index of the PDE that points to the base physical address of the page directory.
	/// Xi also plays the role of the index of the PDE-pretending-to-be-PTE that actually maps the page directory as an
	/// individual page in the virtual address space.
	///
	/// If Di is the index of a PDE, and Ti is the index of a PTE, and O is the offset of a byte within a page, then a
	/// virtual address V is calculated like this:
	///
	/// V = (Di << (PTE_BITS + PAGE_BITS)) + (Ti << PAGE_BITS) + O
	///
	/// So, with O = 0, Di = Xi, and Ti = Xi, we get:
	///
	/// V = (Xi << (PTE_BITS + PAGE_BITS)) + (Xi << PAGE_BITS)
	/// V = X + (Xi << PAGE_BITS)
	/// V = X + ((X >> (PTE_BITS + PAGE_BITS)) << PAGE_BITS)
	/// V = X + (X >> PTE_BITS)
	MM_CURRENT_PAGE_DIRECTORY_BASE = MM_CURRENT_PAGE_TABLES_BASE + (MM_CURRENT_PAGE_TABLES_BASE >> PTE_BITS)
};



/// \brief	Returns the physical address of the first byte after the kernel image (including its
///			BSS section).
///
/// \return the physical address of the first byte after the kernel.
static inline phys_addr_t MM_getKernelEndPhysAddr()
{
	extern uint8_t BssEndPhys;

	// MAINTENANCE NOTE: This looks really strange, but it's the way the linker works. BssEndPhys
	// is just a symbol -- it doesn't represent actual storage. The value of the symbol (i.e. --
	// the address of the variable) is what we're after. Note that it is already a physical address,
	// but it comes in the size of a virtual address. This implementation does not use PAE, so this
	// is ok.
	return (phys_addr_t) &BssEndPhys;	// DON'T FORGET THE ADDRESS-OF OPERATOR!!
}


#endif
