/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Print debug log 
*
*/


#ifndef	REMOTELOCKTRACE_H
#define	REMOTELOCKTRACE_H

#include <e32base.h>

//#define RL_TRACE

#ifdef RL_TRACE
#include <e32svr.h>
#define RL_TRACE_PRINT( aText ) { RDebug::Print( _L(aText) ); }
#define RL_TRACE_PRINT_NUM( aText, aNum ) \
		{ RDebug::Print( _L(aText), aNum ); }
#else
#define RL_TRACE_PRINT( aText )
#define RL_TRACE_PRINT_NUM( aText, aNum ) 
#endif

#endif	
//EOF