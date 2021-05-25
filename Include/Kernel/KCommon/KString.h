// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/KCommon/KString.h
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
/// \brief	Defines string-handling functionality for the kernel.
///
// ===========================================================================

#ifndef _KERNEL_KCOMMON_KSTRING_H_
#define _KERNEL_KCOMMON_KSTRING_H_


#include <stddef.h>


/// \brief	Gets the length of the given null-terminated string.
///
/// \param str	the string.
///
/// \return the length of \a str in characters, not including the null terminator.
size_t KString_length( const char* str );


#endif

