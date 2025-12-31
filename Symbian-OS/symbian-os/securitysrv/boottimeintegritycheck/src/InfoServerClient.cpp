/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for direct use of cert provisioner info server.
*
*/


// INCLUDE FILES
#include <mmtsy_names.h>

#include "InfoServerClient.h"
#include "IntegrityCheckDefs.h"
#include "IntegrityCheckDebug.h"

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ---------------------------------------------------------------------------
// StartServer()
//
// Start CertProvisionInfoServer
// ---------------------------------------------------------------------------
//
static TInt StartServer()
    {
    BTIC_TRACE_PRINT("[CInfoServerClient] StartServer --->");
    
    const TUidType serverUid( 
        KNullUid, 
        KNullUid, 
        KBTICCertProvInfoServerUid3 );

    RProcess server;

    TInt r = server.Create( KBTICCertProvInfoServer, KNullDesC );
    
    if ( r != KErrNone )
        {
        BTIC_TRACE_PRINT_NUM( "[Info server client] server start failed %d", r );
        return r;
        }

    TRequestStatus stat;
    server.Rendezvous( stat );

    if ( stat != KRequestPending )
       {
       server.Kill( 0 );
       }
    else
       {
       server.Resume();
       }

    User::WaitForRequest( stat );

    r = ( server.ExitType() == EExitPanic ) ? KErrGeneral : stat.Int();

    server.Close();

    BTIC_TRACE_PRINT("[CInfoServerClient] StartServer <---");

    return r;
    }


// ---------------------------------------------------------------------------
// Connect()
//
// Connect to CertProvisionInfoServer
// ---------------------------------------------------------------------------
//
TInt RInfoServerClient::Connect()
    {
    BTIC_TRACE_PRINT("[CInfoServerClient] Connect --->");
    
    TInt retry( 2 );

    for (;;)
        {
        TInt status = 
            CreateSession( KBTICCertProvInfoServer, TVersion( 0, 0, 0 ), 1 );

        if ( ( status != KErrNotFound ) && ( status != KErrServerTerminated ) )
            {
            return status;
            }
       
        if ( --retry == 0 )
            {
            return status;
            }

        status = StartServer();

        if ( ( status != KErrNone ) && ( status != KErrAlreadyExists ) )
            {
            return status;
            }
        }               
    }


// ---------------------------------------------------------------------------
// Close()
// ---------------------------------------------------------------------------
//
void RInfoServerClient::Close()
    {
    BTIC_TRACE_PRINT("[CInfoServerClient] Close");
    RSessionBase::Close();
    }


// ---------------------------------------------------------------------------
// GetSerialNumber()
//
// Get device's serial number (IMEI) from info server.
// ---------------------------------------------------------------------------
//
TInt RInfoServerClient::GetSerialNumber( TDes& aSerialNumber )
    {
    BTIC_TRACE_PRINT("[CInfoServerClient] GetSerialNumber");
    
    return SendReceive( EPSSerialNumber, TIpcArgs( &aSerialNumber ) );        
    }

