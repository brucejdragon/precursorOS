// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/HAL/IO.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Jan/11
//
// ===========================================================================
///
/// \file
///
/// \brief	This file defines a utility for accessing the I/O address space on
///			the x86 architecture.
///
// ===========================================================================

#ifndef _KERNEL_ARCHITECTURE_X86_HAL_IO_H_
#define _KERNEL_ARCHITECTURE_X86_HAL_IO_H_


#include <stdint.h>


/// \brief	Reads an 8-bit value from the given I/O port.
///
/// \param port	the 16-bit I/O address of the port from which to read.
///
/// \return an 8-bit value read from \a port.
uint8_t IO_in8( uint16_t port );


/// \brief	Reads a 16-bit value from the given I/O port.
///
/// \param port	the 16-bit I/O address of the port from which to read.
///
/// \return a 16-bit value read from \a port.
uint16_t IO_in16( uint16_t port );


/// \brief	Reads a 32-bit value from the given I/O port.
///
/// \param port	the 16-bit I/O address of the port from which to read.
///
/// \return a 32-bit value read from \a port.
uint32_t IO_in32( uint16_t port );


/// \brief	Writes an 8-bit value to the given I/O port.
///
/// \param port	the 16-bit I/O address of the port to which to write.
/// \param val	an 8-bit value to write to \a port.
void IO_out8( uint16_t port, uint8_t val );


/// \brief	Writes a 16-bit value to the given I/O port.
///
/// \param port	the 16-bit I/O address of the port to which to write.
/// \param val	a 16-bit value to write to \a port.
void IO_out16( uint16_t port, uint16_t val );


/// \brief	Writes a 32-bit value to the given I/O port.
///
/// \param port	the 16-bit I/O address of the port to which to write.
/// \param val	a 32-bit value to write to \a port.
void IO_out32( uint16_t port, uint32_t val );


#endif

