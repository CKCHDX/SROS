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
* Description:  Operation codes for WimClient and WimServer message passing
*
*/


#ifndef WIMOPCODE_H
#define WIMOPCODE_H

// Opcodes used in message passing between client and server
enum TWimServRqst
    {
    ENon,               // Opcode has not been assigned yet
    /* -- WIM management ---------------------------------------------------- */
    EWimInitialize,     // Initialize WIM
    ECancelWimInitialize, //Cancel Initialize WIM
    EGetWIMCount,       // WIM query operations code
    EGetWIMRefs,        // Get the array of Wim structures
    EGetWIMInfo,        // Get WIM info (manufacturer etc.)
    EIsWIMOpen,         // WIM operations code
    ECloseWIMAfter,     // Set closing time for WIM
    EGetCloseWIMAfter,  // Get value for WIM closing timeout
    EWimTimeRemaining,  // Time that WIM is still open   
    EWIMClose,          // Closes the WIM
    ENotifyOnRemoval,   // Notify client when the token has been removed
    ECancelNotifyOnRemoval, // Cancel NotifyOnRemoval request  
    EFreeMemory,        // Free the address list
    EFreeWIMMemoryLst,  // Free the address list
    EFreeMemoryLst,     // Free the address list
    
    /* -- PIN & Key management ---------------------------------------------- */
    EGetPINCount,       // Get count of PINs in WIM
    EGetPINRefs,        // Get the array of Pin structures
    EGetPINInfo,        // Get PIN info
    EGetPINsInfo,
    EIsPinBlocked,      // Check if PIN is blocked or not
    EIsDisabledPinBlocked,    
    EChangePINReq,      // Change the PIN value
    ECancelChangePin,   //Cancel change Pin value
    EEnablePINReq,      // Enable PIN
    ECancelEnablePin,   //Cancel enable pin
    EUnblockPinReq,     // Unblock blocked PIN
    ECancelUnblockPin,  //Cancel unblock pin
    EVerifyPinReq,      // Verify PIN
    ECancelVerifyPin,   //Cancel verify pin
    EVerifyDisabledPinReq, //Verify Disabled PIN
    ECancelDisabledPin, //Cancel disabled pin
    EGetKeyDetails,     // Get Key details based on public key
    EGetKeyList,        // Get list of keys in WIM
    EDoesPvKeyExist,    // Check if private key exists
    ERetrieveAuthObjsInfo,   // Get the auth Obj info
    
    /* -- Certificate management -------------------------------------------- */
    EGetWIMCertLst,     // Get the certifcate list
    EGetCertCount,      // Get the certificate count
    EGetWIMCertDetails, // Get the certificate details
    EGetCertExtras,     // Get certificate extra data
    EStoreCertificate,  // Store certificate to card
    ERemoveCertificate, // Remove certificate from card
    EExportPublicKey,   // Export public key of certificate
    
    /* -- Digital signature ------------------------------------------------- */
    ESignTextReq,       // Sign some text

    /* -- OMA Provisioning -------------------------------------------------- */
    EGetOMAFileSize,    // Get OMA Provisioning file size
    EGetOMAFile,        // Get OMA Provisioning file
   
    /* -- Java Proisioning ---------------------------------------------------*/
    EGetACIFFileSize,
    EGetACIFFile,
    EGetACFFileSize,
    EGetACFFile,
    EGetLabelAndPath,
     
    /* -- Trust Settings Store ---------------------------------------------- */
    EGetTrustSettings,          // Get trust settings for given certificate
    ESetApplicability,          // Set applicability for given certificate
    ESetTrust,                  // Set trust flag for given certificate
    ESetDefaultTrustSettings,   // Set default settings for given certificate
    ERemoveTrustSettings,       // Remove trust settings of given certificate
    ECancelTrustSettings        // Cancel any issued asynchronous call
    };

#endif      // WIMOPCODE_H
// End of File
