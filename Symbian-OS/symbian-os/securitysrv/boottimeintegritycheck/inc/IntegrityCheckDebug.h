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
* Description:  Header file for debug settings.
*
*/


#ifndef INTEGRITYCHECKDEBUG_H
#define INTEGRITYCHECKDEBUG_H

// INCLUDES 
#include    <e32base.h>

// CONSTANTS 
    
// MACROS 

#ifdef _DEBUG
// Enable trace printing
#define INTEGRITYCHECK_TRACE
#endif

#ifdef INTEGRITYCHECK_TRACE
#include <e32svr.h>
#define BTIC_TRACE_PRINT( aText ) { RDebug::Print( _L(aText) ); }
#define BTIC_TRACE_PRINT_NUM( aText, aNum ) \
    { RDebug::Print( _L(aText), aNum ); }
#define BTIC_TRACE_PRINT_RAW( aPtr ) \
    { RDebug::RawPrint( aPtr ); RDebug::Print( _L("\n") ); }  
#define BTIC_TRACE_PRINT_ALLOC_CELLS( aText ) \
    { RDebug::Print( _L(aText), User::CountAllocCells() ); }
#else 
#define BTIC_TRACE_PRINT( aText )
#define BTIC_TRACE_PRINT_NUM( aText, aNum )
#define BTIC_TRACE_PRINT_RAW( aPtr )
#define BTIC_TRACE_PRINT_ALLOC_CELLS( aText )  
#endif

// DATA TYPES 

// FUNCTION PROTOTYPES 

// FORWARD DECLARATIONS

// LOCAL FUNCTION PROTOTYPES 

// CLASS DECLARATION 

#endif //INTEGRITYCHECKDEBUG_H

//EOF 
