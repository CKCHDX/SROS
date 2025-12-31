/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  GBA Common definitions
*
*/


#ifndef GBACOMMON_H
#define GBACOMMON_H

#include <e32base.h>

_LIT8( KHTTPFilterGBAName, "GBA2");
_LIT(KGbaServerName,"gbaserver2");
_LIT8(KHTTPTag,"http://");
_LIT8(KHTTPSTag,"https://");

//the server version. A version must be specified when 
//creating a session with the server
const TUint KGbaServMajorVersionNumber=0;
const TUint KGbaServMinorVersionNumber=1;
const TUint KGbaServBuildVersionNumber=1;

// Dictionary store UID for GBA ini
const TUid KGbaIniUid = { 0x00001 };
const TUid KGbaBSFConfiguration = { 0x00002 };

// Dictionary store UID for GBA credentials
// IMPI, Ks, Rand, B-TID, key lifetime
const TUid KUidGBACredRoot = { 0x00001 };
const TUid KUidIMPI = { 0x00002 };
const TUid KUidKs = { 0x00003 };
const TUid KUidRand = { 0x00004 };
const TUid KUidBTID = { 0x00005 };
const TUid KUidkeylifetime = { 0x00006 };
const TUid KUidGBARunType = { 0x00007 };
const TUid KUidGBAUAvail = { 0x00008 };

//opcodes used in message passing between client and server
enum TGbaServRqst
    { 
    EGbaServRequestBootstrap,
    EGbaServCancelRequestBootstrap,
    EGbaServQueryOption,
    EGbaServWriteOption,
    EGbaServIsGBAUSupported,
    };

#endif // GBACOMMON_H

//EOF
