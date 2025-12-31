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
* Description:  Main smard card object, acts as client for Scard Server
*
*/



#ifndef RSCARD_H
#define RSCARD_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*  The basic client class.
*  The basic client class representing a channel of communication 
*  with the server. The connect function starts the server, if it 
*  is not already running. An RSessionBase sends messages to the 
*  server with the function RSessionBase::SendReceive(); specifying 
*  an opcode (TScardServerRqst) and and array of argument pointers. 
*  This class is a friend to the classes Comm and Listener.
*  All four are classes that need to communicate 
*  directly with the server.
*
*  @lib Scard.lib
*  @since Series60 2.1
*/
class RScard : public RSessionBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static RScard* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~RScard();

    private:
        
        /**
        * C++ default constructor.
        */
        RScard();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Version of server
        * @return Version number
        */
        TVersion Version() const;
        
    private:    // Friend classes
        friend class CScardComm;
        friend class CScardListener;
    };

#endif      // RSCARD_H
            
// End of File
