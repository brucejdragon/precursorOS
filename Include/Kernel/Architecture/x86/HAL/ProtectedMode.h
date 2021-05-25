// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/Architecture/x86/HAL/ProtectedMode.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Jan/01
//
// ===========================================================================
///
/// \file
///
/// \brief	This file defines all the x86 protected-mode data structures and
///			system register fields used by the Precursor OS. It also defines
///			inline utility functions for manipulating these data structures.
///
// ===========================================================================

#ifndef _KERNEL_ARCH_X86_HAL_PROTECTEDMODE_H_
#define _KERNEL_ARCH_X86_HAL_PROTECTEDMODE_H_


#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "Kernel/KCommon/KMem.h"
#include "Kernel/KCommon/KDebug.h"

#ifdef __GNUC__ //////////////////////////////////////////////////////////////

	#define PACKED __attribute__((packed))

#else ////////////////////////////////////////////////////////////////////////

	#error Currently only GCC is supported for building the kernel!

#endif ///////////////////////////////////////////////////////////////////////


/// \brief	Defines the fields of the EFLAGS register.
///
/// \note
/// In the notes on each field, MSB means most significant byte, LSB means least significant byte,
/// MSb means most significant bit, and LSb means least significant bit.
typedef struct
{
	uint32_t
		CF : 1,			///< Carry Flag -- set if carry/borrow generated, or on unsigned overflow.
		Reserved1 : 1,	///< Reserved -- must be set to 1.
		PF : 1,			///< Parity Flag -- set if LSB of result has even number of 1's.
		Reserved3 : 1,	///< Reserved -- must be set to 0.
		AF : 1,			///< Adjust Flag -- set if carry/borrow out of bit 3 generated. For BCD.
		Reserved5 : 1,	///< Reserved -- must be set to 0.
		ZF : 1,			///< Zero Flag -- set if result is zero; cleared otherwise.
		SF : 1,			///< Sign Flag -- set to the MSb of the result. 0 = positive, 1 = negative.
		TF : 1,			///< Trap Flag -- set to enable single-step mode.
		IF : 1,			///< Interrupt enable Flag -- set to enable interrupts.
		DF : 1,			///< Direction Flag -- controls string instructions. 1 = --, 0 = ++.
		OF : 1,			///< Overflow Flag -- set on signed overflow/underflow.
		IOPL : 2,		///< I/O Privilege level of currently running task.
		NT : 1,			///< Nested Task flag -- controls chaining of interrupted and called tasks.
		Reserved15 : 1,	///< Reserved -- must be set to 0.
		RF : 1,			///< Resume Flag -- set to temporarily disable debug exceptions (DB).
		VM : 1,			///< Virtual-8086 Mode -- set to enable virtual-8086 mode.
		AC : 1,			///< Alignment Check -- set this and the AM flag in CR0 to enable.
		VIF : 1,		///< Virtual Interrupt Flag -- virtual image of IF bit; used for V8086.
		VIP : 1,		///< Virtual Interrupt Pending -- s/w sets, CPU reads. Used with VIF.
		ID : 1,			///< Identification -- ability to set/clear this indicates CPUID support.
		Reserved22to31 : 10;	///< Reserved -- must be set to 0.
} PACKED EFlagsRegister;



/// \brief	Defines the fields of the CR0 system register.
typedef struct
{
	uint32_t
		PE : 1,	///< Protection Enable -- set enables protected mode; clear enables real mode.
		MP : 1,	///< Monitor Coprocessor -- if this and TS are set, WAIT/FWAIT raises NM.
		EM : 1,	///< Emulation -- set forces emulation of all FPU instructions by raising NM.
		TS : 1,	///< Task Switched -- set on h/w task switch. Used for FPU/MMX/etc. context switch.
		ET : 1,	///< Extension Type -- 386/486, 1 = 387 math co is present. PentiumN: hardcoded 1.
		NE : 1,	///< Numeric Error -- controls x87 FPU error reporting; set = native, clear = old.
		Reserved6to15 : 10,	///< Reserved.
		WP : 1,	///< Write Protect -- set prevents writing to read-only user-mode pages.
		Reserved17 : 1,		///< Reserved.
		AM : 1,	///< Alignment Mask -- set this and the AC flag in EFLAGS to enable.
		Reserved19to28: 10,	///< Reserved.
		NW : 1,	///< Not Write-through -- clear this flag and CD to enable write-back/through.
		CD : 1,	///< Cache Disable -- clear this flag and NW to enable caching.
		PG : 1;	///< Paging enable -- set to enable paging; clear to disable.
} PACKED CR0Register;



/// \brief	Defines the fields of the CR3 system register.
///
/// \note
/// In the notes on each field, WB means write-back, while WT means write-through.
typedef struct
{
	uint32_t
		Reserved0to2 : 3,		///< Reserved.
		PWT : 1,	///< Page-level Writes Transparent -- control page-dir caching. 1 = WT; 0 = WB.
		PCD : 1,	///< Page-level Cache Disable -- set to disable caching of page directory.
		Reserved5to11 : 7,		///< Reserved.
		PageDirectoryBase : 20;	///< Base physical address of the page directory; Page-aligned.
} PACKED CR3Register;



/// \brief	Defines the fields of the CR4 system register.
typedef struct
{
	uint32_t
		VME : 1,	///< Virtual-8086 Mode Extensions -- set to enable V86 interrupt extensions.
		PVI : 1,	///< Protected Mode Virtual Interrupts -- set to enable h/w support for VIF.
		TSD : 1,	///< Time Stamp Disable -- RDTSC can be executed only in ring 0 when set.
		DE : 1,		///< Debugging Extensions -- 1 = r/w of DR4/5 raises UD when set; 0 = aliased.
		PSE : 1,	///< Page Size Extensions -- enables 4MB pages when set; 4KB pages when clear.
		PAE : 1,	///< Physical Address Extensions -- enables 36-bit physical addresses when set.
		MCE : 1,	///< Machine-Check Enable -- enables machine-check exception when set.
		PGE : 1,	///< Page Global Enable -- (P6 and higher) enables global pages when set.
		PCE : 1,	///< Perf Counter Enable -- RDPMC can be executed only in ring 0 when clear.
		OSFXSR : 1,	///< OS Supports FXSAVE and FXRSTOR -- enables various SSE/SSE2/SSE3 stuff.
		OSXMMEXCPT : 1,	///< OS Supports unmasked SIMD FPU exceptions -- enable XF handling (SSE).
		Reserved11to31 : 21;	///< Reserved (set to 0).
} PACKED CR4Register;



/// \brief	Defines the fields of a segment selector.
typedef struct
{
	uint16_t
		RPL : 2,			///< Requested Privilege Level -- overrides CPL (for "impersonation").
		TableIndicator : 1,	///< a.k.a. TI -- 0 = GDT; 1 = LDT.
		Index : 13;			///< Index -- selects 1 of 8192 64-bit entries in the GDT or LDT.
} PACKED SegmentSelectorFields;



/// \brief	Defines the fields of a code segment selector.
typedef struct
{
	uint16_t
		CPL : 2,			///< Current Privilege Level.
		TableIndicator : 1,	///< a.k.a. TI -- 0 = GDT; 1 = LDT.
		Index : 13;			///< Index -- selects 1 of 8192 64-bit entries in the GDT or LDT.
} PACKED CodeSegmentSelectorFields;



/// \brief	Defines the fields of a stack segment selector.
typedef CodeSegmentSelectorFields StackSegmentSelectorFields;



/// \brief	Defines the a convenience type for manipulating segment selectors.
typedef union
{
	CodeSegmentSelectorFields	CodeSeg;	///< Fields for a code segment selector.
	StackSegmentSelectorFields	StackSeg;	///< Fields for a stack segment selector.
	SegmentSelectorFields		Seg;		///< Segment selector fields.
	uint16_t					RawValue;	///< Segment selector as a 16-bit value.
} PACKED SegmentSelector;



/// \brief	Defines the general format of a segment descriptor for a segment marked as
///			"not present".
typedef struct
{
	uint32_t	AvailableLow0to31;		///< Available for use by system software.
	uint8_t		AvailableHigh0to7;		///< Available for use by system software.
	uint8_t
		Type : 4,				///< Indicates the type of the segment and other attributes.
		SegDescriptorType : 1,	///< a.k.a. S -- 1 = code or data segment; 0 = system segment.
		DPL : 2,				///< Descriptor Privilege Level -- privilege level of the segment.
		Present : 1;			///< a.k.a. P -- by definition, this is 0.
	uint16_t	AvailableHigh16to31;	///< Available for use by system software.
} PACKED NotPresentSegmentDescriptor;



/// \brief	Defines the format of a code segment descriptor.
typedef struct
{
	/// \brief	Low 16 bits of segment limit.
	///
	/// The limit of a segment ranges from 1 byte to 1MB in byte increments or from 4KB to
	/// 4GB in 4KB increments, depending on the Granularity flag. The offset part of a logical
	/// address can range from 0 to the segment limit for code segments.
	uint16_t	Limit0to15;

	/// \brief	Low 16 bits of segment base address.
	///
	/// The base address of a segment indicates the location of address 0 in the linear address
	/// space. It should be aligned on 16-byte boundaries for performance reasons, although this
	/// is not required.
	uint16_t    Base0to15;
	uint8_t		Base16to23;	///< Bits 16 through 23 of the segment base address. See Base0to15.
	uint8_t
		Accessed : 1,			///< a.k.a. A -- set by the processor on access; cleared by s/w.
		Readable : 1,			///< a.k.a. R -- 1 = code seg is read/execute. 0 = execute-only.
		Conforming : 1,			///< a.k.a. C -- 1 = can execute in code seg at caller's CPL.
		Type3 : 1,				///< Bit 3 of the Type field. Must be 1 for a code segment.
		SegDescriptorType : 1,	///< a.k.a. S -- Must be 1 for a code segment.
		DPL : 2,				///< Descriptor Privilege Level -- privilege level of the segment.
		Present : 1;			///< a.k.a. P -- 1 = segment is in memory. 0 = access raises NP.
	uint8_t
		Limit16to19 : 4,			///< The high 4 bits of the segment limit. See Limit0to15.
		AvailableHigh20 : 1,		///< Available for use by system software.
		ReservedHigh21 : 1,			///< Reserved -- set to 0.
		DefaultOperationSize : 1,	///< a.k.a. D -- 1 = addrs. & operands are 32-bit; 0 = 16-bit.
		Granularity : 1;			///< a.k.a. G -- 1 = limit in increments of 4KB; 0 = bytes.
	uint8_t		Base24to31;		///< Bits 24 through 31 of the segment base address. See Base0to15.
} PACKED CodeSegmentDescriptor;



/// \brief	Defines the format of a data segment descriptor.
typedef struct
{
	/// \brief	Low 16 bits of segment limit.
	///
	/// The limit of a segment ranges from 1 byte to 1MB in byte increments or from 4KB to
	/// 4GB in 4KB increments, depending on the Granularity flag. The offset part of a logical
	/// address can range from 0 to the segment limit for expand-up data segments. Offsets can
	/// range from the segment limit to 0xFFFFFFFF (or 0xFFFF if the B flag is clear) for
	/// expand-down data segments.
	uint16_t	Limit0to15;

	/// \brief	Low 16 bits of segment base address.
	///
	/// The base address of a segment indicates the location of address 0 in the linear address
	/// space. It should be aligned on 16-byte boundaries for performance reasons, although this
	/// is not required.
	uint16_t    Base0to15;
	uint8_t		Base16to23;	///< Bits 16 through 23 of the segment base address. See Base0to15.
	uint8_t
		Accessed : 1,		///< a.k.a. A -- set by the processor on access; cleared by s/w.
		Writable : 1,		///< a.k.a. W -- 1 = data seg is read/write. 0 = read-only.
		ExpandDown : 1,		///< a.k.a. E -- 0 = offs. from 0 to limit; 0 = limit to 0xFFFF(FFFF).
		Type3 : 1,			///< Bit 3 of the Type field. Must be 0 for a data segment.
		SegDescriptorType : 1,	///< a.k.a. S -- must be 1 for a data segment.
		DPL : 2,				///< Descriptor Privilege Level -- privilege level of the segment.
		Present : 1;			///< a.k.a. P -- 1 = segment is in memory. 0 = access raises NP.
	uint8_t
		// NOTE: Doxygen has a problem parsing end-of-line comments unless they are all end-of
		// line. The comment for "Big" below is too big to fit as an end-of-line comment.
		///	\brief	The high 4 bits of the segment limit. See Limit0to15.
		Limit16to19 : 4,

		/// \brief	Available for use by system software.
		AvailableHigh20 : 1,
		
		/// \brief	Reserved -- set to 0.
		ReservedHigh21 : 1,
		
		/// \brief	a.k.a. B -- For stack segments, 1 = use 32-bit stack pointer in ESP, 0 = use
		///			16-bit stack pointer in SP; For expand-down data segments (including stack
		///			segments if applicable), 1 = upper bound is 0xFFFFFFFF, 0 = upper bound is
		///			0xFFFF.
		Big : 1,

		/// \brief	a.k.a. G -- 1 = limit in increments of 4KB; 0 = bytes.
		Granularity : 1;
	uint8_t		Base24to31;		///< Bits 24 through 31 of the segment base address. See Base0to15.
} PACKED DataSegmentDescriptor;



/// \brief	Defines the format of an LDT descriptor.
typedef struct
{
	/// \brief	Low 16 bits of segment limit.
	///
	/// The limit of a segment ranges from 1 byte to 1MB in byte increments or from 4KB to
	/// 4GB in 4KB increments, depending on the Granularity flag. The offset part of a logical
	/// address can range from 0 to the segment limit.
	uint16_t	Limit0to15;

	/// \brief	Low 16 bits of segment base address.
	///
	/// The base address of a segment indicates the location of address 0 in the linear address
	/// space. It should be aligned on 16-byte boundaries for performance reasons, although this
	/// is not required.
	uint16_t    Base0to15;
	uint8_t		Base16to23;	///< Bits 16 through 23 of the segment base address. See Base0to15.
	uint8_t
		Type : 4,	///< Indicates the descriptor type; Must be 0010b for an LDT descriptor.
		SegDescriptorType : 1,	///< a.k.a. S -- Must be 0 for an LDT descriptor.
		DPL : 2,				///< Descriptor Privilege Level -- privilege level of the segment.
		Present : 1;			///< a.k.a. P -- 1 = descriptor is valid. 0 = access raises NP.
	uint8_t
		Limit16to19 : 4,		///< The high 16 bits of the segment limit. See Limit0to15.
		AvailableHigh20 : 1,	///< Available for use by system software.
		ReservedHigh21 : 1,		///< Reserved -- set to 0.
		ReservedHigh22 : 1,		///< Reserved -- 0 for LDT descriptors (undocumented).
		Granularity : 1;		///< a.k.a. G -- 1 = limit in increments of 4KB; 0 = bytes.
	uint8_t		Base24to31;		///< Bits 24 through 31 of the segment base address. See Base0to15.
} PACKED LdtDescriptor;



/// \brief	Defines the format of a TSS descriptor.
typedef struct
{
	/// \brief	Low 16 bits of segment limit.
	///
	/// The limit of a segment ranges from 1 byte to 1MB in byte increments or from 4KB to
	/// 4GB in 4KB increments, depending on the Granularity flag. The offset part of a logical
	/// address can range from 0 to the segment limit for code segments or expand-up data
	/// segments. Offsets can range from the segment limit to 0xFFFFFFFF (or 0xFFFF if the B flag
	/// is clear) for expand-down data segments.
	uint16_t	Limit0to15;

	/// \brief	Low 16 bits of segment base address.
	///
	/// The base address of a segment indicates the location of address 0 in the linear address
	/// space. It should be aligned on 16-byte boundaries for performance reasons, although this
	/// is not required.
	uint16_t    Base0to15;
	uint8_t		Base16to23;	///< Bits 16 through 23 of the segment base address. See Base0to15.
	uint8_t
		Type0 : 1,		///< Bit 0 of the Type field. Must be 1 for a TSS.
		Busy : 1,		///< Busy flag -- 1 = task is running or suspended; 0 = task is available.
		Type2to3 : 2,	///< Type bits 2 and 3. 10b for 32-bit TSS; 00b for 16-bit TSS.
		SegDescriptorType : 1,	///< a.k.a. S -- must be 0 for a TSS descriptor.
		DPL : 2,				///< Descriptor Privilege Level -- privilege level of the TSS.
		Present : 1;			///< a.k.a. P -- 1 = descriptor is valid. 0 = access raises NP.
	uint8_t
		Limit16to19 : 4,		///< The high 16 bits of the segment limit. See Limit0to15.
		AvailableHigh20 : 1,	///< Available for use by system software.
		ReservedHigh21 : 1,		///< Reserved -- set to 0.
		ReservedHigh22 : 1,		///< Reserved -- 0 for TSS descriptors.
		Granularity : 1;		///< a.k.a. G -- 1 = limit in increments of 4KB; 0 = bytes.
	uint8_t		Base24to31;		///< Bits 24 through 31 of the segment base address. See Base0to15.
} PACKED TssDescriptor;



/// \brief	Defines the format of a call gate descriptor.
typedef struct
{
	uint16_t		Offset0to15;	///< Bits 0-15 of the offset of the procedure to call.
	SegmentSelector	SegSelector;	///< Selector for the code segment containing the procedure.
	uint8_t
		ParamCount : 5,			///< Number of parameters to copy on stack switch.
		ReservedHigh5to7 : 3;	///< Reserved -- must be 0.
	uint8_t
		Type : 4,				///< Gate type -- 1100b: 32-bit call gate; 0100b: 16-bit.
		SegDescriptorType : 1,	///< Must be 0 for all gate descriptor types.
		DPL : 2,				///< Descriptor Privilege Level -- priv. level of the gate.
		Present : 1;			///< a.k.a. P -- 1 = gate is valid. 0 = access raises NP.
	uint16_t		Offset16to31;	///< Bits 16-31 of the offset of the procedure to call.
} PACKED CallGateDescriptor;



/// \brief	Defines the format of an interrupt gate descriptor.
typedef struct
{
	uint16_t		Offset0to15;	///< Bits 0-15 of the offset of the procedure to call.
	SegmentSelector	SegSelector;	///< Selector for the code segment containing the procedure.
	uint8_t
		ReservedHigh0to4 : 5,		///< Reserved.
		ReservedHigh5to7 : 3;		///< Reserved -- must be 0.
	uint8_t
		Type : 4,				///< Gate type -- 1110b: 32-bit interrupt gate; 0110b: 16-bit.
		SegDescriptorType : 1,	///< Must be 0 for all gate descriptor types.
		DPL : 2,				///< Descriptor Privilege Level -- priv. level of the gate.
		Present : 1;			///< a.k.a. P -- 1 = gate is valid. 0 = access raises NP.
	uint16_t		Offset16to31;	///< Bits 16-31 of the offset of the procedure to call.
} PACKED InterruptGateDescriptor;



/// \brief	Defines the format of a trap gate descriptor.
typedef struct
{
	uint16_t		Offset0to15;	///< Bits 0-15 of the offset of the procedure to call.
	SegmentSelector	SegSelector;	///< Selector for the code segment containing the procedure.
	uint8_t
		ReservedHigh0to4 : 5,		///< Reserved.
		ReservedHigh5to7 : 3;		///< Reserved -- must be 0.
	uint8_t
		Type : 4,				///< Gate type -- 1111b: 32-bit trap gate; 0111b: 16-bit.
		SegDescriptorType : 1,	///< Must be 0 for all gate descriptor types.
		DPL : 2,				///< Descriptor Privilege Level -- priv. level of the gate.
		Present : 1;			///< a.k.a. P -- 1 = gate is valid. 0 = access raises NP.
	uint16_t		Offset16to31;	///< Bits 16-31 of the offset of the procedure to call.
} PACKED TrapGateDescriptor;



/// \brief	Defines the format of a task gate descriptor.
typedef struct
{
	uint16_t		ReservedLow0to15;	///< Reserved.
	SegmentSelector	TssSegSelector;		///< Selector for the TSS of the task to switch to.
	uint8_t			ReservedHigh0to7;	///< Reserved.
	uint8_t
		Type : 4,				///< Gate type -- must be 0101b for a task gate.
		SegDescriptorType : 1,	///< Must be 0 for all gate descriptor types.
		DPL : 2,				///< Descriptor Privilege Level -- priv. level of the gate.
		Present : 1;			///< a.k.a. P -- 1 = gate is valid. 0 = access raises NP.
	uint16_t		ReservedHigh16to31;	///< Reserved.
} PACKED TaskGateDescriptor;



/// \brief	Defines all descriptor types that are allowed in the GDT.
typedef union
{
	LdtDescriptor			Ldt;		///< LDT descriptors are allowed in the GDT.
	TssDescriptor			Tss;		///< TSS descriptors are allowed in the GDT.
	CodeSegmentDescriptor	CodeSeg;	///< Code segment descriptors are allowed in the GDT.
	DataSegmentDescriptor	DataSeg;	///< Data segment descriptors are allowed in the GDT.
	CallGateDescriptor		CallGate;	///< Call gate descriptors are allowed in the GDT.
	TaskGateDescriptor		TaskGate;	///< Task gate descriptors are allowed in the GDT.
} PACKED GdtEntry;



/// \brief	Defines all descriptor types that are allowed in the LDT.
typedef union
{
	CodeSegmentDescriptor	CodeSeg;	///< Code segment descriptors are allowed in an LDT.
	DataSegmentDescriptor	DataSeg;	///< Data segment descriptors are allowed in an LDT.
	CallGateDescriptor		CallGate;	///< Call gate descriptors are allowed in an LDT.
	TaskGateDescriptor		TaskGate;	///< Task gate descriptors are allowed in an LDT.
} PACKED LdtEntry;



/// \brief	Defines all descriptor types that are allowed in the IDT.
typedef union
{
	TaskGateDescriptor		TaskGate;		///< Task gate descriptors are allowed in the IDT.
	TrapGateDescriptor		TrapGate;		///< Trap gate descriptors are allowed in the IDT.
	InterruptGateDescriptor	InterruptGate;	///< Interrupt gate descriptors are allowed in the IDT.
} PACKED IdtEntry;


/// \brief	Defines the format of a task-state segment.
///
/// The following fields are updated by the processor on a task switch:
/// - PreviousTaskLink
/// - EIP
/// - EFLAGS
/// - General purpose registers (EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI).
/// - Segment selectors (ES, CS, SS, DS, FS, GS).
/// .
/// The remaining fields are set up by software when the TSS is created and read by the processor
/// on a task switch.
typedef struct
{
	SegmentSelector	PreviousTaskLink;		///< TSS selector of the task that called this task.
	uint16_t		ReservedBytes2to3;		///< Reserved -- must be 0.
	uint32_t		ESP0;					///< Stack pointer for ring-0 stack.
	SegmentSelector	SS0;					///< Stack segment selector for ring-0 stack.
	uint16_t		ReservedBytes10to11;	///< Reserved -- must be 0.
	uint32_t		ESP1;					///< Stack pointer for ring-1 stack.
	SegmentSelector	SS1;					///< Stack segment selector for ring-1 stack.
	uint16_t		ReservedBytes18to19;	///< Reserved -- must be 0.
	uint32_t		ESP2;					///< Stack pointer for ring-2 stack.
	SegmentSelector	SS2;					///< Stack segment selector for ring-2 stack.
	uint16_t		ReservedBytes26to27;	///< Reserved -- must be 0.
	CR3Register		CR3;	///< Contents of CR3 for this task (page directory phys. address).
	uint32_t		EIP;	///< The state of the EIP register prior to the task switch.
	EFlagsRegister	EFLAGS;	///< The state of the EFLAGS register prior to the task switch.
	uint32_t		EAX;	///< The state of the EAX register prior to the task switch.
	uint32_t		ECX;	///< The state of the ECX register prior to the task switch.
	uint32_t		EDX;	///< The state of the EDX register prior to the task switch.
	uint32_t		EBX;	///< The state of the EBX register prior to the task switch.
	uint32_t		ESP;	///< The state of the ESP register prior to the task switch.
	uint32_t		EBP;	///< The state of the EBP register prior to the task switch.
	uint32_t		ESI;	///< The state of the ESI register prior to the task switch.
	uint32_t		EDI;	///< The state of the EDI register prior to the task switch.
	SegmentSelector	ES;		///< Segment selector stored in ES prior to the task switch.
	uint16_t		ReservedBytes74to75;	///< Reserved -- must be 0.
	SegmentSelector	CS;						///< Selector in CS prior to the task switch.
	uint16_t		ReservedBytes78to79;	///< Reserved -- must be 0.
	SegmentSelector	SS;						///< Selector in SS prior to the task switch.
	uint16_t		ReservedBytes82to83;	///< Reserved -- must be 0.
	SegmentSelector	DS;						///< Selector in DS prior to the task switch.
	uint16_t		ReservedBytes86to87;	///< Reserved -- must be 0.
	SegmentSelector	FS;						///< Selector in FS prior to the task switch.
	uint16_t		ReservedBytes90to91;	///< Reserved -- must be 0.
	SegmentSelector	GS;						///< Selector in GS prior to the task switch.
	uint16_t		ReservedBytes94to95;	///< Reserved -- must be 0.
	SegmentSelector	LdtSelector;			///< Segment selector for the task's LDT.
	uint16_t		ReservedBytes98to99;	///< Reserved -- must be 0.
	uint16_t
		TrapFlag : 1,	///< When set, causes a debug exception when switching to this task.
		ReservedBytes100to101Bits1to15 : 15;	///< Reserved -- must be 0.
	uint16_t		IOMapBaseAddress;	///< Offset from TSS base of I/O permission bit map.
} PACKED TaskStateSegment;



/// \brief	Defines the format of a page directory entry.
typedef struct
{
	uint32_t
		Present : 1,		///< a.k.a. "P" -- 1 - page table is resident. 0 - access raises PF.
		ReadWrite : 1,		///< a.k.a. "R/W" -- 1 - pages in table are writable; 0 - read-only.
		UserSupervisor : 1,	///< a.k.a. "U/S" -- 1 - pages in table are user-mode; 0 - kernel-mode.
		WriteThrough : 1,	///< a.k.a. "PWT" -- 1 - table is write-through cached; 0 - write-back.
		CacheDisabled : 1,	///< a.k.a. "PCD" -- 1 - caching of page-table disabled; 0 - enabled.
		Accessed : 1,		///< a.k.a. "A" -- Set by processor on read/write. Cleared by software.
		Reserved6 : 1,		///< Reserved -- must be 0.
		PageSize : 1,		///< a.k.a. "PS" -- 0 - 4KB pages; 1 - 2 or 4MB pages, depends on PAE.
		Global : 1,			///< a.k.a. "G" -- Ignored.
		Available9to11 : 3,	///< Available for use by system software.
		PageTableBaseAddress : 20;	///< Physical address of the page table. 4KB-aligned.
} PACKED PageDirectoryEntry;



/// \brief	Defines the format of a page table entry.
typedef struct
{
	uint32_t
		Present : 1,		///< a.k.a. "P" -- 1 - page is resident. 0 - access raises PF.
		ReadWrite : 1,		///< a.k.a. "R/W" -- 1 - page is writable; 0 - read-only.
		UserSupervisor : 1,	///< a.k.a. "U/S" -- 1 - page is user-mode; 0 - kernel-mode.
		WriteThrough : 1,	///< a.k.a. "PWT" -- 1 - page is write-through cached; 0 - write-back.
		CacheDisabled : 1,	///< a.k.a. "PCD" -- 1 - caching of page disabled; 0 - enabled.
		Accessed : 1,		///< a.k.a. "A" -- Set by processor on read/write. Cleared by software.
		Dirty : 1,			///< a.k.a. "D" -- Set by processor on write. Cleared by software.
		PageAttribTableIndex : 1,	///< a.k.a. "PAT" -- Only for P3's and higher. Otherwise 0.
		Global : 1,					///< a.k.a. "G" -- 1 - PTE stays in TLB when CR3 is loaded.
		Available9to11 : 3,			///< Available for use by system software.
		PageBaseAddress : 20;		///< Physical address of the page. 4KB-aligned.
} PACKED PageTableEntry;



/// \brief	Defines the format of a page-fault (PF) error code.
/// \note	When a page fault occurs, the processor loads the CR2 register with the linear address
///			that caused the fault.
typedef struct
{
	uint32_t
		PageProtectionFault : 1,	///< a.k.a. "P" -- 1 - page protection. 0 - page not present.
		WriteFault : 1,				///< a.k.a. "W/R" -- 1 - fault on write. 0 - fault on read.
		UserModeFault : 1,			///< a.k.a. "U/S" -- 1 - user fault. 0 - supervisor fault.
		ReservedBitsFault : 1,		///< a.k.a. "RSVD" -- 1 - reserved bits in page dir set to 1.
		Reserved : 28;				///< Reserved -- set to 0.
} PACKED PageFaultErrorCode;



/// \brief	Defines the format of an error code related to a segment.
typedef struct
{
	uint32_t
		External : 1,			///< a.k.a. "EXT" -- 1 - external exception (e.g. h/w intr).
		DescriptorLocation : 1,	///< a.k.a. "IDT" -- 1 - index in IDT. 0 - index in GDT/LDT.
		TableIndex : 1,			///< a.k.a. "TI" -- 1 - index in LDT; 0 - GDT. IDT flag must be 0.
		SelectorIndex : 13,		///< Index of a selector in the IDT, GDT, or LDT.
		Reserved : 16;			///< Reserved -- set to 0.
} PACKED SegmentErrorCode;



/// \brief	Defines the general format of an error code.
typedef union
{
	PageFaultErrorCode	PageFaultCode;	///< Error code for PF exception.
	SegmentErrorCode	SegmentCode;	///< Error code for segment-related exceptions.
	uint32_t			RawValue;		///< Raw integer value. Can be useful for the zero code.
} PACKED GeneralErrorCode;



/// \brief	Defines the various interrupt/exception vector numbers.
typedef enum
{
	INT0_DE_DIVIDE_ERROR			= 0,	///< Fault, no error code, raised by DIV/IDIV.
	INT1_DB_RESERVED				= 1,	///< Fault/trap, no error code, Intel use only.
	INT2_NMI_INTERRUPT				= 2,	///< Interrupt, no error code, non-maskable interrupt.
	INT3_BP_BREAKPOINT				= 3,	///< Trap, no error code, raised by INT 3 instruction.
	INT4_OF_OVERFLOW				= 4,	///< Trap, no error code, raised by INTO instruction.
	INT5_BR_BOUND_RANGE_EXCEEDED	= 5,	///< Fault, no error code, raised by BOUND instruction.
	INT6_UD_INVALID_OPCODE			= 6,	///< Fault, no error code, UD2 (P6+) or bad opcode.
	INT7_NM_NO_MATH_COPROCESSOR		= 7,	///< Fault, no error code, raised by FP or WAIT/FWAIT.
	INT8_DF_DOUBLE_FAULT			= 8,	///< Abort, error code = 0, any exception/NMI/INTR.
	INT9_CO_PROCESSOR_SEG_OVERRUN	= 9,	///< Fault, no error code, FP (reserved, 386 only).
	INT10_TS_INVALID_TSS			= 10,	///< Fault, error code, task switch or TSS access.
	INT11_NP_SEG_NOT_PRESENT		= 11,	///< Fault, error code, loading seg regs./system segs.
	INT12_SS_STACK_SEG_FAULT		= 12,	///< Fault, error code, stack ops/SS reg. loads.
	INT13_GP_GENERAL_PROTECTION		= 13,	///< Fault, error code, memory access/protection check.
	INT14_PF_PAGE_FAULT				= 14,	///< Fault, error code, raised by any memory access.
	INT15_RESERVED					= 15,	///< No error code, reserved (do not use).
	INT16_MF_X87_MATH_FAULT			= 16,	///< Fault, no error code, x87 FPU or WAIT/FWAIT.
	INT17_AC_ALIGNMENT_CHECK		= 17,	///< Fault, error code = 0, mem data reference (486+).
	INT18_MC_MACHINE_CHECK			= 18,	///< Abort, error code/source model dependent (586+).
	INT19_XF_SIMD_FP_EXCEPTION		= 19,	///< Fault, no error code, SSE/SSE2/SSE3 (P3+).
	INT20_RESERVED					= 20,	///< Intel reserved. Do not use.
	INT21_RESERVED					= 21,	///< Intel reserved. Do not use.
	INT22_RESERVED					= 22,	///< Intel reserved. Do not use.
	INT23_RESERVED					= 23,	///< Intel reserved. Do not use.
	INT24_RESERVED					= 24,	///< Intel reserved. Do not use.
	INT25_RESERVED					= 25,	///< Intel reserved. Do not use.
	INT26_RESERVED					= 26,	///< Intel reserved. Do not use.
	INT27_RESERVED					= 27,	///< Intel reserved. Do not use.
	INT28_RESERVED					= 28,	///< Intel reserved. Do not use.
	INT29_RESERVED					= 29,	///< Intel reserved. Do not use.
	INT30_RESERVED					= 30,	///< Intel reserved. Do not use.
	INT31_RESERVED					= 31,	///< Intel reserved. Do not use.
	INT32_FIRST_USER_DEFINED_INTR	= 32,	///< First user-defined interrupt vector.
	INT255_LAST_USER_DEFINED_INTR	= 255	///< Last user-defined interrupt vector.
} InterruptVector;



// Functions.


/// \brief	Creates a valid interrupt gate descriptor with a DPL of 0.
///
/// \param offset		the 32-bit offset of the interrupt handler procedure.
/// \param segSelector	the segment selector of the code segment containing the handler procedure.
///
/// \return a properly-initialized InterruptGateDescriptor.
static inline InterruptGateDescriptor ProtectedMode_createInterruptGate(
	uint32_t		offset,
	SegmentSelector	segSelector
)
{
	InterruptGateDescriptor gate;

	// Zero out everything to start with.
	KMem_set( &gate, 0, sizeof( gate ) );

	gate.Offset0to15		= KMem_low16( offset );
	gate.Offset16to31		= KMem_high16( offset );
	gate.SegSelector		= segSelector;
	gate.Type				= 0xE;	// 32-bit interrupt gate.
	gate.Present			= 1;
	return gate;
}


/// \brief	Creates a valid flat-model code segment descriptor.
///
/// \param isRing0	if \c true, the DPL of the created segment descriptor will be 0. Otherwise,
///					it will be 3.
///
/// \return a properly-initialized CodeSegmentDescriptor.
static inline CodeSegmentDescriptor ProtectedMode_createCodeSegment( bool isRing0 )
{
	CodeSegmentDescriptor codeSeg;

	KMem_set( &codeSeg, 0, sizeof( codeSeg ) );

	codeSeg.Limit0to15				= 0xFFFF;		// 4GB limit, base is 0 (flat model).
	codeSeg.Type3					= 1;			// Must be 1 for a code segment.
	codeSeg.SegDescriptorType		= 1;			// Must be 1 for a code segment.
	codeSeg.DPL						= (isRing0) ? 0 : 3;	// Set DPL accordingly.
	codeSeg.Present					= 1;			// Flat model -- segments always present.
	codeSeg.Limit16to19				= 0xF;			// 4GB limit (flat model).
	codeSeg.DefaultOperationSize	= 1;			// 32-bit segment.
	codeSeg.Granularity				= 1;			// Limit in 4KB increments.
	return codeSeg;
}


/// \brief	Creates a valid flat-model data segment descriptor.
///
/// \param isRing0	if \c true, the DPL of the created segment descriptor will be 0. Otherwise,
///					it will be 3.
///
/// \return a properly-initialized DataSegmentDescriptor.
static inline DataSegmentDescriptor ProtectedMode_createDataSegment( bool isRing0 )
{
	DataSegmentDescriptor dataSeg;

	KMem_set( &dataSeg, 0, sizeof( dataSeg ) );

	dataSeg.Limit0to15			= 0xFFFF;		// 4GB limit, base is 0 (flat model).
	dataSeg.Writable			= 1;			// Read-write. Use paging for read-only.
	dataSeg.SegDescriptorType	= 1;			// Must be 1 for a data segment.
	dataSeg.DPL					= (isRing0) ? 0 : 3;	// Set DPL accordingly.
	dataSeg.Present				= 1;			// Flat model -- segments always present.
	dataSeg.Limit16to19			= 0xF;			// 4GB limit (flat model).
	dataSeg.Big					= 1;			// 32-bit segment.
	dataSeg.Granularity			= 1;			// Limit in 4KB increments.
	return dataSeg;
}


/// \brief	Creates a valid TSS descriptor for the TSS at the given address.
///
/// \param tss	the linear address of the TSS for which to create a descriptor.
///
/// \return a properly-initialized TssDescriptor.
static inline TssDescriptor ProtectedMode_createTssDescriptor( TaskStateSegment* tss )
{
	KDebug_assertArg( tss != NULL );

	enum Local
	{
		TSS_LIMIT = sizeof( TaskStateSegment ) - 1
	};

	uint32_t tssInt = (uint32_t) tss;

	TssDescriptor tssDesc;

	KMem_set( &tssDesc, 0, sizeof( tssDesc ) );

	tssDesc.Limit0to15			= KMem_low16( TSS_LIMIT );
	tssDesc.Base0to15			= KMem_low16( tssInt );
	tssDesc.Base16to23			= KMem_low8( KMem_high16( tssInt ) );
	tssDesc.Type0				= 1;			// Must be 1 for a TSS.
	tssDesc.Type2to3			= 2;			// Must be 2 (10b) for 32-bit TSS.
	tssDesc.Present				= 1;			// Descriptor is valid.
	tssDesc.Limit16to19			= KMem_low8( KMem_high16( TSS_LIMIT ) ) & 0x0F;
	tssDesc.Base24to31			= KMem_high8( KMem_high16( tssInt ) );
	return tssDesc;
}


#endif

