// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/Executive/WritableTrapFrame_x86.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Apr/24
//
// ===========================================================================
///
///	\file
///
/// \brief	Contains the implementation of WritableTrapFrame for the x86 architecture.
///
// ===========================================================================


#include <stddef.h>
#include <stdint.h>
#include "Kernel/Architecture/x86/HAL/ProtectedMode.h"
#include "Kernel/Architecture/x86/HAL/TrapFrame_x86.h"
#include "Kernel/Architecture/x86/HAL/PrecursorVectors_x86.h"
#include "Kernel/KRunTime/KOut.h"
#include "Kernel/KCommon/KDebug.h"
#include "WritableTrapFrame.h"


// Private functions

/// \brief	Writes a debugging-friendly representation of the given interrupt vector number to
///			the given TextWriter.
///
/// \param intrVectorNumber	the interrupt vector number to output.
/// \param writer			the TextWriter to which output is sent.
static void WritableTrapFrame_writeInterruptVectorNumberTo(
	uint32_t	intrVectorNumber,
	TextWriter*	writer
)
{
	KOut_writeTo( writer, "Interrupt %d: ", intrVectorNumber );
	switch (intrVectorNumber)
	{
	case INT0_DE_DIVIDE_ERROR:
		KOut_writeTo( writer, "Divide Error (#DE)" );
		break;

	case INT1_DB_RESERVED:
		KOut_writeTo( writer, "Debug Exception (#DB)" );
		break;

	case INT2_NMI_INTERRUPT:
		KOut_writeTo( writer, "NMI Interrupt" );
		break;

	case INT3_BP_BREAKPOINT:
		KOut_writeTo( writer, "Breakpoint Exception (#BP)" );
		break;

	case INT4_OF_OVERFLOW:
		KOut_writeTo( writer, "Overflow Exception (#OF)" );
		break;

	case INT5_BR_BOUND_RANGE_EXCEEDED:
		KOut_writeTo( writer, "BOUND Range Exceeded Exception (#BR)" );
		break;

	case INT6_UD_INVALID_OPCODE:
		KOut_writeTo( writer, "Invalid Opcode Exception (#UD)" );
		break;

	case INT7_NM_NO_MATH_COPROCESSOR:
		KOut_writeTo( writer, "Device Not Available Exception (#NM)" );
		break;

	case INT8_DF_DOUBLE_FAULT:
		KOut_writeTo( writer, "Double Fault Exception (#DF)" );
		break;

	case INT9_CO_PROCESSOR_SEG_OVERRUN:
		KOut_writeTo( writer, "Coprocessor Segment Overrun" );
		break;

	case INT10_TS_INVALID_TSS:
		KOut_writeTo( writer, "Invalid TSS Exception (#TS)" );
		break;

	case INT11_NP_SEG_NOT_PRESENT:
		KOut_writeTo( writer, "Segment Not Present (#NP)" );
		break;

	case INT12_SS_STACK_SEG_FAULT:
		KOut_writeTo( writer, "Stack Fault Exception (#SS)" );
		break;

	case INT13_GP_GENERAL_PROTECTION:
		KOut_writeTo( writer, "General Protection Exception (#GP)" );
		break;

	case INT14_PF_PAGE_FAULT:
		KOut_writeTo( writer, "Page-Fault Exception (#PF)" );
		break;

	case INT16_MF_X87_MATH_FAULT:
		KOut_writeTo( writer, "x87 FPU Floating-Point Error (#MF)" );
		break;

	case INT17_AC_ALIGNMENT_CHECK:
		KOut_writeTo( writer, "Alignment Check Exception (#AC)" );
		break;

	case INT18_MC_MACHINE_CHECK:
		KOut_writeTo( writer, "Machine-Check Exception (#MC)" );
		break;

	case INT19_XF_SIMD_FP_EXCEPTION:
		KOut_writeTo( writer, "SIMD Floating-Point Exception (#XF)" );
		break;

	case INT15_RESERVED:
	case INT20_RESERVED:
	case INT21_RESERVED:
	case INT22_RESERVED:
	case INT23_RESERVED:
	case INT24_RESERVED:
	case INT25_RESERVED:
	case INT26_RESERVED:
	case INT27_RESERVED:
	case INT28_RESERVED:
	case INT29_RESERVED:
	case INT30_RESERVED:
	case INT31_RESERVED:
		KOut_writeTo( writer, "Reserved" );
		break;

	case INT_HW_IRQ0:
	case INT_HW_IRQ1:
	case INT_HW_IRQ2:
	case INT_HW_IRQ3:
	case INT_HW_IRQ4:
	case INT_HW_IRQ5:
	case INT_HW_IRQ6:
	case INT_HW_IRQ7:
	case INT_HW_IRQ8:
	case INT_HW_IRQ9:
	case INT_HW_IRQ10:
	case INT_HW_IRQ11:
	case INT_HW_IRQ12:
	case INT_HW_IRQ13:
	case INT_HW_IRQ14:
	case INT_HW_IRQ15:
		KOut_writeTo( writer, "Hardware Interrupt (IRQ %d)", intrVectorNumber - INT_HW_IRQ0 );
		break;

	case INT_SYS_CALL:
		KOut_writeTo( writer, "System Call Vector" );
		break;

	default:
		KOut_writeTo( writer, "Unrecognized Interrupt Vector" );
	}
}


/// \brief	Writes the contents of the given TrapFrame to the given TextWriter.
///
/// \param trapFrame	the TrapFrame to output.
/// \param writer		the TextWriter to which output is sent.
static void WritableTrapFrame_writeTo( const TrapFrame* trapFrame, TextWriter* writer )
{
	static const int GPR_WIDTH = 15;
	static const int SEG_WIDTH = 8;

	WritableTrapFrame_writeInterruptVectorNumberTo( trapFrame->interruptVectorNumber, writer );
	KOut_writeTo(
		writer,
		"\n\nEAX: %-*lx\tEDI: %-*lx\nEBX: %-*lx\tESI: %-*lx\nECX: %-*lx\tEBP: %-*lx",
		GPR_WIDTH,
		trapFrame->eax,
		GPR_WIDTH,
		trapFrame->edi,
		GPR_WIDTH,
		trapFrame->ebx,
		GPR_WIDTH,
		trapFrame->esi,
		GPR_WIDTH,
		trapFrame->ecx,
		GPR_WIDTH,
		trapFrame->ebp
	);
	KOut_writeTo(
		writer,
		"\nEDX: %-*lx\tESP: %-*lx\nEIP: %-*lx\tEFLAGS: %-*lx\nCR2: %-*lx",
		GPR_WIDTH,
		trapFrame->edx,
		GPR_WIDTH,
		trapFrame->esp,
		GPR_WIDTH,
		trapFrame->eip,
		GPR_WIDTH,
		*((uint32_t*) &trapFrame->eflags),
		GPR_WIDTH,
		trapFrame->cr2
	);

	KOut_writeTo(
		writer,
		"\n\nCS: %-*hx\tDS: %-*hx\tES: %-*hx\nFS: %-*hx\tGS: %-*hx",
		SEG_WIDTH,
		trapFrame->cs,
		SEG_WIDTH,
		trapFrame->ds,
		SEG_WIDTH,
		trapFrame->es,
		SEG_WIDTH,
		trapFrame->fs,
		SEG_WIDTH,
		trapFrame->gs
	);
	KOut_writeTo( writer, "\n\nError code: %-*lx\n", GPR_WIDTH, trapFrame->errorCode.RawValue );

	// There will be extra fields to print out if the trap came from user space.
	if (!TrapFrame_isKernelInterrupted( trapFrame ))
	{
		KOut_writeTo(
			writer,
			"\nRing 3 SS: %-*hx\nRing 3 ESP: %-*lx\n",
			SEG_WIDTH,
			trapFrame->ss3,
			GPR_WIDTH,
			trapFrame->esp3
		);
	}
}



/// \brief	Interface dispatch table for WritableTrapFrame's implementation of ITextWritable.
static ITextWritable_itable s_itable =
{
	(ITextWritable_writeToFunc) WritableTrapFrame_writeTo
};



// Public functions

ITextWritable WritableTrapFrame_getAsTextWritable( const TrapFrame* trapFrame )
{
	KDebug_assertArg( trapFrame != NULL );

	ITextWritable writable;
	writable.obj	= (void*) trapFrame;	// Point to the given TrapFrame.
	writable.iptr	= &s_itable;			// Point at the right interface dispatch table.
	return writable;
}


