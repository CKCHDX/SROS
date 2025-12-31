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
* Description:  WIMI definitions used by WimServer and WimClient. 
*               All WIMI.lib API functions are described in this file.
*
*/


#ifndef WIMI_H
#define WIMI_H

#include <e32base.h>
#include <libc/stddef.h>     // For size_t

#ifdef __cplusplus
extern "C" {
#endif


// CONSTANTS

/* CDF of certificate */
const TUint WIMLIB_CERTIFICATES_CDF  = 0x20;
const TUint WIMLIB_TRUSTEDCERTS_CDF  = 0x40;
const TUint WIMLIB_USEFULCERTS_CDF   = 0x80;

/* PIN flags */
const TUint8 WIMI_PF_DisableAllowed  = 0x10;
const TUint8 WIMI_PF_UnblockDisabled = 0x20;
const TUint8 WIMI_PF_ChangeDisabled  = 0x40;
const TUint8 WIMI_PF_Enabled         = 0x80;


/* Certificate usage */
const TUint WIMI_CU_Client  = 0;
const TUint WIMI_CU_CA      = 1;

/* Certificate type */
const TUint WIMI_CT_WTLS    = 1;
const TUint WIMI_CT_X509    = 2;
const TUint WIMI_CT_X968    = 3;
const TUint WIMI_CT_URL     = 4;
const TUint WIMI_CT_UNKNOWN = 255;

/* APDU driver status codes */
const TUint8 WIMI_APDU_OK                 = 0;
const TUint8 WIMI_APDU_TRANSMISSION_ERROR = 1;


// DATA TYPES 

/* Return type for status codes */
typedef TUint16 WIMI_STAT;

/* Reference types */
typedef void WIMI_Ref_t;
typedef WIMI_Ref_t* WIMI_Ref_pt;

/* Reference list */
typedef WIMI_Ref_pt* WIMI_RefList_t;

/* Binary data */
typedef struct
    {
    TUint8* pb_buf;
    TUint16 ui_buf_length;
    TUint8  filler_byte[2];     /* filler byte to ensure alignment*/
    } WIM_BIN_DATA;

typedef WIM_BIN_DATA WIMI_BinData_t;

/* Label list */
typedef WIMI_BinData_t* WIMI_labelptr_t;
typedef WIMI_labelptr_t* WIMI_labellist_t;

/* Hash context type */
typedef void WIMI_Hash_t;

/* Transaction ID */
typedef void *WIMI_TransactId_t;

/* WTLS Certificate info structure */
typedef struct 
    { 
    TUint16 ParsedLen;
    TUint8  SignAlg;
    TUint8* Issuer;
    TUint16 IssuerLen;
    TUint32 ValidNotBefore;
    TUint32 ValidNotAfter;
    TUint8* Subject;
    TUint16 SubjectLen;
    TUint8  PublicKeyType;
    TUint16 exponentlen;
    TUint8* exponent;
    TUint16 moduluslen;
    TUint8* modulus;
    TUint16 signaturelen;
    TUint8* signature;
    } WIMI_WTLS_CertInfo_t;

/* File info structure for Provisioning files */
typedef struct
    {
    WIM_BIN_DATA  t_label;
    WIM_BIN_DATA  t_oid;
    WIM_BIN_DATA  b_path;
    TUint16       ui_flags; /* bitmask */
    TUint16       ui_file_size;
    TUint8        ui_pin_ref;
    TUint8        filler_byte[3];     /* filler byte to ensure alignment*/
    } WIMI_File_Info;

/* Status codes */
enum __WIMI_STATUS_CODES
    { 
    WIMI_Ok = 0,
    WIMI_Err,
    WIMI_ERR_Internal,
    WIMI_ERR_OutOfMemory,
    WIMI_ERR_BadParameters,
    WIMI_ERR_ServerCertRejected,
    WIMI_ERR_WrongReference,
    WIMI_ERR_CipherNotSupported,
    WIMI_ERR_MACANotSupported,
    WIMI_ERR_SessionNotSet,
    WIMI_ERR_WrongKES,
    WIMI_ERR_DecodeError,
    WIMI_ERR_ExpiredReference,
    WIMI_ERR_NoKey,
    WIMI_ERR_CertNotYetValid,
    WIMI_ERR_CertExpired,
    WIMI_ERR_UnknownCA,
    WIMI_ERR_CertParseError,
    WIMI_ERR_UnsupportedCertificate,
    WIMI_ERR_SignCheckFailed,
    WIMI_ERR_NoWIM,
    WIMI_ERR_NoKES,
    WIMI_ERR_HashInitFailed,
    WIMI_ERR_HashUpdateFailed,
    WIMI_ERR_HashCopyFailed,
    WIMI_ERR_HashFinalFailed,
    WIMI_ERR_HashInvalidAlgorithm,
    WIMI_ERR_UnknownKeyId,
    WIMI_ERR_KeyStorageFull,
    WIMI_ERR_BadKey,
    WIMI_ERR_CertStorageFull,
    WIMI_ERR_BadCert,
    WIMI_ERR_PStorageError,
    WIMI_ERR_CertNotFound,
    WIMI_ERR_KeyNotFound,
    WIMI_ERR_BadReference,
    WIMI_ERR_OperNotSupported,
    WIMI_ERR_BadPIN,
    WIMI_ERR_CardDriverInitError,
    WIMI_ERR_CardIOError,
    WIMI_ERR_AlgorithmNotYetImplemented,
    WIMI_ERR_PINBlocked,
    WIMI_ERR_OperNotAllowed,
    WIMI_ERR_NoCert,
    WIMI_ERR_CAcertNotYetValid,
    WIMI_ERR_CAcertExpired,
    WIMI_ERR_BadCAcert,
    WIMI_ERR_UserCancelled = 0x8F
    };

/* Operation codes for some WIMI calls */
enum TWimiOpcodes
    {
    ENoOperation,                   // No operation
    EInitializationCertListHashing, // Cert hash
    EOMAProvContentSync             // Synchronous OMA Provisioning file fetch
    };


// =============================================================================
// Callback functions. User of WIMI (WimServer) has to implement these
// functions.
// =============================================================================

/* ------------ WIMI initialization and closing --------------------------- */
/**
   WIMI init respond callback.
   aStatus - operation status 
**/
typedef void ( *WIMI_InitResp_t )( WIMI_STAT aStatus );

/**
   WIMI close respond callback 
   aStatus - operation status 
**/
typedef void ( *WIMI_CloseDownResp_t )( WIMI_STAT aStatus );

/* ------------ Digital signature ----------------------------------------- */
/**
   Respond to the digital signature operation 
   aTrId    - transaction id
   aStatus  - operation status 
   aSignLen - signature length
   aSign    - signature 
**/
typedef void ( *WIMI_SignResp_t )( WIMI_TransactId_t aTrId,
                                   WIMI_STAT aStatus,
                                   TUint8 aSignLen,
                                   TUint8* aSign );

/* ------------ Certificate management ------------------------------------ */
/**
   Respond to the certificate fetch operation
   aTrId    - transaction id
   aStatus  - operation status 
   aCertRef - the reference to certificate that was fetched
   aCertLen - certificate content length
   aCert    - certificate content 
**/
typedef void ( *WIMI_CertificateResp_t )( WIMI_TransactId_t aTrId,
                                          WIMI_STAT aStatus,
                                          WIMI_Ref_t* aCertRef,
                                          TUint16 aCertLen,
                                          TUint8* aCert );

/**
   Respond to the certificate delete operation
   aTrId - transaction id
   aStatus - operation status 
**/
typedef void ( *WIMI_CertificateDeleteResp_t )( WIMI_TransactId_t aTrId,
                                                WIMI_STAT aStatus );

/**
   Respond to the certificate store operation
   aTrId - transaction id
   aStatus - operation status 
   aCertRef - the reference to certificate that was stored 
**/
typedef void ( *WIMI_CertificateStoreResp_t )( WIMI_TransactId_t aTrId,
                                               WIMI_STAT aStatus,
                                               WIMI_Ref_t* aCertRef );


/* ------------ PIN management -------------------------------------------- */
/**
   Respond to the Verify PIN operation
   aTrId - transaction id
   aStatus - operation status 
**/
typedef void ( *WIMI_VerifyPINResp_t )( WIMI_TransactId_t aTrId,
                                        WIMI_STAT aStatus );

/**
   Respond to the Change PIN operation
   aTrId - transaction id
   aStatus - operation status 
**/
typedef void ( *WIMI_ChangePINResp_t )( WIMI_TransactId_t aTrId,
                                        WIMI_STAT aStatus );

/**
   Respond to the Unblock PIN operation
   aTrId - transaction id
   aStatus - operation status 
**/
typedef void ( *WIMI_UnblockPINResp_t )( WIMI_TransactId_t aTrId,
                                         WIMI_STAT aStatus );

/**
   Respond to the Enable PIN operation
   aTrId - transaction id
   aStatus - operation status 
**/
typedef void ( *WIMI_EnablePINResp_t )( WIMI_TransactId_t aTrId,
                                        WIMI_STAT aStatus );

/* ------------ Hardware driver support ----------------------------------- */
/**
   Card inserted response
   aReader - reader ID
   aStatus - operation status 
   aWimReg - WIM reference
**/
typedef void ( *WIMI_CardInsertedResp_t )( TUint8 aReader,
                                           WIMI_STAT aStatus,
                                           WIMI_Ref_t* aWimRef );

/**
   Open WIM request (connection request)
   aReader  - reader ID
**/
typedef TUint8 ( *WIMI_Open_t )( TUint8 aReader );

/**
   Close WIM request (close connection)
   aReader  - reader ID
**/
typedef TUint8 ( *WIMI_Close_t )( TUint8 aReader );

/**
   APDU request
   aReader  - reader ID
   aApdu    - APDU
   aApduLen - length of APDU
**/
typedef void ( *WIMI_APDUReq_t )( TUint8 aReader,
                                  TUint8* aApdu,
                                  TUint16 aApduLen );

/**
   Get ATR request
   aReader - reader ID
**/
typedef void ( *WIMI_GetATRReq_t )( TUint8 aReader );

/**
   Get reader list request
**/
typedef void ( *WIMI_GetReaderListReq_t )( void );

/**
   Read SIM file request
   aReader - reader ID
   aPath   - path to read from
   aOffset - offset
   aSize   - size to be read
**/
typedef void ( *WIMI_ReadSimFileReq_t )( TUint8 aReader,
                                         TUint8* aPath,
                                         TUint16 aOffset,
                                         TUint16 aSize );

/**
   Provisioning file response
   aTrId    - transaction ID
   aStatus  - operation status
   aFileLen - lenght of file
   aFile    - file content
**/
typedef void (*WIMI_ProvSCFileResp_t)( WIMI_TransactId_t aTrId,
                                       WIMI_STAT aStatus,
                                       TUint16 aFileLen,
                                       TUint8* aFile );

/* WIMI Callbacks */
typedef struct __WIMI_CALLBACKS
    {
    WIMI_InitResp_t                InitOkResp;
    WIMI_CloseDownResp_t           CloseDownResp;
    WIMI_SignResp_t                SignResp;
    WIMI_CertificateResp_t         CertificateResp;
    WIMI_CertificateDeleteResp_t   CertificateDeleteResp;
    WIMI_CertificateStoreResp_t    CertificateStoreResp;
    WIMI_VerifyPINResp_t           VerifyPINResp;
    WIMI_ChangePINResp_t           ChangePINResp;
    WIMI_UnblockPINResp_t          UnblockPINResp;
    WIMI_EnablePINResp_t           EnablePINResp;
    WIMI_CardInsertedResp_t        CardInsertedResp;
    WIMI_Open_t                    Open;
    WIMI_Close_t                   Close;
    WIMI_APDUReq_t                 APDUReq;
    WIMI_GetATRReq_t               GetATRReq;
    WIMI_GetReaderListReq_t        GetReaderListReq;
    WIMI_ReadSimFileReq_t          ReadSimFileReq;
    WIMI_ProvSCFileResp_t          ProvSCFileResp;
    } WIMI_Callbacks_t;

/* ------------ Memory allocation/deallocation functions ------------------ */

/**
   Allocate memory block
   aMemorySize - size of memory block to be allocated
**/
void* WSL_OS_Malloc( size_t aMemorySize );

/**
   Deallocate memory block
   aBuffer - Pointer to memory to be freed
**/
void WSL_OS_Free( void* aBuffer );

/* ------------ WIMI initialization and closing --------------------------- */

/**
   WIMI initialization request. 
   WIMI should be initialized before WIM operations.
   aCb parameter should point to the temporary structure 
   which can be deallocated after function call
**/
WIMI_STAT WIMI_InitializeReq( WIMI_Callbacks_t* aCb );

/**
   WIMI close request.
**/
WIMI_STAT WIMI_CloseDownReq( void );

/* ------------ Referencies ----------------------------------------------- */
/**
   Copy and dealloc references 
   Creates a copy of reference. Function doesn't check reference validity.
   Returns NULL in case of memory allocation failure.
   aRef - reference to be copied
**/
WIMI_Ref_t* copy_WIMI_Ref_t( WIMI_Ref_t* aRef );

/* Deallocates memory occupied by reference.
   aRef - reference to be freed
**/
void free_WIMI_Ref_t( WIMI_Ref_t* aRef );

/**
   Creates a copy of reference list. Function doesn't check references
   validity. Returns NULL in case of memory allocation failure.
   aList - reference list to be copied
**/
WIMI_RefList_t copy_WIMI_RefList_t( WIMI_RefList_t aList );

/**
   Deallocates memory occupied by reference list.
   aList - reference list to be freed
**/
void free_WIMI_RefList_t( WIMI_RefList_t aList );

/* ------------ Digital signature ----------------------------------------- */
/**
   Digital signature request. Signed data is returned in callback.
   aTrId    - transaction id. 
              User application data that will be provided in respond callback
              to match request/response.
   aHash    - SHA1 hash of data to be signed.
   aHashLen - length of hash
   aKeyRef  - key reference which should be used for sign operaition.
   aPin     - PIN code for the referenced key.
**/
WIMI_STAT WIMI_SignReq( WIMI_TransactId_t aTrId,
                        TUint8* aHash,
                        TUint8 aHashLen,
                        WIMI_Ref_t* aKeyRef,
                        WIMI_BinData_t* aPin );

/* ------------ Label list ------------------------------------------------ */
/**
   Returns label list for specified reference list. 
   All references in a list should be of one type.
   Possible reference types are: PIN, KEY or Certificate.
   aElNum   - IN  - number of elements
   aRefList - IN  - list of references
   aLabelList - OUT - string list. Should be freed by caller. 
**/
WIMI_STAT WIMI_GetLabelList( TUint8 aElNum,
                             WIMI_RefList_t aRefList,
                             WIMI_labellist_t* aLabelList );

/* ------------ Key management -------------------------------------------- */
/**
   Returns key list for one WIM.
   aWimRef  - IN  - WIM reference
   aKeyNum  - OUT - number of returned keys
   aRefList - OUT - key list. Should be freed by caller.
**/
WIMI_STAT WIMI_GetKeyListByWIM( WIMI_Ref_t* aWimRef,
                                TUint16* aKeyNum,
                                WIMI_RefList_t* aRefList );

/**
Returns key reference by given key hash. 
   aHash   - IN  - SHA1 hash of the public key.
   aKeyRef - OUT - Allocated key reference. Should be freed by caller.
**/
WIMI_STAT WIMI_GetKeyByHash( TUint8* aHash,
                             WIMI_Ref_t** aKeyRef );

/** 
   Returns info for referenced key.
   aKeyRef    - IN  - key reference
   aWimRef    - OUT - reference to WIM where certificate reside. Freed by caller
   aPin       - OUT - PIN code that protects key
   aKeyType   - OUT - key type (1 = RSA, 2 = ECC)
   aKeyNumber - OUT - the number of current key
   aPinNumber - OUT - PIN number
   aUsage     - OUT - PKCS15 key usage flags. See #defines in wimi_types.h 
   aKeyId     - OUT - hash of public key. Should be deallocated by caller.
   aLabel     - OUT - key label. Should be deallocated by caller
   aKeyLength - OUT - key length in bits
**/
WIMI_STAT WIMI_GetKeyInfo( WIMI_Ref_t* aKeyRef,
                           WIMI_Ref_t** aWimRef,
                           WIMI_Ref_t** aPin,
                           TUint8* aKeyType,
                           TUint8* aKeyNumber,
                           TUint8* aPinNumber,
                           TUint16* aUsage,
                           WIMI_BinData_t* aKeyId,
                           WIMI_BinData_t* aLabel,
                           TUint16* aKeyLength ); /* in bits */


/* ------------ Certificate management ------------------------------------ */
/**
   Returns list of referencies to client certificates on all WIMs with
   the same client key reference.
   aKeyRef    - IN  - key reference
   aCertCount - OUT - certificate number
   aRefList   - OUT - allocated reference list. Should be freed by caller
**/
WIMI_STAT WIMI_GetClCertificateListByKey( WIMI_Ref_t* aKeyRef,
                                          TUint8* aCertCount,
                                          WIMI_RefList_t* aRefList );

/**
   Returns list of referencies to certificates on all WIMs with the same
   public key (hash).
   aHash      - IN  - key hash
   aUsage     - IN  - certificate purpose (0 = client, 1 = CA)
   aCertCount - OUT - certificate count
   aRefList   - OUT - allocated reference list. Should be freed by caller
**/
WIMI_STAT WIMI_GetCertificateListByKeyHash( TUint8* aHash,
                                            TUint8 aUsage,
                                            TUint8* aCertCount,
                                            WIMI_RefList_t* aRefList );

/**
   Returns list of referencies to certificates on all WIMs with the same
   public key (hash).
   aCAHash  - IN  - key hash
   aUsage   - IN  - certificate purpose
   aCertNum - OUT - certificate number
   aRefList - OUT - allocated reference list. Should be freed by caller
**/
WIMI_STAT WIMI_GetCertificateListByCAKeyHash( TUint8* aCAHash,
                                              TUint8 aUsage,
                                              TUint8* aCertNum,
                                              WIMI_RefList_t* aRefList );

/**
   Returns list of referencies to certificates on a WIM.
   aWimRef  - IN  - WIM reference
   aUsage   - IN  - Certificate usage (0 = client, 1 = CA)
   aCertNum - OUT - certificate number
   aRefList - OUT - allocated reference list. Should be freed by caller
**/
WIMI_STAT WIMI_GetCertificateListByWIM( WIMI_Ref_t* aWimRef,
                                        TUint8 aUsage,
                                        TUint8* aCertNum,
                                        WIMI_RefList_t* aRefList );

/**
   Returns information about certificate stored in WIM 
   aCert     - IN  - cert. reference.
   aWimRef   - OUT - reference to WIM where cert is stored. Freed by caller
   aLabel    - OUT - certificate label. pb_buf pointer in structure
                 points to allocated buffer that should be deallocated by caller
   aKeyId    - OUT - hash of public key. pb_buf pointer in WIMI_BinData_t struct
                points to allocated buffer that should be deallocated by caller
   aCAId     - OUT - hash of issuer public key. pb_buf pointer in WIMI_BinData_t
       structure points to allocated buffer that should be deallocated by caller
   aTrustedUsage - OUT - trusted usage OID's for this certificate
   aCDFRefs  - OUT - information about the CDF that the certificate was
                     referenced. Possible values: WIMLIB_CERTIFICATES_CDF,
                     WIMLIB_TRUSTEDCERTS_CDF, WIMLIB_USEFULCERTS_CDF
   aUsage    - OUT - 0 = client, 1 = CA
   aType     - OUT - WTLSCert(1), X509Cert(2), X968Cert(3), CertURL(4)
   aCertlen  - OUT - length of certificate content
**/
WIMI_STAT WIMI_GetCertificateInfo( WIMI_Ref_t* aCert,
                                   WIMI_Ref_t** aWimRef,
                                   WIMI_BinData_t* aLabel,
                                   WIMI_BinData_t* aKeyID,
                                   WIMI_BinData_t* aCAId,
                                   WIMI_BinData_t* aIssuerNameHash,
                                   WIMI_BinData_t* aTrustedUsage,
                                   TUint8* aCDFRefs,
                                   TUint8* aUsage,
                                   TUint8* aType,
                                   TUint16* aCertLen,
                                   TUint8* aModifiable );

/**
   Requests certificate contents from WIM.
   aTrId    - transaction id. 
              User application data that will be provided in respond callback
              to match request/response.
   aCertRef - certificate reference 
   aCert    - retrieved certificate
**/
WIMI_STAT WIMI_CertificateReqT( WIMI_TransactId_t aTrId,
                                WIMI_Ref_t* aCertRef, 
                                WIMI_BinData_t* aCert );

/**
   Requests certificate contents from WIM.
   aTrId - transaction id. 
          User application data that will be provided in respond callback
          to match request/response.
   aCertRef - certificate reference
**/
WIMI_STAT WIMI_CertificateReq( WIMI_TransactId_t aTrId,
                               WIMI_Ref_t* aCertRef );

/**
   Requests certificate to be deleted from WIM.
   aTrId    - transaction id. 
              User application data that will be provided in respond callback
              to match request/response.
   aCertRef - certificate reference
**/
WIMI_STAT WIMI_CertificateDeleteReq( WIMI_TransactId_t aTrId, 
                                     WIMI_Ref_t* aCertRef );
/**
   Requests certificate to be stored to WIM.
   aTrId   - transaction id. 
             User application data that will be provided in respond callback
             to match request/response.
   aWimRef - WIM reference where to store the certificate
   aLabel  - certificate label
   aKeyId  - hash of public key.
   aCAId   - hash of issuer public key.
   aIssuerNameHash  - Issuer name hash
   aSubjectNameHash - Subject name hash
   aUsage   - 0 = client, 1 = CA 
   aType    - WTLSCert(1), X509Cert(2), X968Cert(3), CertURL(4)
   aCertLen - certificate contents len
   aCert    - certificate contents
**/
WIMI_STAT WIMI_CertificateStoreReq( WIMI_TransactId_t aTrId, 
                                    WIMI_Ref_t* aWimRef,
                                    WIMI_BinData_t* aLabel,
                                    WIMI_BinData_t* aKeyId,
                                    WIMI_BinData_t* aCAId,
                                    WIMI_BinData_t* aIssuerNameHash,
                                    WIMI_BinData_t* aSubjectNameHash,
                                    TUint8 aUsage,
                                    TUint8 aType,
                                    TUint16 aCertLen,
                                    TUint8* aCert ); 

/* ------------ PIN management -------------------------------------------- */
/**
   Returns status of PIN
   aPinRef    - IN  - PIN reference
   aWimRef    - OUT - WIM reference. Should be freed by caller.
   aFlags     - OUT - bitmask WIMI_PF_Enabled, WIMI_PF_ChangeDisabled,
                      WIMI_PF_UnblockDisabled and WIMI_PF_DisableAllowed
   minlength  - OUT - minimal PIN length
   aPinNumber - OUT - the number of the pin
   aLabel     - OUT - PIN label - Should be deallocated by caller.
**/
WIMI_STAT WIMI_GetPINStatus( WIMI_Ref_t* aPinRef,
                             WIMI_Ref_t** aWimRef,  
                             TUint8* aFlags,
                             TUint8* aMinlength,
                             TUint8* aPinNumber,
                             WIMI_BinData_t* aLabel );

/**
   Returns Info of Authobj
   pWIMref    -IN  WIM reference
   authId     -IN  Authobj Id
   pt_lable   -OUT 
   flags      -OUT 
   pinnumber  -OUT
   minlength  -OUT
   maxlength  -OUT
   pinType    -OUT
   storedlength -OUT
   pinReference -OUT
   padChar      -OUT
**/                             
WIMI_STAT WIMI_GetAuthObjInfo( WIMI_Ref_t* pWIMref,
                              TUint32* authId,
                              WIMI_BinData_t* pt_label,  
                              TUint16* flags,
                              TUint8* pinnumber,
                              TUint8* minLength,
                              TUint8* maxLength,
                              TUint8* pinType,
                              TUint8* storedLength,
                              TUint8* pinReference,
                              TUint8* padChar );                             

/**
   Returns the PINs list for one WIM. 
   If aWimRef is NULL, then returns the list for all WIMs.
   aWimRef  - IN  - WIMI reference
   aPinNum  - OUT - number of PIN in list
   aRefList - OUT - allocated PIN list. Should be freed by caller.
**/
WIMI_STAT WIMI_GetPINList( WIMI_Ref_t* aWimRef,
                           TUint16* aPinNum,
                           WIMI_RefList_t* aRefList );

/**
   PIN verify request. If WIM general PIN is given then WIM becomes open after 
   successful verify operation 
   aTrId   - transaction id. 
             User application data that will be provided in respond callback to
             match request/response.
   aPinRrf - PIN reference
   aPinLen - entered PIN length
   aPin    - entered PIN data
**/
WIMI_STAT WIMI_VerifyPINReq( WIMI_TransactId_t aTrId,
                             WIMI_Ref_t* aPinRef,          
                             TUint8 aPinLen,
                             TUint8* aPin );                            

/**
   PIN change request. 
   aTrId      - transaction id. 
                User application data that will be provided in respond callback
                to match request/response.
   aPinRef    - PIN reference
   aOldPinLen - old PIN length
   aOldPin    - old PIN for granting operation permission
   aNewPinLen - new PIN length
   aNewPin    - new PIN that should replace old
**/
WIMI_STAT WIMI_ChangePINReq( WIMI_TransactId_t aTrId,
                             WIMI_Ref_t* aPinRef,          
                             TUint8 aOldPinLen,
                             TUint8* aOldPin,
                             TUint8 aNewPinLen,
                             TUint8* aNewPin ); 

/**
   PIN unblock request.
   aTrId          - transaction id. 
                    User application data that will be provided in respond
                    callback to match request/response.
   aPinRef        - PIN reference
   aUnblockPinLen - length of unblocking code
   aUnblockPin    - unblocking code
   aNewPinLen     - new PIN length
   aNewPin        - new PIN that should replace old after unblocking
**/
WIMI_STAT WIMI_UnblockPINReq( WIMI_TransactId_t aTrId,
                              WIMI_Ref_t* aPinRef,          
                              TUint8 aUnblockPinLen,
                              TUint8* aUnblockPin,
                              TUint8 aNewPinLen,
                              TUint8* aNewPin ); 

/**
   Enable or disable PIN request.
   aTrId   - transaction id. 
             User application data that will be provided in respond callback
             to match request/response.
   aPinRef - PIN reference
   aPinLen - PIN length
   aPin    - PIN for granting operation permission
   aEnable - if TRUE then enable else disable
**/
WIMI_STAT WIMI_EnablePINReq( WIMI_TransactId_t aTrId,
                             WIMI_Ref_t* aPinRef,          
                             TUint8 aPinLen,
                             TUint8* aPin,
                             TBool aEnable );

/* ------------ Hardware driver support ----------------------------------- */
/**
   Returns WIM information.
   aWimRef  - IN  - WIM reference.
   aFlags   - OUT - reserved for future use
   aSESet   - OUT - security environments bitset. See WIMI_SE_ constants
   aWimId   - OUT - WIM serial number - pb_buf pointer in WIMI_BinData_t
                    should be deallocated by caller 
   aManufacturerId - OUT - manufacturer info - Should be
                     deallocated by caller
   aLabel   - OUT - WIM label - pb_buf pointer in WIMI_BinData_t structure
                    should be deallocated by caller
   aReader  - OUT - reader ID
   aPinG    - OUT - reference to PIN-G
   aSim     - OUT - if TRUE then WIM is SIM only
   aVersion - OUT - version
**/
WIMI_STAT WIMI_GetWIMInfo( WIMI_Ref_t* aWimRef,
                           TUint16* aFlags,
                           TUint8* aSESet,
                           WIMI_BinData_t* aWimId,
                           WIMI_BinData_t* aManufacturerId,
                           WIMI_BinData_t* aLabel,
                           TUint8* aReader, 
                           WIMI_Ref_t** aPinG,
                           TBool* aSim,
                           TUint8* aVersion);

/**
   Returns TRUE if WIM is already opened by general PIN 
   aWimRef - WIM reference
**/
TUint8 WIMI_IsWIMOpened( WIMI_Ref_t* aWimRef );

/**
   Closes the WIM. WIM can be reopened with VerifyPIN operation.
   aWimRef - WIM reference
**/
WIMI_STAT WIMI_CloseWIM( WIMI_Ref_t* aWimRef );

/**
   Returns the reference of WIM currently associated with reader. 
   Returns valid reference if WIM card is inserted and initialized,
   other case NULL.
   aReader - reader number - 0-7
**/
WIMI_Ref_t* WIMI_GetWIMRef( TUint8 aReader );

/**
   Starts the initialization of newly inserted card.
   aReader - reader number - 0-7
**/
WIMI_STAT WIMI_CardInsertedReq( TUint8 aReader );

/**
   Informs WIMI that card was removed from reader.
   aReader - reader number - 0-7
**/
WIMI_STAT WIMI_CardRemoved( TUint8 aReader );

/**
   APDU respond from smartcard driver processing.
   aReader - reader number - 0-7 
   aStatus - WIMI_APDU_ codes
   aAPDU   - received APDU
   aSw     - status word for APDU codec support
**/
void WIMI_APDUResp( TUint8 aReader,
                    TUint8 aStatus,
                    WIMI_BinData_t* aAPDU,
                    TUint16 aSw );

/**
   ATR respond from smartcard driver processing.
   aReader - reader number - 0-7 
   aStatus - WIMI_APDU_ codes
   aATR    - ATR
**/
void WIMI_GetATRResp( TUint8 aReader,
                      TUint8 aStatus,
                      WIMI_BinData_t* aATR );

/**
   Respond for a reader list request.
   aStatus - WIMI_APDU_ codes
   aList - list of card reader statuses
       Status =   0 \
                    1  )  CardReaderIdentity 0-7
                      2 /
                      3     Card reader is not removable
                      4     Card reader is present 
                      5     Card reader is ID-1 size 
                      6     Card is present in reader
                      7     Card in reader is powered on
**/
void WIMI_GetReaderListResp( TUint8 aStatus,
                             WIMI_BinData_t* aList );

/**
   Free memory
   aMem - Pointer to memory area to be freed
**/
void WIMI_Free( void* aMem );

/**
   Respond to ReadSimFileReq
   aReader - reader number
   aStatus - error code for file reading
   aData   - content of read file
**/
void WIMI_SIM_ReadFileResp( TUint8 aReader,
                            TUint8 aStatus,
                            WIMI_BinData_t* aData );

/**
   Return information of OMA Provisioning file on card
   aWimRef    - IN  - WIM reference
   aFileInfos - OUT - File info structure
   aFileCount - OUT - Count of provisioning files
**/
WIMI_STAT WIMI_OMAFileInfo( WIMI_Ref_t* aWimRef,
                            WIMI_File_Info** aFileInfos,
                            TUint16* aFileCount );

/**
   Return information of Java Provisioning file on card
   aWimRef    - IN  - WIM reference
   aFileInfos - OUT - File info structure
   aFileCount - OUT - Count of provisioning files
**/
WIMI_STAT WIMI_JAVAFileInfo( WIMI_Ref_t* aWIMref,
                            WIMI_File_Info** aFileInfos,
                            TUint16* aFileCount );
/**
   Get WIM count
   aWimRef   - IN  - WIM reference.
   aWimCount - OUT - count of WIMs
**/
WIMI_STAT WIMI_GetWIMCount( WIMI_Ref_t* aWimRef, TUint8* aWimCount );

/**
   Get content of OMA Provisioning file. Content is returned by callback.
   aTrId     - IN - Transaction ID
   aWimRef   - IN - WIM reference
   aFileSize - IN - File size
   aPath     - IN - File path
**/
WIMI_STAT WIMI_GetOMAFile( WIMI_TransactId_t aTrId,
                           WIMI_Ref_t* aWimRef,
                           TUint16 aFileSize,
                           WIM_BIN_DATA* aPath );

/**
   Get content of OMA Provisioning file. Synchronous version.
   aTrId        - IN  - Transaction ID
   aWimRef      - IN  - WIM reference
   aPath        - IN  - File path
   aFileMaxSize - IN  - Not used
   aFileContent - OUT - File content. Buffer allocated by WIMI, must be freed
                        by caller.
**/
WIMI_STAT WIMI_GetOMAFileSync( WIMI_TransactId_t aTrId,
                               WIMI_Ref_t* aWimRef,
                               WIM_BIN_DATA* aPath,
                               TUint16 aFileMaxSize,
                               WIM_BIN_DATA* aFileContent );


/**
   Return content of JAVA Provisioning file on card
   pWimRef            - IN  - WIM reference
   pt_path            - IN  - File path
   pt_file_content    - OUT 
**/
WIMI_STAT WIMI_GetJAVAFile(  WIMI_Ref_t* pWIMref,
                             WIM_BIN_DATA* pt_path,
                             WIM_BIN_DATA* pt_file_content );  


/**
   Return content of WIM label and path
   pWimRef            - IN   - WIM reference
   path               - OUT  - File path
   label              - OUT 
**/                             
WIMI_STAT WIMI_GetLabelAndPath(WIMI_Ref_t* pWIMref,
                               WIM_BIN_DATA* path,
                               WIM_BIN_DATA* label );                                                          

#ifdef __cplusplus
}
#endif

#endif      // WIMI_H

// End of File
