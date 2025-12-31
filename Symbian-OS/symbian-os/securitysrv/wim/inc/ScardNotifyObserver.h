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
* Description:  This file contains definition of abstract MScardNotifyObserver
*               class
*
*/



#ifndef MSCARDNOTIFYOBSERVER_H
#define MSCARDNOTIFYOBSERVER_H

//  INCLUDES
#include "ScardDefs.h"

//  CONSTANTS  

// CLASS DECLARATION

/**
*  Defines a pure virtual function for notifying card events.
*  Reader Launcher must send reference of object implementing this interface
*  to specific Reader Handler in order to give Reader Handler a ability to 
*  notify occured events.
*
*  @lib Scard.lib
*  @since Series60 2.1
*/
class MScardNotifyObserver
    {
    public:

        /**
        * Notify on card event. Pure virtual.
        * @param aEvent Scard Event
        * @param aReaderID Reader ID
        * @return void
        */
        virtual void NotifyCardEvent( TScardServiceStatus aEvent, 
                                      TReaderID aReaderID ) = 0;
    };

#endif      // MSCARDNOTIFYOBSERVER_H

// End of File
