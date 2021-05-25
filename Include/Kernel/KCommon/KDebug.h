// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/KCommon/KDebug.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Feb/03
//
// ===========================================================================
///
/// \file
///
/// \brief	This file defines assert macros for the kernel.
///
// ===========================================================================

#ifndef _KERNEL_KCOMMON_KDEBUG_H_
#define _KERNEL_KCOMMON_KDEBUG_H_


/// \brief	Ensures that the given debugging information is in an accessible location on the
///			kernel stack, then traps to the system's breakpoint handler.
///
/// \param msg	a null-terminated string containing a formatted message associated with the debug
///				event.
/// \param file	a null-terminated string containing the name of the source file in which the
///				debug event occurred.
/// \param line	the line number in \a file of the code that triggered the debug event.
///
/// This function ensures that \a line, \a file, \a msg, and the address in the program counter at
/// the time this function was called (i.e. -- the return address) are on the stack according to
/// the C calling convention before trapping to the breakpoint handler. This means that these
/// parameters will be accessible from the breakpoint handler by poking in memory just beyond the
/// TrapFrame ("below" it on the stack/at the next highest addresses). Obviously, retrieving this
/// data is an architecture-specific operation and must take into account exactly how this function
/// is implemented. When this function returns, these items are cleaned from the stack accordingly,
/// if necessary.
void KDebug_triggerDebugTrap( const char* msg, const char* file, int line );


#ifdef NDEBUG /////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS
	#define KDebug_assertMsg( passed, msg ) ((void) 0)
#endif

#else /////////////////////////////////////////////////////


/// \brief	In free builds, does nothing. In checked builds, triggers a debug trap if the
///			given expression evaluates to zero.
///
/// \param passed	the expression to test.
/// \param msg		the message to include with the debug information.
#define KDebug_assertMsg( passed, msg ) ((passed) ? (void) 0 \
	: KDebug_triggerDebugTrap( msg, __FILE__, __LINE__ ))

#endif ////////////////////////////////////////////////////


/// \brief	In free builds, does nothing. In checked builds, triggers a debug trap if the
///			given expression evaluates to zero.
///
/// \param passed	the expression to test.
#define KDebug_assert( passed ) KDebug_assertMsg( (passed), "Assertion failed." )	//***FIXME: i18n


/// \brief	In free builds, does nothing. In checked builds, triggers a debug trap if the
///			given expression evaluates to zero.
///
/// The message included in the debug information mentions "invalid argument" as the reason
/// for the failed assertion.
///
/// \param passed	the expression to test.
#define KDebug_assertArg( passed ) KDebug_assertMsg( (passed), \
	"Assertion failed: Invalid argument." )	//***FIXME: i18n


#endif

