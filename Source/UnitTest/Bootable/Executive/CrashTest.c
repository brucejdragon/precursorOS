#include "Kernel/KRunTime/DisplayTextStream.h"
#include "Kernel/KRunTime/KOut.h"
#include "Kernel/KRunTime/KShutdown.h"
#include "ExceptionDispatcher.h"
#include "InterruptDispatcher.h"
#include "TestHelpers.h"
#include "BootLoaderInfo.h"


void DoCrashTest( const char* welcomeMessage, BootLoaderInfo* bootInfo )
{
	(void) bootInfo;
	DisplayTextStream_init();
	KShutdown_init();
	ExceptionDispatcher_initForCurrentProcessor();
	InterruptDispatcher_initForCurrentProcessor();

	PrintCompyLogo();
	KOut_writeLine( welcomeMessage );

	busyWait( 10 );

	volatile KShutdown* ks = KShutdown_getInstance();

//	KShutdown_setRebootOnFailEnabled( ks, true );
	KShutdown_setRebootOnFailEnabled( ks, false );

//	KShutdown_setRebootDelayInMilliseconds( ks, 5555 );
//	KShutdown_setRebootDelayInMilliseconds( ks, 0 );

//	KShutdown_fail( ks, "Argh! I die, %s!\n", "Horatio" );
//	KShutdown_halt( ks );
//	KShutdown_reboot( ks );
	int* lucky = NULL;
	*lucky = 1;

	busyWait( 10 );

	KOut_writeLine( "\nIf the machine hasn't rebooted by now, the test failed." );
}

