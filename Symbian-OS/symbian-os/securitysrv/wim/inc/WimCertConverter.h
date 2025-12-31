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
* Description:  Interface which handles certificate related operations
*
*/



#ifndef WIMCERTCONVERTER_H
#define WIMCERTCONVERTER_H


//INCLUDES
#include "WimClsv.h"        
#include <e32base.h>
#include <ct.h> 

// FORWARD DECLARATIONS
class RWimCertMgmt;
class CWimCertInfo;
class CCTCertInfo;

// CONSTANTS
typedef HBufC8* PHBufC8;
typedef TPtr8* PTPtr8;



//CLASS DECLARATION
/**
*  Interface which handles certificate related operations 
*  with WimServer.
*  Caller can add a certificate, remove a certificate,
*  list a certificate and get certificate details.
*
*  @lib WimClient
*  @since Series60 2.1
*/
class CWimCertConverter: public CActive
    {

    public:

        /**
        * Two-phased constructor.
        * @param aToken -Reference to current token
        */
        IMPORT_C static CWimCertConverter* NewL( MCTToken& aToken );

        
        /**
        * Restores certificates from WIM cache. If Restore cannot find
        * any certificate, it will return KErrNotFound to caller.
        * @param  aArray    -Array where new certificates are 
        *                   inserted. Caller is responsible 
        *                   to deallocate CWimCertInfo -objects
        *                   by calling ResetAndDestroy.(IN/OUT)
        * @param  aStatus   -Status from caller.         
        * @return void
        */
        IMPORT_C void Restore( RPointerArray<CWimCertInfo>& aArray, 
                               TRequestStatus& aStatus );

        
        /** 
        * Cancels outgoing Restore operation. Sets an internal flag to true. 
        * After necessary cleanup, caller is signalled with KErrCancel 
        * -error code.
        * @return void
        */
        IMPORT_C void CancelRestore();
        
        /**
        * Retrieves the actual certificate. In case of other cert than
        * Url cert the binary encoded certificate is written into the 
        * aEncodedCert parameter. In case of Url cert the data is 
        * the actual url.
        * @param  aIndex. Used to point iCertRefLst and iCertInfoArr 
        *         -arrays for correct certificate.
        * @param  aEncodedCert. A buffer to put the certificate in. 
        * @param  aStatus. A request status that is completed when the 
        *         operation has finished.(IN/OUT)
        * @return void
        */
        IMPORT_C void RetrieveCertByIndexL( const TInt aIndex,
                                           TDes8& aEncodedCert, 
                                           TRequestStatus& aStatus );

        /**
        * Cancels an ongoing Retrieve operation.
        * If retrieve is cancelled, then operation is completed with 
        * KErrCancel -error code.
        * @return void
        */
        IMPORT_C void CancelRetrieve();

        
        /** 
        * Adds a certificate to the cert store.
        * The caller of this function owns all its parameters.
        * @since Series 60 2.6
        * @param  aLabel  -The label of the certificate to add 
        * @param  aFormat  -The format of the certificate. Supported formats are
                           EX509Certificate, EWTLSCertificate,
                           EX509CertificateUrl and EWTLSCertificateUrl 
        * @param  aCertificateOwnerType  -The owner type. Supported types are
                                        ECACertificate and EUserCertificate.
        * @param  aSubjectKeyId  -The Subject key ID
        * @param  aIssuerKeyId  -The issuer key ID
        * @param  aCert  -The certificate to add
        * @param  aStatus  This is completed with the return result 
        *         when the add has completed (IN/OUT)
        * @return  void
        */
        IMPORT_C void AddCertificate( const TDesC& aLabel, 
                              const TCertificateFormat aFormat,
                              const TCertificateOwnerType aCertificateOwnerType, 
                              const TKeyIdentifier& aSubjectKeyId,
                              const TKeyIdentifier& aIssuerKeyId,
                              const TDesC8& aCert, 
                              TRequestStatus& aStatus );
        
        /**
        * Cancels an ongoing add operation.
        * @since Series 60 2.6
        * @return void
        */
        IMPORT_C void CancelAddCertificate();

        /** 
        * Removes a certificate from WIM.
        * @since Series 60 2.6
        * @param aIndex     -Used to point iCertInfoArr 
        *                   for the certificate to be removed.
        * @param aStatus    -Status from caller 
        * @return void      
        */
        IMPORT_C void RemoveL( const TInt aIndex, TRequestStatus& aStatus );

        /** 
        * Cancels ongoing remove operation.
        * @since Series 60 2.6
        * @return void
        */
        IMPORT_C void CancelRemove();

        /**
        * Destructor
        */
        IMPORT_C virtual ~CWimCertConverter();

    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * C++ default constructor.
        * @param aToken -Reference to current token
        */
        CWimCertConverter( MCTToken& aToken );

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
        * @param aError -The error code which caused this function call.
        * @return TInt  -Error code to activescheduler, is always KErrNone.
        */ 
        TInt RunError( TInt aError );
        
    private:
        
        /**
        * Allocates memory for the array which is filled by server.
        * @param  aWimCertInfoArr    -Array to be initialized 
        * @param  aCount             -Count of elements in the array         
        * @return void
        */
        void AllocWimCertInfoL( TWimCertInfo* aWimCertInfoArr, TInt aCount );  
        
        /**
        * Deallocates memory from the array.
        * @return void
        */
        void DeallocWimCertInfo();   
        
        /**
        * Allocates memory for a struct which is filled by server.
        * @return void
        */
        void AllocMemoryForCertDetailsL();

        /**
        * Creates new certificate objects which can be returned to the caller.
        * @return void
        */
        void CreateNewCertObjectsL();
        
        /**
        * Creates new wim certificate objects.
        * @return void
        */
        void CreateNewWimCertObjectL();

        /**
        * Gets trusted usages.
        * @return void
        */
        void GetTrustedUsagesL();

        /**
        * Completion of trusted usages.
        * @return void
        */
        void TrustedUsagesDoneL();

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
        * Allocates memory for member variables, which are needed when adding
        * a certificate asynchronously to WIM.
        * @param aLabel -certificate label (IN)
        * @param aIssuerKeyId -Issuer KeyId of a certificate 
        * @param aSubjectKeyId -Subject KeyId of a certificate
        * @param aCert -certificate data
        * @return void
        */
        void AllocMemoryForAddCertL( const TDesC& aLabel, 
                            const TKeyIdentifier& aIssuerKeyId, 
                            const TKeyIdentifier& aSubjectKeyId,
                            const TDesC8& aCert );

        /**
        * Copies data to caller's buffer by using pointer.
        * @return void
        */
        void CopyRetrievedCertData();


        /**
        * DeAllocates memory from member variables, which are used
        * when communicating with WIM.
        * @return void
        */
        void DeallocCertHBufs();

        /**
        * If user has cancelled initialization process, dealloc references 
        * from loaded certs. 
        * @return void
        */
        void DeallocReferences();

        /**
        * Returns certificate format according to received index.
        * @param aIndex -index of the certificate to be returned.
        * @return TCertificateFormat -format of the certificate
        */
        TCertificateFormat GetCertFormatByIndex( TInt aIndex );

    private:

        /**
        * Ten different phases, which are used to 
        * Read certificates from WIM cache, create new certificate objects,
        * retrieve certificate, add certificate and remove certificate.
        */
        enum TPhase
            {
            EListCertsFromWim,
            ECreateNewCertObjects,
            EGetTrustedUsages,
            ECertObjectsDone,
            ETrustedUsagesDone,
            ERetrieveCertificate,
            ERetrievingCompleted,
            EAddCertificate,
            EAddCertificateCompleted,
            ERemove,
            ERemoveCompleted
            };
                   
    private:
              
        //Client status is stored here while operation
        //on the server side is done.
        TRequestStatus*                 iClientStatus;
        
        //Array which is used to contain new certificate objects.
        //Not owned. Caller is responsible to destroy array.
        RPointerArray<CWimCertInfo>*    iArray;
        
        //Details of the certificate. Owned.
        TWimCertDetails                 iWimCertDetails;
        
        //Struct which is used when adding a certificate
        //to WIM.
        TWimCertAddParameters           iParam;

        //Struct which is used when removing a certificate for WIM.
        TWimCertRemoveAddr              iWimCertRemoveAddr;    
        
        //Reference to current token. Needed in CCTCertInfo creation.
        MCTToken&                       iToken;
        
        //Handle to connection with server. Owned.
        RWimCertMgmt*                   iConnectionHandle;
        
        //Typedefs for the reference to certificate. Owned.
        TCertificateAddressList         iCertRefLst; 
        
        // Certificate information structure. Owned.
        TWimCertInfo*                    iCertInfoArr;
        
        //Used to handle different phases.
        TPhase                          iPhase;
                
        //Index to point which certificate we want to be retrieved
        TInt                            iCertRetrieveIndex;
                 
        //Used to inform the size of the array, needed in array construction/
        //destruction.
        TInt                            iArraySize;
        
        //The count of certificates
        TUint8                          iCertCount;

        //Pointer to HBufC8*. This is used as an array when listing every 
        //certificate from WIM to an array during startup. Owned.
        PHBufC8*                        iLabel;

        //Pointer to HBufC8*. This is used as an array when listing every 
        //certificate from WIM to an array during startup. Owned.
        PHBufC8*                        iKeyId;

        //Pointer to HBufC8*. This is used as an array when listing every 
        //certificate from WIM to an array during startup. Owned.
        PHBufC8*                        iCAId;

        //Pointer to HBufC8*. This is used as an array when listing every 
        //certificate from WIM to an array during startup. Owned.
        PHBufC8*                        iIssuerHash;

        //Pointer to iLabel. Also used as an array. Owned.        
        PTPtr8*                         iLabelPtr;

        //Pointer to iKeyId. Also used as an array. Owned.        
        PTPtr8*                         iKeyIdPtr;

        //Pointer to iCAId. Also used as an array. Owned.        
        PTPtr8*                         iCAIdPtr;

        //Pointer to iIssuerHash. Also used as an array. Owned.
        PTPtr8*                         iIssuerHashPtr;
            
        //Modifiable 8-bit descriptor which points
        //to caller's descriptor. We copy retrieved certificate
        //data to this descriptor. Owned.
        TDes8*                          iEncodedCert;

        //Buffer which is used to contain certificate data. Owned.
        HBufC8*                         iCertHBufOne;
        //Pointer to iCertHBufOne. Owned.
        TPtr8*                          iCertHBufOnePtr;

        //Buffer which is used to contain certificate data. Owned.        
        HBufC8*                         iCertHBufTwo;
        //Pointer to iCertHBufTwo. Owned.
        TPtr8*                          iCertHBufTwoPtr;

        //Buffer which is used to contain certificate data. Owned.        
        HBufC8*                         iCertHBufThree;
        //Pointer to iCertHBufThree. Owned.
        TPtr8*                          iCertHBufThreePtr;

        //Buffer which is used to contain certificate data. Owned.        
        HBufC8*                         iCertHBufFour;
        //Pointer to iCertHBufFour. Owned.
        TPtr8*                          iCertHBufFourPtr;
        //ActiveSchedulerWaiter for certificate extra information
        //fetching.
        CActiveSchedulerWait            iActiveSchedulerWait;
        //Pointer for trusted usage buffer
        HBufC*                          iTrustedUsages;
        //Pointer to trusted usage buffer pointer
        TPtr*                           iTrustedUsagesPtr;
        //Index pointing to certificate elements
        TUint8                          iIndex;
        //Pointer to one certificate info, owned
        CCTCertInfo*                    iCert;
        //Pointer to one WIM specific certificate info, owned
        CWimCertInfo*                   iCertInfo;
        //An array of trusted usage oids
        RArray<HBufC*>*                 iOids;
        //Package structure for certificate extra info
        TCertExtrasInfo                 iCertExtrasInfo;
        //Pointer for key identifier buffer
        HBufC8*                         iKeyIdBuf;
        //Used to point right keyId when retrieving extra data
        TPtr8*                          iKeyIdPointer;

    };


#endif  //WIMCERTCONVERTER_H
