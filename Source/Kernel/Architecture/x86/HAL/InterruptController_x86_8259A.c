// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/Architecture/x86/HAL/InterruptController_x86_8259A.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/May/29
//
// ===========================================================================
///
/// \file
///
/// \brief	This file defines the InterruptController class for the x86 UP
///			architecture. It encapsulates a pair of Intel 8259A-compatible PICs
///			chained together (in other words, the norm for AT-compatible PCs).
///
// ===========================================================================


#include <stddef.h>
#include <stdint.h>
#include "IO.h"
#include "Kernel/HAL/InterruptController.h"
#include "Kernel/KCommon/KDebug.h"
#include "Kernel/KCommon/KMem.h"
#include "Kernel/Architecture/x86/HAL/PrecursorVectors_x86.h"	// For INT_HW_IRQ0.


/// \brief	Declaration of the InterruptController object type.
struct InterruptControllerStruct
{
	// Empty for now.
};



/// \brief	There is only one pair of PICs in an x86 UP system...
static volatile InterruptController s_instance;


/// \brief	Defines local constants for the InterruptController class.
enum InterruptController_consts
{
	NUM_IRQS_PER_PIC	= 8,					///< Number of IRQs handled per 8259A PIC.
	NUM_IRQS			= NUM_IRQS_PER_PIC * 2,	///< Total # IRQs. Used to sanity-check parameters.
	PIC_MASTER_COMMAND	= 0x0020,	///< Master PIC command port.
	PIC_SLAVE_COMMAND	= 0x00A0,	///< Slave PIC command port.
	PIC_MASTER_DATA		= 0x0021,	///< Master PIC data port.
	PIC_SLAVE_DATA		= 0x00A1,	///< Slave PIC data port.
	PIC_EOI				= 0x20,		///< Signals EOI to the PIC.
	CHAINED_IRQ			= 0x02,		///< IRQ2 is connected to the slave PIC.
	ICW1_ICW4			= 0x01,		///< 1 = ICW4 needed; 0 otherwise.
	ICW1_INIT			= 0x10,		///< Initialization - required!
	ICW4_8086			= 0x01		///< 1 = 8086/88 mode, 0 = MCS-80/85 mode.
};



// Private functions.

/// \brief	Helper function that determines which PIC I/O port and PIC-relative IRQ correspond
///			to the given IRQ number.
///
/// \param	irqNumber		an IRQ number between 0 and 15, inclusive.
/// \param	port			output parameter that receives the I/O port address of the correct PIC
///							to use (either the master or the slave).
/// \param	picRelativeIrq	output parameter that receives an IRQ number between 0 and 7. This is
///							the equivalent of \a irqNumber, but specific to either the master or
///							slave PIC.
static inline void InterruptController_choosePortAndPicRelativeIrq(
	uint32_t	irqNumber,		// in
	uint16_t*	port,			// out
	uint8_t*	picRelativeIrq	// out
)
{
	KDebug_assertArg( irqNumber < NUM_IRQS );
	KDebug_assertArg( port != NULL );
	KDebug_assertArg( picRelativeIrq != NULL );

	if (irqNumber < NUM_IRQS_PER_PIC)
	{
		*port = PIC_MASTER_DATA;
		*picRelativeIrq = (uint8_t) irqNumber;
	}
	else
	{
		*port = PIC_SLAVE_DATA;
		*picRelativeIrq = (uint8_t) (irqNumber - NUM_IRQS_PER_PIC);
	}
}



// Public functions.

void InterruptController_initForCurrentProcessor( void )
{
	// Mask all IRQs.
	IO_out8( PIC_MASTER_DATA, 0xFF );
	IO_out8( PIC_SLAVE_DATA, 0xFF );

	// Send ICW1 -- tell the PICs to expect ICW4 later.
	IO_out8( PIC_MASTER_COMMAND, ICW1_INIT | ICW1_ICW4 );
	IO_out8( PIC_SLAVE_COMMAND, ICW1_INIT | ICW1_ICW4 );

	// Send ICW2 -- vector numbers.
	IO_out8( PIC_MASTER_DATA, INT_HW_IRQ0 );
	IO_out8( PIC_SLAVE_DATA, INT_HW_IRQ0 + NUM_IRQS_PER_PIC );

	// Send ICW3 -- IRQ2 chaining business.
	IO_out8( PIC_MASTER_DATA, KMem_bitSet8( 0, CHAINED_IRQ ) );
	IO_out8( PIC_SLAVE_DATA, CHAINED_IRQ );

	// Send ICW4 -- 8086 mode.
	IO_out8( PIC_MASTER_DATA, ICW4_8086 );
	IO_out8( PIC_SLAVE_DATA, ICW4_8086 );

	// Unmask all IRQs.
	IO_out8( PIC_MASTER_DATA, 0 );
	IO_out8( PIC_SLAVE_DATA, 0 );
}


volatile InterruptController* InterruptController_getForCurrentProcessor( void )
{
	return &s_instance;
}


void InterruptController_mask( InterruptController* controller, uint32_t irqNumber )
{
	(void) controller;	// Ignore -- no extra state needed.

	uint16_t port;
	uint8_t picRelativeIrq;

	InterruptController_choosePortAndPicRelativeIrq( irqNumber, &port, &picRelativeIrq );

	uint8_t mask = IO_in8( port );
	IO_out8( port, KMem_bitSet8( mask, picRelativeIrq ) );
}


void InterruptController_unmask( InterruptController* controller, uint32_t irqNumber )
{
	(void) controller;	// Ignore -- no extra state needed.

	uint16_t port;
	uint8_t picRelativeIrq;

	InterruptController_choosePortAndPicRelativeIrq( irqNumber, &port, &picRelativeIrq );

	uint8_t mask = IO_in8( port );
	IO_out8( port, KMem_bitClear8( mask, picRelativeIrq ) );
}


void InterruptController_endOfInterrupt( InterruptController* controller, uint32_t irqNumber )
{
	(void) controller;	// Ignore -- no extra state needed.

	KDebug_assertArg( irqNumber < NUM_IRQS );

	// Send EOI to the master.
	IO_out8( PIC_MASTER_COMMAND, PIC_EOI );

	if (irqNumber >= NUM_IRQS_PER_PIC)
	{
		// Send EOI to the slave.
		IO_out8( PIC_MASTER_COMMAND, PIC_EOI );
	}
}


