// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/HAL/InterruptController.h
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
/// \brief	This file defines the InterruptController class, which encapsulates
///			programmable interrupt controller functionality for a given Processor.
///
// ===========================================================================

#ifndef _KERNEL_HAL_INTERRUPTCONTROLLER_H_
#define _KERNEL_HAL_INTERRUPTCONTROLLER_H_


#include <stdint.h>


/// \brief	Forward declaration of the InterruptController object type.
typedef struct InterruptControllerStruct InterruptController;


/// \brief	Initializes the interrupt controller hardware for the current processor.
///
/// This method must be called once on each processor in the system as soon as possible during
/// kernel initialization. It must be called with interrupts disabled for the current processor.
void InterruptController_initForCurrentProcessor( void );


/// \brief	Gives access to the InterruptController for the current Processor.
///
/// InterruptController_initForCurrentProcessor() must be called on a processor before this method
/// is called on that processor.
///
/// \return an InterruptController* to use when calling other InterruptController functions.
volatile InterruptController* InterruptController_getForCurrentProcessor( void );


/// \brief	Disables the given IRQ.
///
/// \param controller	the InterruptController on which the IRQ will be disabled.
/// \param irqNumber	a non-negative integer indicating which IRQ to disable. The interpretation
///						of this value is architecture-specific.
///
/// This function disables exactly the IRQ specified without affecting the status of any other IRQs.
/// For example, if the interrupt controller hardware has a prioritization scheme in effect, calling
/// this function does not disable (or enable) any IRQs of lower priority than the one indicated by
/// the \a irqNumber parameter.
///
/// This function must be called with interrupts disabled on the current processor.
void InterruptController_mask( InterruptController* controller, uint32_t irqNumber );


/// \brief	Enables the given IRQ.
///
/// \param controller	the InterruptController on which the IRQ will be enabled
/// \param irqNumber	a non-negative integer indicating which IRQ to enable. The interpretation
///						of this value is architecture-specific.
///
/// This function enables exactly the IRQ specified without affecting the status of any other IRQs.
/// For example, if the interrupt controller hardware has a prioritization scheme in effect, calling
/// this function does not enable (or disable) any IRQs of lower priority than the one indicated by
/// the \a irqNumber parameter.
///
/// This function must be called with interrupts disabled on the current processor.
void InterruptController_unmask( InterruptController* controller, uint32_t irqNumber );


/// \brief	Signals to the interrupt controller that processing of the current IRQ is complete.
///
/// \param controller	the InterruptController to notify.
/// \param irqNumber	a non-negative integer indicating which IRQ is in service. The
///						interpretation of this value is architecture-specific.
///
/// On some architectures, the interrupt controller suppresses lower-priority interrupts while a
/// particular interrupt is being handled. It is up to the ISR executing on the processor to notify
/// the interrupt controller when it is ok to re-enable any lower-priority interrupts. This function
/// sends that notification, if applicable.
///
/// This function must be called with interrupts disabled on the current processor.
void InterruptController_endOfInterrupt( InterruptController* controller, uint32_t irqNumber );


#endif

