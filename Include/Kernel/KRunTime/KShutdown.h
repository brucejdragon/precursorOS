// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/KRunTime/KShutdown.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Apr/06
//
// ===========================================================================
///
/// \file
///
/// \brief	Implements the KShutdown class, which provides a means to halt or
///			reboot the machine and optionally display diagnostics.
///
/// All methods of this class are protected from concurrent access except for
/// KShutdown_init(). Note that if multiple processors attempt to call one of
/// KShutdown_fail(), KShutdown_halt(), or KShutdown_reboot(), then one of them
/// will succeed and put the KShutdown instance (and therefore the kernel
/// itself) into shutdown mode. The implementation of KShutdown on MP systems
/// guarantees that all other processors will be halted once the kernel enters
/// shutdown mode. This means that any other processor concurrently calling
/// one of the above methods will wait for interrupts in an infinite loop upon
/// discovering that the system is in shutdown mode. Soon after the calling
/// processor initiates shutdown mode, all other processors in the system
/// will receive an IPI instructing them to shut themselves down. If through
/// a bug in the kernel the processor that initiated shutdown mode ends up
/// re-entering one of the above methods, the system will hard reset.
///
/// This class assumes that the kernel display is not under kernel control at
/// the time it enters shutdown mode. Therefore, once it is certain that all
/// other processors have been halted, this class will reset the kernel
/// display by calling DisplayTextStream_reset().
///
/// Once in shutdown mode, this class uses the kernel display without any
/// synchronization. This is to avoid possible deadlock in an MP system. See
/// DisplayTextStream_getTextStream() for more details.
// ===========================================================================

#ifndef _KERNEL_KRUNTIME_KSHUTDOWN_H_
#define _KERNEL_KRUNTIME_KSHUTDOWN_H_


#include <stdbool.h>


/// \brief	Forward declaration of the KShutdown object type.
typedef struct KShutdownStruct KShutdown;


/// \brief	Initializes the KShutdown singleton.
///
/// This function must be called before KShutdown_getInstance() can be called. It initializes
/// the properties of the KShutdown instance to their default values.
///
/// This function should be called exactly once during second-phase bootup (i.e. -- when paging
/// is enabled and the C language environment of the kernel is already initialized properly).
void KShutdown_init( void );


/// \brief	Returns a pointer to the one-and-only KShutdown object.
///
/// \note
/// This method is safe to call from multiple processors concurrently.
///
/// \return a KShutdown* to be used when calling other KShutdown functions.
volatile KShutdown* KShutdown_getInstance( void );


/// \brief	Indicates whether the system is in shutdown mode.
///
/// \param kshutdown	the KShutdown instance managing kernel shut-down.
///
/// The system enters shutdown mode the first time one of the shutdown methods are called. The
/// shutdown methods include KShutdown_fail(), KShutdown_halt(), and KShutdown_reboot().
///
/// \note
/// This method is safe to call from multiple processors concurrently.
///
/// \retval true	one of the shutdown methods has been called already by either this processor
///					or another processor.
/// \retval false	none of the shutdown methods have been called yet.
bool KShutdown_isInShutdownMode( const volatile KShutdown* kshutdown );


/// \brief	Indicates whether the machine will reboot or halt when KShutdown_fail() is called.
///
/// \param kshutdown	the KShutdown instance managing kernel shut-down.
///
/// This method will return \c true by default after the KShutdown is first initialized.
///
/// \note
/// This method is safe to call from multiple processors concurrently.
///
/// \retval true	the machine will reboot on KShutdown_fail().
/// \retval false	all processors will halt on KShutdown_fail(), but the machine will not reboot.
bool KShutdown_isRebootOnFailEnabled( const volatile KShutdown* kshutdown );


/// \brief	Specifies whether the machine will reboot or halt when KShutdown_fail() is called.
///
/// \param kshutdown	the KShutdown instance managing kernel shut-down.
/// \param rebootOnFail	when \c true, the machine will reboot on KShutdown_fail(); Otherwise,
///						all processors will halt.
///
/// \note
/// This method is safe to call from multiple processors concurrently.
void KShutdown_setRebootOnFailEnabled( volatile KShutdown* kshutdown, bool rebootOnFail );


/// \brief	Gets the length of the delay in milliseconds before the machine will reboot if
///			KShutdown_reboot() or KShutdown_fail() are called.
///
/// \param kshutdown	the KShutdown instance managing kernel shut-down.
///
/// This method will return 10000 by default after the KShutdown is first initialized.
///
/// \note
/// This method is safe to call from multiple processors concurrently.
///
/// \returns the delay in milliseconds.
int KShutdown_getRebootDelayInMilliseconds( const volatile KShutdown* kshutdown );


/// \brief	Sets the length of the delay in milliseconds before the machine will reboot if
///			KShutdown_reboot() or KShutdown_fail() are called.
///
/// \param kshutdown			the KShutdown instance managing kernel shut-down.
/// \param delayInMilliseconds	the delay in milliseconds. Must be non-negative.
///
/// \note
/// This method is safe to call from multiple processors concurrently.
void KShutdown_setRebootDelayInMilliseconds(
	volatile KShutdown*	kshutdown,
	int					delayInMilliseconds
);


/// \brief	Instructs the kernel to enter shutdown mode, formats and outputs the given text on
///			the kernel display, and either reboots or halts the machine.
///
/// \param kshutdown	the KShutdown instance managing kernel shut-down.
/// \param formatString	the format string to use for output to the kernel display. If it contains
///						any format specifiers, then corresponding parameters of the correct
///						number and type must be supplied by the caller. See KOut for details on
///						the allowed format specifiers and special characters.
///
/// Whether this method causes the machine to halt or reboot can be specified by calling
/// KShutdown_setRebootOnFailEnabled(). The delay before a reboot can be specified by calling
/// KShutdown_setRebootDelayInMilliseconds().
///
/// \note
/// This method is safe to call from multiple processors concurrently. If a processor calls this
/// method after shutdown mode has already been entered, it will simply wait in an infinite
/// loop for the IPI that will "offically" instruct it to shut down. If by some unfortunate
/// circumstance the processor that entered shutdown mode ends up re-entering KShutdown through
/// this method, it will hard reset the system immediately.
void KShutdown_fail( volatile KShutdown* kshutdown, const char* formatString, ... );


/// \brief	Instructs the kernel to enter shutdown mode, outputs a stock message to the kernel
///			display, and halts all processors in the system.
///
/// \param kshutdown	the KShutdown instance managing kernel shut-down.
///
/// \note
/// This method is safe to call from multiple processors concurrently. If a processor calls this
/// method after shutdown mode has already been entered, it will simply wait in an infinite
/// loop for the IPI that will "offically" instruct it to shut down. If by some unfortunate
/// circumstance the processor that entered shutdown mode ends up re-entering KShutdown through
/// this method, it will hard reset the system immediately.
void KShutdown_halt( volatile KShutdown* kshutdown );


/// \brief	Instructs the kernel to enter shutdown mode, outputs a stock message to the kernel
///			display, and reboots the system, possibly after a certain delay.
///
/// \param kshutdown	the KShutdown instance managing kernel shut-down.
///
/// The delay before a reboot can be specified by calling KShutdown_setRebootDelayInMilliseconds().
/// The delay performed by this function may not be very accurate. It does not use any timer
/// interrupts, and must therefore use less accurate methods. The actual mechanism is
/// machine-specific.
///
/// \note
/// This method is safe to call from multiple processors concurrently. If a processor calls this
/// method after shutdown mode has already been entered, it will simply wait in an infinite
/// loop for the IPI that will "offically" instruct it to shut down. If by some unfortunate
/// circumstance the processor that entered shutdown mode ends up re-entering KShutdown through
/// this method, it will hard reset the system immediately.
void KShutdown_reboot( volatile KShutdown* kshutdown );


#endif

