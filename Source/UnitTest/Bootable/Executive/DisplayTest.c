#include "Kernel/HAL/KernelDisplay.h"
#include "Kernel/KRunTime/TextWriter.h"
#include "Kernel/KRunTime/DisplayTextStream.h"
#include "Kernel/KRunTime/KOut.h"
#include "TestHelpers.h"
#include "BootLoaderInfo.h"



void DoDisplayTest( const char* welcomeMessage, BootLoaderInfo* bootInfo )
{
	(void) bootInfo;
	DisplayTextStream_init();

	// NOTE: No concurrency allowed during the unit tests, so it's OK to cast away volatile.
	KernelDisplay*	kdisplay	= (KernelDisplay*) KernelDisplay_getInstance();
	TextWriter		out			= TextWriter_create( DisplayTextStream_getTextStream() );

	KernelDisplay_setBackgroundColor( kdisplay, KDISPLAY_COLOR_DARKRED );
	KernelDisplay_setForegroundColor( kdisplay, KDISPLAY_COLOR_YELLOW );
	KernelDisplay_clear( kdisplay );
	PrintCompyLogo();

	KOut_writeLineTo( &out, welcomeMessage );
	KOut_writeLine(
		"Test 0: Funny characters.\n\t%s\n%s\n%s\n%s\n%s\n%s",
		"0Oo: something; statement. something, something else.",
		"foo( bar );",
		"\"Queen's Ransom\"",
		"U V r a y s",
		"Ultimate Voyage",
		"Xeno's Zany paradox"
	);

	const char* someCode = "\
	template <class T>										\n\
	void foo()												\n\
	{														\n\
		_@[i] = $myvar[j];									\n\
		%foo = #bar;										\n\
		$baz = &ref;										\n\
															\n\
		$perl ~= '100% unreadable';							\n\
		g/use\\\\as/much\\\\punctuation/as/you/can\\\\/s;	\n\
															\n\
		/home/bruce/osdev									\n\
		`ok'												\n\
															\n\
		Foo* oldSchool = NULL;								\n\
		Bar^ newSchool = new Bar();							\n\
		x += 2 - 3;											\n\
		bar || baz;											\n\
		bar && baz;											\n\
		y = (x) ? 1 : 0;									\n\
		santa@hohoho.com									\n\
	}";

	KOut_writeLine( "And now, for some code:\n%s", someCode );
	busyWait( 30 );
	KOut_write( "\tTest 1: Short tabbed text." );

	for (int i = 0; i < 5; i++)
	{
		busyWait( 1 );
		KOut_write( "\nTest 2: Text with newlines.\n" );
		KOut_write( "Test 3: Text with\tembedded tabs." );
	}

	KOut_write( "\n" );
	for (int i = 0; i < 10; i++)
	{
		busyWait( 1 );
		KOut_write( "\tTest 4: Wrapping text." );
	}

	busyWait( 1 );
	const char* longMessage =
		"\nTest 5: I'm pretty sure this line is too long to fit within 80 columns. Yep, youbetcha.\
 Just too dang long, if yew ask me... I'm a hillbilly by the way, in case you didn't notice...\n";
	KOut_write( longMessage );

	char tooMuch[KDISPLAY_NUM_COLUMNS * (KDISPLAY_NUM_ROWS + 10)];
	for (size_t i = 0; i < sizeof( tooMuch ); i++)
	{
		tooMuch[i] = 'a' + i;
	}

	busyWait( 5 );
	KOut_writeLine( "Test 6: Too much text to fit on a single screen." );
	busyWait( 3 );
	// Must use KernelDisplay directly to make it a fair test. Otherwise buffering may interfere.
	KernelDisplay_print( kdisplay, tooMuch, sizeof( tooMuch ) );
	busyWait( 4 );
	KOut_write( "Test 7: Too much text to fit in the remaining screen." );
	busyWait( 3 );
	KOut_writeLine( "\nAll display tests complete." );
}

