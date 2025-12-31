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
* Description:  Interface which fetches all keyInfos from WIM -card.
*
*/



#ifndef WIMKEYDETAILS_H
#define WIMKEYDETAILS_H


#include "WimClsv.h"
#include <ct.h>
#include <mctkeystore.h>
#include <e32base.h>


//FORWARD DECLARATION
class RWimCertMgmt;


//CLASS DECLARATION
/**
*  This class lists all keyInfos found from WIM -card.
*  @lib WimClient
*  @since Series60 2.1
*/
class CWimKeyDetails: public CActive
    {
    public:

        /**
        * Two-phased constructor.
        * @param aToken -Reference to current token
        */
        IMPORT_C static CWimKeyDetails* NewL( MCTToken& aToken );
        
        /**
        * Destructor
        */
        IMPORT_C virtual ~CWimKeyDetails();


        /** 
        * Lists all the keys in the WIM and creates CCTKeyInfo objects 
        * from received information. CCTKeyInfo objects are inserted to array 
        * aKeys.
        * @param aKeys      -Array where keyInfos are 
        *                   inserted. Caller is responsible 
        *                   to deallocate Array.(OUT)
        * @param aKeyNumbers -Array where keynumbers are inserted. Each key
        *                   has its own number. This number helps to solve 
        *                   which key is associated with which PIN.
        *                    Caller is responsible to 
        *                   deallocate Array. (OUT)
        * @aStatus          -TRequestStatus from caller (IN/OUT)
        * @return void
        */
        IMPORT_C void GetKeyList( RPointerArray<CCTKeyInfo>& aKeys,
                                  CArrayFixFlat<TUint8>& aKeyNumbers,
                                  TRequestStatus& aStatus );
        
        /* 
        * Cancels outgoing listing operation
        * @return void
        */
        IMPORT_C void CancelList();

        /*  
        * Sign some data. Authentication is handled by server.
        * @since Series 60 2.6
        * @param aData    The data to be signed - this should be some 
        *                 form of hash of the actual message to be signed.  
        *                 If the data is too long, this method will return 
        *                 KErrOverflow through aStatus. (IN)
        * @param aSignature Signature which is given back to caller. (OUT)
        * @param aKeyId.  KeyId for signing operation.(IN)
        * @param aStatus    Caller's status (IN/OUT)
        * @return void
        */
        IMPORT_C void Sign( const TDesC8& aData,
                            TDesC8& aKeyId,
                            HBufC8*& aSignature,
                            TRequestStatus& aStatus );

        /** 
        * Cancel signing operation
        * @since Series 60 2.6
        * @return void
        */
        IMPORT_C void CancelSign();

        /** 
        * Export public key
        * @since Series 60 2.6
        * @param aKeyId  -The KeyId of the key to be exported (IN)
        * @param aPublicKey -DER Encoded public key. Must be allocated
        *                    by caller(OUT)
        * @param aStatus    -Caller's status (IN/OUT)
        */
        IMPORT_C void ExportPublicKeyL( TDesC8& aKeyId, 
                                        HBufC8*& aPublicKey,
                                        TRequestStatus& aStatus );

        /** 
        * Cancels ongoing export operation
        * @since Series 60 2.6
        * @return void
        */
        IMPORT_C void CancelExport();

    private:
        /** 
        * Allocates memory for Array which is filled by server.
        * @return void
        */
        void AllocMemoryForKeyListL();
        
        /** 
        * Deallocates memory from the Array which was filled by server.
        * @return void
        */
        void DeallocMemoryFromKeyList();

        
        /** 
        * Allocates memory for Keyinfo structure.
        * @param aKeyInfo   -Struct where allocated pointers are inserted.
        * @return void
        */
        void AllocMemoryForKeyInfoL( TKeyInfo& aKeyInfo );   
 
        /** 
        * Deallocates memory from Keyinfo structure.
        * @return void
        */
        void DeallocMemoryFromKeyInfo();

        /** 
        * Allocates memory for data to be signed
        * @param aData -Data to be signed
        * @param aKeyId -KeyId of the key to be used for sign
        * @return void
        */
        void AllocMemoryForSignL( const TDesC8& aData, const TDesC8& aKeyId );

        /** 
        * Deallocates previously allocated data after signing operation.
        * @return void
        */
        void DeallocMemoryFromSign();
        
        
        /**
        * Sets own iStatus to KRequestPending, and signals it 
        * with User::RequestComplete() -request. This gives chance 
        * to activescheduler to run other active objects. After a quick
        * visit in activescheduler, signal returns to RunL() and starts next
        * phase of operation. 
        * @return void
        */
        void SignalOwnStatusAndComplete();

        /** 
        * Converts key list parameters. Extracts data out from key list 
        * & key number. Extracted data is inserted to an RArray.
        * @return void
        */
        void ConvertKeyListL();

        /*  
        * Converts key usage to match new key usage requirement
        * @param aKeyUsage  -Value to be converted
        * @return TKeyUsagePKCS15 -New form of key usage
        */
        TKeyUsagePKCS15 ConvertKeyUsage( TUint16 aKeyUsage );


    private: //from CActive
        
        /**
        * Different phases are handled here.
        * @return void
        */
        void RunL();

        /**
        * Cancellation function
        * Deallocates member variables and completes client status with
        * KErrCancel error code.
        * @return void
        */
        void DoCancel();

        /**
        * The active scheduler calls this function if this active 
        * object's RunL() function leaves. 
        * Handles necessary cleanup and completes request with
        * received error code.
        * @param aError -Error which caused this event
        * @return TInt  -Error code to activescheduler, is always KErrNone.
        */ 
        TInt RunError( TInt aError );

    private:

        /**
        * Default constructor is private.
        */
        void ConstructL();
        
        /** 
        * Default constructor
        * @param aToken -Reference to current token
        */
        CWimKeyDetails( MCTToken& aToken );

    private:

        /** 
        * Seven different phases which handles list, sign and export public
        * key operations
        */
        enum TPhase
            {
            EGetKeyList,
            EConvertParams,
            EGetKeyInfo,
            ESign,
            ESignCompleted,
            EExportPublicKey
            };
        
    private:
        //Reference to current token. Needed in CCTCertInfo creation.
        MCTToken&                       iToken;

        //Handle to connection with server. Owned.
        RWimCertMgmt*                   iConnectionHandle;

        //Client status is stored here while operation
        //on the server side is done.        
        TRequestStatus*                 iClientStatus;

        //Used to handle different phases in RunL()
        TPhase                          iPhase;

        //Array which is used to contain key list.
        //Not owned. Caller is responsible to destroy array.
        RPointerArray<CCTKeyInfo>*      iKeys;

        //Buffer which is used to contain keyreference information. Owned.
        HBufC8*                         iKeyList;

        //Pointer to iKeylist. Owned.
        TPtr8*                          iKeyListPtr;

        // The number of keys
        TInt                            iKeyNumber;

        //Buffer which is used for keys label when constructing new keyinfo
        //-objects. Owned.
        HBufC8*                         iLabel;
     
        //Pointer to iLabel. Owned.
        TPtr8*                          iLabelPtr;

        //Buffer which is used to contain unique modifier for a key. Used when
        //constructing new keyinfo -objects. Owned.
        HBufC8*                         iKeyId;
     
        //Pointer to iKeyId. Owned.
        TPtr8*                          iKeyIdPtr;
        
        //Array which holds keyReferences. These references are needed when
        //getting keyinfo for a single key.Owned.
        RArray<TInt32>                  iKeyReferences;

        //Array which hold keyNumber for each CCTKeyInfo -object, which are
        //created and inserted to iKeys -array. Not owned.
        CArrayFixFlat<TUint8>*          iKeyNumberArray;   
        
        //counter which informs how many keyinfos have been created.
        TInt                            iFetchedKeyInfos;

        //Struct which is used when signing data.
        TKeySignParameters              iKeySignParameters;

        //Struct which is used when exporting a key.
        TExportPublicKey                iExportPublicKey;

        //Buffer which holds data to be signed. Owned.
        HBufC8*                         iSigningData;
        
        //Pointer to iSigningData. Owned.
        TPtr8*                          iSigningDataPtr;

        //Holds information which key is used for signing
        TInt                            iSignKeyHandle;
        
        //Buffer which holds received signature from WIM, if signing was
        //succesful. Not owned
        HBufC8*                         iSignature;

        //Pointer to iSignature. Owned.
        TPtr8*                          iSignaturePtr;

        //Buffer which holds DER encoded public key. Not owned.
        HBufC8*                         iPublicKey;

        //Pointer to iPublicKey.Owned
        TPtr8*                          iPublicKeyPtr;
        
        //The pack to store key number
        TPckg<TInt>                     iPckg;

    };


#endif  //WIMKEYDETAILS_H





















