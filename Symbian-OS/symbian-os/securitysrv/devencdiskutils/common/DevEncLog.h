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
* Description:  Debug logging functionality
*
*/


#ifndef __DEVENC_LOG_H__
#define __DEVENC_LOG_H__

#include "DevEncConfig.hrh"

#if defined( _DEBUG ) && defined ( DEVENC_LOG )

#include <e32base.h>
#include <e32cmn.h>
#include <e32debug.h>

class CFileLog : public CBase
	{
    public:
    	static void Printf( TRefByValue<const TDesC> aFmt, ... );
	};

// Debug trace to stdout
#define DLOG( aText ) \
    { \
    _LIT( KText, aText ); \
    RDebug::Print( KText ); \
    }

// Debug trace to stdout
#define DLOG2( aText, aParam ) \
    { \
    _LIT( KText, aText ); \
    RDebug::Print( KText, aParam ); \
    }

// Debug trace to stdout
#define DLOG3( aText, aParam1, aParam2 ) \
    { \
    _LIT( KText, aText ); \
    RDebug::Print( KText, aParam1, aParam2 ); \
    }

// Debug trace to file
#define FLOG( aText ) \
    { \
    _LIT( KLogText, aText ); \
    CFileLog::Printf( KLogText ); \
    }

// Debug trace to file
#define FLOG2( aText, b ) \
    { \
    _LIT( KLogText, aText ); \
    CFileLog::Printf( KLogText, b ); \
    }

// Debug trace to file
#define FLOG3( aText, b, c ) \
    { \
    _LIT( KLogText, aText ); \
    CFileLog::Printf( KLogText, b, c ); \
    }

// Debug trace to stdout and file
#define DFLOG( aText ) \
    { \
    DLOG( aText ); \
    FLOG( aText ); \
    }

// Debug trace to stdout and file
#define DFLOG2( aText, aParam ) \
    { \
    DLOG2( aText, aParam ); \
    FLOG2( aText, aParam ); \
    }

// Debug trace to stdout and file
#define DFLOG3( aText, aParam1, aParam2 ) \
    { \
    DLOG3( aText, aParam1, aParam2 ); \
    FLOG3( aText, aParam1, aParam2 ); \
    }

// Debug trace a string literal to file
#define FLOGBUF( aText ) \
    { \
    CFileLog::Printf( aText ); \
    }

// Debug trace a string literal
#define DLOGBUF( aText ) \
    { \
    RDebug::Print( aText ); \
    }

// Debug trace a string literal to file and stdout
#define DFLOGBUF( aText ) \
    { \
    DLOGBUF( aText ); \
    FLOGBUF( aText ); \
    }
    
#else   // _DEBUG not defined, no logging code will be included at all

#define DLOG( a )
#define DLOG2( a, b )
#define DLOG3( a, b, c )
#define FLOG( a )
#define FLOG2( a, b )
#define FLOG3( a, b, c )
#define DFLOG( a )
#define DFLOG2( a, b )
#define DFLOG3( a, b, c )
#define FLOGBUF( a )
#define DFLOGBUF( a )
    
#endif // _DEBUG

#endif // __DEVENC_LOG_H__

// End of File

