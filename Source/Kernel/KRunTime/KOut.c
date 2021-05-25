// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/KRunTime/KOut.c
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
/// \brief	Implements a utility class for performing formatted output in the
///			kernel.
///
// ===========================================================================


#include <stddef.h>
#include "Kernel/KCommon/KDebug.h"
#include "Kernel/KRunTime/KOut.h"
#include "Kernel/KRunTime/DisplayTextStream.h"


/// \brief	Represents one of the modifier types of a format specifier.
typedef enum
{
	BYTE_MODIFIER = 0,	///< An integer should be output as a byte.
	SHORT_MODIFIER,		///< An integer should be output as a short.
	LONG_MODIFIER,		///< An integer should be output as a long.
	NO_MODIFIER			///< An integer should be output as its "native" size.
} FormatSpecModifier;



/// \brief	Represents the state accumulated by the format specifier parser.
typedef struct
{
	bool				leftAlign;	///< Whether left-alignment is to be used.
	FormatSpecModifier	modifier;	///< The modifier to use for formatting.
	size_t				width;		///< Width specifier for the formatted field.
} FormatSpec;



/// \brief	Represents the possible states of the format specifier parser.
typedef enum
{
	FOUND_NOTHING_STATE = 0,	///< Initial state -- found no specifiers.
	FOUND_PERCENT_STATE,		///< \% was just found in the input.
	FOUND_LEFTALIGN_STATE,		///< A - was found in the input right after \%.
	FOUND_WIDTH_STATE,			///< A * was found in the input, either after \% or after -.
	FOUND_MODIFIER_STATE,		///< A modifier was found in the input, after \%, -, or *.
	FINAL_STATE,				///< Final state.
	NUM_STATES					///< Indicates how many states are in the state machine.
} ParserState;



/// \brief	Represents possible input characters that drive the format specifier parser state
///			machine.
typedef enum
{
	PERCENT_CHAR = 0,	///< The input character is \%.
	MINUS_CHAR,			///< The input character is -.
	STAR_CHAR,			///< The input character is *.
	MOD_CHAR,			///< The input character is one of the modifiers.
	TYPE_CHAR,			///< The input character is one of the type specifiers.
	NULL_CHAR,			///< The input character is a null terminator.
	NORMAL_CHAR,		///< The input character is not of the special ones recognized.
	NUM_CHAR_TYPES		///< Indicates how many character types exist.
} ParserInputChar;



/// \brief	Represents the different actions taken on various state transitions of the format
///			specifier parser.
typedef enum
{
	SET_LEFTALIGN_ACTION = 0,	///< Indicates that left alignment should be set.
	SET_WIDTH_ACTION,			///< Indicates that width should be set.
	SET_MODIFIER_ACTION,		///< Indicates that the modifier should be set.
	WRITE_OUTPUT_ACTION,		///< Indicates that the output should be written.
	WRITE_FORMAT_CHAR_ACTION,	///< Indicates that the current format char should be written.
	NO_ACTION,					///< Indicates that nothing should be done (eat the input).
	ABORT_ACTION				///< Indicates that there was a parse error.
} ParserAction;



/// \brief	Defines an entry in the format specifier parser's state transition table.
typedef struct
{
	ParserAction	action;		///< Action to take on state transition.
	ParserState		nextState;	///< State to switch to as part of this state transition.
} ParserStateTransition;



/// \brief	This is a state transition table for the format specifier parser. The first index is
///			the current ParserState. The second index is the current ParserInputChar.
static ParserStateTransition s_parserStates[NUM_STATES][NUM_CHAR_TYPES] = {
	// FOUND_NOTHING_STATE
	{{NO_ACTION, FOUND_PERCENT_STATE},					// %
	 {WRITE_FORMAT_CHAR_ACTION, FOUND_NOTHING_STATE},	// -
	 {WRITE_FORMAT_CHAR_ACTION, FOUND_NOTHING_STATE},	// *
	 {WRITE_FORMAT_CHAR_ACTION, FOUND_NOTHING_STATE},	// mod
	 {WRITE_FORMAT_CHAR_ACTION, FOUND_NOTHING_STATE},	// type
	 {NO_ACTION, FINAL_STATE},							// null
	 {WRITE_FORMAT_CHAR_ACTION, FOUND_NOTHING_STATE}},	// normal

	// FOUND_PERCENT_STATE
	{{WRITE_FORMAT_CHAR_ACTION, FOUND_NOTHING_STATE},	// %
	 {SET_LEFTALIGN_ACTION, FOUND_LEFTALIGN_STATE},		// -
	 {SET_WIDTH_ACTION, FOUND_WIDTH_STATE},				// *
	 {SET_MODIFIER_ACTION, FOUND_MODIFIER_STATE},		// mod
	 {WRITE_OUTPUT_ACTION, FOUND_NOTHING_STATE},		// type
	 {ABORT_ACTION, FINAL_STATE},						// null
	 {ABORT_ACTION, FINAL_STATE}},						// normal

	// FOUND_LEFTALIGN_STATE
	{{ABORT_ACTION, FINAL_STATE},					// %
	 {ABORT_ACTION, FINAL_STATE},					// -
	 {SET_WIDTH_ACTION, FOUND_WIDTH_STATE},			// *
	 {SET_MODIFIER_ACTION, FOUND_MODIFIER_STATE},	// mod
	 {WRITE_OUTPUT_ACTION, FOUND_NOTHING_STATE},	// type
	 {ABORT_ACTION, FINAL_STATE},					// null
	 {ABORT_ACTION, FINAL_STATE}},					// normal

	// FOUND_WIDTH_STATE
	{{ABORT_ACTION, FINAL_STATE},					// %
	 {ABORT_ACTION, FINAL_STATE},					// -
	 {ABORT_ACTION, FINAL_STATE},					// *
	 {SET_MODIFIER_ACTION, FOUND_MODIFIER_STATE},	// mod
	 {WRITE_OUTPUT_ACTION, FOUND_NOTHING_STATE},	// type
	 {ABORT_ACTION, FINAL_STATE},					// null
	 {ABORT_ACTION, FINAL_STATE}},					// normal

	// FOUND_MODIFIER_STATE
	{{ABORT_ACTION, FINAL_STATE},					// %
	 {ABORT_ACTION, FINAL_STATE},					// -
	 {ABORT_ACTION, FINAL_STATE},					// *
	 {ABORT_ACTION, FINAL_STATE},					// mod
	 {WRITE_OUTPUT_ACTION, FOUND_NOTHING_STATE},	// type
	 {ABORT_ACTION, FINAL_STATE},					// null
	 {ABORT_ACTION, FINAL_STATE}},					// normal

	// FINAL_STATE
	{{ABORT_ACTION, FINAL_STATE},	// %
	 {ABORT_ACTION, FINAL_STATE},	// -
	 {ABORT_ACTION, FINAL_STATE},	// *
	 {ABORT_ACTION, FINAL_STATE},	// mod
	 {ABORT_ACTION, FINAL_STATE},	// type
	 {ABORT_ACTION, FINAL_STATE},	// null
	 {ABORT_ACTION, FINAL_STATE}}	// normal
};


// Private functions

/// \brief	Given an input char, returns an indicator of what it represents.
///
/// \param c the character.
///
/// \return a ParserInputChar value.
static ParserInputChar KOut_convertInputChar( char c )
{
	switch (c)
	{
	case '%':
		return PERCENT_CHAR;

	case '-':
		return MINUS_CHAR;

	case '*':
		return STAR_CHAR;

	case 'b':
	case 'l':
	case 'h':
		return MOD_CHAR;

	case 'c':
	case 'd':
	case 'i':
	case 's':
	case 'u':
	case 'x':
	case 'p':
	case 'O':
		return TYPE_CHAR;
		
	case '\0':
		return NULL_CHAR;

	default:
		return NORMAL_CHAR;
	}
}


/// \brief	Given an input char, returns an indicator of what modifier it represents.
///
/// \param c the character.
///
/// \return a FormatSpecModifier value.
static FormatSpecModifier KOut_convertModifier( char c )
{
	switch (c)
	{
	case 'b':
		return BYTE_MODIFIER;
		
	case 'h':
		return SHORT_MODIFIER;
		
	case 'l':
		return LONG_MODIFIER;

	default:
		KDebug_assert( false );
		return NO_MODIFIER;
	}
}


/// \brief	Writes the next argument in the given variable-length argument list to the given
///			TextWriter, using the given type and formatting information.
///
/// \param typeSpecifier	a character whose value is a valid type specifier.
/// \param formatSpec		a collection of information that specifies how the value should be
///							formatted for output.
/// \param writer			the TextWriter to use for output.
/// \param args				the current variable-length argument list. This function consumes only
///							the head of the list and returns the tail.
///
/// \return the tail of \a args.
static va_list KOut_writeNextArg(
	char				typeSpecifier,
	const FormatSpec*	formatSpec,
	TextWriter*			writer,
	va_list				args
)
{
	KDebug_assertArg( formatSpec != NULL );
	KDebug_assertArg( writer != NULL );

	bool oldHexMode		= TextWriter_isHexMode( writer );
	bool oldAlignMode	= TextWriter_isLeftAlign( writer );
	size_t oldWidth		= TextWriter_getWidth( writer );

	TextWriter_setHexMode( writer, false );
	TextWriter_setLeftAlign( writer, formatSpec->leftAlign );
	TextWriter_setWidth( writer, formatSpec->width );

	switch (typeSpecifier)
	{
	case 'c':
		TextWriter_writeChar( writer, (char) va_arg( args, int ) );
		break;

	case 'd':
	case 'i':
		switch (formatSpec->modifier)
		{
		case BYTE_MODIFIER:
			TextWriter_writeInt8( writer, (int8_t) va_arg( args, int ) );
			break;

		case SHORT_MODIFIER:
			TextWriter_writeInt16( writer, (int16_t) va_arg( args, int ) );
			break;

		case LONG_MODIFIER:
			TextWriter_writeInt32( writer, va_arg( args, int32_t ) );
			break;

		case NO_MODIFIER:
			TextWriter_writeIntPtr( writer, va_arg( args, intptr_t ) );
			break;

		default:
			KDebug_assert( false );
			break;
		}
		break;

	case 's':
		TextWriter_writeString( writer, va_arg( args, const char* ) );
		break;

	case 'x':
		TextWriter_setHexMode( writer, true );
		// Fall through...
	case 'u':
		switch (formatSpec->modifier)
		{
		case BYTE_MODIFIER:
			TextWriter_writeUInt8( writer, (uint8_t) va_arg( args, unsigned int ) );
			break;

		case SHORT_MODIFIER:
			TextWriter_writeUInt16( writer, (uint16_t) va_arg( args, unsigned int ) );
			break;

		case LONG_MODIFIER:
			TextWriter_writeUInt32( writer, va_arg( args, uint32_t ) );
			break;

		case NO_MODIFIER:
			TextWriter_writeUIntPtr( writer, va_arg( args, uintptr_t ) );
			break;

		default:
			KDebug_assert( false );
			break;
		}
		break;

	case 'p':
		TextWriter_writePointer( writer, va_arg( args, void* ) );
		break;

	case 'O':
		TextWriter_writeObject( writer, va_arg( args, ITextWritable ) );
		break;

	default:
		KDebug_assert( false );
		break;
	}

	TextWriter_setHexMode( writer, oldHexMode );
	TextWriter_setLeftAlign( writer, oldAlignMode );
	TextWriter_setWidth( writer, oldWidth );
	return args;
}



// Public functions

bool KOut_write( const char* formatString, ... )
{
	KDebug_assertArg( formatString != NULL );

	va_list args;
	va_start( args, formatString );

	bool ret = KOut_vWrite( formatString, args );

	va_end( args );
	return ret;
}


bool KOut_writeLine( const char* formatString, ... )
{
	KDebug_assertArg( formatString != NULL );

	va_list args;
	va_start( args, formatString );

	bool ret = KOut_vWriteLine( formatString, args );

	va_end( args );
	return ret;
}


bool KOut_writeTo( TextWriter* writer, const char* formatString, ... )
{
	KDebug_assertArg( writer != NULL );
	KDebug_assertArg( formatString != NULL );

	va_list args;
	va_start( args, formatString );

	bool ret = KOut_vWriteTo( writer, formatString, args );

	va_end( args );
	return ret;
}


bool KOut_writeLineTo( TextWriter* writer, const char* formatString, ... )
{
	KDebug_assertArg( writer != NULL );
	KDebug_assertArg( formatString != NULL );

	va_list args;
	va_start( args, formatString );

	bool ret = KOut_vWriteLineTo( writer, formatString, args );

	va_end( args );
	return ret;
}


bool KOut_vWrite( const char* formatString, va_list args )
{
	KDebug_assertArg( formatString != NULL );
	TextWriter displayWriter = TextWriter_create( DisplayTextStream_getTextStream() );
	return KOut_vWriteTo( &displayWriter, formatString, args );
}


bool KOut_vWriteLine( const char* formatString, va_list args )
{
	KDebug_assertArg( formatString != NULL );
	TextWriter displayWriter = TextWriter_create( DisplayTextStream_getTextStream() );
	return KOut_vWriteLineTo( &displayWriter, formatString, args );
}


bool KOut_vWriteTo( TextWriter* writer, const char* formatString, va_list args )
{
	KDebug_assertArg( writer != NULL );
	KDebug_assertArg( formatString != NULL );

	const char* current	= formatString;

	// Set up the state machine.

	FormatSpec formatSpec;
	formatSpec.leftAlign	= false;
	formatSpec.width		= 0;
	formatSpec.modifier		= NO_MODIFIER;

	ParserState currentState = FOUND_NOTHING_STATE;

	while (currentState != FINAL_STATE)
	{
		ParserInputChar nextChar = KOut_convertInputChar( *current );

		ParserStateTransition transition = s_parserStates[currentState][nextChar];
		currentState = transition.nextState;

		switch (transition.action)
		{
		case SET_LEFTALIGN_ACTION:
			formatSpec.leftAlign = true;
			break;

		case SET_WIDTH_ACTION:
			formatSpec.width = va_arg( args, size_t );
			break;

		case SET_MODIFIER_ACTION:
			formatSpec.modifier = KOut_convertModifier( *current );
			break;

		case WRITE_OUTPUT_ACTION:
			args = KOut_writeNextArg( *current, &formatSpec, writer, args );

			// Reset everything for the next format specifier.
			formatSpec.leftAlign	= false;
			formatSpec.width		= 0;
			formatSpec.modifier		= NO_MODIFIER;
			break;

		case WRITE_FORMAT_CHAR_ACTION:
			TextWriter_writeChar( writer, *current );
			break;

		case NO_ACTION:
			; // Do nothing.
			break;

		case ABORT_ACTION:
			return false;	// Parse error -- stop dead.

		default:
			KDebug_assert( false );
			return false;
		}

		// Eat the just-read character.
		current++;

	} // end while

	// Flush whatever has been buffered.
	TextWriter_writeChar( writer, '\0' );
	return true;	// Fallthough -- everything completed ok.
}


bool KOut_vWriteLineTo( TextWriter* writer, const char* formatString, va_list args )
{
	KDebug_assertArg( writer != NULL );
	KDebug_assertArg( formatString != NULL );

	bool ret = KOut_vWriteTo( writer, formatString, args );
	TextWriter_writeChar( writer, '\n' );
	return ret;
}


