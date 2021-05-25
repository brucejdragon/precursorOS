#include "Kernel/KRunTime/DisplayTextStream.h"
#include "Kernel/KRunTime/KOut.h"
#include "Kernel/HAL/Atomic.h"
#include "TestHelpers.h"
#include "BootLoaderInfo.h"


// This function doesn't really test whether atomic is atomic. Its intent is just to test
// the basic logic.
void DoAtomicTest( const char* welcomeMessage, BootLoaderInfo* bootInfo )
{
	(void) bootInfo;
	DisplayTextStream_init();

	PrintCompyLogo();
	KOut_writeLine( welcomeMessage );

	uintptr_t target	= 777;
	uintptr_t compare	= target;
	uintptr_t update	= 555;
	
	KOut_writeLine(
		"\n\nTarget value: %d\tCompare value: %d\tUpdate value: %d",
		target,
		compare,
		update
	);

	if (Atomic_compareAndSwap( &target, compare, update ))
	{
		KOut_writeLine( "Atomic test succeeded." );
	}
	else
	{
		KOut_writeLine( "Atomic test failed!" );
	}

	KOut_writeLine(
		"\nTarget value: %d\tCompare value: %d\tUpdate value: %d",
		target,
		compare,
		update
	);

	update = Atomic_swap( &update, update );
	KOut_writeLine( "Update should still be %d... %s", update, (update == 555) ? "yes" : "no!" );
	uintptr_t oldUpdate = Atomic_swap( &update, 444 );
	KOut_writeLine( "Update value now %d", update );
	KOut_writeLine( "It used to be %d", oldUpdate );

	if (Atomic_compareAndSwap( &target, compare, update ))
	{
		KOut_writeLine( "Atomic test failed!." );
	}
	else
	{
		KOut_writeLine( "Atomic test succeeded." );
	}

	KOut_writeLine(
		"\nTarget value: %d\tCompare value: %d\tUpdate value: %d",
		target,
		compare,
		update
	);

	KOut_writeLine( "\nAtomic test complete." );
}

