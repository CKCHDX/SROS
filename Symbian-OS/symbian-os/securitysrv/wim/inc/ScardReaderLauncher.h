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
* Description:  This file contains definition of abstract Card Reader Launcher
*               interface class from which the final Reader Launcher classes 
*               are derived.
*
*/



#ifndef SCARDREADERLAUNCHER_H
#define SCARDREADERLAUNCHER_H

//  INCLUDES
#include "ScardReader.h"
#include "ScardNotifyObserver.h"

//  CONSTANTS  

// The UID for Card Reader DLLs.
// The client imposes this on DLLs which are required
// to satisfy the protocol 

const TInt KReaderLauncherUidValue = 0x1000086E;
const TUid KReaderLauncherUid      = {KReaderUidValue};

// CLASS DECLARATION

/**
*  Defines pure virtual functions for getting notifyobservers.
*
*  @lib Scard.lib
*  @since Series60 2.1
*/
class MScardReaderService 
    {

    public:
        /**
        * Return notify observer object
        * @return Pointer to MScardNotifyObserver
        */
        virtual MScardNotifyObserver* NotifyObserver() = 0;

    };

// CLASS DECLARATION

/**
*  Defines pure virtual functions for load and unload service resources.
*
*  @lib Scard.lib
*  @since Series60 2.1
*/
class MScardReaderLauncher
    {

    public:  // Constructors and destructor

        /**
        * Reader service object is the one that Launcher can query needed 
        * information to connect reader for framework wide services.
        * @param aReaderService ReaderService
        * @return void
        */
        virtual void ConstructL( MScardReaderService* aReaderService ) = 0;

    public: // New functions
 
        /**
        * Create specific reader object, just create not attach the physical
        * reader device, Open() in Reader interface will do this.
        * @param aReaderID Reader ID
        * @return Pointer to MScardReader object
        */
        virtual MScardReader* CreateReaderL( TReaderID aReaderID ) = 0;

        /**
        * Delete the reader object, should be Closed() before this is called
        * @param aReaderID Reader ID
        * @return void
        */
        virtual void DeleteReader( TReaderID aReaderID ) = 0;

    };
    
#endif      // SCARDREADERLAUNCHER_H

// End of File
