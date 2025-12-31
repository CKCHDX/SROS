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
* Description:  Inline implementations for the TWIMSrvStartSignal class
*
*/



// -----------------------------------------------------------------------------
// TWIMSrvStartSignal::TWIMSrvStartSignal
// Default constructor.
// -----------------------------------------------------------------------------
//
inline TWIMSrvStartSignal::TWIMSrvStartSignal()
    {
    }

// -----------------------------------------------------------------------------
// TWIMSrvStartSignal::TWIMSrvStartSignal
// Default constructor.
// -----------------------------------------------------------------------------
//
inline TWIMSrvStartSignal::TWIMSrvStartSignal( TRequestStatus& aStatus )
    : iStatus( &aStatus ),
      iId( RThread().Id() )
    {
    aStatus = KRequestPending;
    }

// -----------------------------------------------------------------------------
// TWIMSrvStartSignal::Command
// Returns TPtrC which is created by using this.
// -----------------------------------------------------------------------------
//
inline TPtrC TWIMSrvStartSignal::Command() const
    {
    return TPtrC( (const TText*)this, sizeof( *this ) / sizeof( TText ) );
    }

// -----------------------------------------------------------------------------
// TWIMSrvStartSignal::Set
// Setter function which is called from Wimserver.cpp.
// -----------------------------------------------------------------------------
//
inline TInt TWIMSrvStartSignal::Set( const TDesC& aCommand )
    {
    TInt ret;

    if ( aCommand.Size() != sizeof( *this ) )
        {
        ret = KErrGeneral;
        }
    else
        {
        Mem::Copy( this, aCommand.Ptr(), sizeof( *this ) );
        ret = KErrNone;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// TWIMSrvStartSignal::RequestStatus
// Return RequestStatus set in construction
// -----------------------------------------------------------------------------
//
inline TRequestStatus* TWIMSrvStartSignal::RequestStatus()
    {
    return iStatus;
    }

// -----------------------------------------------------------------------------
// TWIMSrvStartSignal::ThreadId
// Return thread ID set in construction
// -----------------------------------------------------------------------------
//
inline TThreadId TWIMSrvStartSignal::ThreadId()
    {
    return iId;
    }

// End of File

