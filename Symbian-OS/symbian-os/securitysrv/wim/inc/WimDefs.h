/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definitions for WimClient, WimServer and WimPlugin
*
*/


#ifndef WIMDEFS_H
#define WIMDEFS_H


//  INCLUDES
#include <e32std.h>

//  CONSTANTS

typedef TUint8  TWimKeyType;
typedef TUint8  TWimApduStatus;

// Key types.
const TWimKeyType KWimDigitalSignature = 0x80;
const TWimKeyType KWimNonRepudiation   = 0x40;

// WimApdu errors
// These errors are defined in WIM library design spec.
const TWimApduStatus KWimApduOk                 = 0x00;
const TWimApduStatus KWimApduTransmiossionError = 0x01;
const TWimApduStatus KWimApduCardMute           = 0x02;
const TWimApduStatus KWimApduCardDisconnected   = 0x03;
const TWimApduStatus KWimApduNoCause            = 0x04;
const TWimApduStatus KWimApduReaderNotValid     = 0x05;
const TWimApduStatus KWimApduFormatError        = 0x06;
const TWimApduStatus KWimApduTypeNotValid       = 0x07;
const TWimApduStatus KWimApduReaderRemoved      = 0x08;
const TWimApduStatus KWimApduCardRemoved        = 0x09;
const TWimApduStatus KWimApduCardReaderBusy     = 0x0a;
const TWimApduStatus KWimApduCardPoweredOff     = 0x0b;
const TWimApduStatus KWimApduNoMemory           = 0xa0;

//  DATA TYPES

// State of WIM server
enum TWimServerRunningState
    {
    EWimServerStateUnknown = 0,
    EWimServerRunning      = 1,
    EWimServerNotRunning   = 2
    };

// Data types Cert
enum TWimCertLocation
    {
    EWimCertLocationCertMan,
    EWimCertLocationPhoneMemory,
    EWimCertLocationPhoneMemoryURL,
    EWimCertLocationWIMCard,
    EWimCertLocationWIMURL
    };

enum TWimCertType
    {
    EWimCertTypeCA,
    EWimCertTypePersonal
    };

enum TWimCertFormat
    {
    EWimWTLSCertificate,
    EWimX509Certificate,
    EWimX968Certificate,
    EWimCertificateURL
    };

enum TWimEntryType
    {
    EWimEntryTypeAll,
    EWimEntryTypeCA,
    EWimEntryTypePersonal
    };

enum TWimKeyLocation
    {
    EWimKeyLocationWIMCard,
    EWimKeyLocationPhoneMemory,
    EWimKeyLocationNoKey
    };

enum TWimCertStoringStatus
    {
    EWimCertNotStored
    };

// Enumerator for certificate location
enum TWimCertificateCDF
    {
    EWimUnknownCDF      = 0,
    EWimCertificatesCDF = 0x20,
    EWimTrustedCertsCDF = 0x40,
    EWimUsefulCertsCDF  = 0x80
    };

#endif  // WIMDEFS_H

// End of File
