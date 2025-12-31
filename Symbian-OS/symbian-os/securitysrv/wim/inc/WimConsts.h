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
* Description:  Consts for WimClient, WimPlugin and WimServer
*
*/


#ifndef WIMCONSTS_H
#define WIMCONSTS_H


//  INCLUDES
#include <e32base.h>
#include <data_caging_path_literals.hrh>

// Common error codes
const TInt KWimSMInfoError         = -7;
const TInt KWimCardDriverInitError = -37;


// Extract of the constant values as specified in WIM spec and ISO/IEC 7812-1
const TInt KLabelLen         = 255; 
const TInt KManufacturerLen  = 32;
const TInt KSerialNumberLen  = 20;
const TInt KVersionNumberLen = 20; //Not sure of the length, may be less than 20
const TInt KIssuerHash       = 20;
const TInt KCertHash         = 20;
// Hacking this. Some spec somewhere says it is 4, WIMI returns 20.
const TInt KKeyIdLen         = 20;
const TInt KPkcs15IdLen      = 20;
const TInt KSoftId           = 255;
const TInt KWimMaxCount      = 8;
const TInt KMaxPinLen        = 8;
const TInt KMinPinLen        = 4;

// PKCS15KeyUsageFlags
const TInt KPkcs15KeyUsageFlagsEncrypt        = 0x8000; 
const TInt KPkcs15KeyUsageFlagsDecrypt        = 0x4000; 
const TInt KPkcs15KeyUsageFlagsSign           = 0x2000; 
const TInt KPkcs15KeyUsageFlagsSignRecover    = 0x1000; 
const TInt KPkcs15KeyUsageFlagsWrap           = 0x0800; 
const TInt KPkcs15KeyUsageFlagsUnwrap         = 0x0400; 
const TInt KPkcs15KeyUsageFlagsVerify         = 0x0200; 
const TInt KPkcs15KeyUsageFlagsVerifyRecover  = 0x0100; 
const TInt KPkcs15KeyUsageFlagsDerive         = 0x0080; 
const TInt KPkcs15KeyUsageFlagsNonRepudiation = 0x0040; 

// WIMSTA_XX errors
const TUint8 KWimStatusOK      = 0x00; // WIMSTA_OK
const TUint8 KWimStatusIOError = 0xa2; // WIMSTA_IO_ERROR

// Server startup specific
_LIT( KWimServerFile, "WimServer.EXE" );

const TUid KWimServerUid = {0x101F79DD};
// path is \\private\\<WimServerUid>
_LIT( KWimTrustSettingsStorePath, "\\private\\101F79DD\\" );

const TInt KWimServerTerminatingMaxRetryCount = 10;
const TInt KWimServerTerminatingRetryTimeout  = 200000; /* 200 msec */

// Timeout limit values for WIM SetCloseAfter (in seconds)
const TInt KWimNoTimeout  = -1;    // No timeout
const TInt KWimTimeoutMax = 2146;  // Maximum when converted to microseconds

// Public Key export
const TInt KPublicKeyLength = 512;

// Digital signature
const TInt KMaxRSADigestSize   = 512;
const TInt KMaxSignatureLength = 512;

#endif      // WIMCONSTS_H

// End of File
