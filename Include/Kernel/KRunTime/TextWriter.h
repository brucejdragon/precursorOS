// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/KRunTime/TextWriter.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2004/Dec/22
//
// ===========================================================================
///
/// \file
///
/// \brief	Implements the TextWriter class, which is a utility for formatting
///			data of various primitive types into strings and sending them to an
///			ITextStream for output.
///
/// The TextWriter implementation is focused on converting primitive
/// data to its string representation. As such, it does not perform any buffering.
/// This is up to the ITextStream implementation to which a TextWriter delegates.
///
/// \note
/// There is no function for outputting a 64-bit value on 32-bit systems. There
/// is a certain amount of extra work involved in getting 64-bit arithmetic to
/// work in a 32-bit kernel, and there is no forseen need for it. For the purposes
/// of kernel diagnostics, most output will likely be pointers, which are 32 bits
/// on 32-bit systems anyway. It will of course be possible to output 64-bit
/// integers on 64-bit systems by calling either TextWriter_writePointer(),
/// TextWriter_writeIntPtr(), or TextWriter_writeUIntPtr().
// ===========================================================================

#ifndef _KERNEL_KRUNTIME_TEXTWRITER_H_
#define _KERNEL_KRUNTIME_TEXTWRITER_H_


#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "Kernel/KRunTime/ITextStream.h"
#include "Kernel/KRunTime/ITextWritable.h"



/// \brief	Defines the fields of TextWriter.
typedef struct TextWriterStruct
{
#ifdef _KERNEL_KRUNTIME_TEXTWRITER_C_

	/// \brief	The ITextStream to which the TextWriter sends its output.
	ITextStream m_stream;

	/// \brief	Indicates whether the TextWriter outputs integers in hexidecimal or decimal
	///			format.
	bool m_isHexMode;

	/// \brief	Minimum length of generated output strings. Output less than this length are
	///			padded with blanks.
	size_t m_width;

	/// \brief	Controls whether padded output is left-aligned or right-aligned. Output is only
	///			padded if m_width is non-zero.
	bool m_leftAlign;

#else

	#ifndef DOXYGEN_SHOULD_SKIP_THIS
	ITextStream	m_reserved0;
	bool		m_reserved1;
	size_t		m_reserved2;
	bool		m_reserved3;
	#endif

#endif
}  TextWriter;



/// \brief	Creates a new TextWriter instance that is used to send all output to the given stream.
///
/// \param stream	the ITextStream to which the new TextWriter should send all its output.
///
/// The new TextStream has padding of generated output strings disabled, and outputs integers in
/// decimal format by default.
///
/// \return a new TextWriter instance.
TextWriter TextWriter_create( ITextStream stream );


/// \brief	Gets a reference to the ITextStream implementation that the TextWriter writes to.
///
/// \param writer	the TextWriter instance.
///
/// \return the ITextStream that \a writer writes to.
ITextStream TextWriter_getTextStream( const TextWriter* writer );


/// \brief	Sets the ITextStream reference of the TextWriter to the given reference.
///
/// \param writer	the TextWriter instance.
/// \param stream	the ITextStream that \a writer will write to.
void TextWriter_setTextStream( TextWriter* writer, ITextStream stream );


/// \brief	Gets the current width setting for the TextWriter.
///
/// \param writer	the TextWriter instance.
///
/// \retval zero		there is no active width setting -- output is not padded.
/// \retval non-zero	the minimum length of each item written to the output stream. Generated
///						strings are padded to this length with blanks if they are too short. They
///						are \e not truncated if they are too long.
size_t TextWriter_getWidth( const TextWriter* writer );


/// \brief	Sets the width for the output of the TextWriter.
///
/// \param writer	the TextWriter instance.
/// \param width	the new width setting to use. See TextWriter_getWidth() for an explanation of
///					this setting. Pass zero for this parameter to disable output padding.
void TextWriter_setWidth( TextWriter* writer, size_t width );


/// \brief	Indicates whether the TextWriter is outputting integers in hexidecimal or decimal
///			format.
///
/// \param writer	the TextWriter instance.
///
/// By default, a TextWriter will output integers in decimal format.
///
/// \retval true	\a writer is outputting all integers in hexidecimal format.
/// \retval false	\a writer is outputting all integers in decimal format.
bool TextWriter_isHexMode( const TextWriter* writer );


/// \brief	Sets the integer output format mode of the TextWriter.
///
/// \param writer	the TextWriter instance.
/// \param isHex	if \c true, all integers will be output in hexidecimal format. If \c false,
///					all integers will be output in decimal format.
void TextWriter_setHexMode( TextWriter* writer, bool isHex );


/// \brief	Indicates whether the TextWriter is left-aligning or right-aligning padded output.
///
/// \param writer	the TextWriter instance.
///
/// By default, a TextWriter will right-align output, but only if its width setting is non-zero.
///
/// \retval true	\a writer is left-aligning all padded output.
/// \retval false	\a writer is right-aligning all padded output.
bool TextWriter_isLeftAlign( const TextWriter* writer );


/// \brief	Sets the alignment mode of the TextWriter.
///
/// \param writer		the TextWriter instance.
/// \param leftAlign	if \c true, all padded output will be left-aligned. If \c false, all padded
///						output will be right-aligned. Output will only be padded (and therefore
///						aligned) if the width setting of \a writer is non-zero.
void TextWriter_setLeftAlign( TextWriter* writer, bool leftAlign );


/// \brief	Writes the given string to a text stream.
///
/// \param writer	the TextWriter that will write to its text stream.
/// \param str		the string to output.
///
/// \note
/// There is no special interpretation of the contents of the given string. No conversion or
/// additional processing is performed.
void TextWriter_writeString( TextWriter* writer, const char* str );


/// \brief	Writes the given character to a text stream.
///
/// \param writer	the TextWriter that will write to its text stream.
/// \param c		the character to output.
///
/// \note
/// There is no special interpretation of the given character. No conversions are performed.
void TextWriter_writeChar( TextWriter* writer, char c );


/// \brief	Writes the value of the given pointer to a text stream.
///
/// \param writer	the TextWriter that will write to its text stream.
/// \param p		the pointer to output.
///
/// \note
/// The address contained in \a p will be output in hexidecimal format, regardless of whether
/// or not \a writer is in hex mode. The address will be padded with leading zeroes as necessary
/// to output it with the appropriate number of hex digits (e.g. -- 8 for 32-bit systems, 16 for
/// 64-bit systems, etc.).
void TextWriter_writePointer( TextWriter* writer, const void* p );


/// \brief	Writes the string representation of an 8-bit signed integer to a text stream.
///
/// \param writer	the TextWriter that will write to its text stream.
/// \param i		the integer whose string representation will be output.
///
/// When outputting hex, this function will pad with leading zeroes as necessary to make the
/// resulting number 2 hex digits long.
void TextWriter_writeInt8( TextWriter* writer, int8_t i );


/// \brief	Writes the string representation of a 16-bit signed integer to a text stream.
///
/// \param writer	the TextWriter that will write to its text stream.
/// \param i		the integer whose string representation will be output.
///
/// When outputting hex, this function will pad with leading zeroes as necessary to make the
/// resulting number 4 hex digits long.
void TextWriter_writeInt16( TextWriter* writer, int16_t i );


/// \brief	Writes the string representation of a 32-bit signed integer to a text stream.
///
/// \param writer	the TextWriter that will write to its text stream.
/// \param i		the integer whose string representation will be output.
///
/// When outputting hex, this function will pad with leading zeroes as necessary to make the
/// resulting number 8 hex digits long.
void TextWriter_writeInt32( TextWriter* writer, int32_t i );


/// \brief	Writes the string representation of a "native" signed integer to a text stream.
///
/// \param writer	the TextWriter that will write to its text stream.
/// \param i		the integer whose string representation will be output.
///
/// When outputting hex, this function will pad with leading zeroes as necessary to make the
/// resulting number the appropriate number of hex digits long.
void TextWriter_writeIntPtr( TextWriter* writer, intptr_t i );


/// \brief	Writes the string representation of an 8-bit unsigned integer to a text stream.
///
/// \param writer	the TextWriter that will write to its text stream.
/// \param i		the integer whose string representation will be output.
///
/// When outputting hex, this function will pad with leading zeroes as necessary to make the
/// resulting number 2 hex digits long.
void TextWriter_writeUInt8( TextWriter* writer, uint8_t i );


/// \brief	Writes the string representation of a 16-bit unsigned integer to a text stream.
///
/// \param writer	the TextWriter that will write to its text stream.
/// \param i		the integer whose string representation will be output.
///
/// When outputting hex, this function will pad with leading zeroes as necessary to make the
/// resulting number 4 hex digits long.
void TextWriter_writeUInt16( TextWriter* writer, uint16_t i );


/// \brief	Writes the string representation of a 32-bit unsigned integer to a text stream.
///
/// \param writer	the TextWriter that will write to its text stream.
/// \param i		the integer whose string representation will be output.
///
/// When outputting hex, this function will pad with leading zeroes as necessary to make the
/// resulting number 8 hex digits long.
void TextWriter_writeUInt32( TextWriter* writer, uint32_t i );


/// \brief	Writes the string representation of a "native" unsigned integer to a text stream.
///
/// \param writer	the TextWriter that will write to its text stream.
/// \param i		the integer whose string representation will be output.
///
/// When outputting hex, this function will pad with leading zeroes as necessary to make the
/// resulting number the appropriate number of hex digits long.
void TextWriter_writeUIntPtr( TextWriter* writer, uintptr_t i );


/// \brief	Writes the given ITextWritable object to a text stream.
///
/// \param writer	the TextWriter that will write to its text stream.
/// \param writable	the ITextWritable implementation that will use \a writer to output its own
///					contents.
void TextWriter_writeObject( TextWriter* writer, ITextWritable writable );


#endif

