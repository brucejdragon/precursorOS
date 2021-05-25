// ===========================================================================
//
//             Copyright (C) 2004-2005 Bruce Johnston
//
// ===========================================================================
//
//   //osdev/precursor/Include/Kernel/MM/IPmmRegionList.h
//
// ===========================================================================
//
//	Originating Author:	BruceJ
//	Originating Date:	2005/Dec/01
//
// ===========================================================================
///
/// \file
///
/// \brief	Defines the IPmmRegionList interface, which exposes a forward-only
///			iterator for traversing a sequence of PmmRegion structures.
///
// ===========================================================================

#ifndef _KERNEL_MM_IPMMREGIONLIST_H_
#define _KERNEL_MM_IPMMREGIONLIST_H_


#include <stdbool.h>
#include "Kernel/MM/PmmRegion.h"


/// \brief	Defines the method signature for IPmmRegionList_reset().
///
/// \param this the object that implements IPmmRegionList.
///
/// This method resets the iterator to the position before the first element of the list (if any).
/// This method must be called before an IPmmRegionList is used for the first time.
typedef void (*IPmmRegionList_resetFunc)( void* this );


/// \brief	Defines the method signature for IPmmRegionList_moveNext().
///
/// \param this the object that implements IPmmRegionList.
///
/// This method attempts to advance the iterator forward by one position. If the list is empty,
/// this method will return \c false. Otherwise, if the iterator has just been reset, this method
/// moves it to the first valid position and returns \c true. If the iterator is positioned on the
/// last element in the list when this method calls, it will be advanced beyond the end of the list
/// and this method will return \c false. Any subsequent call to this method will return \c false
/// until IPmmRegionList_reset() is called. In all other cases, this method will return \c true.
///
/// \retval true	the iterator advanced to the next valid position.
/// \retval false	the iterator advanced beyond the last valid position, the iterator was already
///					beyond the last valid position, or the list is empty.
typedef bool (*IPmmRegionList_moveNextFunc)( void* this );


/// \brief	Defines the method signature for IPmmRegionList_getCurrent().
///
/// \param this the object that implements IPmmRegionList.
///
/// This method returns the PmmRegion at the current position in the list if the last call to
/// IPmmRegionList_moveNext() returned \c true. If not, this is considered a logic error in the
/// kernel and will likely result in system failure.
///
/// \return the PmmRegion at the current position in the list.
typedef PmmRegion (*IPmmRegionList_getCurrentFunc)( const void* this );



/// \brief	Defines the interface dispatch table for IPmmRegionList.
typedef struct
{
	/// \brief	Pointer to a particular implementation of IPmmRegionList_reset().
	IPmmRegionList_resetFunc reset;

	/// \brief	Pointer to a particular implementation of IPmmRegionList_moveNext().
	IPmmRegionList_moveNextFunc moveNext;

	/// \brief	Pointer to a particular implementation of IPmmRegionList_getCurrent().
	IPmmRegionList_getCurrentFunc getCurrent;

} IPmmRegionList_itable;



/// \brief	Defines a reference to an implementation of the IPmmRegionList interface.
typedef struct
{
	const IPmmRegionList_itable*	iptr;	///< Pointer to the interface dispatch table.
	void*							obj;	///< Pointer to the object that implements the interface.

} IPmmRegionList;


#endif

