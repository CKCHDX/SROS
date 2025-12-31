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
* Description:  Listens for smard card events
*
*/



#ifndef CSCARDLISTENER_H
#define CSCARDLISTENER_H

//  INCLUDES
#include "ScardDefs.h"

// FORWARD DECLARATIONS
class RScard;

// CLASS DECLARATION

/**
*  A base class for listening card events.
*  This class is a virtual base class used to transmit card events from 
*  a spesific reader to the terminal, and it intended for use in all 
*  applications that need to use smart cards. When an event occurs, 
*  it is reported to the listener, and the listener will launch the 
*  ProcessEvent function. This function is pure virtual, so a user of 
*  this class must derive a concrete class from CScardListener, 
*  and give an implementation to the ProcessEvent function.
*/
class CScardListener : public CActive
    { 
    public: // New functions
        
        /**
        * This function initiates the first request from this listener 
        * to the server to be notified of card events from the spesified 
        * reader. After this the listener will listen and report all 
        * card events from the reader.
        * @param aReaderName Name of the reader
        * @return an error code (KErrNone if succesful)
        */
        IMPORT_C TInt ListenCardEvents( const TScardReaderName& aReaderName );

    protected:  // New functions
        
        /**
        * Construction.
        * @param aScard pointer to scard class
        * @return void
        */
        IMPORT_C CScardListener( RScard* aScard );

        /**
        * Destruction.
        */
        IMPORT_C virtual ~CScardListener();

        /**
        * This function is called from RunL() and it is pure virtual, 
        * so an implementation must be provided by derived classes to 
        * handle the received events.
        * @param aEvent
        */
        virtual void ProcessEvent( const TScardServiceStatus aEvent) = 0;

        
    protected:  // Functions from base classes
        
        /**
        * From CActive Cancel request
        */
        IMPORT_C void DoCancel();

        /**
        * From CActive Service completed request
        */
        IMPORT_C void RunL();

    
    private:

        /**
        * Tell the server to notify this object of card events.
        * @param aStatus Request status
        * @param aReaderName Name of the reader
        * @return an error code (KErrNone if succesful)
        */
        TInt NotifyChange( TRequestStatus& aStatus, 
                           const TScardReaderName& aReaderName );

        /**
        * Cancel notification.
        * @param aStatus Request status
        * @param aFriendlyName Name of the reader
        */
        /*void CancelNotifyChange( TRequestStatus& aStatus, 
                                 const TScardReaderName& aFriendlyName );*/
        TInt CancelNotifyChange( const TScardReaderName& aFriendlyName );
        
    private:    // Data
        // Pointer to CScard class. Not owned.
        RScard*          iScard;
        // Reader name
        TScardReaderName iReaderName;
        //Transfer to server side
        TPckgBuf<TRequestStatus*> iPckg;
    };

#endif      // CSCARDLISTENER_H   
            
// End of File
