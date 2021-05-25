#include "Kernel/KRunTime/DisplayTextStream.h"
#include "Kernel/KRunTime/KOut.h"
#include "Kernel/KRunTime/KShutdown.h"
#include "Kernel/KCommon/KDebug.h"
#include "Kernel/HAL/Processor.h"
#include "ExceptionDispatcher.h"
#include "InterruptDispatcher.h"
#include "BootLoaderInfo.h"
#include "TestHelpers.h"


void DoBootLoaderInfoTest( const char* welcomeMessage, BootLoaderInfo* bootInfo )
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

	IPmmRegionList ramList		= BootLoaderInfo_getRamMemMap( bootInfo );
	IPmmRegionList reservedList	= BootLoaderInfo_getReservedMemMap( bootInfo );
	IPmmRegionList moduleList	= BootLoaderInfo_getModuleMemMap( bootInfo );
	
	KOut_writeLine( "\nCommandLine: %s", BootLoaderInfo_getKernelCommandLine( bootInfo ) );

	size_t numModules = BootLoaderInfo_getNumModules( bootInfo );

	KOut_writeLine( "\n%d Module%s:", numModules, (numModules == 1) ? "" : "s" );
	for (size_t i = 0; i < numModules; i++)
	{
		BootModuleInfo module = BootLoaderInfo_getModule( bootInfo, i );

		KOut_writeLine(
			"\tExtents: %x to %x",
			PmmRegion_base( &(module.Extents) ),
			PmmRegion_last( &(module.Extents) )
		);
		KOut_writeLine( "\tModule string: %s\n", module.ModuleString );
	}

	KOut_writeLine( "\nRAM regions:" );

	ramList.iptr->reset( ramList.obj );

	while (ramList.iptr->moveNext( ramList.obj ))
	{
		PmmRegion currentRegion = ramList.iptr->getCurrent( ramList.obj );
		KOut_writeLine(
			"\t%x to %x",
			PmmRegion_base( &currentRegion ),
			PmmRegion_last( &currentRegion )
		);
	}

	KOut_writeLine( "\nReserved physical address regions:" );

	reservedList.iptr->reset( reservedList.obj );

	while (reservedList.iptr->moveNext( reservedList.obj ))
	{
		PmmRegion currentRegion = reservedList.iptr->getCurrent( reservedList.obj );
		KOut_writeLine(
			"\t%x to %x",
			PmmRegion_base( &currentRegion ),
			PmmRegion_last( &currentRegion )
		);
	}

	KOut_writeLine( "\nRAM already in use:" );

	moduleList.iptr->reset( moduleList.obj );

	while (moduleList.iptr->moveNext( moduleList.obj ))
	{
		PmmRegion currentRegion = moduleList.iptr->getCurrent( moduleList.obj );
		KOut_writeLine(
			"\t%x to %x",
			PmmRegion_base( &currentRegion ),
			PmmRegion_last( &currentRegion )
		);
	}

	KOut_writeLine( "\nBootLoaderInfo test complete..." );
}

