// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/HAL/ShutdownHardware_x86_uni.c
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
/// \brief	This file implements the ShutdownHardware class for the x86 UP
///			architecture.
///
// ===========================================================================


#include "Kernel/HAL/Processor.h"
#include "Kernel/HAL/ShutdownHardware.h"
#include "IO.h"


void ShutdownHardware_reboot()
{
	// First try a soft reset. Forget about returning to real mode and jumping to the
	// BIOS' restart vector... it seems like a lot of work for no real benefit. Instead,
	// just poke the 8042 keyboard controller and get it to pulse the RESET# pin on the
	// processor...

	enum Local_consts
	{
		KB_COMMAND_PORT	= 0x64,	// Write commands here.
		KB_STATUS_PORT	= 0x64,	// Read status here.
		KB_STATUS_BUSY	= 0x02,	// 1 = data in input reg. not yet read by controller; 0 = ready.
		KB_PULSE_RESET	= 0xFE	// Command to pulse the reset line.
	};

	// Wait until the keyboard controller is not busy (but don't wait forever -- paranoia).
	for (int i = 0x10000; i > 0; i--)
	{
		if ((IO_in8( KB_STATUS_PORT ) & KB_STATUS_BUSY) == 0)
		{
			break;
		}
	}

	//***FIXME: delay here.

	// Pulse the reset line. There is some code in Minix that pulses the A20 gate here as well,
	// but it is unclear whether this is required in a protected-mode kernel... even though the
	// Minix code in question only runs in protected mode. It's a mystery, but if bugs show up in
	// the future relating to problems rebooting, perhaps this should be looked into again. In
	// the meantime, there is a thread about it on the Mega-Tokyo forum:
	//
	// http://www.mega-tokyo.com/forum/index.php?board=1;action=display;threadid=7289
	IO_out8( KB_COMMAND_PORT, KB_PULSE_RESET );

	//***FIXME: another delay here.

	// The soft reset didn't work... Time to get medieval...
	Processor_hardReset();	// This will halt the processor if for some reason it fails to reset.
}


void ShutdownHardware_haltAllOtherProcessors()
{
	// This is a uniprocessor system. There are no other processors to halt.
	; // Do nothing.
}


void ShutdownHardware_halt()
{
	Processor_halt();
}

