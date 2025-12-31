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
* Description:  Server header file
*
*/


#ifndef INTEGRITYCHECKSERVER_H
#define INTEGRITYCHECKSERVER_H

// INCLUDES

#include <e32std.h>
#include <e32base.h>
#include <e32cons.h>
#include <e32svr.h>
#include <f32file.h>

// CONSTANTS

_LIT( KIntecrityCheckServer,"IntegrityCheckServer" );
_LIT( KIntecrityCheckServerName,"!IntegrityCheckServer" );
_LIT( KIntecrityCheckServerText,"Boot Time Integrity Check Server" );

const TInt KParam0 = 0;
// BTIC Server auto close time.
const TInt KServerCloseTime = 8000000; // 8 sec.

// ---------------------------------------------------------------------------
// Server's policy 
// ---------------------------------------------------------------------------

//Total number of ranges
const TUint CPRangeCount = 2;

//Definition of the ranges of IPC numbers
const TInt CPRanges[CPRangeCount] =
        {
        0,
        1
        };

//Policy to implement for each of the above ranges
const TUint8 CPElementsIndex[CPRangeCount] =
        {
        0,
        CPolicyServer::ENotSupported
        };

//Specific capability checks
const CPolicyServer::TPolicyElement CPElements[] =
        {
        {_INIT_SECURITY_POLICY_C1(ECapabilityReadDeviceData), 
        CPolicyServer::EPanicClient},
        };

//Package all the above together into a policy
const CPolicyServer::TPolicy CPPolicy =
        {
        CPolicyServer::EAlwaysPass, // All connect attempts pass
        CPRangeCount,               // Number of ranges
        CPRanges,                   // Ranges array
        CPElementsIndex,            // Elements<->ranges index
        CPElements,                 // Array of elements
        };

// CLASS DECLARATION

// MACROS

// DATA TYPES 

// FUNCTION PROTOTYPES 

// FORWARD DECLARATIONS

// LOCAL FUNCTION PROTOTYPES 

// CLASS DECLARATION 

/**
*  CShutdown class
*
*  @lib
*/
class CShutdown : public CTimer
    {
    public:
        CShutdown();           
        void ConstructL();               
        void Start();
        
    private:
        void RunL();        
    };

/**
*  CIntegrityCheckServer class
*
*  @lib
*  @since 3.1
*/
class CIntegrityCheckServer : public CPolicyServer
    {
    public:
    
        static CServer2* NewLC();
        
        void AddSession();
        
        void RemoveSession();        

    private:
    
        /**
        * Default constructor        
        */
        CIntegrityCheckServer();

        void ConstructL();
        
        /**
        * From CServer2
        */       
        virtual CSession2* NewSessionL( 
            const TVersion& aVersion, 
            const RMessage2& aMessage ) const;       
        
    private:
        TInt iSessionCount;
        CShutdown iShutDown;       
    };

/**
*  CIntegrityCheckSession class
*  Class for Integrity Check server session
*
*  @lib
*  @since 3.1
*/
class CIntegrityCheckSession : public CSession2
    {
    public:
    
        /**
        * Constructor        
        */
        CIntegrityCheckSession();
        
        /**
        * Called by the CServer2 framework after constructor.
        */       
        void CreateL();
 
        /**
         * Called by the CServer2 framework after client close.
         */     
        void Disconnect( const RMessage2 &aMessage );

    private:
    
        /**
        * Destructor.
        *
        */
        ~CIntegrityCheckSession();
                
        void ServiceL( const RMessage2& aMessage );        

        void ServiceError( const RMessage2& aMessage, TInt aError );

        /**
        * Get device sw boot reason
        * @param aMessage user parameters        
        */
        void GetSWBootReasonL( const RMessage2& aMessage );

    private: // Data
                

    };

#endif  // INTEGRITYCHECKSERVER_H

//EOF
