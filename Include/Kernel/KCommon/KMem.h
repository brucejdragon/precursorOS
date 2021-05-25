// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/KCommon/KMem.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Jan/16
//
// ===========================================================================
///
/// \file
///
/// \brief	This file defines utility functions for copying and initializing
///			memory, for extracting 8- and 16- bit values from larger values,
///			and for bit manipulation.
///
// ===========================================================================

#ifndef _KERNEL_KCOMMON_KMEM_H_
#define _KERNEL_KCOMMON_KMEM_H_


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "Kernel/KCommon/KDebug.h"


/// \brief	Copies memory from a source to a destination address.
///
/// \param dest		the address of the destination region.
/// \param source	the address of the source region.
/// \param numBytes	the number of bytes to copy.
///
/// It is the caller's responsibility to ensure that the regions pointed to by \a dest and
/// \a source do not overlap. If they do, the result is undefined.
void KMem_copy( volatile void* dest, const volatile void* source, size_t numBytes );


/// \brief	Moves memory from a source to a destination address.
///
/// \param dest		the address of the destination region.
/// \param source	the address of the source region.
/// \param numBytes	the number of bytes to copy.
///
/// This function behaves similarly to KMem_copy(), except that it guarantees that the
/// destination memory is written correctly in the case of overlap.
void KMem_move( volatile void* dest, const volatile void* source, size_t numBytes );


/// \brief	Fills memory with a given value.
///
/// \param dest		the address of the destination region.
/// \param val		the value with which to fill the region pointed to by \a dest.
/// \param numBytes	the number of bytes to fill.
void KMem_set( volatile void* dest, char val, size_t numBytes );


/// \brief	Extracts the least-significant 8-bits from the given 16-bit value.
///
/// \param val	a 16-bit value.
///
/// The implementation of this function will vary between little-endian and bit-endian
/// architectures.
///
/// \return the 8 least-significant bits of \a val.
uint8_t KMem_low8( uint16_t val );


/// \brief	Extracts the most-significant 8-bits from the given 16-bit value.
///
/// \param val	a 16-bit value.
///
/// The implementation of this function will vary between little-endian and bit-endian
/// architectures.
///
/// \return the 8 most-significant bits of \a val.
uint8_t KMem_high8( uint16_t val );


/// \brief	Extracts the least-significant 16-bits from the given 32-bit value.
///
/// \param val	a 32-bit value.
///
/// The implementation of this function will vary between little-endian and bit-endian
/// architectures.
///
/// \return the 16 least-significant bits of \a val.
uint16_t KMem_low16( uint32_t val );


/// \brief	Extracts the most-significant 16-bits from the given 32-bit value.
///
/// \param val	a 32-bit value.
///
/// The implementation of this function will vary between little-endian and bit-endian
/// architectures.
///
/// \return the 16 most-significant bits of \a val.
uint16_t KMem_high16( uint32_t val );


/// \brief	Finds the least significant set (1) bit in the given value.
///
/// \param val	the value to scan for set bits.
///
/// \retval <0	\a val is zero.
/// \retval >=0	the offset of the least significant one bit in \a val.
int KMem_findLowestSetBit( uintptr_t val );


/// \brief	Sets the given bit in the given 8-bit value.
///
/// \param val	the value in which to set the bit.
/// \param bit	the 0-based number of the bit to set.
///
/// This function works the same way on all architectures, regardless of endian-ness. Bit 0 is
/// always the least-significant bit.
///
/// \return \a val after setting \a bit.
static inline uint8_t KMem_bitSet8( uint8_t val, uint8_t bit )
{
	KDebug_assertArg( bit < 8 );
	return (val | (1 << bit));
}


/// \brief	Sets the given bit in the given 16-bit value.
///
/// \param val	the value in which to set the bit.
/// \param bit	the 0-based number of the bit to set.
///
/// This function works the same way on all architectures, regardless of endian-ness. Bit 0 is
/// always the least-significant bit.
///
/// \return \a val after setting \a bit.
static inline uint16_t KMem_bitSet16( uint16_t val, uint8_t bit )
{
	KDebug_assertArg( bit < 16 );
	return (val | (1 << bit));
}


/// \brief	Sets the given bit in the given 32-bit value.
///
/// \param val	the value in which to set the bit.
/// \param bit	the 0-based number of the bit to set.
///
/// This function works the same way on all architectures, regardless of endian-ness. Bit 0 is
/// always the least-significant bit.
///
/// \return \a val after setting \a bit.
static inline uint32_t KMem_bitSet32( uint32_t val, uint8_t bit )
{
	KDebug_assertArg( bit < 32 );
	return (val | (1 << bit));
}


/// \brief	Sets the given bit in the given pointer-sized value.
///
/// \param val	the value in which to set the bit.
/// \param bit	the 0-based number of the bit to set.
///
/// This function works the same way on all architectures, regardless of endian-ness. Bit 0 is
/// always the least-significant bit.
///
/// \return \a val after setting \a bit.
static inline uintptr_t KMem_bitSet( uintptr_t val, uint8_t bit )
{
	KDebug_assertArg( bit < sizeof( uintptr_t ) * 8 );
	return (val | (1 << bit));
}


/// \brief	Clears the given bit in the given 8-bit value.
///
/// \param val	the value in which to clear the bit.
/// \param bit	the 0-based number of the bit to clear.
///
/// This function works the same way on all architectures, regardless of endian-ness. Bit 0 is
/// always the least-significant bit.
///
/// \return \a val after setting \a bit.
static inline uint8_t KMem_bitClear8( uint8_t val, uint8_t bit )
{
	KDebug_assertArg( bit < 8 );
	return (val & ~(1 << bit));
}


/// \brief	Clears the given bit in the given 16-bit value.
///
/// \param val	the value in which to clear the bit.
/// \param bit	the 0-based number of the bit to clear.
///
/// This function works the same way on all architectures, regardless of endian-ness. Bit 0 is
/// always the least-significant bit.
///
/// \return \a val after setting \a bit.
static inline uint16_t KMem_bitClear16( uint16_t val, uint8_t bit )
{
	KDebug_assertArg( bit < 16 );
	return (val & ~(1 << bit));
}


/// \brief	Clears the given bit in the given 32-bit value.
///
/// \param val	the value in which to clear the bit.
/// \param bit	the 0-based number of the bit to clear.
///
/// This function works the same way on all architectures, regardless of endian-ness. Bit 0 is
/// always the least-significant bit.
///
/// \return \a val after setting \a bit.
static inline uint32_t KMem_bitClear32( uint32_t val, uint8_t bit )
{
	KDebug_assertArg( bit < 32 );
	return (val & ~(1 << bit));
}


/// \brief	Clears the given bit in the given pointer-sized value.
///
/// \param val	the value in which to clear the bit.
/// \param bit	the 0-based number of the bit to clear.
///
/// This function works the same way on all architectures, regardless of endian-ness. Bit 0 is
/// always the least-significant bit.
///
/// \return \a val after setting \a bit.
static inline uintptr_t KMem_bitClear( uintptr_t val, uint8_t bit )
{
	KDebug_assertArg( bit < sizeof( uintptr_t ) * 8 );
	return (val & ~(1 << bit));
}


/// \brief	Indicates whether the given bit in the given 8-bit value is set.
///
/// \param val	the value in which to test the bit.
/// \param bit	the 0-based number of the bit to test.
///
/// This function works the same way on all architectures, regardless of endian-ness. Bit 0 is
/// always the least-significant bit.
///
/// \return \c true if \a bit is set in \a val; \c false otherwise.
static inline bool KMem_isBitSet8( uint8_t val, uint8_t bit )
{
	KDebug_assertArg( bit < 8 );
	return ((val & (1 << bit)) != 0);
}


/// \brief	Indicates whether the given bit in the given 16-bit value is set.
///
/// \param val	the value in which to test the bit.
/// \param bit	the 0-based number of the bit to test.
///
/// This function works the same way on all architectures, regardless of endian-ness. Bit 0 is
/// always the least-significant bit.
///
/// \return \c true if \a bit is set in \a val; \c false otherwise.
static inline bool KMem_isBitSet16( uint16_t val, uint8_t bit )
{
	KDebug_assertArg( bit < 16 );
	return ((val & (1 << bit)) != 0);
}


/// \brief	Indicates whether the given bit in the given 32-bit value is set.
///
/// \param val	the value in which to test the bit.
/// \param bit	the 0-based number of the bit to test.
///
/// This function works the same way on all architectures, regardless of endian-ness. Bit 0 is
/// always the least-significant bit.
///
/// \return \c true if \a bit is set in \a val; \c false otherwise.
static inline bool KMem_isBitSet32( uint32_t val, uint8_t bit )
{
	KDebug_assertArg( bit < 32 );
	return ((val & (1 << bit)) != 0);
}


/// \brief	Indicates whether the given bit in the given pointer-sized value is set.
///
/// \param val	the value in which to test the bit.
/// \param bit	the 0-based number of the bit to test.
///
/// This function works the same way on all architectures, regardless of endian-ness. Bit 0 is
/// always the least-significant bit.
///
/// \return \c true if \a bit is set in \a val; \c false otherwise.
static inline bool KMem_isBitSet( uintptr_t val, uint8_t bit )
{
	KDebug_assertArg( bit < sizeof( uintptr_t ) * 8 );
	return ((val & (1 << bit)) != 0);
}


/// \brief	Indicates whether the given address is aligned on a 16-bit boundary.
///
/// \param addr	the address to test for alignment.
///
/// \return \c true if \a addr is aligned on a 16-bit boundary; \c false otherwise.
static inline bool KMem_isAligned16( uintptr_t addr )
{
	return (addr & ((uintptr_t) 1)) == 0;
}


/// \brief	Indicates whether the given address is aligned on a 32-bit boundary.
///
/// \param addr	the address to test for alignment.
///
/// \return \c true if \a addr is aligned on a 32-bit boundary; \c false otherwise.
static inline bool KMem_isAligned32( uintptr_t addr )
{
	return (addr & ((uintptr_t) 3)) == 0;
}


/// \brief	Returns the next address from the given address that is aligned on a 16-bit boundary.
///
/// \param addr	the address to test for alignment.
///
/// If \a addr is already aligned, this function returns \a addr.
///
/// \return an address greater than or equal to \a addr.
static inline uintptr_t KMem_makeAligned16( uintptr_t addr )
{
	return ((addr + ((uintptr_t) 1)) & ~((uintptr_t) 1));
}


/// \brief	Returns the next address from the given address that is aligned on a 32-bit boundary.
///
/// \param addr	the address to test for alignment.
///
/// If \a addr is already aligned, this function returns \a addr.
///
/// \return an address greater than or equal to \a addr.
static inline uintptr_t KMem_makeAligned32( uintptr_t addr )
{
	return ((addr + ((uintptr_t) 3)) & ~((uintptr_t) 3));
}


#endif

