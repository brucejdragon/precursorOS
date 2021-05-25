// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/KCommon/KString.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2004/Sep/27
//
// ===========================================================================
///
///	\file
///
/// \brief	Implements string-handling functionality for the kernel-mode C
///			run-time library.
///
// ===========================================================================


#include "Kernel/KCommon/KString.h"
#include "Kernel/KCommon/KDebug.h"


size_t KString_length( const char* str )
{
	KDebug_assertArg( str != NULL );

	size_t length = 0;
	while (*str++ != '\0')
	{
		length++;
	}
	return length;
}

