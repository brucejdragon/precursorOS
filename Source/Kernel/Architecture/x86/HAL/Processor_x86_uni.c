// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/HAL/Processor_x86_uni.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2004/Nov/01
//
// ===========================================================================
///
/// \file
///
/// \brief	Implements the Processor class specifically for uniprocessor x86
///			machines.
///
/// ***FIXME: lots to say.
///
// ===========================================================================


#include "Kernel/HAL/Processor.h"
#include "Kernel/KCommon/KMem.h"
#include "Kernel/KCommon/KDebug.h"
#include "Kernel/Architecture/x86/HAL/TrapFrame_x86.h"
#include "Kernel/Architecture/x86/HAL/ProtectedMode.h"



/// \brief	Resets the Processor in response to an interrupt.
///
/// \param this			ignored.
/// \param trapFrame	the trap frame created to capture the critical state of the running thread
///						at the time the interrupt occurred.
static TrapFrame* DefaultHandler_handleInterrupt( volatile void* this, TrapFrame* trapFrame )
{
	// MAINTENANCE NOTE: *DO NOT* assert in this handler, or you will cause nasty
	// re-entrancy chaos!
	(void) this;		// Ignored. This is effectively a static method.
	(void) trapFrame;	// Ignored. This handler doesn't do much.
	Processor_hardReset();
	return NULL;
}



/// \brief	Interface dispatch table for DefaultHandler's implementation of IInterruptHandler.
static IInterruptHandler_itable s_defaultHandler_itable = { DefaultHandler_handleInterrupt };



/// \brief	Gets the IInterruptHandler that is to be installed in the Processor's dispatch table
///			at initialization time.
static IInterruptHandler DefaultHandler_getHandler( void )
{
	IInterruptHandler handler;
	handler.obj		= NULL;
	handler.iptr	= &s_defaultHandler_itable;
	return handler;
}



/// \brief	Convenience macro for creating IDT entries for handlers that do not push an error
///			code on the stack.
#define CREATE_INT_HANDLER_IDT_ENTRY( proc, intrVec ) \
	(proc)->m_idt[(intrVec)].InterruptGate = \
		ProtectedMode_createInterruptGate( \
			(uint32_t) Int##intrVec##Handler, \
			KERNEL_CODESEG_SELECTOR \
		);


/// \brief	Convenience macro for creating IDT entries for handlers that push an error code on
///			the stack.
#define CREATE_ERROR_INT_HANDLER_IDT_ENTRY( proc, intrVec ) \
	(proc)->m_idt[(intrVec)].InterruptGate = \
		ProtectedMode_createInterruptGate( \
			(uint32_t) ErrorInt##intrVec##Handler, \
			KERNEL_CODESEG_SELECTOR \
		);



/// \brief	Defines file-local constants for the Processor class.
enum Processor_consts
{
	NUM_GDT_ENTRIES	= 6,	///< 1 for null selector, 1 for TSS, 4 for code + data ring 0 + 3.
	NUM_IDT_ENTRIES = 49,	///< 19 exceptions, NMI, 12 reserved, 16 IRQs, 1 syscall vector.
	NULL_GDT_INDEX	= 0,	///< Index of "null selector" in GDT.
	TSS_GDT_INDEX	= 1,	///< Index of global TSS in GDT.
	CS0_GDT_INDEX	= 2,	///< Index of kernel code segment in GDT.
	SS0_GDT_INDEX	= 3,	///< Index of kernel data segment in GDT.
	CS3_GDT_INDEX	= 4,	///< Index of user code segment in GDT.
	SS3_GDT_INDEX	= 5,	///< Index of user data segment in GDT.
};


/// \brief	Ring 0 code segment selector.
static const SegmentSelector KERNEL_CODESEG_SELECTOR = {.Seg = {0, 0, CS0_GDT_INDEX}};

/// \brief	Ring 0 data segment selector.
static const SegmentSelector KERNEL_DATASEG_SELECTOR = {.Seg = {0, 0, SS0_GDT_INDEX}};



/// \brief	Implementation of Processor.
struct ProcessorStruct
{
	IInterruptHandler	m_dispatchTable[NUM_IDT_ENTRIES];	///< Registered C interrupt handlers.
	IdtEntry			m_idt[NUM_IDT_ENTRIES];				///< Interrupt dispatch table.
	GdtEntry			m_gdt[NUM_GDT_ENTRIES];				///< Global descriptor table.
	// MAINTENANCE NOTE: The Intel manual says this should live in nicely page-aligned memory such
	// that the first 104 bytes are within a single page, and the rest of the TSS is physically
	// contiguous. Since this is part of the kernel's bss section, it is already guaranteed to
	// be physically contiguous, and the entire kernel must always be mapped into every address
	// space anyway. So, we ignore the manual's warning here.
	TaskStateSegment	m_tss;								///< TSS. Must be the last field!
};



/// \brief	The one-and-only instance of Processor, since this is for a uniprocessor kernel.
static volatile Processor s_instance;



// Private functions

// These functions are implemented in assembler. Their addresses are needed here to install in
// the IDT.
extern void Int0Handler( void );		///< Called by hardware in response to interrupt vector 0.
extern void Int1Handler( void );		///< Called by hardware in response to interrupt vector 1.
extern void Int2Handler( void );		///< Called by hardware in response to interrupt vector 2.
extern void Int3Handler( void );		///< Called by hardware in response to interrupt vector 3.
extern void Int4Handler( void );		///< Called by hardware in response to interrupt vector 4.
extern void Int5Handler( void );		///< Called by hardware in response to interrupt vector 5.
extern void Int6Handler( void );		///< Called by hardware in response to interrupt vector 6.
extern void Int7Handler( void );		///< Called by hardware in response to interrupt vector 7.
extern void ErrorInt8Handler( void );	///< Called by hardware in response to interrupt vector 8.
extern void Int9Handler( void );		///< Called by hardware in response to interrupt vector 9.
extern void ErrorInt10Handler( void );	///< Called by hardware in response to interrupt vector 10.
extern void ErrorInt11Handler( void );	///< Called by hardware in response to interrupt vector 11.
extern void ErrorInt12Handler( void );	///< Called by hardware in response to interrupt vector 12.
extern void ErrorInt13Handler( void );	///< Called by hardware in response to interrupt vector 13.
extern void ErrorInt14Handler( void );	///< Called by hardware in response to interrupt vector 14.
extern void Int15Handler( void );		///< Called by hardware in response to interrupt vector 15.
extern void Int16Handler( void );		///< Called by hardware in response to interrupt vector 16.
extern void ErrorInt17Handler( void );	///< Called by hardware in response to interrupt vector 17.
extern void Int18Handler( void );		///< Called by hardware in response to interrupt vector 18.
extern void Int19Handler( void );		///< Called by hardware in response to interrupt vector 19.
extern void Int20Handler( void );		///< Called by hardware in response to interrupt vector 20.
extern void Int21Handler( void );		///< Called by hardware in response to interrupt vector 21.
extern void Int22Handler( void );		///< Called by hardware in response to interrupt vector 22.
extern void Int23Handler( void );		///< Called by hardware in response to interrupt vector 23.
extern void Int24Handler( void );		///< Called by hardware in response to interrupt vector 24.
extern void Int25Handler( void );		///< Called by hardware in response to interrupt vector 25.
extern void Int26Handler( void );		///< Called by hardware in response to interrupt vector 26.
extern void Int27Handler( void );		///< Called by hardware in response to interrupt vector 27.
extern void Int28Handler( void );		///< Called by hardware in response to interrupt vector 28.
extern void Int29Handler( void );		///< Called by hardware in response to interrupt vector 29.
extern void Int30Handler( void );		///< Called by hardware in response to interrupt vector 30.
extern void Int31Handler( void );		///< Called by hardware in response to interrupt vector 31.
extern void Int32Handler( void );		///< Called by hardware in response to interrupt vector 32.
extern void Int33Handler( void );		///< Called by hardware in response to interrupt vector 33.
extern void Int34Handler( void );		///< Called by hardware in response to interrupt vector 34.
extern void Int35Handler( void );		///< Called by hardware in response to interrupt vector 35.
extern void Int36Handler( void );		///< Called by hardware in response to interrupt vector 36.
extern void Int37Handler( void );		///< Called by hardware in response to interrupt vector 37.
extern void Int38Handler( void );		///< Called by hardware in response to interrupt vector 38.
extern void Int39Handler( void );		///< Called by hardware in response to interrupt vector 39.
extern void Int40Handler( void );		///< Called by hardware in response to interrupt vector 40.
extern void Int41Handler( void );		///< Called by hardware in response to interrupt vector 41.
extern void Int42Handler( void );		///< Called by hardware in response to interrupt vector 42.
extern void Int43Handler( void );		///< Called by hardware in response to interrupt vector 43.
extern void Int44Handler( void );		///< Called by hardware in response to interrupt vector 44.
extern void Int45Handler( void );		///< Called by hardware in response to interrupt vector 45.
extern void Int46Handler( void );		///< Called by hardware in response to interrupt vector 46.
extern void Int47Handler( void );		///< Called by hardware in response to interrupt vector 47.
extern void Int48Handler( void );		///< Called by hardware in response to interrupt vector 48.


// The following functions cannot be static because they are called from assembler.

/// \brief	Called by hardware whenever the kernel is entered via an interrupt or exception.
///
/// \param processor	the processor that raised the interrupt or exception.
/// \param trapFrame	the state of the interrupted procedure on the current kernel stack.
///
/// This function finds and invokes the registered handler for the vector of the interrupt or
/// exception that occurred. The handler is passed \a trapFrame to give it access to the
/// interrupted program state. The handler has the option of switching contexts by returning the
/// address of a different TrapFrame. If this happens, this function ensures that a stack switch
/// to the new context will occur when it returns. It also ensures that if the new context is
/// for a user-mode thread, the ESP0 field of the processor's TSS is initialized to point to the
/// bottom of the new kernel stack. This must be done so that the next switch from user-mode to
/// kernel-mode will result in a switch to the correct kernel stack.
TrapFrame* Processor_dispatchToHandler( Processor* processor, TrapFrame* trapFrame )
{
	// MAINTENANCE NOTE:
	// It is unwise to assert inside this function, since it will re-enter in response to the
	// breakpoint exception raised by the assertion.

	// Get the handler object and call it.
	IInterruptHandler handler = processor->m_dispatchTable[trapFrame->interruptVectorNumber];

	TrapFrame* newFrame = handler.iptr->handleInterrupt( handler.obj, trapFrame );

	if (newFrame == NULL)	// No context switch happening...
	{
		// The asm code will switch stacks after this function returns. Make sure we use the
		// current stack for iret (which means it basically switches the current stack to itself,
		// effectively not switching at all, which is what we want).
		return trapFrame;
	}
	else
	{
		// The handler has returned, indicating that a context switch is in progress.
		if (!TrapFrame_isKernelInterrupted( newFrame ))
		{
			// We are switching contexts to user mode. This means we must update the TSS
			// to point to the "bottom" (i.e. -- highest address) of the new kernel stack.
			processor->m_tss.ESP0 = ((size_t) newFrame) + sizeof(TrapFrame);
		}

		// The asm code will switch stacks after this function returns. Make sure we use the new
		// stack for iret.
		return newFrame;
	}
}


/// \brief	Initializes the given processor's GDT.
///
/// \param processor	the processor being initialized.
///
/// This function creates GDT entries containing the following:
///  - A selector for the processor's TSS.
///  - Code segment descriptors for the ring 0 and ring 3 flat address spaces.
///	 - Data segment descriptors for the ring 0 and ring 3 flat address spaces.
/// .
GdtEntry* Processor_initGdt( Processor* processor )
{
	// MAINTENANCE NOTE:
	// It is unwise to assert inside this function, since it is called very early in kernel
	// initialization (i.e. -- before a breakpoint handler can been properly invoked).

	KMem_set( processor->m_gdt, 0, NUM_GDT_ENTRIES * sizeof( GdtEntry ) );

	processor->m_gdt[TSS_GDT_INDEX].Tss = ProtectedMode_createTssDescriptor( &(processor->m_tss) );
	processor->m_gdt[CS0_GDT_INDEX].CodeSeg	= ProtectedMode_createCodeSegment( true );
	processor->m_gdt[SS0_GDT_INDEX].DataSeg	= ProtectedMode_createDataSegment( true );
	processor->m_gdt[CS3_GDT_INDEX].CodeSeg	= ProtectedMode_createCodeSegment( false );
	processor->m_gdt[SS3_GDT_INDEX].DataSeg	= ProtectedMode_createDataSegment( false );

	return processor->m_gdt;
}


/// \brief	Initializes the given processor's IDT.
///
/// \param processor	the processor being initialized.
///
/// This function creates an IDT entry for each of the assembly-language stubs.
IdtEntry* Processor_initIdt( Processor* processor )
{
	// MAINTENANCE NOTE:
	// It is unwise to assert inside this function, since it is called very early in kernel
	// initialization (i.e. -- before a breakpoint handler can been properly invoked).

	KMem_set( processor->m_idt, 0, NUM_IDT_ENTRIES * sizeof( IdtEntry ) );

	CREATE_INT_HANDLER_IDT_ENTRY( processor, 0 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 1 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 2 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 3 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 4 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 5 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 6 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 7 );
	CREATE_ERROR_INT_HANDLER_IDT_ENTRY( processor, 8 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 9 );
	CREATE_ERROR_INT_HANDLER_IDT_ENTRY( processor, 10 );
	CREATE_ERROR_INT_HANDLER_IDT_ENTRY( processor, 11 );
	CREATE_ERROR_INT_HANDLER_IDT_ENTRY( processor, 12 );
	CREATE_ERROR_INT_HANDLER_IDT_ENTRY( processor, 13 );
	CREATE_ERROR_INT_HANDLER_IDT_ENTRY( processor, 14 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 15 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 16 );
	CREATE_ERROR_INT_HANDLER_IDT_ENTRY( processor, 17 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 18 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 19 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 20 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 21 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 22 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 23 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 24 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 25 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 26 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 27 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 28 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 29 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 30 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 31 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 32 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 33 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 34 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 35 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 36 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 37 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 38 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 39 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 40 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 41 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 42 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 43 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 44 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 45 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 46 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 47 );
	CREATE_INT_HANDLER_IDT_ENTRY( processor, 48 );

	return processor->m_idt;
}


/// \brief	Initializes the given processor's Task State Segment (TSS).
///
/// \param processor	the processor being initialized.
///
/// This function does a very minimal job of initializing the TSS, on the assumption that it will
/// represent the only task in the system (i.e. -- the OS will use software task switching). This
/// means that this function can, for example, omit initialization of the CR3 field of the TSS.
/// This is fine because this TSS will never be switched to, since it will never be switched away
/// from in the first place.
///
/// This function also omits initialization of the ESP0 field. This is OK because ESP0 will be
/// initialized by dispatchToHandler() as it prepares to switch to the primeval thread. At that
/// time, ESP0 will be initialized to point to the bottom of the primeval thread's kernel stack.
/// There is no point in initializing ESP0 in this function, since the only value to give it at
/// this early stage of initialization would be the address of the bottom of the initial kernel
/// stack, which is never used again once the primeval thread starts running. In other words, it
/// is not possible for a switch from user-mode to kernel-mode to take place that would require
/// switching to the initial kernel stack. This is what ESP0 is for, so it is unnecessary to
/// initialize it at this early stage.
///
/// \note
/// This function returns an integer type rather than a SegmentSelector for easier integration
///	with assembler code. The default protocol for returning structs by value is somewhat complex.
///
/// \return the 16-bit selector for the processor's TSS, as an unsigned 16-bit integer.
uint16_t Processor_initTss( Processor* processor )
{
	// MAINTENANCE NOTE:
	// It is unwise to assert inside this function, since it is called very early in kernel
	// initialization (i.e. -- before a breakpoint handler can be properly invoked).

	KMem_set( &(processor->m_tss), 0, sizeof( TaskStateSegment ) );

	// Leave I/O permission map base address, CR3, ESP0, and everything else as 0 for now.
	// ASSUMPTION: This is the only TSS in the system, so it's ok. In the case of ESP0, it will
	// be initialized when switching to the primeval thread.
	processor->m_tss.SS0 = KERNEL_CODESEG_SELECTOR;

	SegmentSelector tssSelector;
	tssSelector.Seg.RPL				= 0;
	tssSelector.Seg.TableIndicator	= 0;	// GDT.
	tssSelector.Seg.Index			= TSS_GDT_INDEX;

	return tssSelector.RawValue;
}


/// \brief	Called by Processor_initPrimary() to insure that the IInterruptHandler dispatch table
///			is initialized to a known state.
///
/// \param processor	the Processor whose dispatch table will be initialized.
void Processor_initDispatchTable( Processor* processor )
{
	IInterruptHandler defaultHandler = DefaultHandler_getHandler();

	// Make sure that the rest of the kernel doesn't forget to register its handlers. The default
	// handler installed here resets the system.
	for (size_t i = 0; i < NUM_IDT_ENTRIES; i++)
	{
		processor->m_dispatchTable[i] = defaultHandler;
	}
}



// Public functions.

// NOTE: Processor_initPrimary(), Processor_halt(), Processor_hardReset(), and
// Processor_triggerDebugTrap() are implemented in assembler.

volatile Processor* Processor_getCurrent( void )
{
	// There's only one processor, so it must be the current one.
	return &s_instance;
}


int Processor_getID( const volatile Processor* processor )
{
	// Check the parameter, but otherwise ignore it.
	KDebug_assertArg( processor != NULL );
	(void) processor;

	// This a UP system, so the ID is always the same.
	return 0;
}


void Processor_registerHandler(
	Processor*			processor,
	IInterruptHandler	handler,
	uint32_t			intrVector
)
{
	KDebug_assertArg( processor != NULL );
	KDebug_assertArg( intrVector < NUM_IDT_ENTRIES );

	processor->m_dispatchTable[intrVector] = handler;
}


