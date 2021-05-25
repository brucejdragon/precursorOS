// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Executive/main.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Mar/28
//
// ===========================================================================
///
/// \file
///
/// \brief	This file defines the C-language entry point for the Precursor
///			microkernel. It initializes all kernel-subsystems, and triggers
///			the creation of the primeval thread.
///
// ===========================================================================


#include "Kernel/KRunTime/DisplayTextStream.h"
#include "Kernel/KRunTime/KOut.h"
#include "Kernel/KRunTime/KShutdown.h"
#include "Kernel/KCommon/KDebug.h"
#include "Kernel/HAL/Processor.h"
#include "Kernel/MM/PhysicalMemoryManager.h"
#include "ExceptionDispatcher.h"
#include "InterruptDispatcher.h"
#include "BootLoaderInfo.h"


#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifdef NDEBUG

static const char* BUILD_NAME = "Free";

#else

static const char* BUILD_NAME = "Checked";

#endif

#endif


/// \brief	C-language entry point of the Precursor microkernel.
///
/// \param bootInfo	information from the bootloader that will be used to initialize the kernel.
///
/// This function initializes all kernel-subsystems, and triggers the creation of the primeval
/// thread.
void kmain( BootLoaderInfo* bootInfo )
{
	DisplayTextStream_init();
	KShutdown_init();
	ExceptionDispatcher_initForCurrentProcessor();
	InterruptDispatcher_initForCurrentProcessor();

	volatile KShutdown* kshutdown = KShutdown_getInstance();
	KShutdown_setRebootOnFailEnabled( kshutdown, false );

	// Make sure the bootloader info was mapped properly.
	if (bootInfo == NULL)
	{
		volatile KShutdown* kshutdown = KShutdown_getInstance();
		//***FIXME: i18n?
		KShutdown_fail(
			kshutdown,
			"SYSTEM FAILURE\n%s\n%s\n\nReason: %s\n\n",
			"An unrecoverable error has occurred and the system must be shut down.",
			"We apologize for the inconvenience.",
			"Failed to read the boot loader information."
		);
	}

	KOut_writeLine( "Precursor OS 1.0.0000 x86 Uniprocessor %s", BUILD_NAME );
	KOut_writeLine( "Copyright (C) 2004-2005 Bruce Johnston" );

	// Initialize the Physical Memory Manager.
	IPmmRegionList ramList		= BootLoaderInfo_getRamMemMap( bootInfo );
	IPmmRegionList reservedList	= BootLoaderInfo_getReservedMemMap( bootInfo );
	IPmmRegionList moduleList	= BootLoaderInfo_getModuleMemMap( bootInfo );
	
	size_t spaceRequiredForPmm =
		PhysicalMemoryManager_initStageOne( ramList, reservedList, moduleList );
		
	KOut_writeLine(
		"\nSpace required for Physical Memory Manager: %d frames ***FIXME",
		spaceRequiredForPmm
	);
	
	//***FIXME: Allocate & map enough space and then call initStageTwo().

	KOut_writeLine( "\nI'd boot, but I don't know how yet..." );

	Processor_enableInterrupts();

	while (true)
	{
		Processor_waitForInterrupt();
	}
}

