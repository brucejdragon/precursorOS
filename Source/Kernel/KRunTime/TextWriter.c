// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/KRunTime/TextWriter.c
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
/// \brief	This file implements the TextWriter class.
///
// ===========================================================================


#include <stddef.h>

#define _KERNEL_KRUNTIME_TEXTWRITER_C_
#include "Kernel/KRunTime/TextWriter.h"
#undef _KERNEL_KRUNTIME_TEXTWRITER_C_

#include "Kernel/KCommon/KMath.h"
#include "Kernel/KCommon/KString.h"
#include "Kernel/KCommon/KDebug.h"


/// \brief	Defines local constants for the TextWriter class.
enum TextWriter_consts
{
	BITS_PER_HEX_DIGIT		= 4,	///< Number of bits in a hex digit.
	HEX_DIGITS_PER_BYTE		= 2,	///< Number of hex digits in a byte.
	MAX_DECIMAL_DIGITS		= 20,	///< Max # of chars needed to represent 64-bit ints.
	HEX_STRING_PREFIX_SIZE	= 2,	///< # of chars in prefix for hex string representation (0x).
	NEG_STRING_PREFIX_SIZE	= 1		///< # of chars in prefix for negative numbers (-).
};



// Private functions.

/// \brief	Writes the given integer in hexidecimal format, padded to the given number of hex
///			digits.
///
/// \param writer		the TextWriter that will write to its text stream.
/// \param i			the integer whose string representation will be output.
/// \param numDigits	the number of hex digits to which the output will be padded.
static void TextWriter_writeHex( TextWriter* writer, uintptr_t i, size_t numDigits )
{
	KDebug_assertArg( writer != NULL );

	// Create a buffer big enough to hold all the hex digits, +1 for a null terminator.
	char hexRep[(sizeof( uintptr_t ) * HEX_DIGITS_PER_BYTE) + HEX_STRING_PREFIX_SIZE + 1];

	hexRep[0] = '0';
	hexRep[1] = 'x';

	int lastIndex = HEX_STRING_PREFIX_SIZE + numDigits;

	KDebug_assert( lastIndex < (int) (sizeof( hexRep ) * sizeof( char )) );

	// Null-terminate the buffer.
	hexRep[lastIndex] = '\0';

	uintptr_t val = i;

	// -1 is to put j at a valid index. Beware the off-by-one errors!
	for (int j = lastIndex - 1; j >= HEX_STRING_PREFIX_SIZE; j--)
	{
		uint8_t nibble = (uint8_t) (val & 0xF);
		char c;

		if (nibble < 10)
		{
			c = ((char) nibble) + '0';
		}
		else
		{
			c = ((char) (nibble - 10)) + 'A';
		}

		hexRep[j] = c;
		val >>= BITS_PER_HEX_DIGIT;
	}

	TextWriter_writeString( writer, hexRep );
}


/// \brief	Writes the given integer in decimal format.
///
/// \param writer		the TextWriter that will write to its text stream.
/// \param i			the integer whose string representation will be output.
/// \param isNegative	if \c true, indicates that a minus sign should be output before the rest
///						of the digits.
static void TextWriter_writeDecimal( TextWriter* writer, uintptr_t i, bool isNegative )
{
	KDebug_assertArg( writer != NULL );

	// There are generally two ways to spit out decimal numbers:
	// 1. Spit them out backwards then reverse the string.
	// 2. Count how many digits are needed, then spit them out forwards.
	// We choose option #2 because it requires no additional support from the KString library.
	// Not that it matters much, but option #2 should also be faster because it involves fewer
	// memory writes.

	int numDigits = 0;

	// Special case: If the value is zero, it actually has one digit to display, not zero digits.
	if (i == 0)
	{
		numDigits = 1;
	}
	else
	{
		// Get a working copy of the value.
	    uintptr_t val = i;

		// Count the number of significant digits.
		while (val > 0)
		{
			val /= 10;
			numDigits++;
		}
	}

	KDebug_assert( numDigits <= MAX_DECIMAL_DIGITS );

	// Create a buffer big enough to hold all the digits, possibly a minus sign, and a null char.
	char strRep[MAX_DECIMAL_DIGITS + NEG_STRING_PREFIX_SIZE + 1];

	// Make room for a minus sign if necessary.
	int minusSignSize = (isNegative) ? NEG_STRING_PREFIX_SIZE : 0;

	int lastIndex = minusSignSize + numDigits;

	KDebug_assert( lastIndex < (int) (sizeof( strRep ) * sizeof( char )) );

	// Null-terminate the buffer.
	strRep[lastIndex] = '\0';

	// Get a working copy of the value.
	uintptr_t val = i;

	// -1 is to put j at a valid index. Beware the off-by-one errors!
	for (int j = lastIndex - 1; j >= minusSignSize; j--)
	{
		// REVISIT: On some architectures, this math may require FP support from the processor,
		// which is pretty worrisome for kernel-mode stuff.
		uint8_t	digit	= (uint8_t) (val % 10);
		char	c		= ((char) digit) + '0';

		strRep[j] = c;
		val /= 10;
	}

	if (isNegative)
	{
		strRep[0] = '-';
	}

	TextWriter_writeString( writer, strRep );
}



// Public functions.

TextWriter TextWriter_create( ITextStream stream )
{
	TextWriter newWriter;
	newWriter.m_isHexMode	= false;
	newWriter.m_stream		= stream;
	newWriter.m_width		= 0;
	newWriter.m_leftAlign	= false;
	return newWriter;
}


ITextStream TextWriter_getTextStream( const TextWriter* writer )
{
	KDebug_assertArg( writer != NULL );
	return writer->m_stream;
}


void TextWriter_setTextStream( TextWriter* writer, ITextStream stream )
{
	KDebug_assertArg( writer != NULL );
	writer->m_stream = stream;
}


size_t TextWriter_getWidth( const TextWriter* writer )
{
	KDebug_assertArg( writer != NULL );
	return writer->m_width;
}


void TextWriter_setWidth( TextWriter* writer, size_t width )
{
	KDebug_assertArg( writer != NULL );
	writer->m_width = width;
}


bool TextWriter_isHexMode( const TextWriter* writer )
{
	KDebug_assertArg( writer != NULL );
	return writer->m_isHexMode;
}


void TextWriter_setHexMode( TextWriter* writer, bool isHex )
{
	KDebug_assertArg( writer != NULL );
	writer->m_isHexMode = isHex;
}


bool TextWriter_isLeftAlign( const TextWriter* writer )
{
	KDebug_assertArg( writer != NULL );
	return writer->m_leftAlign;
}


void TextWriter_setLeftAlign( TextWriter* writer, bool leftAlign )
{
	KDebug_assertArg( writer != NULL );
	writer->m_leftAlign = leftAlign;
}


void TextWriter_writeString( TextWriter* writer, const char* str )
{
	KDebug_assertArg( writer != NULL );
	KDebug_assertArg( str != NULL );

	ITextStream	stream	= writer->m_stream;
	size_t		width	= writer->m_width;
	size_t		length	= KString_length( str );

	if ((!writer->m_leftAlign) && (length < width))
	{
		// Padding is needed... Right-align by padding to the left.
		for (size_t i = width - length; i > 0; i--)
		{
			stream.iptr->write( stream.obj, ' ' );
		}
	}

	while (*str != '\0')
	{
		stream.iptr->write( stream.obj, *str++ );
	}

	if ((writer->m_leftAlign) && (length < width))
	{
		// Padding is needed... Left-align by padding to the right.
		for (size_t i = width - length; i > 0; i--)
		{
			stream.iptr->write( stream.obj, ' ' );
		}
	}
}


void TextWriter_writeChar( TextWriter* writer, char c )
{
	KDebug_assertArg( writer != NULL );

	ITextStream	stream	= writer->m_stream;
	size_t		width	= writer->m_width;

	if ((!writer->m_leftAlign) && (width > 1))
	{
		// Padding is needed... Right-align by padding to the left.
		for (size_t i = width - 1; i > 0; i--)
		{
			stream.iptr->write( stream.obj, ' ' );
		}
	}

	stream.iptr->write( stream.obj, c );

	if ((writer->m_leftAlign) && (width > 1))
	{
		// Padding is needed... Left-align by padding to the right.
		for (size_t i = width - 1; i > 0; i--)
		{
			stream.iptr->write( stream.obj, ' ' );
		}
	}
}


void TextWriter_writePointer( TextWriter* writer, const void* p )
{
	KDebug_assertArg( writer != NULL );

	// Make sure the value is zero-extended, not sign-extended.
	uintptr_t ui = (uintptr_t) p;
	TextWriter_writeHex( writer, ui, sizeof( const void* ) * HEX_DIGITS_PER_BYTE );
}


void TextWriter_writeInt8( TextWriter* writer, int8_t i )
{
	KDebug_assertArg( writer != NULL );

	// Make sure the integer is sign-extended, not zero-extended.
	intptr_t ip = (intptr_t) i;

	if (writer->m_isHexMode)
	{
		TextWriter_writeHex( writer, (uintptr_t) ip, sizeof( int8_t ) * HEX_DIGITS_PER_BYTE );
	}
	else
	{
		TextWriter_writeDecimal( writer, KMath_abs( ip ), i < 0 );
	}
}


void TextWriter_writeInt16( TextWriter* writer, int16_t i )
{
	KDebug_assertArg( writer != NULL );

	// Make sure the integer is sign-extended, not zero-extended.
	intptr_t ip = (intptr_t) i;

	if (writer->m_isHexMode)
	{
		TextWriter_writeHex( writer, (uintptr_t) ip, sizeof( int16_t ) * HEX_DIGITS_PER_BYTE );
	}
	else
	{
		TextWriter_writeDecimal( writer, KMath_abs( ip ), i < 0 );
	}
}


void TextWriter_writeInt32( TextWriter* writer, int32_t i )
{
	KDebug_assertArg( writer != NULL );

	// Make sure the integer is sign-extended, not zero-extended.
	intptr_t ip = (intptr_t) i;

	if (writer->m_isHexMode)
	{
		TextWriter_writeHex( writer, (uintptr_t) ip, sizeof( int32_t ) * HEX_DIGITS_PER_BYTE );
	}
	else
	{
		TextWriter_writeDecimal( writer, KMath_abs( ip ), i < 0 );
	}
}


void TextWriter_writeIntPtr( TextWriter* writer, intptr_t i )
{
	KDebug_assertArg( writer != NULL );

	if (writer->m_isHexMode)
	{
		TextWriter_writeHex( writer, (uintptr_t) i, sizeof( intptr_t ) * HEX_DIGITS_PER_BYTE );
	}
	else
	{
		TextWriter_writeDecimal( writer, KMath_abs( i ), i < 0 );
	}
}


void TextWriter_writeUInt8( TextWriter* writer, uint8_t i )
{
	KDebug_assertArg( writer != NULL );

	// Make sure the integer is zero-extended, not sign-extended.
	uintptr_t uip = (uintptr_t) i;

	if (writer->m_isHexMode)
	{
		TextWriter_writeHex( writer, uip, sizeof( uint8_t ) * HEX_DIGITS_PER_BYTE );
	}
	else
	{
		TextWriter_writeDecimal( writer, uip, false );
	}
}


void TextWriter_writeUInt16( TextWriter* writer, uint16_t i )
{
	KDebug_assertArg( writer != NULL );

	// Make sure the integer is zero-extended, not sign-extended.
	uintptr_t uip = (uintptr_t) i;

	if (writer->m_isHexMode)
	{
		TextWriter_writeHex( writer, uip, sizeof( uint16_t ) * HEX_DIGITS_PER_BYTE );
	}
	else
	{
		TextWriter_writeDecimal( writer, uip, false );
	}
}


void TextWriter_writeUInt32( TextWriter* writer, uint32_t i )
{
	KDebug_assertArg( writer != NULL );

	// Make sure the integer is zero-extended, not sign-extended.
	uintptr_t uip = (uintptr_t) i;

	if (writer->m_isHexMode)
	{
		TextWriter_writeHex( writer, uip, sizeof( uint32_t ) * HEX_DIGITS_PER_BYTE );
	}
	else
	{
		TextWriter_writeDecimal( writer, uip, false );
	}
}


void TextWriter_writeUIntPtr( TextWriter* writer, uintptr_t i )
{
	KDebug_assertArg( writer != NULL );

	if (writer->m_isHexMode)
	{
		TextWriter_writeHex( writer, i, sizeof( uintptr_t ) * HEX_DIGITS_PER_BYTE );
	}
	else
	{
		TextWriter_writeDecimal( writer, i, false );
	}
}


void TextWriter_writeObject( TextWriter* writer, ITextWritable writable )
{
	KDebug_assertArg( writer != NULL );

	// Double-dispatch to the writable object.
	writable.iptr->writeTo( writable.obj, writer );
}


