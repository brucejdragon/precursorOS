// ===========================================================================
//
//             Copyright (C) 2004-2006 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Source/Kernel/MM/PhysicalMemoryManager.c
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2006/Feb/05
//
// ===========================================================================
///
///	\file
///
/// \brief	Encapsulates the Physical Memory Manager subsystem of the Precursor
///			microkernel.
///
// ===========================================================================


#include <stdbool.h>
#include "Kernel/MM/PhysicalMemoryManager.h"
#include "Kernel/MM/ConcatPmmRegionList.h"
#include "Kernel/MM/MM.h"
#include "PmmWatermarkAllocator.h"
#include "Kernel/KCommon/KMem.h"
#include "Kernel/KCommon/KDebug.h"



/// \brief	Implementation of PhysicalMemoryManager.
struct PhysicalMemoryManagerStruct
{
	IPmmAllocator			m_currentAllocator;		///< The current allocator for kernel requests.
	char					m_pfdb;					///< The PageFrameDatabase. ***FIXME
	PmmWatermarkAllocator	m_initialAllocator;		///< The allocator for "initialization" mode.
	size_t					m_initialAllocatorSpace[NUM_BLOCKS];	///< For initial allocator.
	IPmmRegionList			m_ramList;				///< The list of all RAM regions.
	IPmmRegionList			m_reservedList;			///< The list of all reserved regions.
	IPmmRegionList			m_moduleList;			///< The list of all kernel & module regions.
	ConcatPmmRegionList		m_usedList;				///< The union of the reserved & module lists.
	bool					m_isFullyInitialized;	///< \c true after initStageTwo() is called.
};



/// \brief	The one-and-only instance of PhysicalMemoryManager.
static volatile PhysicalMemoryManager s_instance;



size_t PhysicalMemoryManager_initStageOne(
	IPmmRegionList ramList,
	IPmmRegionList reservedList,
	IPmmRegionList moduleList
)
{
	// It's ok to cast away volatile because this method is supposed to be called only once on
	// one CPU with interrupts disabled.
	PhysicalMemoryManager* pmm = (PhysicalMemoryManager*) &s_instance;
	
	KMem_set( pmm, 0, sizeof( PhysicalMemoryManager ) );

	// First, calculate how much space the PFDB will need for initStageTwo(). First we need to
	// calculate how many frames the PMM will be managing. This can be discovered by looking for
	// the highest region in the RAM list.
	phys_addr_t highestAddr = 0;
	
	ramList.iptr->reset( ramList.obj );
	while (ramList.iptr->moveNext( ramList.obj ))
	{
		PmmRegion crnt = ramList.iptr->getCurrent( ramList.obj );
		phys_addr_t last = PmmRegion_last( &crnt );
		if (last > highestAddr)
		{
			highestAddr = last;
		}
	}

	size_t numFrames = MM_getFrameNumber( highestAddr ) + 1;

	pmm->m_isFullyInitialized	= false;
	pmm->m_ramList				= ramList;
	pmm->m_reservedList			= reservedList;
	pmm->m_moduleList			= moduleList;

	// Chain the module and reserved lists together. They are passed in separately so that the PFDB
	// can tell the difference between the kernel & module regions and the reserved regions
	// reported by the bootloader.	
	pmm->m_usedList = ConcatPmmRegionList_create( pmm->m_reservedList, pmm->m_moduleList );

	// Create the initial watermark allocator. Don't forget to use the complete "reserved" list
	// that includes the kernel and its modules!
	pmm->m_initialAllocator =
		PmmWatermarkAllocator_create(
			pmm->m_ramList,
			ConcatPmmRegionList_getAsPmmRegionList( &(pmm->m_usedList) ),
			pmm->m_initialAllocatorSpace
		);

	// Point the current allocator at the initial allocator. It will handle requests from the rest
	// of the kernel until initStageTwo() is called.		
	pmm->m_currentAllocator =
		PmmWatermarkAllocator_getAsPmmAllocator( &(pmm->m_initialAllocator) );
		
	return numFrames;	//***FIXME: Pass this to PFDB_calculateYaddaYadda()...
}


void PhysicalMemoryManager_initStageTwo(
	void*	workingSpace,	// in, own
	size_t	sizeInBytes		// in
)
{
	//***FIXME: Implement.
	(void) workingSpace;
	(void) sizeInBytes;
}


volatile PhysicalMemoryManager* PhysicalMemoryManager_getInstance()
{
	return &s_instance;
}


//***FIXME: Return type.
volatile char* PhysicalMemoryManager_getPageFrameDatabase(
	volatile PhysicalMemoryManager* pmm
)
{
	KDebug_assertArg( pmm != NULL );
	return &(pmm->m_pfdb);
}


IPmmAllocator PhysicalMemoryManager_getAllocator( const volatile PhysicalMemoryManager* pmm )
{
	KDebug_assertArg( pmm != NULL );
	return pmm->m_currentAllocator;
}

