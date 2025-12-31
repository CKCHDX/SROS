/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Definitions for smart card base
*
*/


#ifndef CSCARDBASE_H
#define CSCARDBASE_H

//  INCLUDES
#include <e32base.h>

//  CONSTANTS  

////////////////////////////////////////////////////////////////////////////////
//  Scard Panic codes
////////////////////////////////////////////////////////////////////////////////
const TInt KScPanicNoMemory                 = -1;   // Not enough free memory
const TInt KScPanicNullPointer              = -2;   // A NULL pointer was encountered
const TInt KScPanicNullLength               = -3;   // A buffer or data member has null lenght
const TInt KScPanicNotSupported             = -4;   // A given operation or resource is not supported
const TInt KScPanicInvalidValue             = -5;   // A value was invalid in the given context
const TInt KScPanicNoResourceConnection     = -6;   // Some external resource (eg. file server) 
                                                    // could not be contacted
//  7-9 rfu
const TInt KScUtilPanicOverFlow             = -10; // A buffer overflow
const TInt KScUtilPanicUnderFlow            = -11; // A buffer underflow
//  12-14 rfu
const TInt KScServerPanicInternalError      = -15; // Some internal error has occurred
const TInt KScServerPanicBadRequest         = -16; // A service request had an unknown value
const TInt KScServerPanicBadDescriptor      = -17; // A descriptor was too short or corrupted
const TInt KScServerPanicDescrNonNumeric    = -18;  
const TInt KScServerPanicMainSchedulerError = -19; // The active scheduler could not be activated
const TInt KScServerPanicSvrCreateServer    = -20; // Server creatíon failed
const TInt KScServerPanicCreateTrapCleanup  = -21; // CleanupStack creation failed
const TInt KScServerPanicBadCounterRemove   = -22; 
const TInt KScServerPanicBadSubsessionHandle= -23; // A subsession had an invalid handle
//  24-29 rfu
const TInt KScServicePanicAlreadyActive     = -30; // The service is still running the previous request
const TInt KScServicePanicBadFormat         = -31; // A parameter did not conform to a spesified format
const TInt KScServicePanicDataCorruption    = -32; // Some data (internal/external) has 
                                                   // been terminally corrupted
//  33-n proprietary panics

////////////////////////////////////////////////////////////////////////////////
//  Scard error codes
////////////////////////////////////////////////////////////////////////////////
const TInt KScErrUnknown                    = 1;  // An undefined internal error has occurred
const TInt KScErrGeneral                    = 2;  // An internal error
const TInt KScErrNotFound                   = 3;  // A search did not find the wanted resource
const TInt KScErrCancelled                  = 4;  // Operation was cancelled
const TInt KScErrTimeOut                    = 5;  // A command has timed out
const TInt KScErrNotSupported               = 6;  // The operation or resource (reader etc.) 
                                                  // is not supported
const TInt KScErrUnknownGroup               = 7;  // The reader group could not be identified
const TInt KScErrUnknownCard                = 8;  // The card could not be identified
const TInt KScErrUnknownReader              = 9;  // The reader could not be identified
const TInt KScErrBadArgument                = 10; // An argument has unacceptable value / does not
                                                  // conform to a spesified format
const TInt KScErrEmpty                      = 11; // A buffer or descriptor data area was empty, when it 
                                                  // was expected to contain data
const TInt KScErrFull                       = 12; // A buffer or descriptor is full
const TInt KScErrNullPointer                = 13; // A null pointer was encountered
const TInt KScErrNoMemory                   = 14; // There was not enough free memory to complete a task
const TInt KScErrInsufficientBuffer         = 15; // A buffer had insufficient length
const TInt KScErrFileFailure                = 16; // An operation could not complete due to file 
                                                  // handling errors
const TInt KScErrAlreadyExists              = 17; // A resource could not be installed because a resource
                                                  // of same type by that name already exists
const TInt KScErrInUse                      = 18; // A resource was in use


// 18 - 29 rfu

const TInt KScReaderErrNoCard               = 30; // The SC was removed from the reader
const TInt KScReaderErrNoReader             = 31; // The reader could not be reached
const TInt KScReaderErrUnpoweredCard        = 32; // The SC had no power on
const TInt KScReaderErrUnsupportedCard      = 33; // The SC could not be supported
const TInt KScReaderErrUnresponsiveCard     = 34; // The SC did not response
const TInt KScReaderErrCardFailure          = 35; // A SC failure
const TInt KScReaderErrReaderFailure        = 36; // A reader failure
const TInt KScReaderErrCommunicationFailure = 37; // A communication failure other than the two previous ones
const TInt KScReaderErrResponseTimeout      = 38; // A response time-out limit reached without a valid 
                                                  // response from the SC or the reader
const TInt KScReaderErrPowerFailure         = 39; // The reader has encountered a power failure
const TInt KScReaderErrNotReady             = 40; // The reader or the SC was not ready
const TInt KScReaderErrCannotConnect        = 41; // Reader handler is unable to connect to the reader
                                                  // or the SC
const TInt KScReaderErrUnsupportedTag       = 42; // In function GetCapabilities a unsupported 
                                                  // capability tag was requested
const TInt KScReaderErrUnsupportedProtocol  = 43; // In function NegotiateProtocol an unsupported
                                                  // protocol was requested
// 44 - 59 proprietary reader errors

const TInt KScServerErrIllegalChannel       = 61; // The logical channel the client attempted to
                                                  // use was not available for this client (no previous
                                                  // ManageChannel command)
const TInt KScServerErrIllegalOperation     = 62; // A client attempted to send a ManageChannel apdu via
                                                  // TransmitToCard() instead of ManageChannel()
// 62 - 79 rfu

//  File services
const TInt KScServiceErrDirNotFound         = 80; // The requested directory could not be found
const TInt KScServiceErrFileNotFound        = 81; // The requested file could not be found
const TInt KScServiceErrNoDir               = 82; // The request did not contain a valid directoryname
const TInt KScServiceErrNoFile              = 83; // The request did not contain a valid filename
const TInt KScServiceErrNoAccess            = 84; // The resource could not be accessed because of a 
                                                  // sharing violation
const TInt KScServiceErrCorruptedFile       = 85; // The requested file was corrupted
const TInt KScServiceErrRecordNotFound      = 86; // A requested record was not found
const TInt KScServiceErrRunning             = 87; // The service is already running a previous request
const TInt KScServiceErrEOF                 = 88; // An end of file was encountered before a successful
                                                  // completion of a task
const TInt KScServiceErrInvalidOffset       = 89; // The offset could not be mapped inside the file/resource
const TInt KScServiceErrWriteTooMany        = 90; // A write counter limit has been reached (?)
const TInt KScServiceErrFPEmpty             = 91; // A file pointer was empty (?)

// Application Management services
const TInt KScServiceErrInvalidAID          = 92; // The given application identifier did not match any
                                                  // applications on the SC
const TInt KScServiceErrApplicationBlocked  = 93; // The requested application was blocked
const TInt KScServiceErrNoSupportedApps     = 94; // The card contains no supportable applicatíons
const TInt KScServiceErrSecurityViolation   = 95; // The task could not be completed because of a security
                                                  // violation

//These tags from PC/SC, shoud be moved to scard.h 
////////////////////////////////////////////////////////////////////////////////
//  Tags for reader capabilities           Length   Encoding
////////////////////////////////////////////////////////////////////////////////
const TInt32 KVendorName     = 0x0100; //  32       ASCII string
const TInt32 KReaderType     = 0x0101; //  32       ASCII string
const TInt32 KReaderVersion  = 0x0102; //  4        Dword 0xMMmmbbb
                                       //               MM = major version
                                       //               mm = minor version
                                       //               bbbb = build number
const TInt32 KReaderSerial   = 0x0103; //  32       ASCII string

// Channel ID skipped

const TInt32 KAsyncProtocols = 0x0120; //  4        Dword 0x0000PPPP
                                       //        1 in a given position indicates
                                       //        support for associated protocol
                                       //        e.g 0x00000002 for T=1 support 
//other protocol parameters skipped

const TInt32 KPowerMgt       = 0x0131; //  1 (4)    0 = not supported
const TInt32 KAuthFeatures   = 0x0140; //  4        See PC/SC part 3 page 7

//Mechanical charasteristics skipped

//Vendor defined tags from 0x0180 - 0x01F0

////////////////////////////////////////////////////////////////////////////////
//  Tags for Card state                    Length   Encoding
////////////////////////////////////////////////////////////////////////////////
const TInt32 KCardPrecence   = 0x0300; //  1        0 = not present, 1 = present
const TInt32 KCardInterface  = 0x0301; //  1        0 = contact inactive, 
                                       //           1 = contact active
const TInt32 KCardType       = 0x0304; //  1        0 = unknown, 1 = 7816 async,
const TInt32 KCardStatus     = 0x0310; //  1        bit 1: card powered
                                       //           bit 2: card present
                                       //           bit 3: card reader id1 size
                                       //           bit 4: card reader present
                                       //           bit 5: card reader removable

////////////////////////////////////////////////////////////////////////////////
//  Tags for Reader Protocol options       Length   Encoding
////////////////////////////////////////////////////////////////////////////////
const TInt32 KCurrentProtocol= 0x0201; //  4        As with AsyncProtocols
const TInt32 KCurrentF       = 0x0203; //  4               
const TInt32 KCurrentD       = 0x0204; //  4               
const TInt32 KCurrentN       = 0x0205; //  4  

////////////////////////////////////////////////////////////////////////////////
//  Connection parameters
////////////////////////////////////////////////////////////////////////////////
const TInt KAnyReader               = 0x01;
const TInt KExplicitReader          = 0x02;
const TInt KExcludedReader          = 0x04;

const TInt KAnyCard                 = 0x10;
const TInt KATRSpesified            = 0x20;
const TInt KApplicationSpesified    = 0x40;
const TInt KServiceSpesified        = 0x80;

const TInt KNewCardsOnly            = 0x100;
const TInt KNewReadersOnly          = 0x200;

////////////////////////////////////////////////////////////////////////////////
//  Reader handler definitions
////////////////////////////////////////////////////////////////////////////////
_LIT( KReaderName, "SWIM Reader" );
const TInt KReaderID = 1;

_LIT( KGroupName, "SWIM Readers" );
const TInt KGroupID  = 1;

_LIT( KSwimReaderDLL, "SwimReader.dll" );

#endif      // CSCARDBASE_H

// End of File
