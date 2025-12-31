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
* Description:  General definitions used in message passing between
*                client and server
*
*/


#ifndef __WIMCLSV_H__
#define __WIMCLSV_H__

//  INCLUDES
#include <e32base.h>
#include <securitydefs.h>
#include "WimOpcode.h" 
#include "WimConsts.h"
#include "WimDefs.h"
#include <secdlg.h>


//  CONSTANTS
_LIT( KWIMServerName, "WIMIServer" );

// Number allocation for our certificates. Includes only WTLS CA
// certificates.
const TUid KUidWTLSTrustedCerts = {0x1000AA53}; 

// Number allocation for our certificates. Includes only X509 client
// certificates.
const TUid KUidWTLSUserCerts = {0x1000AB5B}; 

// A version must be specifyed when creating a session with the server
const TUint KWIMServMajorVersionNumber = 1;
const TUint KWIMServMinorVersionNumber = 0;
const TUint KWIMServBuildVersionNumber = 1;

// needed for creating server thread.
const TUint KWIMDefaultHeapSize = 0x100000;

// Maximum count of applications trusting to certificate
const TInt KMaxApplicationCount = 50;

// Mask for PIN status. Keeping MSBs and masking off LSBs.
const TUint8 KPINStatusMask = 0xF0;
const TUint8 KWimAuthObjectBlocked= 0x08;

// Typedefs for the references to wim, pin and certificate 
typedef TUint32                 TWimAddress;
typedef TWimAddress*            TWimAddressList;
typedef TWimAddress             TPinAddress; 
typedef TWimAddress*            TPinAddressList;
typedef TUint32                 TCertificateAddress;
typedef TCertificateAddress*    TCertificateAddressList;

// Server panic reasons
enum TWimServerPanic
    {
    EWimMainSchedulerError,
    EWimSvrCreateServer,
    EWimCreateTrapCleanup,
    EWimSubSessionRefCountInvalid,
    EWimThreadOpenError,
    EWimIncorrectPolicy
    };

// PIN status
typedef TUint8 TWimPinStatus;

//PIN Type
enum TWimPin
    {
    EWimPinG,
    EWimPinNR
    };

// WIM information structure
struct TWimSecModuleStruct
    {
    TBuf<KLabelLen>        iLabel;
    TBuf<KManufacturerLen> iManufacturer;
    TBuf<KSerialNumberLen> iSerialNumber;
    TUint8                 iVersion;
    TUint8                 iReader;   
    TPinAddress            iRefPinG;
    };

// PIN information structure
struct TWimPinStruct 
    {
    TWimPin         iPinType;
    TBuf<KLabelLen> iLabel;
    TWimPinStatus   iStatus;
    TUint8          iPinNumber;
    };

// Certificate info structure
struct TWimCertAddParameters
    {
    TBuf8<KLabelLen> iLabel;
    TBuf8<KKeyIdLen> iKeyId;
    TBuf8<KCertHash> iCaId;
    TWimCertType     iUsage;
    TWimCertFormat   iFormat;
    };

// Certificate details
struct TWimCertDetails
    {
    TPtr8* iCert; //The whole cert
    };

// Address of a certificate to be removed
struct TWimCertRemoveAddr
    {
    TCertificateAddress iCertAddr;
    TWimCertLocation    iLocation;
    };

// Certificate information structure
struct TWimCertInfo
    {
    TBuf8<KLabelLen>   iLabel;
    TBuf8<KKeyIdLen>   iKeyId;
    TBuf8<KCertHash>   iCAId;
    TBuf8<KIssuerHash> iIssuerHash;
    TUint8             iCDFRefs;
    TUint              iUsage;
    TUint              iType;
    TUint              iCertlen; 
    TUint              iModifiable;
    TUint              iTrustedUsageLength;
    };

// Certificate extra information structure
struct TCertExtrasInfo
    {
    TPtr*  iTrustedUsage;
    TUint8 iCDFRefs;
    };

// Key info struct
struct TKeyInfo
    {
    TUint8           iKeyNumber;
    TUint8           iType; //1 = rsa, 2 = ECC
    TUint16          iUsage;
    TBuf8<KLabelLen> iLabel;
    TBuf8<KKeyIdLen> iKeyId;
    TUint16          iLength;
    TUint8           iPinNumber;
    };

// Digital signature struct
struct TKeySignParameters
    {
    TPtr8*           iSigningData;
    TBuf8<KKeyIdLen> iKeyId;
    TPtr8*           iSignature;
    };

//Public key exportion struct
struct TExportPublicKey
    {
    TPtr8*           iPublicKey;
    TBuf8<KKeyIdLen> iKeyId;
    };

// struct for PIN state request
struct TPINStateRequest
    {
    TBool iEnable;
    TBool iRetry;
    };

struct TOmaProv
    {
    TInt   iSize;
    TInt   iOmaType;
    TPtr8* iOmaData;
    };
    
struct TJavaProv
    {
    TBuf8<8>* iPath;
    TInt   iSize;
    TPtr8* iJavaData;	
    };    

struct TTrustSettings
    {
    TBool    iTrusted;
    TInt32   iUids[KMaxApplicationCount];
    TInt     iApplicationCount;
    };

#endif      // __WIMCLSV_H__

// End of File
