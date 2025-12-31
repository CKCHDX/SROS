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
* Description:  API for managing PIN
*
*/


#ifndef WIMPIN_H
#define WIMPIN_H


//  INCLUDES
#include "WimClsv.h"
#include <e32base.h>
#include <secdlg.h> 


class RWimMgmt;

/**
*  API for managing PIN.
*  This API provides methods to manage PIN: check statuses and
*  change settings.
*
*  @lib WimClient
*  @since Series 60 2.6
*/
class CWimPin: public CActive
    {
    public:  
        
        /**
        * Two-phased constructor.
        * @param aPin       -the type of Pin
        * @param aPinAddr   -Reference to Pin structure
        * @param aTokenLabel -the name of the token
        */
        IMPORT_C static CWimPin* NewL( TWimPin aPin,  
                                       const TPinAddress aPinAddr,
                                       TDesC& aTokenLabel );  

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CWimPin();

        
    public: // New functions

        /**
        * Enables Pin query of the Pin.
        * The PinStatus() function should be called before 
        * calling this function to make sure the status of
        * pin
        * @param aStatus -Caller's status.
        * @return void
        */        
        IMPORT_C void EnablePinQuery( TRequestStatus& aStatus );
        
        /**
        * Cancel enables Pin query of the Pin.
        * @return void
        */  
        IMPORT_C void CancelEnablePinQuery();
        
        /**
        * Disables Pin query of the Pin.
        * The PinStatus() function should be called before 
        * calling this function to make sure the status of
        * pin
        * @param aStatus -Caller's status.
        * @return void
        */                
        IMPORT_C void DisablePinQuery( TRequestStatus& aStatus );
        
        /**
        * Cancel disables Pin query of the Pin.
        * @return void
        */  
        IMPORT_C void CancelDisablePinQuery();
        
        /**
        * Changes Pin.
        * The PinStatus() function should be called before 
        * calling this function to make sure the status of
        * pin
        * @param aStatus -Caller's status
        * @return void
        */        
        IMPORT_C void ChangePin( TRequestStatus& aStatus );
        
        /**
        * Cancel changes Pin.
        * @return void
        */ 
        IMPORT_C void CancelChangePin();
        
        /**
        * Unblocks Pin
        * The PinStatus() function should be called before 
        * calling this function to make sure the status of
        * pin
        * @param aStatus -Caller's status
        * @return void
        */        
        IMPORT_C void UnblockPin( TRequestStatus& aStatus );
        
        /**
        * Cancel unblocks Pin
        * @return void
        */  
        IMPORT_C void CancelUnblockPin();
        
        /**
        * Verifies the enterd Pin -request.
        * The PinStatus() function should be called before 
        * calling this function to make sure the status of
        * pin
        * @param aStatus -Caller's status 
        * @return void
        */        
        IMPORT_C void VerifyPin( TRequestStatus& aStatus );
        
        /**
        * Cancel verifies the enterd Pin -request.
        * @return void
        */ 
        IMPORT_C void CancelVerifyPin();
        
        /**
        * Returns PIN's label.
        * @return Pointer descriptor containing label.
        */
        IMPORT_C TPtrC Label();
   
        /**
        * Returns the status of Pin.
        * Variations are: WIMI_PF_Enabled           
        *                 WIMI_PF_ChangeDisabled    
        *                 WIMI_PF_UnblockDisabled   
        *                 WIMI_PF_DisableAllowed
        * @return iStatus -status of Pin
        */        
        IMPORT_C TWimPinStatus PinStatus();

        /** 
        * Returns the number of the pin. It is a number, which 
        * identifies pin object in the Wim -card. Pin number
        * can be eg. 0,1,2....
        * @return TUint8    -PinNumber
        */
        IMPORT_C TUint8 PinNumber();

    public:

        /*
        * Sets RWimMgmt* pointer to iClientSession
        * @param aClientSession
        * @return void
        */
        void SetClientSession( RWimMgmt* aClientSession );
        
        /*
        * Sets Label 
        * return void
        */
        void SetLabel( TBuf<KLabelLen>& aLabel );
        
        /*
        * Sets PinStatus 
        * return void
        */
    	  void SetPinStatus( TWimPinStatus& aStatus );
    	  
    	  /*
        * Sets Pin Number 
        * return void
        */
    	  void SetPinNumber( TUint8& aPinNumber );
    
    private:

        /** 
        * Constructs pinparams struct.
        * @param aPinParams -params to be constructed.
        * @return TPINParams
        */
        void PinParams( TPINParams& aPinParams );

        /**
        * Checks is Pin changeable.
        * @return ETrue if PIN is changeable else returns EFalse.
        */
        TBool PinChangeable();

        /**
        * C++ constructor.
        * @param aPin   -the type of the pin
        * @param aPinAddr -Reference to Pin structure.
        */
        CWimPin( TWimPin aPin, TPinAddress aPinAddr );

        /**
        * Default constructor is private.
        * @param aTokenLabel -Label of the token
        * @return void
        */
        void ConstructL( const TDesC& aTokenLabel );

        /*
        * Get Pin Info. Initializes member variables.
        * @return void
        */
        void PinInfo( TRequestStatus& aStatus );

        /**
        * Sets own iStatus to KRequestPending, and signals it 
        * with User::RequestComplete() -request. This gives change 
        * to activescheduler to run other active objects. After a quick
        * visit in activescheduler, signal returns to RunL() and starts next
        * phase of operation. 
        * @return void
        */
        void SignalOwnStatusAndComplete();


    private: //From CActive

        /**
        * Different phases are handled here.
        * @return void
        */
        void RunL();

        /**
        * Cancellation function
        * Not allowed to cancel
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
        
        //Ten different phases for PIN handling
        enum TPinPhase
            {
            EEnablePinQueryStart,
            EEnablePinQueryEnd,
            EDisablePinQueryStart,
            EDisablePinQueryEnd,
            EChangePinStart,
            EChangePinEnd,
            EUnblockPinStart,
            EUnblockPinEnd,
            EVerifyPinStart,
            EVerifyPinEnd,
            EUpdatePinStatus,
            EUpdatePinStatusDone
            };

        //Used to handle different phases in RunL()
        TPinPhase iPhase;
        
        TPinPhase iCurrentPhase;
        
    private:    

        //Client status is stored here while operation
        //on the server side is done.        
        TRequestStatus*             iClientStatus;
        // Reference to PIN.
        const TPinAddress           iReference; 
        //Pin type: Pin-G or PinNR
        TWimPin                     iPinType;
        //Holds label. Owned
        HBufC*                      iLabel;
        //Holds tokenlabel.Owned.
        HBufC*                      iTokenLabel;
        //Holds Pin-status
        TWimPinStatus               iPinStatus;
        //pointer to clientSession. Owned by CWimSecModuleMgr.
        RWimMgmt*                   iClientSession;
        //Number that identifies Pin in the Wim. Its a running number 0,1,2..
        TUint8                      iPinNumber;
        //Struct for PIN parameters
        TPINParams                  iPinParams;
        //Struct for enable/disable information
        TPINStateRequest            iPinStateRequest;
        //To store the status value from wimserver
        TInt                        iReceivedStatus;
        
        TWimPinStruct               iPinStruct;

    };

#endif       
// End of File
