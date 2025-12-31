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
* Description:  This is the header file for the TWIMSrvStartSignal class
*
*/



#ifndef WIMSIG_H
#define WIMSIG_H

//  INCLUDES
#include <e32std.h>


// CLASS DECLARATION

/**
*  TWIMSrvStartSignal
*  
*  @since Series60 2.1
*/
class TWIMSrvStartSignal
    {
    public:  // Constructors and destructor

        /**
        * Constructor.
        */
        inline TWIMSrvStartSignal();
        
        /**
        * Constructor.
        */
        inline TWIMSrvStartSignal( TRequestStatus& aStatus );

    public: // New functions        

        /**
        * Setter that is used on server.cpp: InitProcess
        * @param aCommand Command
        * @return TInt
        */
        inline TInt Set( const TDesC& aCommand );

        /**
        * Returns TPtrC which is created by using this.
        * @return TPtrC
        */
        inline TPtrC Command() const;

        /** 
        * Returns iStatus member variable
        * @return TRequestStatus*
        */
        inline TRequestStatus* RequestStatus();

        /** 
        * Returns Threads ID
        * @return TThreadId
        */
        inline TThreadId ThreadId();

    private:

        /** 
        * Copy constructor
        * @param aSource -Reference to class object.
        */
        TWIMSrvStartSignal( const TWIMSrvStartSignal& aSource );

        /** 
        * Assigment operator
        * @param aParam -Reference to class object.
        */
        TWIMSrvStartSignal& operator = ( const TWIMSrvStartSignal& aParam );
    
    private: // Data
        // //Status from caller
        TRequestStatus* iStatus;
        // Thread ID
        const TThreadId iId;
    };


// FUNCTION PROTOTYPES
//Imported function to start Wim server
IMPORT_C TInt StartWimServer( TWIMSrvStartSignal& aSignal );

#include "WimSig.inl" // Inline impelementations

#endif      // WIMSIG_H

// End of File
