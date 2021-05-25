#include "Kernel/KRunTime/DisplayTextStream.h"
#include "Kernel/KRunTime/KOut.h"
#include "Kernel/KRunTime/KShutdown.h"
#include "Kernel/MM/PhysicalMemoryManager.h"
#include "ExceptionDispatcher.h"
#include "InterruptDispatcher.h"
#include "BootLoaderInfo.h"
#include "TestHelpers.h"


static const int WAIT_TIME = 2;


void DoPmmTest( const char* welcomeMessage, BootLoaderInfo* bootInfo )
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
		KShutdown_fail(
			kshutdown,
			"SYSTEM FAILURE\n%s\n%s\n\nReason: %s\n\n",
			"An unrecoverable error has occurred and the system must be shut down.",
			"We apologize for the inconvenience.",
			"Failed to read the boot loader information."
		);
	}

	// Put up the Compy 386 logo just for laughs. Eventually we'll even have the sound to go with
	// it. :-)
	PrintCompyLogo();

	KOut_writeLine( welcomeMessage );

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
	
	// This is a test of the initial watermark allocator. We will allocate all the frames we can,
	// displaying them one "page" at a time. As a final flourish, we will try to free one of them,
	// which should result in a system failure in checked builds.
	KOut_writeLine( "\nStarting alloc() test." );
	busyWait( WAIT_TIME );
	
	volatile PhysicalMemoryManager* pmm = PhysicalMemoryManager_getInstance();
	IPmmAllocator allocator = PhysicalMemoryManager_getAllocator( pmm );

	phys_addr_t frameAddr = allocator.iptr->allocate( allocator.obj, NULL );
	phys_addr_t lastFrameAddr = PHYS_NULL;

	KOut_writeLine( "\tFirst: %x", frameAddr );
	while (frameAddr != PHYS_NULL)
	{
		if (frameAddr != lastFrameAddr + PAGE_SIZE)
		{
			KOut_writeLine( "\tGap: %x - %x", lastFrameAddr, frameAddr );
		}
		lastFrameAddr = frameAddr;
		frameAddr = allocator.iptr->allocate( allocator.obj, NULL );
	}
	KOut_writeLine( "\tLast: %x", lastFrameAddr );
	KOut_writeLine( "\nStarting free() test. If this is a checked build, prepare for a crash." );
	busyWait( WAIT_TIME );

	allocator.iptr->free( allocator.obj, lastFrameAddr );	
	
	KOut_writeLine( "\nPMM tests complete (if you got here, this must be a free build)." );
}

