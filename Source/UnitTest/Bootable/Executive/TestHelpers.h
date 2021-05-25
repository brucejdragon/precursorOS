#ifndef TESTHELPERS_H_
#define TESTHELPERS_H_

#include <limits.h>
#include "Kernel/KRunTime/KOut.h"
#include "Kernel/KCommon/KMem.h"


static inline void PrintCompyLogo( void )
{
	char c = (char) 219;	// This should be the solid block.

	char seg1[4];
	char seg2[5];
	char seg3[9];
	char seg4[11];

	KMem_set( seg1, c, sizeof( seg1 ) - 1 );
	seg1[sizeof( seg1 ) - 1] = '\0';

	KMem_set( seg2, c, sizeof( seg2 ) - 1 );
	seg2[sizeof( seg2 ) - 1] = '\0';

	KMem_set( seg3, c, sizeof( seg3 ) - 1 );
	seg3[sizeof( seg3 ) - 1] = '\0';

	KMem_set( seg4, c, sizeof( seg4 ) - 1 );
	seg4[sizeof( seg4 ) - 1] = '\0';

	KOut_write( "\n\n\n\n\n\n\n\n\t\t\t\t\t\t  %s\t   %s\t   %s\n", seg3, seg3, seg3 );
	KOut_write( "\t\t\t\t\t\t  %s\t   %s\t   %s\n", seg3, seg3, seg3 );
	KOut_write( "\n\n\t\t\t\t\t\t%s \t%s   %s\t%s   %s\n", seg1, seg2, seg2, seg2, seg2 );
	KOut_write( "\t\t\t\t\t\t%s \t%s   %s\t%s   %s\n", seg1, seg2, seg2, seg2, seg2 );
	KOut_write( "\n\n\t\t\t\t\t\t\t  %s\t   %s\t %s\n", seg2, seg3, seg4 );
	KOut_write( "\t\t\t\t\t\t\t  %s\t   %s\t %s\n", seg2, seg3, seg4 );
	KOut_write( "\n\n\t\t\t\t\t\t%s \t%s   %s\t%s   %s\t%s\n", seg1, seg2, seg2, seg2, seg2, seg2 );
	KOut_write( "\t\t\t\t\t\t%s \t%s   %s\t%s   %s\t%s\n", seg1, seg2, seg2, seg2, seg2, seg2 );
	KOut_write( "\n\n\t\t\t\t\t\t  %s\t   %s\t   %s\n", seg3, seg3, seg3 );
	KOut_write( "\t\t\t\t\t\t  %s\t   %s\t   %s\n", seg3, seg3, seg3 );
	KOut_write( "\n\n\n\t\t\t\t\t\t\t\t\t\t%s\n\n", "version 1.5" );
}


static inline void busyWait( int waitFactor )
{
	const int NUM_BUSY_LOOPS = INT_MAX / 30;

	for (int i = 0; i < waitFactor; i++)
	{
		for (int j = 0; j < NUM_BUSY_LOOPS; j++)
		{
			; // Do nothing.
		}
	}
}


#endif

