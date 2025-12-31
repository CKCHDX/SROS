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
* Description:  Functionality for accessing uicc
*
*/


#ifndef _GBAUICC_H_
#define _GBAUICC_H_

#include <e32base.h>

_LIT8(KAKAV1, "USIM");
_LIT8(KSIMV1, "SIM");
_LIT8(KISIMV1, "ISIM");
_LIT8(KUICC_DEFAULT, "");

#define GBAUICC_INTERFACE  0x2000F86B

const TUid KGBAUICCInterfaceUID = { GBAUICC_INTERFACE };

/**
* An abstract interface for accessing applications on UICC. Use this interface to query identity and authentication parameters from UICC.
*/
class MUICCInterface
    {
    public:

        /**
         * Fetches identity stored on the UICC
         * param stores
         * exception if UICC is not accessible call will leave
         * result is returned 3GPP TS 23.003: "3rd Generation Partnership Project; Technical Specification Group Core Network; Numbering, addressing and identification".
         */
        virtual void QueryIdentityL(TDes8& aIdentity) = 0;
   
        /**
         * Fetches Home network domain name
         * exception if UICC is not accessible call will leave
         * result is returned 
         * see 3GPP TS 23.003: "3rd Generation Partnership Project; Technical Specification Group Core Network; Numbering, addressing and identification".
         */
        virtual void QueryHomeNetworkDnL(TDes8& aHNDN) = 0;

        /**
         * Queries authentication vector from UICC application
         * param challenge in
         * param calculated response
         * return true on success false on failure
         * exception if UICC is not accessible call will leave
         */
        virtual TBool QueryAuthenticationL(const TDesC8& aNonce, TDes8& aResponse, TDes8& aResync ) = 0; 
  
        /**
         * Fetches key material from UICC
         * param resulting key material
         */
        virtual void QueryKeyMaterialL  (   const TDesC8& aKeyMaterial,
                                            const TDesC8& aRand,
                                            const TDesC8& aIMPI,
                                            const TDesC8& aUTF8_NAF_ID,
                                            TDes8& aDerivedKey )  = 0; 
  
       /**
        * Fetches the master keys in what ever format they exist
        */
       virtual TBool QueryKs(TDes8& aKS) =0;


       /**
        * Check if the 2G interface
        * Return ETrue when only 2G interface available 
        */
       virtual TBool InterfaceIs2G() = 0;


        /**
         * The API used to update B-TID and key lifetime
         */
        virtual TInt UpdateGBADataL( const TDesC8& aBTID, const TDesC8& aLifetime ) = 0;

        /**
         * API to get the GBA_U availability
         * Set aGBAAvail to ETrue if smart card is GBA_U card
         */
        virtual void QueryGBAUAvailabilityL( TBool& aGBAAvail ) = 0;
    
        /**
         * delete the objects
         */
        virtual void Release() = 0;
        
        /*
         * Notify the smart card is changed
         */
         virtual void NotifyCardChangeL() = 0; 
    }; 

#endif // _GBAUICC_H_
//EOF


