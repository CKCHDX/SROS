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
* Description:  Client source file
*
*/


// INCLUDE FILES
#include "IntegrityCheckClient.h"
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
// ---------------------------------------------------------------------------
//
static TInt StartServer()
    {
    BTIC_TRACE_PRINT("[BTIC Client] StartServer"); 
    
    const TUidType serverUid( 
        KNullUid, 
        KNullUid, 
        KBTICServerUid3 );

    RProcess server;

    TInt err = server.Create( KBTICServer, KNullDesC );
    BTIC_TRACE_PRINT_NUM("StartServer: Create error = %d", err );
    
    if ( err != KErrNone )
        {      
        return err;
        }

    TRequestStatus status;
    
    server.Rendezvous( status );

    if ( status != KRequestPending )
        {
        server.Kill( 0 );
        }
    else
        {
        server.Resume();
        }
    
    // Wait to server to start.
    User::WaitForRequest( status );

    err = ( server.ExitType() == EExitPanic ) ? KErrGeneral : status.Int();

    server.Close();

    return err;
    }


// ---------------------------------------------------------------------------
// RIntegrityCheckClient::Connect()
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RIntegrityCheckClient::Connect()
    {
    BTIC_TRACE_PRINT("[BTIC Client] Connect"); 
    TInt retry( 2 );

    for (;;)
        {
   
        TInt status = 
            CreateSession( KBTICServer, TVersion( 0, 0, 0 ), 1 );
        BTIC_TRACE_PRINT_NUM("Connect: CreateSession status = %d", status );

        if ( ( status != KErrNotFound ) && ( status != KErrServerTerminated ) )
            {
            return status;
            }

        if ( --retry == 0 )
            {
            return status;
            }
        
        BTIC_TRACE_PRINT("Connect: StartServer"); 
        status = StartServer();
        BTIC_TRACE_PRINT_NUM("Connect: StartServer status = %d", status );
        
        if ( ( status != KErrNone ) && ( status != KErrAlreadyExists ) )
            {
            return status;
            }       
        }
    }


// ---------------------------------------------------------------------------
// RIntegrityCheckClient::Close()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void RIntegrityCheckClient::Close()
    {
    BTIC_TRACE_PRINT("[BTIC Client] Close"); 
    RSessionBase::Close();
    }


// ---------------------------------------------------------------------------
// RIntegrityCheckClient::GetSWBootReason()
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RIntegrityCheckClient::GetSWBootReason( TInt& aBootReason  )
    {
    BTIC_TRACE_PRINT("[BTIC Client] GetSWBootReason"); 
    TPckg<TInt> pckgBootReason( aBootReason );
    return SendReceive( EBTICBootReasonSW, TIpcArgs( &pckgBootReason ) );
    }

