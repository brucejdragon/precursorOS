// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/KRunTime/KOut.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2004/Dec/23
//
// ===========================================================================
///
///	\file
///
/// \brief	Defines a utility class for performing formatted output in the
///			kernel.
///
/// \warning
/// In order to avoid deadlock, the system failure handler cannot call any
/// of the methods of this class that send their output directly to the kernel
/// display. Instead, it must create its own TextWriter to wrap an
/// unsynchronized display stream and pass this to the write*To/vWrite*To methods.
// ===========================================================================

#ifndef _KERNEL_KRUNTIME_KOUT_H_
#define _KERNEL_KRUNTIME_KOUT_H_


#include <stdarg.h>
#include <stdbool.h>
#include "Kernel/KRunTime/TextWriter.h"


/// \brief	Writes formatted output to the kernel display.
///
/// \param formatString	the printf-style format string to use for generating the formatted output.
///
/// See the documentation for KOut_writeTo() for a list of format specifiers that can be used in
/// \a formatString.
///
/// \retval true	all output completed successfully.
/// \retval false	a malformed format specifier was found in \a formatString.
bool KOut_write( const char* formatString, ... );


/// \brief	Writes formatted output terminated with a newline to the kernel display.
///
/// \param formatString	the printf-style format string to use for generating the formatted output.
///
/// See the documentation for KOut_writeTo() for a list of format specifiers that can be used in
/// \a formatString.
///
/// \retval true	all output completed successfully.
/// \retval false	a malformed format specifier was found in \a formatString.
bool KOut_writeLine( const char* formatString, ... );


/// \brief	Writes formatted output to the given TextWriter.
///
/// \param writer		the TextWriter to which output should be sent.
/// \param formatString	the printf-style format string to use for generating the formatted output.
///
/// Each format specifier in \a formatString has the following syntax:\n
/// <tt>\%[-][*][modifier]type</tt>
///
/// The following is a list of format types and their meanings:
/// \li \b c	Character
/// \li <b>d or i</b>	Signed decimal integer
/// \li \b s	Pointer to a null-terminated string of ANSI characters
/// \li \b u	Unsigned decimal integer
/// \li \b x	Unsigned hexidecimal integer
/// \li \b p	Address pointed by the argument
/// \li \b O	Object (must be an ITextWritable struct, passed by value. This is capital O to
///				distinguish it from standard printf's "octal" type specifier.
///
/// The optional \e - flag after the % symbol is used to left-align the formatted output
/// (right-align is the default).
///
/// The optional \e * specifier is used to indicate that the argument to be formatted is preceeded
/// in the argument list by an integer. This integer contains the minimum number of characters to
/// be printed. If the argument contains fewer characters than indicated by this number, it will be
/// padded with spaces. It is never truncated, even if it is longer than the given width.
///
/// The following is a list of format modifiers and their meanings:
/// \li \b h The argument is interpreted as a short int (integer types).
/// \li \b l The argument is interpreted as a long int (integer types).
/// \li \b b The argument is interpreted as a byte (integer types).
///
/// Note that if the argument is an integer but no modifier is present, it will be interpreted as
/// a "native" machine word (i.e. -- as either an intptr_t or uintptr_t, depending on whether it is
/// signed or unsigned).
///
/// To include a percent sign in the actual output, put \c \%\% in the \a formatString.
///
/// Note that if an error is encountered while parsing a format specifier, this function will
/// immediately return \c false and no more characters will be printed. This is the safest
/// approach, since continuing to read from a variable-length argument list based on a malformed
/// format string could lead to memory corruption. For the variations of this function that
/// append a newline, the newline is appended whether or not a parse error occurred.
///
/// This method (along with all the other write* methods), if successful, flushes any buffered
/// output before returning to the caller.
///
/// \retval true	all output completed successfully.
/// \retval false	a malformed format specifier was found in \a formatString.
bool KOut_writeTo( TextWriter* writer, const char* formatString, ... );


/// \brief	Writes formatted output terminated with a newline to the given TextWriter.
///
/// \param writer		the TextWriter to which output should be sent.
/// \param formatString	the printf-style format string to use for generating the formatted output.
///
/// See the documentation for KOut_writeTo() for a list of format specifiers that can be used in
/// \a formatString.
///
/// \retval true	all output completed successfully.
/// \retval false	a malformed format specifier was found in \a formatString.
bool KOut_writeLineTo( TextWriter* writer, const char* formatString, ... );


/// \brief	Writes formatted output to the kernel display.
///
/// \param formatString	the printf-style format string to use for generating the formatted output.
/// \param args			the variable-length argument list of items to be formatted.
///
/// This method is basically equivalent to KOut_write(), except that it takes its variable
/// argument list in the form of a va_list rather than directly. This allows other methods that
/// take variable-length argument lists to delegate safely to this method.
///
/// See KOut_writeTo() for details on the allowable format specifiers and their syntax.
///
/// \retval true	all output completed successfully.
/// \retval false	a malformed format specifier was found in \a formatString.
bool KOut_vWrite( const char* formatString, va_list args );


/// \brief	Writes formatted output terminated with a newline to the kernel display.
///
/// \param formatString	the printf-style format string to use for generating the formatted output.
/// \param args			the variable-length argument list of items to be formatted.
///
/// This method is basically equivalent to KOut_writeLine(), except that it takes its variable
/// argument list in the form of a va_list rather than directly. This allows other methods that
/// take variable-length argument lists to delegate safely to this method.
///
/// See KOut_writeTo() for details on the allowable format specifiers and their syntax.
///
/// \retval true	all output completed successfully.
/// \retval false	a malformed format specifier was found in \a formatString.
bool KOut_vWriteLine( const char* formatString, va_list args );


/// \brief	Writes formatted output to the given TextWriter.
///
/// \param writer		the TextWriter to which output should be sent.
/// \param formatString	the printf-style format string to use for generating the formatted output.
/// \param args			the variable-length argument list of items to be formatted.
///
/// This method is basically equivalent to KOut_writeTo(), except that it takes its variable
/// argument list in the form of a va_list rather than directly. This allows other methods that
/// take variable-length argument lists to delegate safely to this method.
///
/// See KOut_writeTo() for details on the allowable format specifiers and their syntax.
///
/// \retval true	all output completed successfully.
/// \retval false	a malformed format specifier was found in \a formatString.
bool KOut_vWriteTo( TextWriter* writer, const char* formatString, va_list args );


/// \brief	Writes formatted output terminated with a newline to the given TextWriter.
///
/// \param writer		the TextWriter to which output should be sent.
/// \param formatString	the printf-style format string to use for generating the formatted output.
/// \param args			the variable-length argument list of items to be formatted.
///
/// This method is basically equivalent to KOut_writeLineTo(), except that it takes its variable
/// argument list in the form of a va_list rather than directly. This allows other methods that
/// take variable-length argument lists to delegate safely to this method.
///
/// See KOut_writeTo() for details on the allowable format specifiers and their syntax.
///
/// \retval true	all output completed successfully.
/// \retval false	a malformed format specifier was found in \a formatString.
bool KOut_vWriteLineTo( TextWriter* writer, const char* formatString, va_list args );


#endif

