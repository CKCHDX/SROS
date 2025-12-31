/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for WIM Trust Settings Store
*
*/



#ifndef WIMTRUSTSETTINGSAPI_H
#define WIMTRUSTSETTINGSAPI_H


//INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class RWimTrustSettingsMgmt;
class CWimCertInfo;


//CLASS DECLARATION
/**
*  Interface which handles Trust Settings Store related operations 
*  with WimServer.
*
*  @lib WimClient.lib
*  @since Series60 3.0
*/
class CWimTrustSettingsAPI : public CActive
    {
    public:  // Constructor and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CWimTrustSettingsAPI* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CWimTrustSettingsAPI();

    public: // New functions
        
        /**
        * Fetches trust settings for given certificate. 
        * Acts like an asynchronous for caller but not activates itself.
        * @param  aCert (IN) Certificate
        * @param  aTrusted  (OUT) Is certificate trusted
        * @param  aApplications  (OUT) Array of applications supported 
        *         by certificate
        * @param  aStatus (IN/OUT) Request Status of asynchronous call
        * @return void
        */
        IMPORT_C void GetTrustSettings( const CWimCertInfo& aCert,
                                        TBool& aTrusted,
                                        RArray<TUid>& aApplications, 
                                        TRequestStatus& aStatus );

        /**
        * Set applicability for given certificate. Calls DoSetApplicabilityL,
        * which handles actual operation. New certificate entry is set if
        * one not found from database (trust flag is set to EFalse).
        * Acts like an asynchronous for caller but not activates itself.
        * @param aCert (IN) Certificate
        * @param aApplications (IN) Array of applications supported by 
        *        certificate
        * @param aStatus Request (IN/OUT) Status of asynchronous call.
        * @return void
        */
        IMPORT_C void SetApplicability( const CWimCertInfo& aCert,
                                        const RArray<TUid>& aApplications,
                                        TRequestStatus& aStatus );

        /**
        * Set trust flag for given certificate. Calls DoSetTrustL,
        * which handles actual operation. New certificate entry is set if
        * one not found from database.
        * Acts like an asynchronous for caller but not activates itself.
        * @param aCert (IN) Certificate
        * @param aTrusted (IN) Is certificate trusted
        * @param aStatus (IN/OUT) Request Status of asynchronous call.
        * @return void
        */
        IMPORT_C void SetTrust( const CWimCertInfo& aCert,
                                TBool aTrusted,
                                TRequestStatus& aStatus );

        /**
        * Set default trust settings for given certificate. If certificate not
        * found from database new entry is inserted.
        * Acts like an asynchronous for caller but not activate itself.
        * @param aCert (IN) Certificate
        * @param aAddApps (IN) Are default applications inserted 
        *        (ETrue means yes)
        * @param aStatus (IN/OUT) Request Status of asynchronous call
        * @return void
        */
        IMPORT_C void SetDefaultTrustSettings( const CWimCertInfo& aCert,
                                               TBool aAddApps,
                                               TRequestStatus& aStatus );

        /**
        * Remove trust settings of given certificate. If certificate
        * is not found, return with status.Int() = KErrNotFound
        * Acts like an asynchronous for caller but not activates itself.
        * @param aCert (IN) Certificate
        * @param aStatus (IN/OUT) Request Status of asynchronous call
        * @return void
        */
        IMPORT_C void RemoveTrustSettings( const CWimCertInfo& aCert,
                                           TRequestStatus& aStatus );

        /**
        * Delete TrustSettingsStore instance and release all resources
        * @return void
        */
        IMPORT_C void Close();

        /**
        * Cancel any issued asynchronous call
        * @return void
        */
        IMPORT_C void CancelDoing();

    private:

        /**
        * Default constructor.
        */
        CWimTrustSettingsAPI();

        /**
        * 2nd phase constructor
        */
        void ConstructL();

        /**
        * Handle trapped leave. Completes client request in case leave
        * has occurred.
        * @return void
        */
        void HandleLeaveError( TInt aError ); 


    private: // From base class CActive 

        /**
        * Handle asyncronous response
        * @return void
        */
        void RunL();

        /**
        * Handle asyncronous call cancel
        * @return void
        */
        void DoCancel();

    private:
              
        //Client status is stored here while operation
        //on the server side is done.
        TRequestStatus*                 iClientStatus;
        
        // Used for saving caller status. Not owned
        TRequestStatus*                 iOriginalRequestStatus;

        //Handle to connection with server. Owned.
        RWimTrustSettingsMgmt*          iConnectionHandle;

        // Pointer to client array for applications. Not owned.
        RArray<TUid>*                   iApplications;

        // Pointer to flag telling if certificate is trusted or not. Not owned.
        TBool*                          iTrusted;

        // Different phases of Trust Settings Store operation
        enum TPhase
            {
            EGetTrustSettings,
            ESetDefaultTrustSettings,
            ESetApplicability,
            ESetTrust,
            ERemoveTrustSettings
            };

        // Current phase of Trust Settings Store operation
        TPhase                          iPhase;
 
    };

#endif  //WIMTRUSTSETTINGSAPI_H

// End of File
