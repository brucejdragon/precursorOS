// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/HAL/ShutdownHardware.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Feb/04
//
// ===========================================================================
///
/// \file
///
/// \brief	This file defines utility functions for halting and rebooting the
///			system.
///
// ===========================================================================

#ifndef _KERNEL_HAL_SHUTDOWNHARDWARE_H_
#define _KERNEL_HAL_SHUTDOWNHARDWARE_H_


/// \brief	Reboots the system.
///
/// This function guarantees that it will not return to the caller, one way or another. The
/// mechanism it uses to restart the system is machine-specific, but generally speaking it
/// will attempt the "softest" reset possible first. If this fails, it will progress to "hard"
/// reset, and if no method of resetting works (which is extremely unlikely), it will simply halt
/// the system.
void ShutdownHardware_reboot();


/// \brief	Halts the current processor.
///
/// This function never returns.
void ShutdownHardware_halt();


/// \brief	On MP systems, triggers an IPI to tell all processors other than the current one to
///			halt. On UP systems, does nothing.
///
/// On MP systems, this function will not return until all other processors in the system have
/// halted.
void ShutdownHardware_haltAllOtherProcessors();


#endif

