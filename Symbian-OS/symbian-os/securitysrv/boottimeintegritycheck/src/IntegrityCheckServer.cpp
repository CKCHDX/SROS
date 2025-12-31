/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Server source file
*
*/


// INCLUDE FILES

#include <centralrepository.h>
#include <startupreason.h>
#include <starterdomaincrkeys.h>

#include "IntegrityCheckServer.h"
#include "IntegrityCheckDefs.h"
#include "IntegrityCheckDebug.h"

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// ---------------------------------------------------------------------------
// RunServerL
//
// ---------------------------------------------------------------------------

static void RunServerL()
    {
    BTIC_TRACE_PRINT("[BTIC Server] RunServerL");
        
    User::LeaveIfError( RThread::RenameMe(KIntecrityCheckServerName) );
    
    // Create and install the active scheduler
    CActiveScheduler* scheduler = new ( ELeave ) CActiveScheduler;   
    CleanupStack::PushL( scheduler );
    
    CActiveScheduler::Install( scheduler );

    // Create server
    CIntegrityCheckServer::NewLC();

    // Initialisation complete
    RProcess::Rendezvous( KErrNone );

    BTIC_TRACE_PRINT("[BTIC Server] RunServerL: CActiveScheduler::Start");
    // Start active scheduler
    CActiveScheduler::Start();
    
    CleanupStack::PopAndDestroy( 2 ); // scheduler, CIntegrityCheckServer
    }


// ---------------------------------------------------------------------------
// E32Main()
//
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
    BTIC_TRACE_PRINT("[BTIC Server] E32Main");
    CTrapCleanup* cleanup = CTrapCleanup::New();
    
    if ( cleanup == NULL )
        {
        return KErrNoMemory;
        }
    
    TRAPD( err, RunServerL() );
    
    __ASSERT_ALWAYS( !err, User::Panic( KIntecrityCheckServerText, err ) );
    
    delete cleanup;

    __UHEAP_MARKEND;

    return KErrNone;
    }


//======================== CIntegrityCheckServer =============================

// ---------------------------------------------------------------------------
// CIntegrityCheckServer::CIntegrityCheckServer
//
// ---------------------------------------------------------------------------
//
CIntegrityCheckServer::CIntegrityCheckServer()
    :CPolicyServer( 0, CPPolicy, ESharableSessions )
    {
    }


// ---------------------------------------------------------------------------
// CIntegrityCheckServer::NewLC
//
// ---------------------------------------------------------------------------
//
CServer2* CIntegrityCheckServer::NewLC()
    {
    CIntegrityCheckServer* self = new ( ELeave ) CIntegrityCheckServer;
    CleanupStack::PushL( self );
    
    self->ConstructL();
    
    return self;
    }


// ---------------------------------------------------------------------------
// CIntegrityCheckServer::ConstructL
//
// ---------------------------------------------------------------------------
//
void CIntegrityCheckServer::ConstructL()
    {
    BTIC_TRACE_PRINT("[BTIC Server] ConstructL");
    iSessionCount = 0;
    StartL( KIntecrityCheckServer );    
    iShutDown.ConstructL();
    BTIC_TRACE_PRINT("[BTIC Server] ConstructL: Start shutdown timer");
    iShutDown.Start();    
    }


// ---------------------------------------------------------------------------
// CIntegrityCheckServer::NewSessionL
//
// ---------------------------------------------------------------------------
//
CSession2* CIntegrityCheckServer::NewSessionL(
    const TVersion&,
    const RMessage2&) const
    {
    BTIC_TRACE_PRINT("[BTIC Server] NewSessionL");
    return new (ELeave) CIntegrityCheckSession();
    }

// -----------------------------------------------------------------------------
// CIntegrityCheckServer::AddSession()
// Cancel the shutdown timer now, the new session is connected
//
// -----------------------------------------------------------------------------
//
void CIntegrityCheckServer::AddSession()
    {
    BTIC_TRACE_PRINT("[BTIC Server] AddSession");
    iSessionCount++;
    BTIC_TRACE_PRINT("[BTIC Server] Cancel shutdown timer");
    iShutDown.Cancel();  // Cancel any outstanding shutdown timer    
    }
 
// -----------------------------------------------------------------------------
// CIntegrityCheckServer::RemoveSession
// Decrement the session counter and start the shutdown timer if the last client
// has disconnected
// -----------------------------------------------------------------------------
//
void CIntegrityCheckServer::RemoveSession()
    {
    BTIC_TRACE_PRINT("[BTIC Server] RemoveSession");
    BTIC_TRACE_PRINT_NUM("[BTIC Server] Session count = %d", iSessionCount );
    if ( --iSessionCount == 0 )
        {      
        BTIC_TRACE_PRINT("[BTIC Server] Remove session and shutdown ");
        iShutDown.Start();
        }
    }


//======================== CIntegrityCheckSession ============================

// ---------------------------------------------------------------------------
// CIntegrityCheckSession::CIntegrityCheckSession
//
// ---------------------------------------------------------------------------
//
CIntegrityCheckSession::CIntegrityCheckSession()
    {    
    }


// ---------------------------------------------------------------------------
// CIntegrityCheckSession::CreateL
// Called by the CServer2
// ---------------------------------------------------------------------------
//
void CIntegrityCheckSession::CreateL()
    {
    BTIC_TRACE_PRINT("[BTIC Srv session] CIntegrityCheckSession");
    
    CIntegrityCheckServer* server( static_cast<CIntegrityCheckServer*>
            ( const_cast<CServer2*>( CSession2::Server() ) ) );    
    // Ok we have client so let's cancel shutdown timer.
    server->AddSession();    
    }


// ---------------------------------------------------------------------------
// CIntegrityCheckSession::~CIntegrityCheckSession
//
// ---------------------------------------------------------------------------
//
CIntegrityCheckSession::~CIntegrityCheckSession()
    {
    BTIC_TRACE_PRINT("[BTIC Srv session] ~CIntegrityCheckSession");
    
    CIntegrityCheckServer* server( static_cast<CIntegrityCheckServer*>
            ( const_cast<CServer2*>( CSession2::Server() ) ) );
    
    server->RemoveSession();
    }


// ---------------------------------------------------------------------------
// CIntegrityCheckSession::Disconnect
//
// ---------------------------------------------------------------------------
//
void CIntegrityCheckSession::Disconnect( const RMessage2 &aMessage )
    {
    BTIC_TRACE_PRINT("[BTIC Srv session] Disconnect");    
    CSession2::Disconnect( aMessage );
    }


// ---------------------------------------------------------------------------
// CIntegrityCheckSession::ServiceL
//
// ---------------------------------------------------------------------------
//
void CIntegrityCheckSession::ServiceL(const RMessage2& aMessage)
    {
    BTIC_TRACE_PRINT("[BTIC Srv session] ServiceL");
    
    switch ( aMessage.Function() )
        {
        case EBTICBootReasonSW:
            {            
            TRAPD( err, GetSWBootReasonL( aMessage ) );
            
            aMessage.Complete( err );
            break;
            }
        default:      
            aMessage.Complete( KErrGeneral );
            break;
        }
    }


// ---------------------------------------------------------------------------
// CIntegrityCheckSession::ServiceError
//
// ---------------------------------------------------------------------------
//
void CIntegrityCheckSession::ServiceError( 
    const RMessage2& aMessage,
    TInt aError )
    {
    BTIC_TRACE_PRINT("[BTIC Srv session] ServiceError");
    CSession2::ServiceError( aMessage, aError );
    }


// ---------------------------------------------------------------------------
// CIntegrityCheckSession::GetSWBootReasonL
//
// ---------------------------------------------------------------------------
//
void CIntegrityCheckSession::GetSWBootReasonL( const RMessage2& aMessage )
    {  
    BTIC_TRACE_PRINT("[BTIC Srv session] GetSWBootReasonL");  
    // Get sw boot reason from system.   
    TInt bootReason = KErrNone;     
    TInt startupReason = KErrNone;
    CRepository* repository( NULL );
    BTIC_TRACE_PRINT("[BTIC Srv session] Get startup reason from CRepository");
        
    TInt err = KErrNone;
            
    TRAP( err, repository = CRepository::NewL( KCRUidStartup ) );
    
    BTIC_TRACE_PRINT_NUM("[BTIC Srv session] CRepository TRAPD Err = %d", err );
	
    if ( err == KErrNone )
        {
        CleanupStack::PushL( repository );
                               
        err = repository->Get( KStartupReason, startupReason );        
        
        BTIC_TRACE_PRINT_NUM("[BTIC Srv session] CRepository Get Err = %d", err );
        
	    if ( err == KErrNone )
		   {
		   BTIC_TRACE_PRINT_NUM("Returned startup reason: %d ",startupReason );
		    
            if ( startupReason == EDeepRFSReset )
			    {
			     // C drive will be formatted, so let's stop checking.
			     bootReason = EBTICRestoreFactorySetDeep;
			     BTIC_TRACE_PRINT("[BTIC Srv session] Startup reason is FORMAT");
			     }
            else
                {
                // All other reasons are "normal" boot for BTIC.
                bootReason = EBTICNormalBoot;
                BTIC_TRACE_PRINT("[BTIC Srv session] Startup reason is NORMAL");
                }    			    
            }
        else if ( err == KErrNotFound )
            {
            // Reason not found. Propably first boot etc.
            bootReason = EBTICNoSWReason;
            BTIC_TRACE_PRINT("[BTIC Srv session] No SW Startup reason found");            
            }
        else
            {
            // Some other error.
            bootReason = err;
            }                
                
	    CleanupStack::PopAndDestroy(); //repository    
	    }
        
    TPckg<TInt> bootReasonValue( bootReason );
    
    BTIC_TRACE_PRINT("[BTIC Srv session] Write result to client side.");
    // Write boot reason to client side.
    aMessage.WriteL( KParam0, bootReasonValue );      
    }
 

//============================== CShutdown =====================================

// -----------------------------------------------------------------------------
// CShutdown::CShutdown
// 
//
// -----------------------------------------------------------------------------
//
CShutdown::CShutdown():CTimer(-1)
    {
    BTIC_TRACE_PRINT("[BTIC Shutdown] CShutdown: Add scheduler");
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CShutdown::ConstructL
// 
//
// -----------------------------------------------------------------------------
//
void CShutdown::ConstructL()
    {
    BTIC_TRACE_PRINT("[BTIC Shutdown] ConstructL");
    CTimer::ConstructL();
    }

// -----------------------------------------------------------------------------
// CShutdown::Start
// Set shutdown timer for server.
//
// -----------------------------------------------------------------------------
//
void CShutdown::Start()
    {
    BTIC_TRACE_PRINT("[BTIC Shutdown] Start timer");
    After( KServerCloseTime );
    }

// -----------------------------------------------------------------------------
// CShutdown::RunServerL
// Initiates server exit when the timer expires
//
// -----------------------------------------------------------------------------
//
void CShutdown::RunL()
    {
    BTIC_TRACE_PRINT("[BTIC Shutdown] RunL: Stop BTIC server");
    CActiveScheduler::Stop();
    }


//EOF
