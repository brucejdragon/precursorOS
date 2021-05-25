// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/HAL/Processor.h
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
/// \brief	Defines the Processor class, which encapsulates the architecture-
///			specific and machine-specific details of the CPU hardware.
///
/// The intention of this class is to isolate CPU details from the rest of the
/// kernel, when it is likely that these details will change from machine to
/// machine. For example, CPU initialization and processor-local storage
/// will differ greatly between uniprocessor and multiprocessor implementations
/// on the same architecture.
///
/// The services provided by the Processor class include the following:
/// - Primary (bootstrap) processor initialization.
/// - Secondary processor initialization (MP only).
/// - Inter-processor communication via IPIs (MP only).
/// - Processor-local storage.
/// - Flexible interrupt, exception, and system call dispatching.
/// - Basic context switching (kernel stack & register switching).
/// .
///
// ===========================================================================

#ifndef _KERNEL_HAL_PROCESSOR_H_
#define _KERNEL_HAL_PROCESSOR_H_


#include <stdint.h>
#include "Kernel/HAL/TrapFrame.h"
#include "Kernel/HAL/IInterruptHandler.h"


/// \brief	Forward declaration of the Processor object type.
typedef struct ProcessorStruct Processor;


/// \brief	Called by the kernel's entry-point code to intialize the bootstrap processor.
///
/// This function is the first function to be called in the kernel. It is called even before main().
/// It is responsible for initializing the bootstrap processor for normal operation. This includes
/// initialization of the MMU and any hardware structures pertaining to memory management and
/// interrupt/exception dispatching. It also includes any processor state required to support the
/// kernel's C language run-time environment.
///
/// \note
/// This function is guaranteed to be called with interrupts disabled. In MP systems, all other
/// processors will be spinning while this function executes. This function can therefore safely
/// access any shared resource.
void Processor_initPrimary( void );


/// \brief	Returns a pointer to the Processor executing the current thread.
volatile Processor* Processor_getCurrent( void );


/// \brief	Enables interrupts on the current processor.
void Processor_enableInterrupts( void );


/// \brief	Disables interrupts on the current processor.
void Processor_disableInterrupts( void );


/// \brief	Returns \c true if interrupts are disabled on the current processor;
///			\c false otherwise.
bool Processor_areInterruptsDisabled( void );


/// \brief	Halts the current processor.
///
/// This function is guaranteed never to return. It disables interrupts before halting the
/// processor.
void Processor_halt( void );


/// \brief	Forces the current processor to reset, possibly in an inelegant manner.
void Processor_hardReset( void );


/// \brief	Halts the current processor until an interrupt occurs.
///
/// This function halts the processor until an interrupt occurs. This function returns after the
/// handler for that interrupt has finished executing.
void Processor_waitForInterrupt( void );


/// \brief	Gets the ID of the given processor.
///
/// \param processor	the Processor for which to get the ID.
///
/// ***FIXME: Describe MP ID scheme here.
///
/// \note
/// On UP systems, the ID of the one-and-only processor is always 0.
///
/// \return the ID of \a processor.
int Processor_getID( const volatile Processor* processor );


/// \brief	Registers a handler with the Processor that will be called when the given interrupt
///			occurs.
///
/// \param processor	the Processor for which to register the handler.
/// \param handler		the handler object to call when the interrupt occurs.
/// \param intrVector	the vector number of the interrupt that should trigger the handler.
///
/// If the given interrupt vector is out of range, the kernel will initiate a system failure in
/// checked builds.
///
/// This method must be called with interrupts disabled on the current processor.
void Processor_registerHandler(
	Processor*			processor,
	IInterruptHandler	handler,
	uint32_t			intrVector
);


#endif

