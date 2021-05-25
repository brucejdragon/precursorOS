#include <stdbool.h>
#include "BootLoaderInfo.h"

void DoInterruptTest( const char* welcomeMessage, BootLoaderInfo* bootInfo );
void DoDisplayTest( const char* welcomeMessage, BootLoaderInfo* bootInfo );
void DoCrashTest( const char* welcomeMessage, BootLoaderInfo* bootInfo );
void DoAtomicTest( const char* welcomeMessage, BootLoaderInfo* bootInfo );
void DoBootLoaderInfoTest( const char* welcomeMessage, BootLoaderInfo* bootInfo );
void DoPmmTest( const char* welcomeMessage, BootLoaderInfo* bootInfo );


void kmain( BootLoaderInfo* bootInfo )
{
#ifdef NDEBUG
	const char* welcomeMessage =
		"Welcome to Bruce's OS (x86 uniprocessor free)! (Currently under construction)";
#else
	const char* welcomeMessage =
		"Welcome to Bruce's OS (x86 uniprocessor checked)! (Currently under construction)";
#endif

//	DoInterruptTest( welcomeMessage, bootInfo );
//	DoDisplayTest( welcomeMessage, bootInfo );
//	DoCrashTest( welcomeMessage, bootInfo );
//	DoAtomicTest( welcomeMessage, bootInfo );
//	DoBootLoaderInfoTest( welcomeMessage, bootInfo );
	DoPmmTest( welcomeMessage, bootInfo );

	while (true)
	{
		; // Do nothing.
	}
}

