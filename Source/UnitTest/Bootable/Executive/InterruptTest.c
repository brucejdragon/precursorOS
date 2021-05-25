#include <stdbool.h>
#include <stddef.h>
#include "ExceptionDispatcher.h"
#include "InterruptDispatcher.h"
#include "Kernel/KRunTime/KShutdown.h"
#include "Kernel/KRunTime/DisplayTextStream.h"
#include "Kernel/KRunTime/TextWriter.h"
#include "Kernel/KRunTime/KOut.h"
#include "Kernel/KCommon/KDebug.h"
#include "Kernel/HAL/Processor.h"
#include "Kernel/HAL/InterruptController.h"
#include "TestHelpers.h"
#include "BootLoaderInfo.h"


void DoInterruptTest( const char* welcomeMessage, BootLoaderInfo* bootInfo )
{
	(void) bootInfo;
	DisplayTextStream_init();
	KShutdown_init();
	ExceptionDispatcher_initForCurrentProcessor();
	InterruptDispatcher_initForCurrentProcessor();

	volatile KShutdown* kshutdown = KShutdown_getInstance();
	KShutdown_setRebootOnFailEnabled( kshutdown, false );

	TextWriter out = TextWriter_create( DisplayTextStream_getTextStream() );

	// Put up the Compy 386 logo just for laughs. Eventually we'll even have the sound to go with
	// it. :-)
	PrintCompyLogo();

	// Delay for a while before resuming our usual unit testing...
	busyWait( 2 );	// Comment out for VMWare testing...

	KOut_writeLineTo( &out, welcomeMessage );

	// Cause a breakpoint exception.
//	KDebug_assertMsg( false, "Foobar is a baz." );

	// Simulate an NMI.
//	__asm volatile ("int $2");

	// Cause a page fault.
//	char* bogus = NULL;
//	*bogus = '!';

	// NOTE: No concurrency allowed during unit testing, so it's OK to cast away volatile.
	InterruptController* pic = (InterruptController*) InterruptController_getForCurrentProcessor();

	// Allow for keyboard interrupts.
	for (uint8_t irq = 0; irq < 16; irq++)
	{
		InterruptController_mask( pic, irq );
	}
	InterruptController_unmask( pic, 1 );
	Processor_enableInterrupts();
	Processor_waitForInterrupt();

	KOut_writeTo( &out, "All interrupt tests %s%c", "complete.", '\0' );
}

