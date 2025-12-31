/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file contains definition of abstract Card Reader interface
*               class from which the final Reader classes in Reader Handler 
*               level are derived.
*
*/



#ifndef CSCARDREADER_H
#define CSCARDREADER_H

//  INCLUDES
#include "ScardBase.h"
#include "ScardDefs.h"

//  CONSTANTS  

// The UID for Card Reader DLLs.
// The client imposes this on DLLs which are required
// to satisfy the protocol 
const TInt KReaderUidValue = 0x1000086E;
const TUid KReaderUid      = {KReaderUidValue};

// CLASS DECLARATION

/**
*  Smart card reader. 
*
*  @lib Scard.lib
*  @since Series60 2.1
*/
class MScardReader
    {
    public: 
        
        /**
        * Initializes the Card Reader. Implementations of this method must carry
        * out all steps required to set the concrete reader into a proper state.
        * After invoking this method it should be possible to communicate with 
        * the reader properly.
        * @param aStatus Request status
        * @return void
        */
        virtual void OpenAsync( TRequestStatus& aStatus ) = 0;

        /**
        * Closes the Card Reader. Implementations of this method must carry out
        * all steps required to close the concrete terminal and free resources 
        * held by it.
        * @return Symbian error code
        */
        virtual TInt Close() = 0;

        /**
        * Cancel any pending requests (if any)
        * @return void
        */
        virtual void CancelTransmit() = 0;

        /**
        * Return the ATR bytes. 
        * @param anATR ATR
        * @return Symbian error code
        */
        virtual TInt GetATR( TScardATR& anATR ) = 0;
        
        /**
        * Return the value corresponding to the specified TAG parameter
        * @param aStatus Request status
        * @param aTag Tag
        * @param aValue Value for capabilities
        * @param aTimeout Timeout
        * @return ETrue/EFalse
        */
        virtual TBool GetCapabilities( TRequestStatus& aStatus, 
                                       const TInt32 aTag,
                                       TPtr8& aValue,
                                       const TInt32 aTimeout ) = 0;

        /**
        * Send a Command(APDU) to the card and return Response(APDU)
        * received from card.
        * @param aStatus Request status
        * @param aCommand Command APDU 
        * @param aResponse Response APDU
        * @param aTimeout Timeout
        * @return void
        */
        virtual void TransmitToCard( TRequestStatus& aStatus, 
                                     const TPtrC8& aCommand, 
                                     TPtr8& aResponse,
                                     const TInt32 aTimeout ) = 0;

    };
    
#endif      // CSCARDREADER_H

// End of File
